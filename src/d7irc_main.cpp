#include <qobject.h>
#include <qapplication.h>
#include "d7irc_qt.h"
#include "d7irc_ui.h"

int main(int argc, char** argv){
	QApplication app(argc, argv);
	QMainWindow* win = new QMainWindow;

	Ui::SamuraIRC ui;
	ui.setupUi(win);

	IRCGUILogic ui_logic(&ui);

	QThread* irc_thread = new QThread;
	IRCWorker* worker = new IRCWorker(irc_thread);

	QObject::connect(worker, &IRCWorker::privmsg, ui.chat_lines, &QTextEdit::append);
	QObject::connect(irc_thread, &QThread::started, worker, &IRCWorker::begin);

	IRCBufferModel buffers(ui.chat_lines, ui.serv_list);

	QObject::connect(worker, &IRCWorker::connect, &buffers, &IRCBufferModel::addServer, Qt::QueuedConnection);
	QObject::connect(worker, &IRCWorker::join, &buffers, &IRCBufferModel::addChannel, Qt::QueuedConnection);

	QObject::connect(&buffers, &IRCBufferModel::serverAdded, ui.serv_list, &QTreeView::expand, Qt::QueuedConnection);

	// TODO: connect worker join to serv_list select

	QObject::connect(
		ui.serv_list->selectionModel(), &QItemSelectionModel::currentChanged,
		&ui_logic, &IRCGUILogic::bufferChange
	);

	irc_thread->start();
	win->show();
	return app.exec();
}
