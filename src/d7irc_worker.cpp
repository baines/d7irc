#include <libircclient.h>
#include <unordered_set>
#include "d7irc_qt.h"

static std::unordered_set<irc_session_t*> servers;

static void
irc_cb(irc_session_t* s, const char* ev, const char* origin, const char** argv, unsigned n)
{
	IRCWorker* w = static_cast<IRCWorker*>(irc_get_ctx(s));

	QString str = QString::asprintf("[%s] [%s] :: ", ev, origin);
	
	for(unsigned i = 0; i < n; ++i){
		str.append(argv[i]);
		str.append(" :: ");
	}

	emit w->privmsg(str);
}

static void
irc_cb2(irc_session_t* s, unsigned ev, const char* origin, const char** argv, unsigned n)
{
	IRCWorker* w = static_cast<IRCWorker*>(irc_get_ctx(s));

	QString str = QString::asprintf("[%u] [%s] :: ", ev, origin);
	
	for(unsigned i = 0; i < n; ++i){
		str.append(argv[i]);
		str.append(" :: ");
	}

	emit w->privmsg(str);
}

static void
irc_connect(irc_session_t* s, const char* ev, const char* origin, const char** argv, unsigned n)
{
	IRCWorker* w = static_cast<IRCWorker*>(irc_get_ctx(s));
	emit w->connect(w->server);

	irc_cmd_join(s, "#test", NULL);
}

static void
irc_join(irc_session_t* s, const char* ev, const char* origin, const char** argv, unsigned n)
{
	IRCWorker* w = static_cast<IRCWorker*>(irc_get_ctx(s));
	QString chan(argv[0]);
	printf("join: %s %s\n", w->server.toUtf8().constData(), argv[0]);
	emit w->join(w->server, chan);
}

IRCWorker::IRCWorker(QThread* thread)
{
	this->moveToThread(thread);
}

// probably pass in server name / other params
void IRCWorker::begin()
{
	// set up irc_stuff
	irc_callbacks_t cb = {};
	cb.event_connect = irc_connect;
	cb.event_channel = irc_cb;
	cb.event_privmsg = irc_cb;
	cb.event_join = irc_join;
	cb.event_part = irc_cb;
	cb.event_nick = irc_cb;
	cb.event_ctcp_action = irc_cb;
	cb.event_unknown = irc_cb;
	cb.event_numeric = irc_cb2;

	irc_session = irc_create_session(&cb);
	if(!irc_session){
		emit privmsg("FUCK");
		return;
	}

	irc_set_ctx(irc_session, this);
	servers.insert(irc_session);

	if(irc_connect(irc_session, "127.0.0.1", 6667, NULL, "d7", "d7", "d7") != 0){
		emit privmsg("WTF");
		return;
	}

	emit privmsg("Connecting...");
	server = "127.0.0.1";

	// TODO: this sucks, find a better way. maybe QSocketNotifier can be hacked in somehow
	timer = new QTimer;
	QObject::connect(timer, &QTimer::timeout, this, &IRCWorker::tick);
	timer->start(100);
}

void IRCWorker::tick()
{
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
}

void IRCWorker::sendPrivmsg(const QString& target, const QString& msg)
{
	irc_cmd_msg(irc_session, target.toUtf8().constData(), msg.toUtf8().constData());
}

void IRCWorker::sendRaw(const QString& msg)
{
	irc_send_raw(irc_session, "%s", msg.toUtf8().constData());
}
