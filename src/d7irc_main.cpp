#include <qobject.h>
#include <qapplication.h>
#include "d7irc_qt.h"
#include "d7irc_ui.h"

int main(int argc, char** argv){
	QApplication app(argc, argv);
	QMainWindow* win = new QMainWindow;

	Ui::SamuraIRC ui;
	ui.setupUi(win);

	QThread* irc_thread = new QThread;
	IRCWorker* worker = new IRCWorker(irc_thread);

	QObject::connect(worker, &IRCWorker::privmsg, ui.chat_lines, &QTextEdit::append);
	QObject::connect(irc_thread, &QThread::started, worker, &IRCWorker::begin);

//	QObject::connect(ui.text_input, &IRCTextEntry::textSubmit, worker, &

	irc_thread->start();

	win->show();
	return app.exec();
}
