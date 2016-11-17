#include "d7irc_data.h"
#include "d7irc_ui.h"
#include <libircclient.h>
#include <libirc_rfcnumeric.h>
#include <qscrollbar.h>
#include <cassert>

static const QString& serv_id2name(int id){
	auto* info = SamuraIRC->settings->getDetails(id);
	assert(info);
	return info->unique_name;
}

void IRCMessageHandler::handleIRCConnect(int id){
	const auto& serv = serv_id2name(id);

	SamuraIRC->buffers->addServer(serv);
}

void IRCMessageHandler::handleIRCJoin(int id, const QString& chan, const QString& user){
	const auto& serv = serv_id2name(id);

	IRCBuffer* buf = SamuraIRC->buffers->addChannel(serv, chan);
	buf->addLine("-->", user);
	buf->nicks->add(user);
}

void IRCMessageHandler::handleIRCPart(int id, const QString& chan, const QString& user){
	const auto& serv = serv_id2name(id);

	IRCBuffer* buf = SamuraIRC->buffers->addChannel(serv, chan);
	if(buf->nicks->del(user)){
		buf->addLine("<--", user);
	}
}

void IRCMessageHandler::handleIRCQuit(int id, const QString& user, const QString& msg){
	const auto& serv = serv_id2name(id);

	IRCBuffer* buf = SamuraIRC->buffers->addServer(serv);
	QString text = user + " quit";
	if(!msg.isEmpty()){
		text += " (" + msg + ")";
	}

	for(IRCBuffer* chan = buf->child; chan; chan = chan->sibling){
		if(chan->nicks->del(user)){
			chan->addLine("<--", text);
		}
	}
}

void IRCMessageHandler::handleIRCNick(int id, const QString& user, const QString& new_nick){
	const auto& serv = serv_id2name(id);

	IRCBuffer* buf = SamuraIRC->buffers->addServer(serv);

	char old_nick[1024];
	irc_target_get_nick(user.toUtf8().constData(), old_nick, sizeof(old_nick));

	for(IRCBuffer* chan = buf->child; chan; chan = chan->sibling){
		if(chan->nicks->del(user)){
			chan->nicks->add(new_nick);
			QString msg = QString(old_nick) + " is now known as " + new_nick;
			chan->addLine("--", msg);
		}
	}
}

void IRCMessageHandler::handleIRCPrivMsg(int id, const QString& from, const QString& to, const QString& msg){
	const auto& serv = serv_id2name(id);

	IRCBuffer* buf;
	char nick[1024];
	irc_target_get_nick(from.toUtf8().constData(), nick, sizeof(nick));

	//FIXME: real nick
	if(to == "d12ninja"){
		buf = SamuraIRC->buffers->addChannel(serv, nick);
	} else {
		buf = SamuraIRC->buffers->addChannel(serv, to);
	}

	buf->addLine(nick, msg);
	SamuraIRC->downloader->checkMessage(msg, buf);
}

// TODO: use prefixes in server buf instead?
static const char nick_start_symbols[] = "[]\\`_^{|}";

IRCPrefix* get_prefix(IRCConnectionInfo* info, char prefix){
	for(int i = 0; i < info->prefixes.size(); ++i){
		if(info->prefixes[i].symbol == prefix){
			return &info->prefixes[i];
		}
	}
	return nullptr;
}

void IRCMessageHandler::handleIRCNumeric(int id, uint32_t event, QStringList data){
	const auto& serv = serv_id2name(id);
	IRCBuffer* sbuf  = SamuraIRC->buffers->addServer(serv);

	IRCConnectionInfo* info = SamuraIRC->connections->getInfo(id);

	switch(event){
		case LIBIRC_RFC_RPL_NAMREPLY: {
			if(data.size() < 4) break;

			QStringList names = data[3].split(' ', QString::SkipEmptyParts);
			IRCBuffer* buf = SamuraIRC->buffers->addChannel(serv, data[2]);

			for(int i = 0; i < names.size(); ++i){
				char c = names[i].toLatin1().constData()[0];

				if(!isalpha(c) && !strchr(nick_start_symbols, c)){
					buf->nicks->add(names[i].mid(1), get_prefix(info, c));
				} else {
					buf->nicks->add(names[i]);
				}
			}
		} break;

		case LIBIRC_RFC_RPL_ENDOFNAMES: {
			// ignore
		} break;

		case 5: /* RPL_ISUPPORT */ {
			static QRegExp pre_reg("^PREFIX=\\(([^\\)]+)\\)(.*)$");
			for(auto& s : data){
				if(pre_reg.exactMatch(s)){
					puts("got prefix match");
					QString modes = pre_reg.cap(1);
					QString prefixes = pre_reg.cap(2);
					if(modes.size() != prefixes.size()) return;

					info->prefixes.clear();
					for(int i = 0; i < modes.size(); ++i){
						IRCPrefix p = {
							i,
							prefixes[i].toLatin1(),
							modes[i].toLatin1()
						};
						info->prefixes.push_back(p);
					}
				}
			}
		} /* fall through */;

		default: {
			data.removeFirst();
			sbuf->addLine(QString::number(event), data.join(" :: "));
		} break;
	}
}

void IRCMessageHandler::handleIRCDisconnect(int id, int err, int sub_err){
	const auto& serv = serv_id2name(id);

	if(err == IRC_ERR_SELECT){
		fprintf(stderr, "Disconnected: %d: %s\n", err, strerror(sub_err));
	} else {
		fprintf(stderr, "Disconnected: %d: %s\n", err, irc_strerror(sub_err));
	}

	//TODO: mark relevent buffers inactive
}

void IRCMessageHandler::sendIRCMessage(const QString& str){
	QModelIndex idx = SamuraIRC->ui_main->ui->buffer_list->currentIndex();
	IRCBuffer* buf = reinterpret_cast<IRCBuffer*>(idx.internalPointer());
	
	// TODO: get active server buffer -> translate to id

	switch(buf->type){
		case IRC_BUF_INTERNAL: {
			buf->addLine("", "This space intentionally left blank");
		} break;

		case IRC_BUF_SERVER: {
			buf->addLine("", "Say something in a channel instead, would you?");
		} break;

		case IRC_BUF_CHANNEL: {
			puts("sending");
			emit dispatchPrivMsg(0, buf->name, str);

			if(str.startsWith("\001ACTION ")){
				//TODO: real nick
				buf->addLine("*", QString("d12ninja ") + str.mid(8, str.size() - 9));
			} else {
				//TODO: real nick
				buf->addLine("d12ninja", str);
			}
		} break;
	}
}

// TODO: lookup current server
void IRCMessageHandler::sendIRCCommand(const QString& cmd){

	if(cmd.startsWith("me ", Qt::CaseInsensitive)){

		QString msg = QString("\001ACTION %1\001").arg(cmd.mid(3));
		sendIRCMessage(msg);

	} else if(cmd.startsWith("msg ", Qt::CaseInsensitive)){

		QStringList list = cmd.split(" ");
		if(list.size() >= 3){
			list.removeFirst();
			QString to  = list[0];
			list.removeFirst();

			QString msg = QString("PRIVMSG %1 :%2").arg(to).arg(list.join(" "));

			// TODO: real id
			emit dispatchRawMsg(0, msg);
		}

	} else {
		// TODO: real id
		emit dispatchRawMsg(0, cmd);
	}
}

