#ifndef D7IRC_DATA_H
#define D7IRC_DATA_H
#include <qstandarditemmodel.h>
#include <qtextdocument.h>
#include <qtextcursor.h>
#include <qstring.h>
#include <qthread.h>
#include <vector>

// defines / macros

#define DEFAULT_PREFIX_IDX 0xFFFF

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

struct IRCPrefix {
	char prefix;
	char mode;
};

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

// TODO: put this stuff in IRCServerDetails instead probably
struct IRCServerBuffer : public IRCBuffer {
	IRCServerBuffer(const QString& name, IRCBuffer* parent);

	int id;
	QString our_nick;
	std::vector<IRCPrefix> prefixes;
	// modes?
};

struct IRCServerDetails {

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
	QStandardItemModel chans;

	bool autoconnect;

	IRCConnectionStatus status;
};

struct IRCConnectionRegistry {
	IRCConnectionRegistry();

	bool createConnection(int id);
	bool destroyConnection(int id);

private:
	QThread* irc_thread;
	std::vector<IRCConnection*> connections;
};

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
