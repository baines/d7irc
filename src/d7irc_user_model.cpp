#include "d7irc_qt.h"
#include "d7irc_data.h"
#include <cassert>

#define countof(x) (sizeof((x))/sizeof(*(x)))

static const char* palette[] = {
	"#06989A",
	"#8A609D",
	"#409A06",
	"#C4A000",
	"#729FCF",
	"#FFFFFF",
	"#2DE2E2",
	"#A17FA8",
	"#6DE234",
	"#427FED",
	"#00D700",
	"#06D75F",
	"#5FD7D7",
	"#878700",
	"#87AFAF",
	"#87D700",
	"#87D7AF",
	"#87D7FF",
	"#87FF87",
	"#87FFD7",
	"#AF5F00",
	"#AF5FAF",
	"#AF5FFF",
	"#AF87D7",
	"#AFAF00",
	"#AFAFAF",
	"#AFAFFF",
	"#AFD787",
	"#AFFF5F",
	"#D75FAF",
	"#D75FD7",
	"#D787AF",
	"#D787FF",
	"#DFAF5F",
};

QColor IRCUserModel::nickColor(const QString& str){
	size_t sum = 0;
	std::u32string ustr = str.toStdU32String();
	for(char32_t c : ustr){
		sum += (int)c;
	}

	return palette[sum % countof(palette)];
}

IRCUserModel::IRCUserModel(IRCBuffer* server)
: server(server) {

}

int IRCUserModel::rowCount(const QModelIndex& parent) const {
	return nicks.size();
}

int IRCUserModel::columnCount(const QModelIndex& parent) const {
	return 2;
}

QVariant IRCUserModel::data(const QModelIndex& idx, int role) const {
	if(idx.row() < 0 || idx.row() >= nicks.size()){
		return QVariant();
	}

	const User& nick = nicks[idx.row()];

	if(idx.column() == 0){
		switch(role){
			case Qt::DisplayRole: {
				if(nick.prefix_idx == DEFAULT_PREFIX_IDX) return QVariant();
				
				assert(server->type == IRC_BUF_SERVER);
				IRCServerBuffer* buf = reinterpret_cast<IRCServerBuffer*>(server);

				return QVariant(QString(buf->prefixes[nick.prefix_idx].prefix));
			} break;
			case Qt::ForegroundRole:
				return QVariant(QColor(0, 200, 0));
			case Qt::SizeHintRole:
				return QVariant(QSize(20, 20));
			default:
				return QVariant();
		}
	} else {
		switch(role){
			case Qt::DisplayRole:
				return QVariant(nick.nick);
			case Qt::ForegroundRole:
				return QVariant(nickColor(nick.nick));
			case Qt::SizeHintRole:
				return QVariant(QSize(-1, 20));
			default:
				return QVariant();
		}
	}
}

// TODO: multiple prefix support

void IRCUserModel::add(const QString& nick, int prefix_idx) {

	User new_usr = { nick, prefix_idx };

	auto it = std::lower_bound(
		nicks.begin(),
		nicks.end(),
		new_usr,
		[](const User& a, const User& b){
			if(a.prefix_idx != b.prefix_idx){
				return a.prefix_idx < b.prefix_idx;
			} else {
				return QString::localeAwareCompare(a.nick, b.nick) < 0;
			}
		}
	);

	if(it != nicks.end() && it->nick == nick){
		it->prefix_idx = prefix_idx;
		return;
	}

	char real_nick[1024];
	irc_target_get_nick(nick.toUtf8().constData(), real_nick, sizeof(real_nick));

	beginInsertRows(QModelIndex(), it - nicks.begin(), it - nicks.begin());
	nicks.insert(it, User { real_nick, prefix_idx });
	endInsertRows();
}

bool IRCUserModel::del(const QString& nick) {
	char real_nick[1024];
	irc_target_get_nick(nick.toUtf8().constData(), real_nick, sizeof(real_nick));

	for(int i = 0; i < nicks.size(); ++i){
		if(nicks[i].nick.compare(real_nick) == 0){
			beginRemoveRows(QModelIndex(), i, i);
			nicks.erase(nicks.begin() + i);
			endRemoveRows();
			return true;
		}
	}

	return false;
}
