#include "d7irc_data.h"
#include <qtexttable.h>

IRCBuffer::IRCBuffer(IRCBufferType type, const QString& name, IRCBuffer* parent)
: type(type)
, name(name)
, contents(new QTextDocument)
, nicks()
, parent(parent)
, child(nullptr)
, sibling(nullptr)
, cursor(contents) {

	contents->setDefaultFont(QFont("DejaVu Sans Mono", 9));
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

	QString msg2 = msg.toHtmlEscaped();
	msg2.replace(
		QRegExp("\\b((https?://|ftp://|mailto:|gopher://)[^ ]+)"),
		"<a href=\"\\1\"><span style='color: #308CC6'>\\1</span></a>"
	);
	cursor.insertHtml(msg2);
}

void IRCBuffer::addImage(const QImage& img){
	cursor.currentTable()->appendRows(1);
	cursor.movePosition(QTextCursor::NextRow);
	cursor.insertImage(img);
}
