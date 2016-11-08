#ifndef D7IRC_DATA_H
#define D7IRC_DATA_H
#include <qtextdocument.h>
#include <qstring.h>
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

#endif
