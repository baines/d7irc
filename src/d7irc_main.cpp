#include <qobject.h>
#include <qapplication.h>
#include "d7irc_data.h"
#include "d7irc_ui.h"

IRCCtx* SamuraIRC;

int main(int argc, char** argv){

	QApplication app(argc, argv);

	qRegisterMetaType<IRCServerDetails>();
	
	SamuraIRC              = new IRCCtx;
	SamuraIRC->servers     = new IRCServerModel;
	SamuraIRC->msg_handler = new IRCMessageHandler;
	SamuraIRC->downloader  = new IRCExternalDownloader;
	SamuraIRC->ui_main     = new IRCMainUI;
	SamuraIRC->ui_addserv  = new IRCAddServerUI;
	SamuraIRC->buffers     = new IRCBufferModel;
	SamuraIRC->connections = new IRCConnectionRegistry;

	SamuraIRC->ui_main->hookStuffUp();
	SamuraIRC->ui_addserv->hookStuffUp();

	if(SamuraIRC->servers->begin() == SamuraIRC->servers->end()){
		SamuraIRC->ui_addserv->open();
	} else {
		for(auto* serv : *SamuraIRC->servers){
			if(serv->autoconnect){
				SamuraIRC->connections->createConnection(serv->id);
			}
		}
	}

	SamuraIRC->ui_main->show();

	int ret = app.exec();
	
	delete SamuraIRC->connections;
	delete SamuraIRC->buffers;
	delete SamuraIRC->ui_addserv;
	delete SamuraIRC->ui_main;
	delete SamuraIRC->downloader;
	delete SamuraIRC->msg_handler;
	delete SamuraIRC->servers;
	delete SamuraIRC;
	
	return ret;
}
