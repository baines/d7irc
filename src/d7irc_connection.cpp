#include <libircclient.h>
#include <unordered_set>
#include "d7irc_qt.h"
#include "d7irc_data.h"

static void
irc_connect(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* w = static_cast<IRCConnection*>(irc_get_ctx(s));
	emit w->connect(w->server);

	// XXX remove later, get channels from IRCSettings
#if 1
	irc_cmd_join(s, "#random", NULL);
	irc_cmd_join(s, "#dev", NULL);
	irc_cmd_join(s, "#hmn", NULL);
	irc_cmd_join(s, "#hero", NULL);
#else 
	irc_cmd_join(s, "##linux", NULL);
#endif
}

static void
irc_join(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* w = static_cast<IRCConnection*>(irc_get_ctx(s));
	emit w->join(w->server, argv[0], origin);
}

static void
irc_part(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* w = static_cast<IRCConnection*>(irc_get_ctx(s));
	emit w->part(w->server, argv[0], origin);
}

static void
irc_quit(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* w = static_cast<IRCConnection*>(irc_get_ctx(s));
	QString quit_msg;
	for(int i = 0; i < n; ++i){
		quit_msg += argv[i];
	}
	emit w->quit(w->server, origin, quit_msg);
}

static void
irc_privmsg(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* w = static_cast<IRCConnection*>(irc_get_ctx(s));
	QString msg;
	if(n > 1 && argv[1]){
		msg = argv[1];
	}
	emit w->privmsg(w->server, origin, argv[0], msg);
}

static void
irc_nick(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* w = static_cast<IRCConnection*>(irc_get_ctx(s));
	emit w->nick(w->server, origin, argv[0]);
}

static void
irc_action(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* w = static_cast<IRCConnection*>(irc_get_ctx(s));

	char nick[1024] = "";
	irc_target_get_nick(origin, nick, sizeof(nick)-1);
	strcat(nick, " ");

	QString msg(nick);
	if(n > 1 && argv[1]){
		msg += argv[1];
	}

	emit w->privmsg(w->server, "*", argv[0], msg);
}

/*
static void
irc_mode(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* w = static_cast<IRCConnection*>(irc_get_ctx(s));
	emit w->join(w->server, argv[0], origin);
}
*/

// topic
// kick

static void
irc_numeric(irc_session_t* s, unsigned ev, const char* origin, const char** argv, unsigned n){
	IRCConnection* w = static_cast<IRCConnection*>(irc_get_ctx(s));
	QStringList list;
	for(int i = 0; i < n; ++i){
		list << argv[i];
	}

	emit w->numeric(w->server, ev, list);
}

// if only c++ had designated initializers ._.
static irc_callbacks_t irc_callbacks;
extern "C" {
	__attribute__((constructor)) void init_irc_callbacks(void){
		irc_callbacks.event_connect     = irc_connect;
		irc_callbacks.event_channel     = irc_privmsg;
		irc_callbacks.event_privmsg     = irc_privmsg;
		irc_callbacks.event_join        = irc_join;
		irc_callbacks.event_part        = irc_part;
		irc_callbacks.event_quit        = irc_quit;
		irc_callbacks.event_nick        = irc_nick;
		irc_callbacks.event_ctcp_action = irc_action;
		//	irc_callbacks.event_unknown     = irc_cb;
		irc_callbacks.event_numeric     = irc_numeric;
	};
}

// TODO: we need to be given the server ID in IRCSettings
IRCConnection::IRCConnection(QThread* thread, IRCMessageHandler* handler){
	QObject::connect(thread, &QThread::started, this, &IRCConnection::begin);

	QObject::connect(this, &IRCConnection::connect, handler, &IRCMessageHandler::handleIRCConnect, Qt::QueuedConnection);
	QObject::connect(this, &IRCConnection::join   , handler, &IRCMessageHandler::handleIRCJoin   , Qt::QueuedConnection);
	QObject::connect(this, &IRCConnection::part   , handler, &IRCMessageHandler::handleIRCPart   , Qt::QueuedConnection);
	QObject::connect(this, &IRCConnection::quit   , handler, &IRCMessageHandler::handleIRCQuit   , Qt::QueuedConnection);
	QObject::connect(this, &IRCConnection::nick   , handler, &IRCMessageHandler::handleIRCNick   , Qt::QueuedConnection);
	QObject::connect(this, &IRCConnection::privmsg, handler, &IRCMessageHandler::handleIRCPrivMsg, Qt::QueuedConnection);
	QObject::connect(this, &IRCConnection::numeric, handler, &IRCMessageHandler::handleIRCNumeric, Qt::QueuedConnection);

	QObject::connect(this, &IRCConnection::disconnect, handler, &IRCMessageHandler::handleIRCDisconnect, Qt::QueuedConnection);

	this->moveToThread(thread);
}

void IRCConnection::begin(){
	irc_session = irc_create_session(&irc_callbacks);
	if(!irc_session){
		emit disconnect(IRC_ERR_CREATE_SESSION, irc_errno(irc_session));
		return;
	}

#if 0
	const char* temp_connect = "irc.handmade.network";
	int port = 7666;
#else
	const char* temp_connect = "127.0.0.1";
	int port = 6667;
#endif

	//TODO: get nick, user, real name etc from IRCSettings
	const char* temp_name = "d12ninja";
	server = temp_connect;

	irc_set_ctx(irc_session, this);

	//TODO: ssl prepend # logic
	// also, irc_connect6?
	if(irc_connect(irc_session, temp_connect, port, NULL, temp_name, temp_name, temp_name) != 0){
		emit disconnect(IRC_ERR_CONNECT, irc_errno(irc_session));
		return;
	}

	tick();
}

void IRCConnection::tick(){
	puts("tick");

	notifiers.clear();

	int max_fd = 0;
	fd_set in, out;
	FD_ZERO(&in);
	FD_ZERO(&out);

	if(irc_add_select_descriptors(irc_session, &in, &out, &max_fd) != 0){
		emit disconnect(IRC_ERR_ADD, irc_errno(irc_session));
		return;
	}

	struct timeval tv = {};

	int ret = select(max_fd + 1, &in, &out, NULL, &tv);
	if(ret > 0){
		if(irc_process_select_descriptors(irc_session, &in, &out) != 0){
			emit disconnect(IRC_ERR_PROCESS, irc_errno(irc_session));
			return;
		}
	} else if(ret == 0){
		// TODO: PONG logic
	} else {
		// TODO: determine if fatal or not
		emit disconnect(IRC_ERR_SELECT, errno);
		perror("select");
	}

	max_fd = 0;
	FD_ZERO(&in);
	FD_ZERO(&out);

	if(irc_add_select_descriptors(irc_session, &in, &out, &max_fd) != 0){
		emit disconnect(IRC_ERR_ADD, irc_errno(irc_session));
		return;
	}

	for(int i = 3; i <= max_fd; ++i){
		if(FD_ISSET(i, &in)){
			auto* q =  new QSocketNotifier(i, QSocketNotifier::Read);
			QObject::connect(q, &QSocketNotifier::activated, this, &IRCConnection::tick);
			notifiers.emplace_back(q);
		}
		if(FD_ISSET(i, &out)){
			auto* q =  new QSocketNotifier(i, QSocketNotifier::Write);
			QObject::connect(q, &QSocketNotifier::activated, this, &IRCConnection::tick);
			notifiers.emplace_back(q);
		}
	}

}

void IRCConnection::sendPrivmsg(const QString& target, const QString& msg){
	irc_cmd_msg(irc_session, target.toUtf8().constData(), msg.toUtf8().constData());
	tick();
}

void IRCConnection::sendRaw(const QString& msg){
	irc_send_raw(irc_session, "%s", msg.toUtf8().constData());
	tick();
}
