#include "d7irc_data.h"

IRCSettings::IRCSettings()
: servers()
, settings(QSettings::IniFormat, QSettings::UserScope, "samurairc") {
	settings.setFallbacksEnabled(false);
	first_run = settings.value("main/first_run", true).toBool();
	settings.setValue("main/first_run", false);
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
