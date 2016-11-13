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
, mapper() {

	ui->setupUi(this);
	setModal(true);

	// have username / realname mimic nick via placeholder text
	connect(ui->nick, &QLineEdit::textChanged, [this](){
		QString text = ui->nick->text();

		if(text.isEmpty()){
			text = ui->nick->placeholderText();
		}

		ui->username->setPlaceholderText(text);
		ui->realname->setPlaceholderText(text);
	});

	if(SamuraIRC->settings->rowCount() == 0){
		SamuraIRC->settings->newServer();
	}

	// widget mapping
	ui->serv_list->setModel(SamuraIRC->settings);
	mapper.setModel(SamuraIRC->settings);

	mapper.addMapping(ui->nick    , IRC_DAT_NICK);
	mapper.addMapping(ui->address , IRC_DAT_ADDRESS);
	mapper.addMapping(ui->port    , IRC_DAT_PORT);
	mapper.addMapping(ui->ssl     , IRC_DAT_SSL);

	mapper.addMapping(ui->username, IRC_DAT_USERNAME);
	mapper.addMapping(ui->realname, IRC_DAT_REALNAME);
	mapper.addMapping(ui->password, IRC_DAT_PASSWORD);
	mapper.addMapping(ui->nickserv, IRC_DAT_NICKSERV);

	mapper.addMapping(ui->cmds, IRC_DAT_CMDS);

	connect(
		ui->serv_list->selectionModel(), &QItemSelectionModel::currentRowChanged,
		[this](const QModelIndex& idx, const QModelIndex&){
			mapper.setCurrentIndex(idx.row());
			ui->chans->setModel(SamuraIRC->settings->getChannelModel(idx));
		}
	);

	ui->serv_list->setCurrentIndex(SamuraIRC->settings->index(0, 0));

	// + button
	connect(ui->btn_addserv, &QAbstractButton::clicked, [=](bool){
		SamuraIRC->settings->newServer();
	});

	// - button
	connect(ui->btn_delserv, &QAbstractButton::clicked, [=](bool){
		// TODO: clear all fields if pressed when only one server?
		if(SamuraIRC->settings->rowCount() > 1){
			QModelIndex idx = ui->serv_list->currentIndex();
			SamuraIRC->settings->delServer(idx);
		}
	});

	// connect button enable/disable
	connect(ui->address, &QLineEdit::textChanged, [this](const QString& txt){
		// TODO: store connected state in the server details,
		// updated by the IRCConnection
		if(ui->btn_connect->text() == "Connect"){
			ui->btn_connect->setEnabled(!txt.isEmpty());
		}
	});

	connect(ui->btn_connect, &QAbstractButton::clicked, [this](bool){
		// TODO: get the state from IRCServerDetails, not based on label
		if(ui->btn_connect->text() == "Connect"){
			ui->btn_connect->setEnabled(false);
			ui->btn_connect->setText("Connecting...");
		}
	});

	// password echo for channels
	auto* delegate = new IRCChanPassDelegate;
	ui->chans->setItemDelegate(delegate);

	// TODO: on edit finished, sort channels / handle deletion of blank rows?
	connect(delegate, &IRCChanPassDelegate::updated, [this](const QModelIndex& idx, const QString& txt){
		auto* model = ui->chans->model();
		if(!txt.isEmpty() && idx.row() == model->rowCount()-1){
			model->insertRows(model->rowCount(), 1);
		} else if(txt.isEmpty() && idx.row() == model->rowCount()-2){
			model->removeRows(model->rowCount()-1, 1);
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
}
