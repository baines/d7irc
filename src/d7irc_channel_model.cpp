#include "d7irc_qt.h"


int IRCChannelModel::rowCount(const QModelIndex& parent) const {
	return chans ? chans->size() : 0;
}

int IRCChannelModel::columnCount(const QModelIndex& parent) const {
	return 2;
}

QVariant IRCChannelModel::data(const QModelIndex& idx, int role) const {
	if(!chans) return QVariant();
	auto& c = *chans;

	if(role == Qt::DisplayRole){
		if(idx.column() == 0){
			return c[idx.row()].name;
		} else {
			return c[idx.row()].pass.isEmpty() ? QVariant() : QString("***");
		}
	} else if(role == Qt::EditRole){
		if(idx.column() == 0){
			return c[idx.row()].name;
		} else {
			return c[idx.row()].pass;
		}
	} else {
		return QVariant();
	}
}

QVariant IRCChannelModel::headerData(int col, Qt::Orientation, int role) const {
	if(role != Qt::DisplayRole) return QVariant();

	if(col == 0){
		return QString("Name");
	} else {
		return QString("Password");
	}
}

bool IRCChannelModel::setData(const QModelIndex& idx, const QVariant& val, int role) {
	if(!chans || role != Qt::EditRole || !val.canConvert(QMetaType::QString)) return false;
	auto& c = *chans;

	if(idx.column() == 0){
		c[idx.row()].name = val.toString();
		// TODO: sort when this changes?
	} else {
		c[idx.row()].pass = val.toString();
	}

	emit dataChanged(idx, idx);

	return true;
}

Qt::ItemFlags IRCChannelModel::flags(const QModelIndex& i) const {
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void IRCChannelModel::setChannels(std::vector<IRCChanDetails>* chans){
	beginResetModel();
	this->chans = chans;
	endResetModel();
}

void IRCChannelModel::push(void){
	if(!chans) return;
	beginInsertRows(QModelIndex(), chans->size(), chans->size());
	chans->push_back(IRCChanDetails{});
	endInsertRows();
}

void IRCChannelModel::pop(void){
	if(!chans) return;
	beginRemoveRows(QModelIndex(), chans->size()-1, chans->size()-1);
	chans->pop_back();
	endRemoveRows();
}

void IRCChannelModel::remove(int row){
	if(!chans) return;
	beginRemoveRows(QModelIndex(), row, row);
	chans->erase(chans->begin() + row);
	endRemoveRows();
}
