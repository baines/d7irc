#include "d7irc_data.h"
#include <libircclient.h>

IRCMessageHandler::IRCMessageHandler(IRCBufferModel* model, Ui::SamuraIRC* ui)
: buf_model(model)
, ui(ui) {

}

void IRCMessageHandler::handleIRCConnect(const QString& serv){
	buf_model->addServer(serv);
}

void IRCMessageHandler::handleIRCJoin(const QString& serv, const QString& chan, const QString& user){
	IRCBuffer* buf = buf_model->addChannel(serv, chan);
	buf->addLine("-->", user);
	buf->nicks.addNick(user);
}

void IRCMessageHandler::handleIRCPart(const QString& serv, const QString& chan, const QString& user){
	IRCBuffer* buf = buf_model->addChannel(serv, chan);
	buf->addLine("<--", user);
	buf->nicks.delNick(user);
}

void IRCMessageHandler::handleIRCQuit(const QString& serv, const QString& user, const QString& msg){
	IRCBuffer* buf = buf_model->addServer(serv);
	QString text = user + " quit.";
	if(!msg.isEmpty()){
		text += " (" + msg + ")";
	}

	buf->addLine("", text);
	buf->nicks.delNick(user);
}

void IRCMessageHandler::handleIRCNick(const QString& serv, const QString& user, const QString& new_nick){
	IRCBuffer* buf = buf_model->addServer(serv);
	buf->nicks.delNick(user);
	buf->nicks.addNick(new_nick);
}

void IRCMessageHandler::handleIRCPrivMsg(const QString& serv, const QString& from, const QString& to, const QString& msg){

	IRCBuffer* buf;
	char nick[1024];
	irc_target_get_nick(from.toUtf8().constData(), nick, sizeof(nick));

	if(to == "d7"){
		buf = buf_model->addChannel(serv, nick);
	} else {
		buf = buf_model->addChannel(serv, to);
	}

	buf->addLine(nick, msg);
}

void IRCMessageHandler::handleIRCNumeric(const QString& serv, uint32_t event, const QStringList& data){

}

