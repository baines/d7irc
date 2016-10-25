#ifndef D7IRC_QT_H
#define D7IRC_QT_H
#include "d7irc_data.h"
#include <qevent.h>
#include <qtextedit.h>
#include <qobject.h>
#include <qthread.h>
#include <qtimer.h>
#include <qabstractitemmodel.h>
#include <qabstractitemview.h>
#include <qitemdelegate.h>
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
signals:
	void serverAdded (const QModelIndex& idx);
private:
	IRCBuffer* root;
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


struct WTFINeedThisJustToSetTheRowHeight : public QItemDelegate {
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
		QSize sz = QItemDelegate::sizeHint(option, index);
		sz.setHeight(20);
		return sz;
	}
};

// TODO: thread for curl_multi to get images / code snippets
#endif
