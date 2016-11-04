#include "d7irc_data.h"
#include "d7irc_ui.h"
#include <libircclient.h>
#include <libirc_rfcnumeric.h>

IRCMessageHandler::IRCMessageHandler(IRCBufferModel* model, Ui::SamuraIRC* ui, IRCExternalDownloader* dl)
: buf_model(model)
, ui(ui)
, downloader(dl) {

}

void IRCMessageHandler::handleIRCConnect(const QString& serv){
	buf_model->addServer(serv);
}

void IRCMessageHandler::handleIRCJoin(const QString& serv, const QString& chan, const QString& user){
	IRCBuffer* buf = buf_model->addChannel(serv, chan);
	buf->addLine("-->", user);
	buf->nicks.add(user);
}

void IRCMessageHandler::handleIRCPart(const QString& serv, const QString& chan, const QString& user){
	IRCBuffer* buf = buf_model->addChannel(serv, chan);
	buf->addLine("<--", user);
	buf->nicks.del(user);
}

void IRCMessageHandler::handleIRCQuit(const QString& serv, const QString& user, const QString& msg){
	IRCBuffer* buf = buf_model->addServer(serv);
	QString text = user + " quit.";
	if(!msg.isEmpty()){
		text += " (" + msg + ")";
	}

	buf->addLine("", text);
	buf->nicks.del(user);
}

void IRCMessageHandler::handleIRCNick(const QString& serv, const QString& user, const QString& new_nick){
	IRCBuffer* buf = buf_model->addServer(serv);
	buf->nicks.del(user);
	buf->nicks.add(new_nick);
}

void IRCMessageHandler::handleIRCPrivMsg(const QString& serv, const QString& from, const QString& to, const QString& msg){

	IRCBuffer* buf;
	char nick[1024];
	irc_target_get_nick(from.toUtf8().constData(), nick, sizeof(nick));

	//FIXME: real nick
	if(to == "d12ninja"){
		buf = buf_model->addChannel(serv, nick);
	} else {
		buf = buf_model->addChannel(serv, to);
	}

	buf->addLine(nick, msg);
	downloader->checkMessage(msg, buf);
}

static const char nick_start_symbols[] = "[]\\`_^{|}";

int get_prefix_idx(IRCServerBuffer* serv, char prefix){

	for(int i = 0; i < serv->prefixes.size(); ++i){
		if(serv->prefixes[i].prefix == prefix){
			return i;
		}
	}

	return -1;
}

void IRCMessageHandler::handleIRCNumeric(const QString& serv, uint32_t event, const QStringList& data){
	IRCServerBuffer* sbuf = buf_model->addServer(serv);

	switch(event){
		case LIBIRC_RFC_RPL_NAMREPLY: {
			if(data.size() < 4) break;

			QStringList names = data[3].split(' ', QString::SkipEmptyParts);
			IRCBuffer* buf = buf_model->addChannel(serv, data[2]);

			for(int i = 0; i < names.size(); ++i){
				char c = names[i].toLatin1().constData()[0];

				if(!isalpha(c) && !strchr(nick_start_symbols, c)){
					buf->nicks.add(names[i].mid(1), get_prefix_idx(sbuf, c));
				} else {
					buf->nicks.add(names[i]);
				}
			}
		} break;

		// RPL_ISUPPORT
		case 5: {
			static QRegExp pre_reg("^PREFIX=\\(([^\\)]+)\\)(.*)$");
			for(auto& s : data){
				if(pre_reg.exactMatch(s)){
					puts("got prefix match");
					QString modes = pre_reg.cap(1);
					QString prefixes = pre_reg.cap(2);
					if(modes.size() != prefixes.size()) return;

					sbuf->prefixes.clear();
					for(int i = 0; i < modes.size(); ++i){
						sbuf->prefixes.push_back(IRCPrefix { prefixes[i].toLatin1(), modes[i].toLatin1() });
					}
				}
			}
		} break;

		default: {
			IRCBuffer* buf = buf_model->getDefault();
			buf->addLine(QString::number(event), data.join(" :: "));
		} break;
	}
}

void IRCMessageHandler::sendIRCMessage(const QString& str){
	QModelIndex idx = ui->serv_list->currentIndex();
	IRCBuffer* buf = reinterpret_cast<IRCBuffer*>(idx.internalPointer());
	
	// TODO: server - buffer association

	switch(buf->type){
		case IRC_BUF_INTERNAL: {
			buf->addLine("", "This space intentionally left blank");
		} break;

		case IRC_BUF_SERVER: {
			buf->addLine("", "Say something in a channel instead, would you?");
		} break;

		case IRC_BUF_CHANNEL: {
			puts("sending");
			emit tempSend(buf->name, str);
			buf->addLine("d12ninja", str);
		} break;
	}
}
