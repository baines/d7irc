#include "d7irc_data.h"
#include "d7irc_ui.h"
#include <qscrollbar.h>
#include <qshortcut.h>

IRCGUILogic::IRCGUILogic(Ui::SamuraIRC* ui, QMainWindow* win)
: ui(ui) {

	connect(ui->serv_list->selectionModel(), &QItemSelectionModel::currentChanged, this, &IRCGUILogic::bufferChange);

	// alt+up/down for buffer list
	QShortcut* next_buf = new QShortcut(QKeySequence(Qt::Key_Down | Qt::AltModifier), win);
	QShortcut* prev_buf = new QShortcut(QKeySequence(Qt::Key_Up   | Qt::AltModifier), win);

	auto* list = ui->serv_list;
	connect(next_buf, &QShortcut::activated, [=](){
		QKeyEvent* e1 = new QKeyEvent(QEvent::KeyPress  , Qt::Key_Down, Qt::NoModifier);
		QKeyEvent* e2 = new QKeyEvent(QEvent::KeyRelease, Qt::Key_Down, Qt::NoModifier);
		qApp->postEvent(list, e1);
		qApp->postEvent(list, e2);
	});
	connect(prev_buf, &QShortcut::activated, [=](){
		QKeyEvent* e1 = new QKeyEvent(QEvent::KeyPress  , Qt::Key_Up, Qt::NoModifier);
		QKeyEvent* e2 = new QKeyEvent(QEvent::KeyRelease, Qt::Key_Up, Qt::NoModifier);
		qApp->postEvent(list, e1);
		qApp->postEvent(list, e2);
	});

	// chat scrollbar tracking

	QScrollBar* sbar =  ui->chat_lines->verticalScrollBar();
	
	connect(sbar, &QScrollBar::valueChanged, [sbar,this](){
		max_scroll = sbar->value() == sbar->maximum();
	});
	connect(ui->chat_lines, &QTextEdit::textChanged, [sbar,this](){
		if(max_scroll){
			sbar->setValue(sbar->maximum());
		}
	});
}

void IRCGUILogic::bufferChange(const QModelIndex& idx, const QModelIndex&){
	
	IRCBuffer* buf = reinterpret_cast<IRCBuffer*>(idx.internalPointer());
	ui->chat_lines->setDocument(buf->contents);

	QScrollBar* s = ui->chat_lines->verticalScrollBar();
	s->setValue(s->maximum());

	ui->nick_list->setModel(&buf->nicks);
}
