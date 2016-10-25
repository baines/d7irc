#include <qobject.h>
#include <qapplication.h>
#include "d7irc_qt.h"
#include "d7irc_ui.h"

int main(int argc, char** argv){
	QApplication app(argc, argv);
	QMainWindow* win = new QMainWindow;

	Ui::SamuraIRC ui;
	ui.setupUi(win);

	QThread* irc_thread = new QThread;
	IRCWorker* worker = new IRCWorker(irc_thread);

	QObject::connect(worker, &IRCWorker::privmsg, ui.chat_lines, &QTextEdit::append);
	QObject::connect(irc_thread, &QThread::started, worker, &IRCWorker::begin);

	IRCModel model;

	QObject::connect(worker, &IRCWorker::connect, &model, &IRCModel::addServer, Qt::QueuedConnection);
	QObject::connect(worker, &IRCWorker::join, &model, &IRCModel::addChannel, Qt::QueuedConnection);

	QObject::connect(&model, &IRCModel::serverAdded, ui.serv_list, &QTreeView::expand, Qt::QueuedConnection);

	ui.serv_list->header()->hide();
	ui.serv_list->setModel(&model);

	// TODO: connect worker join to serv_list select

	irc_thread->start();
	win->show();
	return app.exec();
}
