#include <qobject.h>
#include <qapplication.h>
#include "d7irc_data.h"
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
	
	IRCAddServerUI        add_serv;
	IRCMainUI             win(&add_serv);
	IRCSettings           settings;
	IRCExternalDownloader downloader;
	IRCMessageHandler     handler(&win, &downloader);

	if(settings.first_run){
		puts("first run, show add server dialogue when it exists...");
	}

	// TODO: move into MainUI constructor?
	QObject::connect(win.ui->text_input, &IRCTextEntry::textSubmit, &handler, &IRCMessageHandler::sendIRCMessage);
	QObject::connect(win.ui->text_input, &IRCTextEntry::command   , &handler, &IRCMessageHandler::sendIRCCommand);
	
	// TODO: create connections on demand
	QThread*       irc_thread = new QThread;
	IRCConnection* connection = new IRCConnection(irc_thread, &handler);

	//XXX temp: need server param
	QObject::connect(&handler, &IRCMessageHandler::tempSend   , connection, &IRCConnection::sendPrivmsg, Qt::QueuedConnection);
	QObject::connect(&handler, &IRCMessageHandler::tempSendRaw, connection, &IRCConnection::sendRaw    , Qt::QueuedConnection);

	irc_thread->start();

	win.show();
	return app.exec();
}
