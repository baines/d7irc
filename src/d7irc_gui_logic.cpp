#include "d7irc_data.h"
#include "d7irc_ui.h"
#include <qscrollbar.h>

IRCGUILogic::IRCGUILogic(Ui::SamuraIRC* ui)
: ui(ui) {

}

void IRCGUILogic::bufferChange(const QModelIndex& idx, const QModelIndex&){
	IRCBuffer* buf = reinterpret_cast<IRCBuffer*>(idx.internalPointer());

	ui->chat_lines->setDocument(buf->contents);

	QScrollBar* s = ui->chat_lines->verticalScrollBar();
	s->setValue(s->maximum());

	ui->nick_list->setModel(&buf->nicks);
}
