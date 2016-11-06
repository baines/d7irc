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

enum IRCUserFlags : unsigned {
	IRC_USR_OP    = (1 << 0),
	IRC_USR_HOP   = (1 << 1),
	IRC_USR_VOICE = (1 << 2),
};

struct IRCUser {
	std::string name;
	IRCUserFlags flags;
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

struct IRCSettings {
	QSettings qsettings;
	// list of configured servers
	//    hostname, port, wanted nick, username, pass, etc
	//    qtkeychain for pass?
	//    ID for each server
};

#endif
