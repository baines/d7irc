#ifndef D7IRC_QT_H
#define D7IRC_QT_H
#include <qevent.h>
#include <qtextedit.h>
#include <qobject.h>
#include <qthread.h>
#include <qtimer.h>
#include <libircclient.h>


class IRCTextEntry : public QTextEdit {
	Q_OBJECT;
public:
	IRCTextEntry(QWidget*& w);
	void keyPressEvent(QKeyEvent* ev) override;
signals:
	void textSubmit(const QString& text);
private:
	int line_height;
	int line_count;
};


class IRCWorker : public QObject {
	Q_OBJECT;
public:
	IRCWorker(QThread* thread);
signals:
	void privmsg (const QString& text);
	void join    (const QString& nick, const QString& chan);
	void part    (const QString& nick, const QString& chan);
public slots:
	void begin();
	void tick();
	void sendPrivmsg(const QString& target, const QString& msg);
	void sendRaw(const QString& raw);
private:
	int serv_id;
	irc_session_t* irc_session;
	QTimer* timer;
};


class IRCController : public QObject {
	Q_OBJECT;
public:
	IRCController() = default;
public slots:
//	void addChannel(int serv_id, const QString& chan);
//	void delChannel(int serv_id, const QString& chan);
};
#endif
