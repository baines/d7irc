#include "d7irc_qt.h"

IRCModel::IRCModel()
: root(new IRCBuffer()){
	root->type  = IRC_BUF_INTERNAL;

	beginInsertRows(QModelIndex(), 0, 0);
	
	root->child = new IRCBuffer();
	root->child->type = IRC_BUF_INTERNAL;
	root->child->name = "d7irc";
	root->child->parent = root;

	endInsertRows();
}

QModelIndex IRCModel::index(int row, int col, const QModelIndex& parent) const {

	if(!hasIndex(row, col, parent)){
		return QModelIndex();
	}

	IRCBuffer* pbuf;
	if(!parent.isValid()){
		pbuf = root;
	} else {
		pbuf = reinterpret_cast<IRCBuffer*>(parent.internalPointer());
	}

	IRCBuffer* buf = pbuf->child;
	for(int i = 0; i < row; ++i){
		buf = buf->sibling;
	}

	if(buf){
		return createIndex(row, col, buf);
	} else {
		return QModelIndex();
	}
}

QModelIndex IRCModel::parent(const QModelIndex& idx) const {

	if(!idx.isValid()){
		return QModelIndex();
	}

	IRCBuffer* buf  = reinterpret_cast<IRCBuffer*>(idx.internalPointer());
	IRCBuffer* pbuf = buf->parent;

	if(pbuf == root){
		return QModelIndex();
	}

	int row = 0;
	pbuf = pbuf->child;
	while(pbuf != buf){
		++row;
		pbuf = pbuf->sibling;
	}

	return createIndex(row, 0, buf->parent);
}

int IRCModel::rowCount(const QModelIndex& parent) const {
	IRCBuffer* p;

	if(!parent.isValid()){
		p = root;
	} else {
		p = reinterpret_cast<IRCBuffer*>(parent.internalPointer());
	}

	int count = 0;
	p = p->child;
	while(p){
		++count;
		p = p->sibling;
	}

	return count;
}

int IRCModel::columnCount(const QModelIndex& parent) const {
	return 1;
}

QVariant IRCModel::data(const QModelIndex& idx, int role) const {

	if(!idx.isValid()){
		return QVariant();
	}

	if(role != Qt::DisplayRole){
		return QVariant();
	}

	const IRCBuffer* p = reinterpret_cast<IRCBuffer*>(idx.internalPointer());
	return QVariant(p->name);
}

Qt::ItemFlags IRCModel::flags(const QModelIndex& idx) const {
	if(!idx.isValid()){
		return 0;
	}

	return QAbstractItemModel::flags(idx);
}

IRCBuffer* IRCModel::addServer(const QString& name){

	IRCBuffer* p = root->child;
	while(p){
		if(p->name == name) return p;
		p = p->sibling;
	}

	IRCBuffer* buf = new IRCBuffer();

	buf->type = IRC_BUF_SERVER;
	buf->name = name;
//	buf->contents
//	buf->nicks

	buf->parent = root;

	int row = 1;

	if(!root->child){
		root->child = buf;
		row = 0;
	} else {
		p = root->child;
		while(p->sibling){
			p = p->sibling;
			++row;
		}
		p->sibling = buf;
	}

	beginInsertRows(QModelIndex(), row, row);
	endInsertRows();

	emit serverAdded(createIndex(row, 0, buf));

	return buf;
}

IRCBuffer* IRCModel::addChannel(const QString& serv, const QString& chan){
	
	IRCBuffer* s = addServer(serv);
	IRCBuffer* p = s->child;

	while(p){
		if(p->name == chan) return p;
		p = p->sibling;
	}

	IRCBuffer* buf = new IRCBuffer();

	buf->type = IRC_BUF_CHANNEL;
	buf->name = chan;
	buf->parent = s;

	int row = 1;

	if(!s->child){
		s->child = buf;
		row = 0;
	} else {
		p = s->child;
		while(p->sibling){
			++row;
			p = p->sibling;
		}
		p->sibling = buf;
	}

	QModelIndex parent_idx;
	{
		int pr = 0;
		IRCBuffer* tmp = root->child;
		while(tmp != s){
			++pr;
			tmp = tmp->sibling;
		}
		parent_idx = createIndex(pr, 0, s);
	}

	beginInsertRows(parent_idx, row, row);
	endInsertRows();

	return buf;
}
