#include "d7irc_qt.h"

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
		sum += c;
	}

	return palette[sum % countof(palette)];
}

IRCUserModel::IRCUserModel() {

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

	const QString& nick = nicks[idx.row()];

	if(idx.column() == 0){
		switch(role){
			case Qt::DisplayRole: {
				return QVariant(QString("@"));
			}
			case Qt::ForegroundRole:
				return QVariant(QColor(0, 200, 0));
			case Qt::SizeHintRole:
				return QVariant(QSize(20, 20));
			default:
				return QVariant();
		}
	} else {
		switch(role){
			case Qt::DisplayRole: {
				return QVariant(nick);
			}
			case Qt::ForegroundRole:
				return QVariant(nickColor(nick));
			case Qt::SizeHintRole:
				return QVariant(QSize(-1, 20));
			default:
				return QVariant();
		}
	}
}

// TODO: we need mode information / operator etc
//       and maybe more for context menu actions.

void IRCUserModel::add(const QString& nick) {

	for(auto& n : nicks){
		if(n == nick){
			return;
		}
	}

	char real_nick[1024];
	irc_target_get_nick(nick.toUtf8().constData(), real_nick, sizeof(real_nick));

	beginInsertRows(QModelIndex(), nicks.size(), 0);
	nicks.push_back(real_nick);
	endInsertRows();
}

void IRCUserModel::del(const QString& nick) {

	char real_nick[1024];
	irc_target_get_nick(nick.toUtf8().constData(), real_nick, sizeof(real_nick));

	for(int i = 0; i < nicks.size(); ++i){
		if(nicks[i].compare(nick) == 0){
			beginRemoveRows(QModelIndex(), i, 0);
			nicks.erase(nicks.begin() + i);
			endRemoveRows();
			return;
		}
	}
}

bool IRCUserModelSorter::lessThan(const QModelIndex& a, const QModelIndex& b) const {
	// TODO;
	return true;
}
