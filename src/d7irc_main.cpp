#include <qobject.h>
#include <qapplication.h>
#include <qshortcut.h>
#include "d7irc_qt.h"
#include "d7irc_ui.h"

#include <QDebug> 
#include <QMetaProperty>

#include <vector>
#include <utility>
#include <algorithm>

void debug(QObject* o){
  auto mo = o->metaObject();
  qDebug() << "## Properties of" << o << "##";
  do {
    qDebug() << "### Class" << mo->className() << "###";
    std::vector<std::pair<QString, QVariant> > v;
    v.reserve(mo->propertyCount() - mo->propertyOffset());
    for (int i = mo->propertyOffset(); i < mo->propertyCount();
          ++i)
      v.emplace_back(mo->property(i).name(),
                     mo->property(i).read(o));
    std::sort(v.begin(), v.end());
    for (auto &i : v)
      qDebug() << i.first << "=>" << i.second;
  } while ((mo = mo->superClass()));
}

int main(int argc, char** argv){
	QApplication app(argc, argv);
	QMainWindow* win = new QMainWindow;

	app.setWindowIcon(QIcon(":/main/icon.png"));

	Ui::SamuraIRC ui;
	ui.setupUi(win);
	ui.chat_lines->setCursorWidth(0);
	ui.text_input->setFocus();

	IRCGUILogic           ui_logic(&ui);
	IRCBufferModel        buffers(ui.chat_lines, ui.serv_list);
	IRCExternalDownloader downloader;
	IRCMessageHandler     handler(&buffers, &ui, &downloader);

	QObject::connect(ui.text_input, &IRCTextEntry::textSubmit, &handler, &IRCMessageHandler::sendIRCMessage);
	QObject::connect(ui.text_input, &IRCTextEntry::command   , &handler, &IRCMessageHandler::sendIRCCommand);
	
	QObject::connect(&buffers, &IRCBufferModel::serverAdded, ui.serv_list, &QTreeView::expand);

	QObject::connect(
		ui.serv_list->selectionModel(), &QItemSelectionModel::currentChanged,
		&ui_logic, &IRCGUILogic::bufferChange
	);

	QShortcut next_buf(QKeySequence(Qt::Key_Down | Qt::AltModifier), win);
	QShortcut prev_buf(QKeySequence(Qt::Key_Up   | Qt::AltModifier), win);

	auto* list = ui.serv_list;
	QObject::connect(&next_buf, &QShortcut::activated, [=](){
		QKeyEvent* e1 = new QKeyEvent(QEvent::KeyPress  , Qt::Key_Down, Qt::NoModifier);
		QKeyEvent* e2 = new QKeyEvent(QEvent::KeyRelease, Qt::Key_Down, Qt::NoModifier);
		qApp->postEvent(list, e1);
		qApp->postEvent(list, e2);
	});
	QObject::connect(&prev_buf, &QShortcut::activated, [=](){
		QKeyEvent* e1 = new QKeyEvent(QEvent::KeyPress  , Qt::Key_Up, Qt::NoModifier);
		QKeyEvent* e2 = new QKeyEvent(QEvent::KeyRelease, Qt::Key_Up, Qt::NoModifier);
		qApp->postEvent(list, e1);
		qApp->postEvent(list, e2);
	});

	// TODO: create connections in ui_logic on demand
	QThread*       irc_thread = new QThread;
	IRCConnection* connection = new IRCConnection(irc_thread, &handler);

	//XXX temp: need server param
	QObject::connect(&handler, &IRCMessageHandler::tempSend   , connection, &IRCConnection::sendPrivmsg, Qt::QueuedConnection);
	QObject::connect(&handler, &IRCMessageHandler::tempSendRaw, connection, &IRCConnection::sendRaw    , Qt::QueuedConnection);

	irc_thread->start();

	win->show();
	return app.exec();
}
