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
#include <qstyleditemdelegate.h>
#include <qabstractitemmodel.h>
#include <qabstractitemview.h>
#include <qstringlistmodel.h>
#include <qdatawidgetmapper.h>
#include <qsocketnotifier.h>
#include <qnetworkaccessmanager.h>
#include <qmenu.h>
#include <qtreeview.h>
#include <libircclient.h>
#include <memory>
#include "d7irc_data.h"


// forward declarations

struct IRCBuffer;

class IRCExternalDownloader;
class IRCMessageHandler;
class IRCMainUI;
class IRCBufferModel;

namespace Ui {
	class SamuraIRC;
	class AddServer;
};


// Subclass to handle return, tab, and sizing logic of the text entry field

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


// There is one IRCConnection object for each server connection.
// They're all running on the same IRC thread (separate from main thread).

class IRCConnection : public QObject {
	Q_OBJECT;
public:
	IRCConnection    (int id, const IRCServerDetails& details, QThread* thread);
	IRCServerDetails details;
	const int        our_id;
signals:
	void connect    (int id);
	void join       (int id, const QString& chan, const QString& user);
	void part       (int id, const QString& chan, const QString& user);
	void quit       (int id, const QString& user, const QString& msg);
	void privmsg    (int id, const QString& from, const QString& to, const QString& msg);
	void nick       (int id, const QString& user, const QString& new_nick);
	void numeric    (int id, unsigned event, QStringList data);
	void disconnect (int id, int err, int sub_err);
public slots:
	void start       ();
	void stop        ();
	void tick        ();
	void sendPrivMsg (int id, const QString& target, const QString& msg);
	void sendRawMsg  (int id, const QString& raw);
private:
	std::vector<std::unique_ptr<QSocketNotifier>> notifiers;
	irc_session_t* irc_session;
};


// There is a single IRCMessageHandler on the main thread that has signal/slot
// connections to each IRCConnection on the IRC thread

class IRCMessageHandler : public QObject {
	Q_OBJECT;
signals:
	void dispatchPrivMsg (int id, const QString& chan, const QString& msg);
	void dispatchRawMsg  (int id, const QString& raw);
public slots:

	// from server to us
	void handleIRCConnect (int id);
	void handleIRCJoin    (int id, const QString& chan, const QString& user);
	void handleIRCPart    (int id, const QString& chan, const QString& user);
	void handleIRCQuit    (int id, const QString& user, const QString& msg);
	void handleIRCNick    (int id, const QString& user, const QString& new_nick);
	void handleIRCPrivMsg (int id, const QString& from, const QString& to, const QString& msg);
	void handleIRCNumeric (int id, uint32_t event, QStringList data);

	void handleIRCDisconnect (int id, int err, int sub_err);

	// from us to server

	void sendIRCMessage (const QString& text);
	void sendIRCCommand (const QString& cmd);
};


// The buffer model stores all the buffers in a way compatible with qt's model/view
// stuff, so that they can be displayed on the side pane.

class IRCBufferModel : public QAbstractItemModel {
	Q_OBJECT;
public:
	IRCBufferModel();

	QModelIndex index   (int row, int col, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent  (const QModelIndex& idx) const override;
	int rowCount        (const QModelIndex& parent = QModelIndex()) const override;
	int columnCount     (const QModelIndex& parent = QModelIndex()) const override;
	QVariant data       (const QModelIndex& idx, int role = Qt::DisplayRole) const override;
	Qt::ItemFlags flags (const QModelIndex& idx) const override;

	QModelIndex buffer2index (IRCBuffer*);

	void markBufferDirty (IRCBuffer*);

	IRCBuffer* addServer  (const QString& name);
	IRCBuffer* addChannel (const QString& serv, const QString& chan);

	IRCBuffer* getDefault ();

	IRCBuffer* findServer  (const QString& name);
	IRCBuffer* findChannel (const QString& serv, const QString& chan);

signals:
	void serverAdded (const QModelIndex& idx);
private:
	IRCBuffer* root;
};


// The user model is another qt compatible model for the nick list side pane.

class IRCUserModel : public QAbstractTableModel {
	Q_OBJECT;
public:
	
	int rowCount    (const QModelIndex& parent = QModelIndex()) const override;
	int columnCount (const QModelIndex& parent = QModelIndex()) const override;
	QVariant data   (const QModelIndex& idx, int role = Qt::DisplayRole) const override;

	void add (const QString& nick, IRCPrefix* prefix = nullptr);
	bool del (const QString& nick);

	void clear();

	static QColor nickColor(const QString& nick);

	struct User {
		QString nick;
		IRCPrefix prefix;
	};

	std::vector<User> nicks;
};


// another model for channels in the 'add server' UI

class IRCChannelModel : public QAbstractTableModel {
	Q_OBJECT;
public:
	int rowCount    (const QModelIndex& parent = QModelIndex()) const override;
	int columnCount (const QModelIndex& parent = QModelIndex()) const override;
	QVariant data   (const QModelIndex& idx, int role = Qt::DisplayRole) const override;

	QVariant headerData (int col, Qt::Orientation, int role = Qt::DisplayRole) const override;

	bool setData        (const QModelIndex& i, const QVariant& v, int role = Qt::EditRole) override;
	Qt::ItemFlags flags (const QModelIndex& i) const override;

	void push();
	void pop();

	void setChannels(std::vector<IRCChanDetails>* chans);

private:
	std::vector<IRCChanDetails>* chans;
};


// Widget on which the "Add server" UI is created, containing the corresponding UI logic.

class IRCAddServerUI : public QDialog {
	Q_OBJECT;
public:
	IRCAddServerUI();
	void hookStuffUp();
	Ui::AddServer* ui;
private:
	QDataWidgetMapper mapper;
	IRCChannelModel chan_model;
};


// Widget on which the main UI is created and handled.

class IRCMainUI : public QMainWindow {
	Q_OBJECT;
public:
	IRCMainUI();
	void hookStuffUp();
	Ui::SamuraIRC* ui;
public slots:
	void bufferChange (const QModelIndex& idx, const QModelIndex& prev);
//	bold/italic/etc buttons
//  context menus
//  click name to get settings / change name?
private:
	QMenu* buffer_menu;
	QMenu* nicks_menu;
	bool max_scroll;
};


// class to manage downloading external images and code snippets.

class IRCExternalDownloader : public QObject {
	Q_OBJECT;
public slots:
	void checkMessage(const QString& msg, IRCBuffer* buf);
private:
	QNetworkAccessManager net;
};



// This thing is necessary to hide the passwords in the table of channels
// in the "Add server" UI.

class IRCChanPassDelegate : public QStyledItemDelegate {
	Q_OBJECT;
public:
	QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const override;
	void paint(QPainter*, const QStyleOptionViewItem&, const QModelIndex&) const override;
signals:
	void updated(const QModelIndex& idx, const QString& txt) const;
};


// This is another qt model that holds the settings for each server, as well
// as global settings / config options for the whole program.

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

	std::vector<IRCChanDetails>* getChannelDetails(const QModelIndex& idx);

	int               serverNameToID (const QString& name);
	IRCServerDetails* getDetails     (int id);

	IRCServerDetails* getDetailsFromModelIdx(const QModelIndex& idx);

	IRCServerDetails** begin          (void);
	IRCServerDetails** end            (void);

	int  getOption (IRCOption opt);
	void setOption (IRCOption opt, int val);

	bool first_run;

private:
	uint64_t id_counter;
	std::vector<IRCServerDetails*> servers;
	QSettings settings;
};

Q_DECLARE_METATYPE(IRCServerDetails);


// holds the list of all such current connections.

class IRCConnectionRegistry : public QObject {
	Q_OBJECT;
public:
	IRCConnectionRegistry();

	bool createConnection(int id);
	bool destroyConnection(int id);

	IRCConnectionInfo* getInfo(int id);

	void setStatus(int id, IRCConnectionStatus status);

signals:
	void statusChanged(int id, IRCConnectionStatus status);
private:
	QThread* irc_thread;
	std::vector<IRCConnectionInfo> connections;
};
#endif
