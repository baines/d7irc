#include "d7irc_qt.h"
#include "d7irc_data.h"
#include <qtexttable.h>
#include <qdebug.h>

IRCBuffer::IRCBuffer(IRCBufferType type, const QString& name, IRCBuffer* parent)
: type(type)
, name(name)
, contents(new QTextDocument)
, cursor(contents)
, nicks(new IRCUserModel)
, active(type == IRC_BUF_INTERNAL)
, level(IRC_BUFLVL_NORMAL)
, parent(parent)
, child(nullptr)
, sibling(nullptr) {
	contents->setMaximumBlockCount(8192);
	contents->setUndoRedoEnabled(false);

	QFont f("Terminus", 9);
	f.setHintingPreference(QFont::PreferVerticalHinting);
	f.setStyleStrategy((QFont::StyleStrategy)(QFont::NoAntialias | QFont::PreferQuality | QFont::ForceIntegerMetrics));
	contents->setDefaultFont(f);
}

void IRCBuffer::addLine(const QString& prefix, const QString& msg){

	if(!cursor.currentTable()){
		QTextTableFormat fmt;

		fmt.setCellSpacing(2.0);
		fmt.setCellPadding(0.0);
		fmt.setBorder(0.0);
		fmt.setHeaderRowCount(0);
		fmt.setTopMargin(0.0);
		fmt.setLeftMargin(0.0);
		fmt.setBottomMargin(0.0);
		fmt.setRightMargin(0.0);

		cursor.insertTable(1, 3, fmt);
	} else {
		cursor.currentTable()->appendRows(1);
		cursor.movePosition(QTextCursor::NextRow);
		cursor.movePosition(QTextCursor::PreviousCell);
		cursor.movePosition(QTextCursor::PreviousCell);
	}

	cursor.beginEditBlock();

	QTextCharFormat  char_fmt;
	QTextBlockFormat block_fmt;

	block_fmt.setAlignment    (Qt::AlignLeft | Qt::AlignVCenter);
	block_fmt.setTopMargin    (0.0);
	block_fmt.setLeftMargin   (0.0);
	block_fmt.setBottomMargin (0.0);
	block_fmt.setRightMargin  (0.0);
	block_fmt.setIndent       (0.0);
	block_fmt.setTextIndent   (0.0);

	cursor.setBlockFormat(block_fmt);

	// timestamp
	cursor.insertText(QTime::currentTime().toString());
	cursor.insertText(" ");
	
	// nick
	cursor.movePosition(QTextCursor::NextCell);
	block_fmt.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	cursor.setBlockFormat(block_fmt);

	char_fmt.setForeground(IRCUserModel::nickColor(prefix));
	cursor.setCharFormat(char_fmt);

	cursor.insertText(prefix);

	// nick separator
	char_fmt.setForeground(QColor(0, 125, 0));
	cursor.setCharFormat(char_fmt);
	cursor.insertText(" | ");

	// message
	cursor.movePosition(QTextCursor::NextCell);

	block_fmt.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	cursor.setCharFormat(QTextCharFormat());
	cursor.setBlockFormat(block_fmt);

	static QRegExp link_regex("\\b((https?://|ftp://|mailto:|gopher://)[^ ]+)");

	int prev_pos = 0, pos = 0;

	while((pos = link_regex.indexIn(msg, prev_pos)) != -1){
		QStringRef plain_text (&msg, prev_pos, pos - prev_pos);
		QStringRef link_text  (&msg, pos, link_regex.matchedLength());

		cursor.insertText(plain_text.toString());

		QString link = QString("<a href=\"%1\"><span style='color: #159FFB'>%1</span></a>").arg(link_text.toString().toHtmlEscaped());
		cursor.insertHtml(link);
		
		cursor.setCharFormat(QTextCharFormat());
		cursor.setBlockFormat(block_fmt);

		prev_pos = pos + link_regex.matchedLength();
	}

	QStringRef plain_text(&msg, prev_pos, msg.size() - prev_pos);
	cursor.insertText(plain_text.toString());

	cursor.endEditBlock();
}

void IRCBuffer::addImage(const QImage& img){
	cursor.currentTable()->appendRows(1);
	cursor.movePosition(QTextCursor::NextRow);
	cursor.insertImage(img);
}
