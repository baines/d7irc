#ifndef D7IRC_DATA_H
#define D7IRC_DATA_H
#include <qtextdocument.h>
#include <qstring.h>
#include <qsettings.h>
#include <vector>
#include "d7irc_qt.h"

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

enum IRCOption : int {
	IRC_OPT_HIDE_JOINPART,
	IRC_OPT_IMG_EXPAND,
	IRC_OPT_IMG_EXPAND_ALL,
	IRC_OPT_CODE_EXPAND,
};

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
	IRCUserModel nicks;

	bool inactive;
	IRCBufferLevel level;

	IRCBuffer* parent;
	IRCBuffer* child;
	IRCBuffer* sibling;
};

struct IRCServerBuffer : public IRCBuffer {
	IRCServerBuffer(const QString& name, IRCBuffer* parent);

	QString our_nick;
	std::vector<IRCPrefix> prefixes;
	// modes?
};

struct IRCServerDetails {
	QString unique_name;

	QString hostname;
	QString nickname;
	QString username;
	QString password;
	uint16_t port;
	bool ssl;

	// will be of form chan:passwd for passworded channels
	QStringList channels;
};

struct IRCSettings {
	IRCSettings();

	int               serverNameToID (const QString& name);
	IRCServerDetails* getDetails     (int id);

	int  getOption (IRCOption opt);
	void setOption (IRCOption opt, int val);

	bool first_run;

private:
	std::vector<IRCServerDetails> servers;
	QSettings settings;
};

#endif
