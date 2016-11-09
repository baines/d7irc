#include "d7irc_data.h"

IRCServerDetails::IRCServerDetails()
: unique_name("New Server") // TODO: make it actually unique...
, nickname()
, hostname()
, port(6667)
, ssl(false)
, username()
, password()
, nickserv()
, commands()
, chans() {
	chans.setHorizontalHeaderLabels(QStringList({ "Name", "Password" }));
}

IRCSettings::IRCSettings()
: servers()
, settings(QSettings::IniFormat, QSettings::UserScope, "samurairc") {
	settings.setFallbacksEnabled(false);
	first_run = settings.value("main/first_run", true).toBool();
	settings.setValue("main/first_run", false);

	// load servers etc
	static QRegExp serv_key_regex("^serv\\..*");
	QStringList keys = settings.childGroups().filter(serv_key_regex);

	for(auto& k : keys){

		printf("key: %s\n", k.toUtf8().constData());

		IRCServerDetails* serv = new IRCServerDetails;

		serv->unique_name = k.mid(5);

		settings.beginGroup(k);

		serv->nickname = settings.value("nickname", "samurai").toString();
		serv->hostname = settings.value("hostname", "irc.example.com").toString();
		serv->port     = settings.value("port"    , 6667).toUInt();
		serv->ssl      = settings.value("ssl"     , false).toBool();

		serv->username = settings.value("username", serv->nickname).toString();
		serv->realname = settings.value("realname", serv->nickname).toString();

		// TODO: store in keychain instead of plaintext...
		serv->password = settings.value("password", "").toString();
		serv->nickserv = settings.value("nickserv", "").toString();
		serv->commands = settings.value("commands", "").toString();

		int nchans = settings.beginReadArray("channels");
		for(int i = 0; i < nchans; ++i){
			settings.setArrayIndex(i);

			QString name = settings.value("name", "").toString();
			QString pass = settings.value("pass", "").toString();

			if(!name.isEmpty()){
				serv->chans.appendRow(QList<QStandardItem*>({ new QStandardItem(name), new QStandardItem(pass) }));
			}
		}
		settings.endArray();
		settings.endGroup();

		serv->chans.insertRows(serv->chans.rowCount(), 1);

		servers.push_back(serv);
	}

}

IRCSettings::~IRCSettings(){
	for(auto* s : servers){
		settings.beginGroup("serv." + s->unique_name);

		settings.setValue("nickname", s->nickname);
		settings.setValue("hostname", s->hostname);
		settings.setValue("port"    , s->port);
		settings.setValue("ssl", s->ssl);

		settings.setValue("username", s->username);
		settings.setValue("realname", s->realname);

		// TODO: store in keychain instead of plaintext...
		settings.setValue("password", s->password);
		settings.setValue("nickserv", s->nickserv);
		settings.setValue("commands", s->commands);

		settings.beginWriteArray("channels");
		for(int i = 0; i < s->chans.rowCount(); ++i){
			settings.setArrayIndex(i);

			QStandardItem* name = s->chans.item(i, 0);
			QStandardItem* pass = s->chans.item(i, 1);

			if(name && !name->text().isEmpty()){
				settings.setValue("name", name->text());
				settings.setValue("pass", pass ? pass->text() : "");
			}
		}
		settings.endArray();
		settings.endGroup();
	}
}

int IRCSettings::rowCount(const QModelIndex&) const {
	return servers.size();
}

int IRCSettings::columnCount(const QModelIndex&) const {
	return 10;
}

QVariant IRCSettings::data(const QModelIndex& idx, int role) const {
	if(role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();

	const IRCServerDetails& serv = *servers[idx.row()];

	switch(idx.column()){
		case IRC_DAT_UNIQUE_NAME:
			return serv.unique_name;
		case IRC_DAT_NICK:
			return serv.nickname;
		case IRC_DAT_ADDRESS:
			return serv.hostname;
		case IRC_DAT_PORT:
			return serv.port;
		case IRC_DAT_SSL:
			return serv.ssl;
		case IRC_DAT_USERNAME:
			return serv.username;
		case IRC_DAT_REALNAME:
			return serv.realname;
		case IRC_DAT_PASSWORD:
			return serv.password;
		case IRC_DAT_NICKSERV:
			return serv.nickserv;
		case IRC_DAT_CMDS:
			return serv.commands;
		default:
			return QVariant();
	}
}

static const int col_types[] = {
	QMetaType::QString,
	QMetaType::QString,
	QMetaType::QString,
	QMetaType::UInt,
	QMetaType::Bool,
	QMetaType::QString,
	QMetaType::QString,
	QMetaType::QString,
	QMetaType::QString,
	QMetaType::QString,
};

bool IRCSettings::setData(const QModelIndex& idx, const QVariant& val, int role){
	if(role != Qt::EditRole) return false;
	if(!val.canConvert(col_types[idx.column()])) return false;

	IRCServerDetails& serv = *servers[idx.row()];

	switch(idx.column()){
		case IRC_DAT_UNIQUE_NAME:
			serv.unique_name = val.toString(); break;
		case IRC_DAT_NICK:
			serv.nickname = val.toString(); break;
		case IRC_DAT_ADDRESS:
			 serv.hostname = val.toString(); break;
		case IRC_DAT_PORT:
			 serv.port = val.toUInt(); break;
		case IRC_DAT_SSL:
			 serv.ssl = val.toBool(); break;
		case IRC_DAT_USERNAME:
			 serv.username = val.toString(); break;
		case IRC_DAT_REALNAME:
			 serv.realname = val.toString(); break;
		case IRC_DAT_PASSWORD:
			 serv.password = val.toString(); break;
		case IRC_DAT_NICKSERV:
			 serv.nickserv = val.toString(); break;
		case IRC_DAT_CMDS:
			 serv.commands = val.toString(); break;
		default: return false;
	}

	return true;
}

Qt::ItemFlags IRCSettings::flags(const QModelIndex&) const {
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void IRCSettings::newServer(){
	beginInsertRows(QModelIndex(), servers.size(), servers.size());
	servers.push_back(new IRCServerDetails);
	endInsertRows();
}

void IRCSettings::delServer(const QModelIndex& i){
	beginRemoveRows(QModelIndex(), i.row(), i.row());
	servers.erase(servers.begin() + i.row());
	endRemoveRows();
}


QStandardItemModel* IRCSettings::getChannelModel(const QModelIndex& idx){
	return &servers[idx.row()]->chans;
}


int IRCSettings::serverNameToID(const QString& name){
	return -1;
}

IRCServerDetails* IRCSettings::getDetails(int id){
	return nullptr;
}

int getOption(IRCOption opt){
	return 0;
}

void setOption(IRCOption opt, int val){

}
