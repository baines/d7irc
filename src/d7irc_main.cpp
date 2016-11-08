#include <qobject.h>
#include <qapplication.h>
#include "d7irc_data.h"
#include "d7irc_ui.h"

IRCSettings* d7irc_settings;

int main(int argc, char** argv){
	QApplication app(argc, argv);
	
	d7irc_settings = new IRCSettings;

	IRCMainUI             win;
	IRCExternalDownloader downloader;
	IRCMessageHandler     handler(&win, &downloader);

	if(d7irc_settings->first_run){
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
	int ret =  app.exec();

	delete d7irc_settings;

	return ret;
}
