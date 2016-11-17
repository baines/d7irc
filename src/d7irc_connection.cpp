#include <libircclient.h>
#include <unordered_set>
#include "d7irc_qt.h"
#include "d7irc_data.h"

static void
irc_connect(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* conn = static_cast<IRCConnection*>(irc_get_ctx(s));
	emit conn->connect(conn->our_id);

	for(const auto& c : conn->details.chans){
		// TODO: should these be done more incrementally?
		irc_cmd_join(s, c.name.toUtf8().constData(), c.pass.isEmpty() ? NULL : c.pass.toUtf8().constData());
	}
}

static void
irc_join(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* conn = static_cast<IRCConnection*>(irc_get_ctx(s));
	emit conn->join(conn->our_id, argv[0], origin);
}

static void
irc_part(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* conn = static_cast<IRCConnection*>(irc_get_ctx(s));
	emit conn->part(conn->our_id, argv[0], origin);
}

static void
irc_quit(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* conn = static_cast<IRCConnection*>(irc_get_ctx(s));
	QString quit_msg;
	for(int i = 0; i < n; ++i){
		quit_msg += argv[i];
	}
	emit conn->quit(conn->our_id, origin, quit_msg);
}

static void
irc_privmsg(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* conn = static_cast<IRCConnection*>(irc_get_ctx(s));
	QString msg;
	if(n > 1 && argv[1]){
		msg = argv[1];
	}
	emit conn->privmsg(conn->our_id, origin, argv[0], msg);
}

static void
irc_nick(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* conn = static_cast<IRCConnection*>(irc_get_ctx(s));
	emit conn->nick(conn->our_id, origin, argv[0]);
}

static void
irc_action(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* conn = static_cast<IRCConnection*>(irc_get_ctx(s));

	char nick[1024] = "";
	irc_target_get_nick(origin, nick, sizeof(nick)-1);
	strcat(nick, " ");

	QString msg(nick);
	if(n > 1 && argv[1]){
		msg += argv[1];
	}

	emit conn->privmsg(conn->our_id, "*", argv[0], msg);
}

/*
static void
irc_mode(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n){
	IRCConnection* conn = static_cast<IRCConnection*>(irc_get_ctx(s));
	emit conn->join(conn->server, argv[0], origin);
}
*/

// topic
// kick

static void
irc_numeric(irc_session_t* s, unsigned ev, const char* origin, const char** argv, unsigned n){
	IRCConnection* conn = static_cast<IRCConnection*>(irc_get_ctx(s));
	QStringList list;
	for(int i = 0; i < n; ++i){
		list << argv[i];
	}

	emit conn->numeric(conn->our_id, ev, list);
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
IRCConnection::IRCConnection(int id, const IRCServerDetails& details, QThread* thread)
: details(details)
, our_id(id)
, notifiers()
, irc_session(nullptr) {

	IRCMessageHandler* handler = SamuraIRC->msg_handler;

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
		emit disconnect(our_id, IRC_ERR_CREATE_SESSION, irc_errno(irc_session));
		return;
	}

	irc_set_ctx(irc_session, this);

	// TODO: make this a setting
	irc_option_set(irc_session, LIBIRC_OPTION_SSL_NO_VERIFY);

	if(details.ssl && !details.hostname.startsWith("#")){
		details.hostname.prepend("#");
	}

	if(details.username.isEmpty()){
		details.username = details.nickname;
	}

	if(details.realname.isEmpty()){
		details.realname = details.nickname;
	}

	int err = irc_connect(
		irc_session,
		details.hostname.toUtf8().constData(),
		details.port,
		details.password.isEmpty() ? NULL : details.password.toUtf8().constData(),
		details.nickname.toUtf8().constData(),
		details.username.toUtf8().constData(),
		details.realname.toUtf8().constData()
	);

	if(err != 0){
		emit disconnect(our_id, IRC_ERR_CONNECT, irc_errno(irc_session));
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
		emit disconnect(our_id, IRC_ERR_ADD, irc_errno(irc_session));
		return;
	}

	struct timeval tv = {};

	int ret = select(max_fd + 1, &in, &out, NULL, &tv);
	if(ret > 0){
		if(irc_process_select_descriptors(irc_session, &in, &out) != 0){
			emit disconnect(our_id, IRC_ERR_PROCESS, irc_errno(irc_session));
			return;
		}
	} else if(ret == 0){
		// TODO: PONG logic
	} else {
		// TODO: determine if fatal or not
		emit disconnect(our_id, IRC_ERR_SELECT, errno);
		perror("select");
	}

	max_fd = 0;
	FD_ZERO(&in);
	FD_ZERO(&out);

	if(irc_add_select_descriptors(irc_session, &in, &out, &max_fd) != 0){
		emit disconnect(our_id, IRC_ERR_ADD, irc_errno(irc_session));
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

void IRCConnection::sendPrivMsg(int id, const QString& target, const QString& msg){
	if(our_id != id) return;

	irc_cmd_msg(irc_session, target.toUtf8().constData(), msg.toUtf8().constData());
	tick();
}

void IRCConnection::sendRawMsg(int id, const QString& msg){
	if(our_id != id) return;

	irc_send_raw(irc_session, "%s", msg.toUtf8().constData());
	tick();
}
