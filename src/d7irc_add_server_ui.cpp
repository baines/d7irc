#include "d7irc_qt.h"
#include "add_server_ui.h"

IRCAddServerUI::IRCAddServerUI()
: ui(new Ui::AddServer){
	ui->setupUi(this);

	connect(ui->nick, &QLineEdit::textChanged, [this](){
		QString text = ui->nick->text();

		if(text.isEmpty()){
			text = ui->nick->placeholderText();
		}

		ui->username->setPlaceholderText(text);
		ui->realname->setPlaceholderText(text);
	});

}
