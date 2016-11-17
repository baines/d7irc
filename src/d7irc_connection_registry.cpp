#include "d7irc_qt.h"

static std::vector<IRCPrefix> default_prefixes = {
	{ 0, '@', 'o' },
	{ 1, '%', 'h' },
	{ 2, '+', 'v' }
};

IRCConnectionRegistry::IRCConnectionRegistry(){
	irc_thread = new QThread;
	irc_thread->start();
}

bool IRCConnectionRegistry::createConnection(int id){
	// TODO: check if it already exists first

	IRCServerDetails* details = SamuraIRC->settings->getDetails(id);
	if(!details){
		return false;
	}

	IRCConnection* conn = new IRCConnection(id, *details, irc_thread);

	IRCConnectionInfo conn_info = {
		id,
		conn,
		details->nickname,
		default_prefixes
	};
	connections.push_back(conn_info);

	// TODO: pass details to begin
	QMetaObject::invokeMethod(conn, "begin");
}

bool IRCConnectionRegistry::destroyConnection(int id){

}

IRCConnectionInfo* IRCConnectionRegistry::getInfo(int id){

	//XXX: returning potentially invalidatable pointer?

	for(auto& c : connections){
		if(c.id == id){
			return &c;
		}
	}

	return nullptr;
}
