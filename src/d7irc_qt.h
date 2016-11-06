#ifndef D7IRC_QT_H
#define D7IRC_QT_H
#include <qevent.h>
#include <qtextedit.h>
#include <qobject.h>
#include <qthread.h>
#include <qtimer.h>
#include <qabstractitemmodel.h>
#include <qabstractitemview.h>
#include <qsortfilterproxymodel.h>
#include <qitemdelegate.h>
#include <qsocketnotifier.h>
#include <qnetworkaccessmanager.h>
#include <qtreeview.h>
#include <libircclient.h>
#include <memory>

#define DEFAULT_PREFIX_IDX 0xFFFF

struct IRCBuffer;
struct IRCServerBuffer;

class IRCMessageHandler;

namespace Ui {
	class SamuraIRC;
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


class IRCGUILogic : public QObject {
	Q_OBJECT;
public:
	IRCGUILogic(Ui::SamuraIRC* ui);
public slots:
	void bufferChange (const QModelIndex& idx, const QModelIndex& prev);
//	bold/italic/etc buttons
//  context menus
//  click name to get settings / change name?
private:
	Ui::SamuraIRC* ui;
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
	IRCMessageHandler(IRCBufferModel* model, Ui::SamuraIRC* ui, IRCExternalDownloader* dl);
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
	Ui::SamuraIRC*  ui;
};

#endif
