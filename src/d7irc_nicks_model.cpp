#include "d7irc_qt.h"

// TODO palette
static QVariant color_hash(const QString& str){
	QColor c;
	return QVariant(c);
}

IRCNicksModel::IRCNicksModel() {

}

int IRCNicksModel::rowCount(const QModelIndex& parent) const {
	return nicks.size();
}

QVariant IRCNicksModel::data(const QModelIndex& idx, int role) const {
	if(idx.row() < 0 || idx.row() >= nicks.size()){
		return QVariant();
	}

	const QString& nick = nicks[idx.row()];

	switch(role){
		case Qt::DisplayRole:
			return QVariant(nick);
		case Qt::ForegroundRole:
			return color_hash(nick);
		default:
			return QVariant();
	}
}

void IRCNicksModel::addNick(const QString& nick) {

}

void IRCNicksModel::delNick(const QString& nick) {

}

void IRCNicksModel::changeNick(const QString& from, const QString& to) {

}
