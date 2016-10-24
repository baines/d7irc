#include "irc_moc.h"
#include <unistd.h>

IRCWorker::IRCWorker(QThread* thread)
{
	this->moveToThread(thread);
}

void IRCWorker::begin()
{
	while(1){
		usleep(1000 * 1000);
		emit privmsg("boop");
	}
}
