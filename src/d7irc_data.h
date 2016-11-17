#ifndef D7IRC_DATA_H
#define D7IRC_DATA_H
#include <qstandarditemmodel.h>
#include <qtextdocument.h>
#include <qtextcursor.h>
#include <qstring.h>
#include <qthread.h>
#include <vector>

// defines / macros

#define EMPTY_PREFIX_IDX 0xFFFF

// forward declarations

class IRCUserModel;
class IRCConnection;
class IRCSettings;
class IRCMainUI;
class IRCAddServerUI;
class IRCExternalDownloader;
class IRCBufferModel;
class IRCConnectionRegistry;
class IRCMessageHandler;

// enums

enum IRCOption : int {
	IRC_OPT_HIDE_JOINPART,
	IRC_OPT_IMG_EXPAND,
	IRC_OPT_IMG_EXPAND_ALL,
	IRC_OPT_CODE_EXPAND,
};

enum IRCError : int {
	IRC_ERR_CREATE_SESSION,
	IRC_ERR_CONNECT,
	IRC_ERR_ADD,
	IRC_ERR_PROCESS,
	IRC_ERR_SELECT,
};

enum IRCBufferType : int {
	IRC_BUF_INTERNAL,
	IRC_BUF_SERVER,
	IRC_BUF_CHANNEL,
};

enum IRCBufferLevel : int {
	IRC_BUFLVL_NORMAL,
	IRC_BUFLVL_ACTIVITY,
	IRC_BUFLVL_HIGHLIGHT,
};

enum IRCServerDatum : int {
	IRC_DAT_UNIQUE_NAME,
	IRC_DAT_NICK,
	IRC_DAT_ADDRESS,
	IRC_DAT_PORT,
	IRC_DAT_SSL,
	IRC_DAT_USERNAME,
	IRC_DAT_REALNAME,
	IRC_DAT_PASSWORD,
	IRC_DAT_NICKSERV,
	IRC_DAT_CMDS,
};

enum IRCConnectionStatus : int {
	IRC_CON_DISCONNECTED,
	IRC_CON_CONNECTING,
	IRC_CON_CONNECTED,
};

// data types

// stores an IRC prefix, which consists of:
//   an index of importance (lower index == higher importance)
//   a symbol such as @ (operator), % (half-op), etc
//   a mode character - e.g. o for operator, since /mode +o gives op

struct IRCPrefix {
	int  index;
	char symbol;
	char mode;

	static IRCPrefix empty() { return IRCPrefix { EMPTY_PREFIX_IDX, 0, 0 }; }
	static bool isEmpty(const IRCPrefix& p){ return p.index == EMPTY_PREFIX_IDX; }
};


// type used to store info for each buffer that appears in the list on the side pane.

struct IRCBuffer {

	IRCBuffer(IRCBufferType type, const QString& name, IRCBuffer* parent);

	void addLine(const QString& prefix, const QString& msg);
	void addImage(const QImage& img);

	IRCBufferType type;
	
	QString name;
	QTextDocument* contents;
	QTextCursor cursor;
	IRCUserModel* nicks;

	bool inactive;
	IRCBufferLevel level;

	IRCBuffer* parent;
	IRCBuffer* child;
	IRCBuffer* sibling;
};

// simple struct for holding channels in server details

struct IRCChanDetails {
	QString name;
	QString pass;
};

// holds the saved details of a server that was added via the add-server ui
// or loaded from the ini config file. The unique ID is chosen at runtime and
// might change for different instances of the program.

struct IRCServerDetails {

	IRCServerDetails() = default;
	IRCServerDetails(int id);

	int unique_id;
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
	std::vector<IRCChanDetails> chans;

	bool autoconnect;

	IRCConnectionStatus status;
};


// holds information about a current connection to a server.
// the ID will match one of the IRCServerDetails IDs

struct IRCConnectionInfo {
	int id;
	IRCConnection* conn;
	QString current_nick;
	std::vector<IRCPrefix> prefixes;
};


// holds the list of all such current connections.

struct IRCConnectionRegistry {
	IRCConnectionRegistry();

	bool createConnection(int id);
	bool destroyConnection(int id);

	IRCConnectionInfo* getInfo(int id);

private:
	QThread* irc_thread;
	std::vector<IRCConnectionInfo> connections;
};


// global context struct for easy access of stuff.

struct IRCCtx {
	IRCSettings*           settings;
	IRCMessageHandler*     msg_handler;
	IRCExternalDownloader* downloader;
	IRCMainUI*             ui_main;
	IRCAddServerUI*        ui_addserv;
	IRCBufferModel*        buffers;
	IRCConnectionRegistry* connections;
};

extern IRCCtx* SamuraIRC;

#endif
