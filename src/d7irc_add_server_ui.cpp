#include "d7irc_data.h"
#include "add_server_ui.h"

IRCAddServerUI::IRCAddServerUI(QWidget* parent)
: QDialog(parent)
, ui(new Ui::AddServer)
, mapper() {

	ui->setupUi(this);

	// have username / realname mimic nick via placeholder text
	connect(ui->nick, &QLineEdit::textChanged, [this](){
		QString text = ui->nick->text();

		if(text.isEmpty()){
			text = ui->nick->placeholderText();
		}

		ui->username->setPlaceholderText(text);
		ui->realname->setPlaceholderText(text);
	});

	if(d7irc_settings->rowCount() == 0){
		d7irc_settings->newServer();
	}

	// widget mapping
	ui->serv_list->setModel(d7irc_settings);
	mapper.setModel(d7irc_settings);

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
			printf("idx change %d\n", idx.row());
			mapper.setCurrentIndex(idx.row());
		}
	);

	connect(
		ui->serv_list->selectionModel(), &QItemSelectionModel::currentChanged,
		[this](const QModelIndex& idx, const QModelIndex&){
			ui->chans->setModel(d7irc_settings->getChannelModel(idx));
		}
	);

	ui->serv_list->setCurrentIndex(d7irc_settings->index(0, 0));

	// + button
	connect(ui->btn_addserv, &QAbstractButton::clicked, [](bool){
		d7irc_settings->newServer();
	});

	// - button
	connect(ui->btn_delserv, &QAbstractButton::clicked, [this](bool){
		// TODO: clear all fields if pressed when only one server?
		if(d7irc_settings->rowCount() > 1){
			QModelIndex idx = ui->serv_list->currentIndex();
			d7irc_settings->delServer(idx);
		}
	});

}
