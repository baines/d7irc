#include <libircclient.h>
#include <unordered_set>
#include "d7irc_qt.h"

static void
irc_connect(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n)
{
	IRCWorker* w = static_cast<IRCWorker*>(irc_get_ctx(s));
	emit w->connect(w->server);

	// XXX remove later
	irc_cmd_join(s, "#random", NULL);
	irc_cmd_join(s, "#dev", NULL);
	irc_cmd_join(s, "#hmn", NULL);
	irc_cmd_join(s, "#hero", NULL);
}

static void
irc_join(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n)
{
	IRCWorker* w = static_cast<IRCWorker*>(irc_get_ctx(s));
	emit w->join(w->server, argv[0], origin);
}

static void
irc_part(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n)
{
	IRCWorker* w = static_cast<IRCWorker*>(irc_get_ctx(s));
	emit w->part(w->server, argv[0], origin);
}

static void
irc_quit(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n)
{
	IRCWorker* w = static_cast<IRCWorker*>(irc_get_ctx(s));
	QString quit_msg;
	if(n > 0 && argv[0]){
		quit_msg = argv[0];
	}
	emit w->quit(w->server, origin, quit_msg);
}

static void
irc_privmsg(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n)
{
	IRCWorker* w = static_cast<IRCWorker*>(irc_get_ctx(s));
	QString msg;
	if(n > 1 && argv[1]){
		msg = argv[1];
	}
	emit w->privmsg(w->server, origin, argv[0], msg);
}

static void
irc_nick(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n)
{
	IRCWorker* w = static_cast<IRCWorker*>(irc_get_ctx(s));
	emit w->nick(w->server, origin, argv[0]);
}

/*
static void
irc_mode(irc_session_t* s, const char*, const char* origin, const char** argv, unsigned n)
{
	IRCWorker* w = static_cast<IRCWorker*>(irc_get_ctx(s));
	emit w->join(w->server, argv[0], origin);
}
*/

// topic
// kick

static void
irc_numeric(irc_session_t* s, unsigned ev, const char* origin, const char** argv, unsigned n)
{
	IRCWorker* w = static_cast<IRCWorker*>(irc_get_ctx(s));
	QStringList list;
	for(int i = 0; i < n; ++i){
		list << argv[i];
	}

	emit w->numeric(w->server, ev, list);
}

IRCWorker::IRCWorker(QThread* thread)
{
	this->moveToThread(thread);
}

// probably pass in server name / other params
void IRCWorker::begin()
{
	irc_callbacks_t cb   = {};
	cb.event_connect     = irc_connect;
	cb.event_channel     = irc_privmsg;
	cb.event_privmsg     = irc_privmsg;
	cb.event_join        = irc_join;
	cb.event_part        = irc_part;
	cb.event_nick        = irc_nick;
//	cb.event_ctcp_action = irc_cb;
//	cb.event_unknown     = irc_cb;
	cb.event_numeric     = irc_numeric;

	irc_session = irc_create_session(&cb);
	if(!irc_session){
		puts("fix me");
		return;
	}

#if 0
	const char* temp_connect = "irc.handmade.network";
	int port = 7666;
#else
	const char* temp_connect = "127.0.0.1";
	int port = 6667;
#endif

	const char* temp_name = "d12ninja";
	server = temp_connect;

	irc_set_ctx(irc_session, this);

	if(irc_connect(irc_session, temp_connect, port, NULL, temp_name, temp_name, temp_name) != 0){
		puts("fix me 2");
		return;
	}

	tick();
}

void IRCWorker::tick()
{
	puts("tick");

	int max_fd = 0;
	fd_set in, out;
	FD_ZERO(&in);
	FD_ZERO(&out);

	if(irc_add_select_descriptors(irc_session, &in, &out, &max_fd) != 0){
		fprintf(stderr, "Error adding select fds: %s\n", irc_strerror(irc_errno(irc_session)));
	}

	struct timeval tv = {};

	int ret = select(max_fd + 1, &in, &out, NULL, &tv);
	if(ret > 0){
		if(irc_process_select_descriptors(irc_session, &in, &out) != 0){
			fprintf(stderr, "Error processing select fds: %s\n", irc_strerror(irc_errno(irc_session)));
		}
	} else if(ret == 0){
		// TODO: PONG logic
	} else {
		perror("select");
	}

	max_fd = 0;
	FD_ZERO(&in);
	FD_ZERO(&out);

	if(irc_add_select_descriptors(irc_session, &in, &out, &max_fd) != 0){
		fprintf(stderr, "Error adding select fds: %s\n", irc_strerror(irc_errno(irc_session)));
	}

	notifiers.clear();

	for(int i = 3; i <= max_fd; ++i){
		if(FD_ISSET(i, &in)){
			auto* q =  new QSocketNotifier(i, QSocketNotifier::Read);
			QObject::connect(q, &QSocketNotifier::activated, this, &IRCWorker::tick);
			notifiers.emplace_back(q);
		}
		if(FD_ISSET(i, &out)){
			auto* q =  new QSocketNotifier(i, QSocketNotifier::Write);
			QObject::connect(q, &QSocketNotifier::activated, this, &IRCWorker::tick);
			notifiers.emplace_back(q);
		}
	}

}

void IRCWorker::sendPrivmsg(const QString& target, const QString& msg)
{
	irc_cmd_msg(irc_session, target.toUtf8().constData(), msg.toUtf8().constData());
	tick();
}

void IRCWorker::sendRaw(const QString& msg)
{
	irc_send_raw(irc_session, "%s", msg.toUtf8().constData());
	tick();
}
