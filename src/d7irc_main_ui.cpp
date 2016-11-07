#include "d7irc_data.h"
#include "d7irc_ui.h"
#include <qscrollbar.h>
#include <qshortcut.h>
#include <qdialog.h>

IRCMainUI::IRCMainUI(IRCAddServerUI* add_serv)
: ui          (new Ui::SamuraIRC)
, add_serv    (add_serv)
, buffers     (nullptr)
, buffer_menu (nullptr)
, nicks_menu  (nullptr)
, max_scroll  (true) {

	ui->setupUi(this);
	ui->chat_lines->setCursorWidth(0);
	ui->text_input->setFocus();

	buffers     = new IRCBufferModel(ui->chat_lines, ui->buffer_list);
	buffer_menu = new QMenu(ui->buffer_list);
	nicks_menu  = new QMenu(ui->nick_list);

	connect(ui->buffer_list->selectionModel(), &QItemSelectionModel::currentChanged, this, &IRCMainUI::bufferChange);

	connect(buffers, &IRCBufferModel::serverAdded, ui->buffer_list, &QTreeView::expand);

	// alt+up/down for buffer list
	QShortcut* next_buf = new QShortcut(QKeySequence(Qt::Key_Down | Qt::AltModifier), this);
	QShortcut* prev_buf = new QShortcut(QKeySequence(Qt::Key_Up   | Qt::AltModifier), this);

	auto* list = ui->buffer_list;
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

	// right-click menus

	// TODO: buffer specific options if selected - part, close, etc

	// TODO: this should have a Connect submenu listing all the servers from IRCSettings
	// with the final option being to add a new server.
	buffer_menu->addAction("Add Server");
	connect(ui->buffer_list, &QWidget::customContextMenuRequested, [this](const QPoint& p){
		buffer_menu->popup(ui->buffer_list->mapToGlobal(p));
	});

	connect(buffer_menu, &QMenu::triggered, [this](QAction* act){
		this->add_serv->open();
	});

}

void IRCMainUI::bufferChange(const QModelIndex& idx, const QModelIndex&){
	
	IRCBuffer* buf = reinterpret_cast<IRCBuffer*>(idx.internalPointer());
	ui->chat_lines->setDocument(buf->contents);

	QScrollBar* s = ui->chat_lines->verticalScrollBar();
	s->setValue(s->maximum());

	ui->nick_list->setModel(&buf->nicks);
}
