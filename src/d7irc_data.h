#ifndef D7IRC_DATA_H
#define D7IRC_DATA_H
#include <qtextdocument.h>
#include <qstring.h>
#include <vector>

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
	IRCBufferType type;
	QString name;
	QTextDocument* contents;
//	std::vector<IRCUser> nicks;

	IRCBuffer* parent;
	IRCBuffer* child;
	IRCBuffer* sibling;

};

#endif
