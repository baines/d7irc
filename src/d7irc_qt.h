#ifndef D7IRC_QT_H
#define D7IRC_QT_H
#include <qevent.h>
#include <qtextedit.h>
#include <qobject.h>
#include <qthread.h>
#include <qtimer.h>
#include <qdialog.h>
#include <qsettings.h>
#include <qmainwindow.h>
#include <qabstractitemmodel.h>
#include <qstandarditemmodel.h>
#include <qabstractitemview.h>
#include <qstringlistmodel.h>
#include <qdatawidgetmapper.h>
#include <qsocketnotifier.h>
#include <qnetworkaccessmanager.h>
#include <qmenu.h>
#include <qtreeview.h>
#include <libircclient.h>
#include <memory>

#define DEFAULT_PREFIX_IDX 0xFFFF

struct IRCBuffer;
struct IRCServerBuffer;

class IRCMessageHandler;
class QMainWindow;

namespace Ui {
	class SamuraIRC;
	class AddServer;
};


class IRCTextEntry : public QTextEdit {
	Q_OBJECT;
public:
	IRCTextEntry(QWidget*& w);
	void keyPressEvent(QKeyEvent* ev) override;
signals:
	void textSubmit(const QString& text);
	void command(const QString& cmd);
private:
	void handleSubmit(void);
	int line_height;
	int line_count;
};


class IRCConnection : public QObject {
	Q_OBJECT;
public:
	IRCConnection(QThread* thread, IRCMessageHandler* handler);
	QString server;
signals:
	void connect (const QString& serv);
	void join    (const QString& serv, const QString& chan, const QString& user);
	void part    (const QString& serv, const QString& chan, const QString& user);
	void quit    (const QString& serv, const QString& user, const QString& msg);
	void privmsg (const QString& serv, const QString& from, const QString& to, const QString& msg);
	void nick    (const QString& serv, const QString& user, const QString& new_nick);
	void numeric (const QString& serv, unsigned event, QStringList data);

	void disconnect (int err, int sub_err);
public slots:
	void begin();
	void tick();
	void sendPrivmsg(const QString& target, const QString& msg);
	void sendRaw(const QString& raw);
private:
	std::vector<std::unique_ptr<QSocketNotifier>> notifiers;
	irc_session_t* irc_session;
};


class IRCBufferModel : public QAbstractItemModel {
	Q_OBJECT;
public:
	IRCBufferModel(QTextEdit* edit, QTreeView* view);

	QModelIndex index   (int row, int col, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent  (const QModelIndex& idx) const override;
	int rowCount        (const QModelIndex& parent = QModelIndex()) const override;
	int columnCount     (const QModelIndex& parent = QModelIndex()) const override;
	QVariant data       (const QModelIndex& idx, int role = Qt::DisplayRole) const override;
	Qt::ItemFlags flags (const QModelIndex& idx) const override;

	IRCServerBuffer* addServer  (const QString& name);
	IRCBuffer*       addChannel (const QString& serv, const QString& chan);
	IRCBuffer*       getDefault  ();

signals:
	void serverAdded (const QModelIndex& idx);
private:
	IRCBuffer* root;
};


class IRCUserModel : public QAbstractTableModel {
	Q_OBJECT;
public:
	IRCUserModel(IRCBuffer* container);

	int rowCount    (const QModelIndex& parent = QModelIndex()) const override;
	int columnCount (const QModelIndex& parent = QModelIndex()) const override;
	QVariant data   (const QModelIndex& idx, int role = Qt::DisplayRole) const override;

	void add (const QString& nick, int prefix_idx = DEFAULT_PREFIX_IDX);
	bool del (const QString& nick);

	static QColor nickColor(const QString& nick);

	struct User {
		QString nick;
		int prefix_idx;
	};

	std::vector<User> nicks;
	IRCBuffer* server;
};


class IRCAddServerUI : public QDialog {
	Q_OBJECT;
public:
	IRCAddServerUI(QWidget* parent);
private:
	Ui::AddServer* ui;
	QDataWidgetMapper mapper;
};


class IRCMainUI : public QMainWindow {
	Q_OBJECT;
public:
	IRCMainUI();

	Ui::SamuraIRC* ui;
	IRCBufferModel* buffers;
public slots:
	void bufferChange (const QModelIndex& idx, const QModelIndex& prev);
//	bold/italic/etc buttons
//  context menus
//  click name to get settings / change name?
private:
	IRCAddServerUI add_serv;

	QMenu* buffer_menu;
	QMenu* nicks_menu;
	bool max_scroll;
};


class IRCExternalDownloader : public QObject {
	Q_OBJECT;
public:
	IRCExternalDownloader() = default;
public slots:
	void checkMessage(const QString& msg, IRCBuffer* buf);
private:
	QNetworkAccessManager net;
};


class IRCMessageHandler : public QObject {
	Q_OBJECT;
public:
	IRCMessageHandler(IRCMainUI* ui, IRCExternalDownloader* dl);
signals:
	void tempSend(const QString& chan, const QString& msg);
	void tempSendRaw(const QString& raw);
public slots:

	// from server to us
	void handleIRCConnect (const QString& serv);
	void handleIRCJoin    (const QString& serv, const QString& chan, const QString& user);
	void handleIRCPart    (const QString& serv, const QString& chan, const QString& user);
	void handleIRCQuit    (const QString& serv, const QString& user, const QString& msg);
	void handleIRCNick    (const QString& serv, const QString& user, const QString& new_nick);
	void handleIRCPrivMsg (const QString& serv, const QString& from, const QString& to, const QString& msg);
	void handleIRCNumeric (const QString& serv, uint32_t event, QStringList data);

	void handleIRCDisconnect (int err, int sub_err);

	// from us to server

	void sendIRCMessage (const QString& text);
	void sendIRCCommand (const QString& cmd);

private:
	IRCExternalDownloader* downloader;
	IRCBufferModel* buf_model;
	Ui::SamuraIRC* ui;
};


enum IRCOption : int {
	IRC_OPT_HIDE_JOINPART,
	IRC_OPT_IMG_EXPAND,
	IRC_OPT_IMG_EXPAND_ALL,
	IRC_OPT_CODE_EXPAND,
};

struct IRCServerDetails {

	IRCServerDetails();

	QString unique_name;

	QString nickname;
	QString hostname;
	uint16_t port;
	bool ssl;

	QString username;
	QString realname;
	QString password;
	QString nickserv;

	QString commands;
	QStandardItemModel chans;
};

class IRCSettings : public QAbstractTableModel {
	Q_OBJECT;
public:
	IRCSettings();
	~IRCSettings();

	int rowCount    (const QModelIndex& p = QModelIndex()) const override;
	int columnCount (const QModelIndex& p = QModelIndex()) const override;
	QVariant data   (const QModelIndex& i, int role = Qt::DisplayRole) const override;

	bool setData        (const QModelIndex& i, const QVariant& v, int role = Qt::EditRole) override;
	Qt::ItemFlags flags (const QModelIndex& i) const override;

	void newServer(void);
	void delServer(const QModelIndex& i);

	QStandardItemModel* getChannelModel(const QModelIndex& idx);

	int               serverNameToID (const QString& name);
	IRCServerDetails* getDetails     (int id);

	int  getOption (IRCOption opt);
	void setOption (IRCOption opt, int val);

	bool first_run;

private:
	std::vector<IRCServerDetails*> servers;
	QSettings settings;
};

extern IRCSettings* d7irc_settings;

#endif
