#include <cassert>
#include "d7irc_qt.h"
#include "add_server_ui.h"

QWidget* IRCChanPassDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& s, const QModelIndex& idx) const {
	QLineEdit* edit = static_cast<QLineEdit*>(QStyledItemDelegate::createEditor(parent, s, idx));
	if(idx.column() == 1){
		edit->setEchoMode(QLineEdit::Password);
	}

	connect(edit, &QLineEdit::textChanged, [=](){
		emit updated(idx, edit->text());
	});

	return edit;
}


void IRCChanPassDelegate::paint(QPainter* p, const QStyleOptionViewItem& s, const QModelIndex& idx) const {
	if(idx.column() == 1){
		// do nothing for now
		// TODO: if i can figure out how to draw the password dots here, do that
	} else {
		QStyledItemDelegate::paint(p, s, idx);
	}
}

IRCAddServerUI::IRCAddServerUI()
: QDialog(SamuraIRC->ui_main)
, ui(new Ui::AddServer)
, mapper()
, chan_model()
, icon_connect(":/main/connect.png")
, icon_disconnect(":/main/disconnect.png"){

	ui->setupUi(this);
	setModal(true);
	ui->chans->setModel(&chan_model);
}

void IRCAddServerUI::hookStuffUp(){

	// have username / realname mimic nick via placeholder text
	connect(ui->nick, &QLineEdit::textChanged, [this](){
		QString text = ui->nick->text();

		if(text.isEmpty()){
			text = ui->nick->placeholderText();
		}

		ui->username->setPlaceholderText(text);
		ui->realname->setPlaceholderText(text);
	});

	if(SamuraIRC->servers->rowCount() == 0){
		SamuraIRC->servers->newServer();
	}

	// widget mapping
	ui->serv_list->setModel(SamuraIRC->servers);
	mapper.setModel(SamuraIRC->servers);

	mapper.addMapping(ui->nick    , IRC_DAT_NICK);
	mapper.addMapping(ui->address , IRC_DAT_ADDRESS);
	mapper.addMapping(ui->port    , IRC_DAT_PORT);
	mapper.addMapping(ui->ssl     , IRC_DAT_SSL);

	mapper.addMapping(ui->username, IRC_DAT_USERNAME);
	mapper.addMapping(ui->realname, IRC_DAT_REALNAME);
	mapper.addMapping(ui->password, IRC_DAT_PASSWORD);
	mapper.addMapping(ui->nickserv, IRC_DAT_NICKSERV);

	mapper.addMapping(ui->cmds, IRC_DAT_CMDS);

	mapper.addMapping(ui->autoconnect, IRC_DAT_AUTOCONNECT);

	// server selection change
	connect(
		ui->serv_list->selectionModel(), &QItemSelectionModel::currentRowChanged,
		[this](const QModelIndex& idx, const QModelIndex&){

			// update models
			mapper.setCurrentIndex(idx.row());
			auto* vec = SamuraIRC->servers->getChannelDetails(idx);
			chan_model.setChannels(vec);

			// disable delete button if connected
			IRCServerDetails*  serv = SamuraIRC->servers->getDetailsFromModelIdx(idx);
			IRCConnectionInfo* info = SamuraIRC->connections->getInfo(serv->id);
			ui->btn_delserv->setEnabled(!info);

			// update connect button
			// TODO: disconnected state probably unnecessary
			if(!info || info->status == IRC_CON_DISCONNECTED){
				ui->btn_connect->setText("Connect");
				ui->btn_connect->setIcon(icon_connect);
				ui->btn_connect->setEnabled(!ui->address->text().isEmpty());
			} else if(info->status == IRC_CON_CONNECTED){
				ui->btn_connect->setText("Disconnect");
				ui->btn_connect->setEnabled(true);
				ui->btn_connect->setIcon(icon_disconnect);
			} else if(info->status == IRC_CON_CONNECTING){
				// TODO: we might want to be able to disconnect even while the connection
				// is in progress...
				ui->btn_connect->setText("Connecting...");
				ui->btn_connect->setEnabled(false);
			}
		}
	);

	ui->serv_list->setCurrentIndex(SamuraIRC->servers->index(0, 0));

	// + button
	connect(ui->btn_addserv, &QAbstractButton::clicked, [=](bool){
		SamuraIRC->servers->newServer();
	});

	// - button
	connect(ui->btn_delserv, &QAbstractButton::clicked, [=](bool){
		// TODO: clear all fields if pressed when only one server?
		if(SamuraIRC->servers->rowCount() > 1){
			QModelIndex idx = ui->serv_list->currentIndex();
			IRCServerDetails* serv = SamuraIRC->servers->getDetailsFromModelIdx(idx);
			SamuraIRC->servers->delServer(idx);
		}
	});

	// (dis)connect button
	connect(ui->btn_connect, &QAbstractButton::clicked, [this](bool){
		auto* serv = SamuraIRC->servers->getDetailsFromModelIdx(ui->serv_list->currentIndex());
		auto* info = SamuraIRC->connections->getInfo(serv->id);

		if(info){ // disconnect
			SamuraIRC->connections->destroyConnection(serv->id);
			ui->btn_connect->setText("Connect");
			ui->btn_connect->setEnabled(true);
			ui->btn_connect->setIcon(icon_connect);
		} else {  // connect
			SamuraIRC->connections->createConnection(serv->id);
			ui->btn_connect->setText("Connecting...");
			ui->btn_connect->setEnabled(false);
			ui->btn_connect->setIcon(icon_disconnect);
		}
	});

	// disable connect if no addr
	connect(ui->address, &QLineEdit::textChanged, [this](){
		ui->btn_connect->setEnabled(!ui->address->text().isEmpty());
	});

	// password echo for channels
	auto* delegate = new IRCChanPassDelegate;
	ui->chans->setItemDelegate(delegate);

	// handle adding / removing rows in the channel table
	connect(delegate, &IRCChanPassDelegate::updated, [this](const QModelIndex& idx, const QString& txt){
		auto* model = qobject_cast<IRCChannelModel*>(ui->chans->model());
		assert(model);

		if(!txt.isEmpty() && idx.row() == model->rowCount()-1){
			model->push();
		} else if(txt.isEmpty() && idx.row() == model->rowCount()-2){
			model->pop();
		}
	});

	connect(&chan_model, &QAbstractItemModel::dataChanged, [this](const QModelIndex& idx, const QModelIndex&){
		QString name = chan_model.data(chan_model.index(idx.row(), 0), Qt::EditRole).toString();
		QString pass = chan_model.data(chan_model.index(idx.row(), 1), Qt::EditRole).toString();

		if(name.isEmpty() && pass.isEmpty() && idx.row() != (chan_model.rowCount() - 1)){
			chan_model.remove(idx.row());
		}
	});

	// advanced toggle
	connect(ui->group_adv, &QGroupBox::toggled, [this](bool on){
		auto& children = ui->group_adv->children();
		for(auto* c : children){
			auto* w = qobject_cast<QWidget*>(c);
			if(w) w->setVisible(on);
		}

		ui->group_cmds->setVisible(on);
		ui->group_adv->setFlat(!on);
		ui->group_adv->setStyleSheet(on ? "" : "border:none;");
	});
	ui->group_adv->setChecked(false);

	// tracking connection changes
	// TODO: probably should disable all widgets not just connect button
	connect(
		SamuraIRC->connections, &IRCConnectionRegistry::statusChanged,
		[this](int id, IRCConnectionStatus status){
			auto* serv = SamuraIRC->servers->getDetailsFromModelIdx(ui->serv_list->currentIndex());
			if(serv->id != id) return;

			ui->btn_connect->setEnabled(status != IRC_CON_CONNECTING);
			switch(status){
				case IRC_CON_CONNECTING: {
					ui->btn_connect->setText("Connecting...");
				} break;
				case IRC_CON_CONNECTED: {
					ui->btn_connect->setText("Disconnect");
				} break;
				case IRC_CON_DISCONNECTED: {
					ui->btn_connect->setText("Connect");
				} break;
			}
		}
	);

	// TODO: when server name changes, update buffer if it exists
}
