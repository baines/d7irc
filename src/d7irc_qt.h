#ifndef D7IRC_QT_H
#define D7IRC_QT_H
#include <qevent.h>
#include <qtextedit.h>
#include <qobject.h>
#include <qthread.h>
#include <qtimer.h>
#include <qabstractitemmodel.h>
#include <qabstractitemview.h>
#include <qitemdelegate.h>
#include <libircclient.h>

struct IRCBuffer;


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
	QString server;
signals:
	void privmsg (const QString& text);
	void connect (const QString& serv);
	void join    (const QString& nick, const QString& chan);
	void part    (const QString& nick, const QString& chan);
public slots:
	void begin();
	void tick();
	void sendPrivmsg(const QString& target, const QString& msg);
	void sendRaw(const QString& raw);
private:
	irc_session_t* irc_session;
	QTimer* timer;
};


class IRCModel : public QAbstractItemModel {
	Q_OBJECT;
public:
	IRCModel();
	
	QModelIndex index   (int row, int col, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent  (const QModelIndex& idx) const override;
	int rowCount        (const QModelIndex& parent = QModelIndex()) const override;
	int columnCount     (const QModelIndex& parent = QModelIndex()) const override;
	QVariant data       (const QModelIndex& idx, int role = Qt::DisplayRole) const override;
	Qt::ItemFlags flags (const QModelIndex& idx) const override;

	IRCBuffer* addServer  (const QString& name);
	IRCBuffer* addChannel (const QString& serv, const QString& chan);

	void addNick (const QString& serv, const QString& chan, const QString& nick);
	void delNick (const QString& serv, const QString& chan, const QString& nick);

signals:
	void serverAdded (const QModelIndex& idx);
private:
	IRCBuffer* root;
};


class IRCNicksModel : public QAbstractListModel {
	Q_OBJECT;
public:
	IRCNicksModel();

	int rowCount  (const QModelIndex& parent = QModelIndex()) const override;
	QVariant data (const QModelIndex& idx, int role = Qt::DisplayRole) const override;

	void addNick (const QString& nick);
	void delNick (const QString& nick);

	void changeNick (const QString& from, const QString& to);
private:
	std::vector<QString> nicks;
};


class IRCController : public QObject {
	Q_OBJECT;
public:
	IRCController() = default;
public slots:
//	void addChannel(int serv_id, const QString& chan);
//	void delChannel(int serv_id, const QString& chan);
//	void goToChannel(const QString& chan); // int id instead?
//
};

// TODO: thread for curl_multi to get images / code snippets
#endif
