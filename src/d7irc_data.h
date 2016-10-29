#ifndef D7IRC_DATA_H
#define D7IRC_DATA_H
#include <qtextdocument.h>
#include <qstring.h>
#include <vector>
#include "d7irc_qt.h"

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

struct IRCBuffer {

	IRCBuffer(IRCBufferType type, const QString& name, IRCBuffer* parent);

	void addLine(const QString& prefix, const QString& msg);

	IRCBufferType type;
	QString name;
	QTextDocument* contents;
	IRCUserModel nicks;

	IRCBuffer* parent;
	IRCBuffer* child;
	IRCBuffer* sibling;

	QTextCursor cursor;
};

#endif
