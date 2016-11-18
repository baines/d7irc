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

	IRCServerDetails* details = SamuraIRC->servers->getDetails(id);
	if(!details){
		return false;
	}

	IRCConnection* conn = new IRCConnection(id, *details, irc_thread);

	IRCConnectionInfo conn_info = {
		id,
		conn,
		IRC_CON_CONNECTING,
		details->nickname,
		default_prefixes
	};
	connections.push_back(conn_info);

	QMetaObject::invokeMethod(conn, "start");

	IRCBuffer* buf = SamuraIRC->buffers->addServer(details->unique_name);
	buf->addLine("--", QString("Connecting to %1:%2 %3...").arg(details->hostname).arg(details->port).arg(details->ssl ? "(SSL)" : ""));

	emit statusChanged(id, IRC_CON_CONNECTING);
	
	return true;
}

bool IRCConnectionRegistry::destroyConnection(int id){
	IRCServerDetails* details = SamuraIRC->servers->getDetails(id);
	if(!details){
		return false;
	}

	IRCConnectionInfo info = {};

	for(int i = 0; i < connections.size(); ++i){
		if(connections[i].id == id){
			info = connections[i];
			connections.erase(connections.begin() + i);
			break;
		}
	}

	if(!info.conn) return false;

	// info.conn is deleted in stop
	QMetaObject::invokeMethod(info.conn, "stop");

	emit statusChanged(id, IRC_CON_DISCONNECTED);

	return true;
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

void IRCConnectionRegistry::setStatus(int id, IRCConnectionStatus status){
	auto* info = getInfo(id);
	if(!info) return;

	info->status = status;
	emit statusChanged(id, status);
}
