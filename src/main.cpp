#include <qobject.h>
#include <qapplication.h>
#include "irc_moc.h"
#include "irc_ui.h"

int main(int argc, char** argv){
    QApplication app(argc, argv);
	QMainWindow* win = new QMainWindow;

    Ui::SamuraIRC ui;
    ui.setupUi(win);

	QThread* irc_thread = new QThread;
	IRCWorker* worker = new IRCWorker(irc_thread);

	QObject::connect(worker, &IRCWorker::privmsg, ui.chat_lines, &QTextEdit::append);
	QObject::connect(irc_thread, &QThread::started, worker, &IRCWorker::begin);

	irc_thread->start();

    win->show();
    return app.exec();
}
