#include <qobject.h>
#include <qapplication.h>
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

	Ui::SamuraIRC ui;
	ui.setupUi(win);
	
	QThread* irc_thread = new QThread;
	IRCWorker* worker = new IRCWorker(irc_thread);
	QObject::connect(irc_thread, &QThread::started, worker, &IRCWorker::begin);

	IRCGUILogic           ui_logic(&ui);
	IRCBufferModel        buffers(ui.chat_lines, ui.serv_list);
	IRCExternalDownloader downloader;
	IRCMessageHandler     handler(&buffers, &ui, &downloader);

	QObject::connect(worker, &IRCWorker::connect, &handler, &IRCMessageHandler::handleIRCConnect, Qt::QueuedConnection);
	QObject::connect(worker, &IRCWorker::join   , &handler, &IRCMessageHandler::handleIRCJoin   , Qt::QueuedConnection);
	QObject::connect(worker, &IRCWorker::part   , &handler, &IRCMessageHandler::handleIRCPart   , Qt::QueuedConnection);
	QObject::connect(worker, &IRCWorker::quit   , &handler, &IRCMessageHandler::handleIRCQuit   , Qt::QueuedConnection);
	QObject::connect(worker, &IRCWorker::privmsg, &handler, &IRCMessageHandler::handleIRCPrivMsg, Qt::QueuedConnection);
	QObject::connect(worker, &IRCWorker::numeric, &handler, &IRCMessageHandler::handleIRCNumeric, Qt::QueuedConnection);

	QObject::connect(ui.text_input, &IRCTextEntry::textSubmit, &handler, &IRCMessageHandler::sendIRCMessage);

	//XXX
	QObject::connect(&handler, &IRCMessageHandler::tempSend, worker, &IRCWorker::sendPrivmsg, Qt::QueuedConnection);

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
