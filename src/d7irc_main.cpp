#include <qobject.h>
#include <qapplication.h>
#include "d7irc_data.h"
#include "d7irc_ui.h"

IRCCtx* SamuraIRC;

int main(int argc, char** argv){
	QApplication app(argc, argv);
	
	SamuraIRC              = new IRCCtx;
	SamuraIRC->settings    = new IRCSettings;
	SamuraIRC->msg_handler = new IRCMessageHandler;
	SamuraIRC->downloader  = new IRCExternalDownloader;
	SamuraIRC->ui_main     = new IRCMainUI;
	SamuraIRC->ui_addserv  = new IRCAddServerUI;
	SamuraIRC->buffers     = new IRCBufferModel;
	SamuraIRC->connections = new IRCConnectionRegistry;

	SamuraIRC->ui_main->hookStuffUp();

	if(SamuraIRC->settings->first_run){
		SamuraIRC->ui_addserv->open();
	}

	// TODO: create connections on demand
	QThread*       irc_thread = new QThread;
	IRCConnection* connection = new IRCConnection(0, irc_thread);

	/*
	//XXX temp: need server param
	QObject::connect(&handler, &IRCMessageHandler::tempSend   , connection, &IRCConnection::sendPrivmsg, Qt::QueuedConnection);
	QObject::connect(&handler, &IRCMessageHandler::tempSendRaw, connection, &IRCConnection::sendRaw    , Qt::QueuedConnection);
	*/

	irc_thread->start();
	SamuraIRC->ui_main->show();

	int ret =  app.exec();
	
	delete SamuraIRC->connections;
	delete SamuraIRC->buffers;
	delete SamuraIRC->ui_addserv;
	delete SamuraIRC->ui_main;
	delete SamuraIRC->downloader;
	delete SamuraIRC->msg_handler;
	delete SamuraIRC->settings;
	delete SamuraIRC;
	
	return ret;
}
