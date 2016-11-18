#include "d7irc_qt.h"
#include <qmimedata.h>

IRCServerDetails::IRCServerDetails(int id, const QString& name)
: id(id)
, unique_name(name)
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

IRCServerModel::IRCServerModel()
: id_counter(0)
, servers()
, settings(QSettings::IniFormat, QSettings::UserScope, "samurairc") {
	settings.setFallbacksEnabled(false);

	// load servers etc
	static QRegExp serv_key_regex("^serv\\..*");
	QStringList keys = settings.childGroups().filter(serv_key_regex);

	for(auto& k : keys){

		printf("key: %s\n", k.toUtf8().constData());

		// TODO: check user hasn't screwed up the uniqueness of the name via manually editing the .ini?
		IRCServerDetails* serv = new IRCServerDetails(id_counter++, k.mid(5));

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

		serv->autoconnect = settings.value("autoconnect", false).toBool();

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

IRCServerModel::~IRCServerModel(){

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

int IRCServerModel::rowCount(const QModelIndex&) const {
	return servers.size();
}

int IRCServerModel::columnCount(const QModelIndex&) const {
	return 11;
}

QVariant IRCServerModel::data(const QModelIndex& idx, int role) const {

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
		case IRC_DAT_AUTOCONNECT:
			return serv.autoconnect;
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
	QMetaType::Bool,
};

bool IRCServerModel::setData(const QModelIndex& idx, const QVariant& val, int role){
	if(role != Qt::EditRole) return false;
	if(!val.canConvert(col_types[idx.column()])) return false;

	IRCServerDetails& serv = *servers[idx.row()];

	switch(idx.column()){
		case IRC_DAT_UNIQUE_NAME: {
			QString wanted_name = val.toString();
			for(auto* s : servers){
				if(s->unique_name == wanted_name){
					return false;
				}
			}
#if 0
			// TODO: the buffers would need to be re-sorted?
			if(IRCBuffer* buf = SamuraIRC->buffers->findServer(serv.unique_name)){
				buf->name = wanted_name;
			}
#endif
			
			serv.unique_name = wanted_name;
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
		case IRC_DAT_AUTOCONNECT:
			 serv.autoconnect = val.toBool(); break;
		default: return false;
	}

	return true;
}

bool IRCServerModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int to_row, int to_col, const QModelIndex &parent) {
	if(!data->hasFormat("application/x-qabstractitemmodeldatalist")) return false;
	if(action != Qt::MoveAction) return false;
	if(parent.model() != this) return false;

	if(to_row == -1 && to_col == -1){
		to_row = parent.row();
		to_col = parent.column();
	}

	if(to_col != IRC_DAT_UNIQUE_NAME) return false;

	QByteArray  bytes  (data->data("application/x-qabstractitemmodeldatalist"));
	QDataStream stream (&bytes, QIODevice::ReadOnly);

	if(stream.atEnd()) return false;

	int from_row, from_col;
	stream >> from_row >> from_col;

	if(from_col != IRC_DAT_UNIQUE_NAME) return false;
	if(from_row == to_row) return false;

	if(from_row < to_row){
		beginMoveRows(QModelIndex(), from_row, from_row, QModelIndex(), to_row+1);
	} else {
		beginMoveRows(QModelIndex(), from_row, from_row, QModelIndex(), to_row);
	}
	auto* tmp = servers[from_row];
	servers.erase(servers.begin() + from_row);
	servers.insert(servers.begin() + to_row, tmp);
	endMoveRows();

	return true;
}

Qt::ItemFlags IRCServerModel::flags(const QModelIndex& idx) const {
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	if(idx.column() == IRC_DAT_UNIQUE_NAME){
		flags |= (Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
	}
	return flags;
}

Qt::DropActions IRCServerModel::supportedDropActions() const {
	return Qt::MoveAction;
}

void IRCServerModel::newServer(){
	QString new_name("New Server");
	bool found;
	int count = 0;

	do {
		found = true;
		for(auto* s : servers){
			if(s->unique_name == new_name){
				found = false;
				new_name = QString("New Server #%1").arg(++count);
				break;
			}
		}
	} while(!found);

	beginInsertRows(QModelIndex(), servers.size(), servers.size());
	servers.push_back(new IRCServerDetails(id_counter++, new_name));
	servers.back()->chans.push_back(IRCChanDetails{});
	endInsertRows();
}

void IRCServerModel::delServer(const QModelIndex& i){
	beginRemoveRows(QModelIndex(), i.row(), i.row());
	delete servers[i.row()];
	servers.erase(servers.begin() + i.row());
	endRemoveRows();
}

IRCServerDetails* IRCServerModel::getDetailsFromModelIdx(const QModelIndex& i){
	return servers[i.row()];
}

std::vector<IRCChanDetails>* IRCServerModel::getChannelDetails(const QModelIndex& idx){
	return &servers[idx.row()]->chans;
}


int IRCServerModel::serverNameToID(const QString& name){
	return -1;
}

IRCServerDetails* IRCServerModel::getDetails(int id){
	for(auto* s : servers){
		if(s->id == id){
			return s;
		}
	}

	return nullptr;
}

IRCServerDetails** IRCServerModel::begin(){
	return servers.data();
}

IRCServerDetails** IRCServerModel::end(){
	return servers.data() + servers.size();
}
