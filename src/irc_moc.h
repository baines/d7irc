#ifndef IRC_MOC_H
#define IRC_MOC_H
#include <qevent.h>
#include <qtextedit.h>
#include <qobject.h>
#include <qthread.h>


class IRCTextEntry : public QTextEdit {
	Q_OBJECT;
public:
	IRCTextEntry(QWidget*& w);
	void keyPressEvent(QKeyEvent* ev) override;
signals:
	void textSubmit(void);
private:
	int line_height;
	int line_count;
};


class IRCWorker : public QObject {
	Q_OBJECT;
public:
	IRCWorker(QThread* thread);
signals:
	void privmsg(const QString& text);
public slots:
	void begin();
};


#endif
