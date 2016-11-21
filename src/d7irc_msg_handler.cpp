#include "d7irc_data.h"
#include "d7irc_ui.h"
#include <libircclient.h>
#include <libirc_rfcnumeric.h>
#include <qscrollbar.h>
#include <cassert>

QString serv_id2name(int id){
	auto* serv = SamuraIRC->servers->getDetails(id);
	assert(serv);
	return serv->unique_name;
}

void IRCMessageHandler::handleIRCConnect(int id){
	IRCServerDetails& serv = *SamuraIRC->servers->getDetails(id);

	for(auto& c : serv.chans){
		if(c.name.isEmpty()) continue;
		IRCBuffer* cbuf = SamuraIRC->buffers->addChannel(serv.unique_name, c.name);
		cbuf->addLine("--", "Joining channel...");
	}

	IRCBuffer* sbuf = SamuraIRC->buffers->addServer(serv.unique_name);
	sbuf->addLine("--", "Connected!");
	sbuf->active = true;
	SamuraIRC->buffers->markBufferDirty(sbuf);

	SamuraIRC->connections->setStatus(id, IRC_CON_CONNECTED);
}

void IRCMessageHandler::handleIRCJoin(int id, const QString& chan, const QString& user){
	IRCServerDetails& serv = *SamuraIRC->servers->getDetails(id);

	IRCBuffer* buf = SamuraIRC->buffers->addChannel(serv.unique_name, chan);

	if(!buf->active){
		buf->active = true;
		SamuraIRC->buffers->markBufferDirty(buf);
	}

	buf->addLine("-->", user);
	buf->nicks->add(user);
}

void IRCMessageHandler::handleIRCPart(int id, const QString& chan, const QString& user){
	IRCServerDetails&  serv = *SamuraIRC->servers->getDetails(id);
	IRCConnectionInfo& info = *SamuraIRC->connections->getInfo(id);

	// TODO: findChannel
	IRCBuffer* buf = SamuraIRC->buffers->addChannel(serv.unique_name, chan);
	if(buf->nicks->del(user)){
		buf->addLine("<--", user);
	}
	
	// TODO: make a util::user2nick();
	if(info.current_nick == user){
		buf->active = false;
		SamuraIRC->buffers->markBufferDirty(buf);
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
	IRCServerDetails&  serv = *SamuraIRC->servers->getDetails(id);
	IRCConnectionInfo& info = *SamuraIRC->connections->getInfo(id);

	IRCBuffer* buf = SamuraIRC->buffers->addServer(serv.unique_name);

	char old_nick[1024];
	irc_target_get_nick(user.toUtf8().constData(), old_nick, sizeof(old_nick));

	bool self_change = false;

	if(info.current_nick == old_nick){
		self_change = true;
		info.current_nick = new_nick;
	}

	for(IRCBuffer* chan = buf->child; chan; chan = chan->sibling){
		if(chan->nicks->del(user)){
			chan->nicks->add(new_nick);
			QString msg;

			if(self_change){
				msg = QString("You are now known as %1").arg(new_nick);
			} else {
				msg = QString(old_nick) + " is now known as " + new_nick;
			}
			chan->addLine("--", msg);
		}
	}
}

void IRCMessageHandler::handleIRCPrivMsg(int id, const QString& from, const QString& to, const QString& msg){
	IRCServerDetails&  serv = *SamuraIRC->servers->getDetails(id);
	IRCConnectionInfo& info = *SamuraIRC->connections->getInfo(id);

	IRCBuffer* buf;
	char nick[1024];
	irc_target_get_nick(from.toUtf8().constData(), nick, sizeof(nick));

	if(to == info.current_nick){
		buf = SamuraIRC->buffers->addChannel(serv.unique_name, nick);
	} else {
		buf = SamuraIRC->buffers->addChannel(serv.unique_name, to);
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
	} else if(err != IRC_ERR_NONE){
		fprintf(stderr, "Disconnected: %d: %s\n", err, irc_strerror(sub_err));
	}

	IRCBuffer* buf = SamuraIRC->buffers->addServer(serv);
	buf->addLine("--", "Disconnected from server.");
	buf->active = false;
	SamuraIRC->buffers->markBufferDirty(buf);

	for(IRCBuffer* c = buf->child; c; c = c->sibling){
		c->addLine("--", "Disconnected from server.");
		c->nicks->clear();
		c->active = false;
		SamuraIRC->buffers->markBufferDirty(c);
	}

}

void IRCMessageHandler::sendIRCMessage(const QString& str){
	QModelIndex idx = SamuraIRC->ui_main->ui->buffer_list->currentIndex();
	IRCBuffer* buf = reinterpret_cast<IRCBuffer*>(idx.internalPointer());

	switch(buf->type){
		case IRC_BUF_INTERNAL: {
			buf->addLine("", "This space intentionally left blank");
		} break;

		case IRC_BUF_SERVER: {
			buf->addLine("", "Say something in a channel instead, would you?");
		} break;

		case IRC_BUF_CHANNEL: {
			IRCBuffer* sbuf = buf->parent;
			int id = SamuraIRC->servers->serverNameToID(sbuf->name);
			if(id == -1){
				// XXX: -1 here actually means the server doesn't exist... that shouldn't happen
				buf->addLine("--", "Not connected!");
				break;
			}

			IRCConnectionInfo* info = SamuraIRC->connections->getInfo(id);
			if(!info){
				buf->addLine("--", "Not connected!");
				break;
			}

			puts("sending");
			emit dispatchPrivMsg(id, buf->name, str);

			if(str.startsWith("\001ACTION ")){
				buf->addLine("*", info->current_nick + " " + str.mid(8, str.size() - 9));
			} else {
				buf->addLine(info->current_nick, str);
			}
		} break;
	}
}

void IRCMessageHandler::sendIRCCommand(const QString& cmd){
	QModelIndex idx = SamuraIRC->ui_main->ui->buffer_list->currentIndex();
	IRCBuffer* buf = reinterpret_cast<IRCBuffer*>(idx.internalPointer());
	int id = -1;
	
	// TODO: write a proper command handling system

	switch(buf->type){
		case IRC_BUF_INTERNAL: {
			buf->addLine("--", "Can't do that here.");
		} break;

		case IRC_BUF_SERVER: {
			id = SamuraIRC->servers->serverNameToID(buf->name);
		} break;

		case IRC_BUF_CHANNEL: {
			id = SamuraIRC->servers->serverNameToID(buf->parent->name);
		} break;
	}

	if(id == -1){
		return;
	}

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

			emit dispatchRawMsg(id, msg);
		}

	} else {
		emit dispatchRawMsg(id, cmd);
	}
}

