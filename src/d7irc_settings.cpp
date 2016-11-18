#include "d7irc_qt.h"

IRCServerDetails::IRCServerDetails(int id)
: id(id)
, unique_name("New Server") // TODO: make it actually unique...
, nickname()
, hostname()
, port(6667)
, ssl(false)
, username()
, password()
, nickserv()
, commands()
, chans()
, autoconnect(false) {

}

IRCSettings::IRCSettings()
: first_run(false)
, id_counter(0)
, servers()
, settings(QSettings::IniFormat, QSettings::UserScope, "samurairc") {
	settings.setFallbacksEnabled(false);

	// load servers etc
	static QRegExp serv_key_regex("^serv\\..*");
	QStringList keys = settings.childGroups().filter(serv_key_regex);

	if(keys.isEmpty()){
		first_run = true;
	}

	for(auto& k : keys){

		printf("key: %s\n", k.toUtf8().constData());

		IRCServerDetails* serv = new IRCServerDetails(id_counter++);

		serv->unique_name = k.mid(5);

		settings.beginGroup(k);

		serv->nickname = settings.value("nickname", "samurai").toString();
		serv->hostname = settings.value("hostname", "").toString();
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
				serv->chans.push_back(IRCChanDetails { name, pass });
			}
		}
		settings.endArray();
		settings.endGroup();

		serv->chans.push_back(IRCChanDetails{});

		servers.push_back(serv);
	}

}

IRCSettings::~IRCSettings(){

	settings.clear();
	settings.setValue("main/first_run", false);
	settings.sync();
	{
		QFile file(settings.fileName());
		file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
	}

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
		for(int i = 0; i < s->chans.size(); ++i){
			settings.setArrayIndex(i);

			const QString& name = s->chans[i].name;
			const QString& pass = s->chans[i].pass;

			if(!name.isEmpty()){
				settings.setValue("name", name);
				settings.setValue("pass", pass);
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

	if(role == Qt::SizeHintRole && idx.column() == 0){
		return QSize(-1, 20);
	}

	if(role != Qt::DisplayRole && role != Qt::EditRole){
		return QVariant();
	}

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
		case IRC_DAT_UNIQUE_NAME: {
			// TODO: change buffer name if it exists
			serv.unique_name = val.toString();
		} break;
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
	servers.push_back(new IRCServerDetails(id_counter++));
	servers.back()->chans.push_back(IRCChanDetails{});
	endInsertRows();
}

void IRCSettings::delServer(const QModelIndex& i){
	beginRemoveRows(QModelIndex(), i.row(), i.row());
	delete servers[i.row()];
	servers.erase(servers.begin() + i.row());
	endRemoveRows();
}

IRCServerDetails* IRCSettings::getDetailsFromModelIdx(const QModelIndex& i){
	return servers[i.row()];
}

std::vector<IRCChanDetails>* IRCSettings::getChannelDetails(const QModelIndex& idx){
	return &servers[idx.row()]->chans;
}


int IRCSettings::serverNameToID(const QString& name){
	return -1;
}

IRCServerDetails* IRCSettings::getDetails(int id){
	for(auto* s : servers){
		if(s->id == id){
			return s;
		}
	}

	return nullptr;
}

int getOption(IRCOption opt){
	return 0;
}

void setOption(IRCOption opt, int val){

}

IRCServerDetails** IRCSettings::begin(){
	return servers.data();
}

IRCServerDetails** IRCSettings::end(){
	return servers.data() + servers.size();
}
