#include "d7irc_data.h"
#include <qtreeview.h>
#include <cassert>

struct {
	QColor color;
	const char* line;
} logo[] = {
	{ { 0x55, 0x7f, 0xff }, R"xx( .::::::.  :::.    .        :   ...    :::::::::..   :::.    :::::::::..    .,-:::::  )xx" },
	{ { 0x7f, 0x7f, 0xff }, R"xx(;;;`    `  ;;`;;   ;;,.    ;;;  ;;     ;;;;;;``;;;;  ;;`;;   ;;;;;;``;;;; ,;;;'````'  )xx" },
	{ { 0xaa, 0x55, 0xff }, R"xx('[==/[[[[,,[[ '[[, [[[[, ,[[[[,[['     [[[[[[,/[[[' ,[[ '[[, [[[[[[,/[[[' [[[         )xx" },
	{ { 0xaa, 0x55, 0x7f }, R"xx(  '''    c$$$cc$$$c$$$$$$$$"$$$$$      $$$$$$$$$c  c$$$cc$$$c$$$$$$$$$c   $$$         )xx" },
	{ { 0xff, 0x55, 0x00 }, R"xx( 88b    dP888   888888 Y88" 88888    .d888888b "88bo888   888888888b "88bo`88bo,__,o, )xx" },
	{ { 0xff, 0x00, 0x00 }, R"xx(  "YMmMY" YMM   ""`MMM  M'  "MMM"YmmMMMM""MMMM   "W"YMM   ""`MMMMMMM   "W"  "YUMMMMMP")xx" },
};

IRCBufferModel::IRCBufferModel(QTextEdit* edit, QTreeView* view)
: root(new IRCBuffer(IRC_BUF_INTERNAL, "root", nullptr)){
	
	beginInsertRows(QModelIndex(), 0, 0);
	root->child = new IRCBuffer(IRC_BUF_INTERNAL, "SamuraIRC", root);
	endInsertRows();

	QTextCursor c(root->child->contents);
	QTextCharFormat f;

	c.beginEditBlock();
	for(auto& line : logo){
		f.setForeground(line.color);
		c.setCharFormat(f);
		c.insertText(line.line);
		c.insertText("\n");
	}
	c.setCharFormat(QTextCharFormat());
	c.insertText(" v0.0.0 by Alex Baines");
	c.endEditBlock();

	edit->setDocument(root->child->contents);

	view->setModel(this);
	view->setCurrentIndex(createIndex(0, 0, root->child));
}

QModelIndex IRCBufferModel::index(int row, int col, const QModelIndex& parent) const {

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

QModelIndex IRCBufferModel::parent(const QModelIndex& idx) const {

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

int IRCBufferModel::rowCount(const QModelIndex& parent) const {
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

int IRCBufferModel::columnCount(const QModelIndex& parent) const {
	return 1;
}

static QVariant chan_colors[] = {
	QVariant(),
	QVariant(QColor(0xF5, 0xFF, 0x3C)),
	QVariant(QColor(0xF2, 0x3C, 0xFF)),
};

QVariant IRCBufferModel::data(const QModelIndex& idx, int role) const {

	if(!idx.isValid()){
		return QVariant();
	}

	const IRCBuffer* p = reinterpret_cast<IRCBuffer*>(idx.internalPointer());

	switch(role){

		case Qt::DisplayRole: {
			if(p->inactive){
				return QVariant(QString("(%1)").arg(p->name));
			} else {
				return QVariant(p->name);
			}
		} break;

		case Qt::SizeHintRole:
			return QVariant(QSize(-1, 18));
		case Qt::ForegroundRole: {
			// TODO: color palette
			if(p->type == IRC_BUF_INTERNAL){
				return QVariant(QBrush(QColor(150, 100, 255)));
			} else if(p->type == IRC_BUF_SERVER){
				return QVariant(QBrush(QColor(100, 255, 100)));
			} else {
				return chan_colors[p->level];
			}
		}
		default:
			return QVariant();
	} 
}

Qt::ItemFlags IRCBufferModel::flags(const QModelIndex& idx) const {
	if(!idx.isValid()){
		return 0;
	}

	return QAbstractItemModel::flags(idx);
}

IRCServerBuffer* IRCBufferModel::addServer(const QString& name){

	int row = 1;
	IRCBuffer** p = &root->child->sibling;
	while(*p && QString::localeAwareCompare(name, (*p)->name) > 0){
		p = &(*p)->sibling;
		++row;
	}

	if(*p && (*p)->name == name){
		assert((*p)->type == IRC_BUF_SERVER);
		return reinterpret_cast<IRCServerBuffer*>(*p);
	}

	IRCServerBuffer* buf = new IRCServerBuffer(name, root);

	beginInsertRows(QModelIndex(), row, row);
	buf->sibling = *p;
	*p = buf;
	endInsertRows();

	emit serverAdded(createIndex(row, 0, buf));

	return buf;
}

IRCBuffer* IRCBufferModel::addChannel(const QString& serv, const QString& chan){
	
	IRCBuffer* s = addServer(serv);
	IRCBuffer** p = &s->child;

	int row = 0;
	while(*p && QString::localeAwareCompare(chan, (*p)->name) > 0){
		p = &(*p)->sibling;
		++row;
	}

	if(*p && (*p)->name == chan){
		assert((*p)->type == IRC_BUF_CHANNEL);
		return *p;
	}

	IRCBuffer* buf = new IRCBuffer(IRC_BUF_CHANNEL, chan, s);

	QModelIndex parent_idx;
	{
		int i = 0;
		for(IRCBuffer* pp = root->child; pp != s; pp = pp->sibling) ++i;
		parent_idx = createIndex(i, 0, s);
	}

	beginInsertRows(parent_idx, row, row);
	buf->sibling = *p;
	*p = buf;
	endInsertRows();

	return buf;
}

IRCBuffer* IRCBufferModel::getDefault(){
	return root->child;
}
