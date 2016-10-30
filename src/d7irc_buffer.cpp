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

		cursor.insertTable(1, 2, fmt);
	} else {
		cursor.currentTable()->appendRows(1);
		cursor.movePosition(QTextCursor::NextRow);
		cursor.movePosition(QTextCursor::PreviousCell);
	}

	QTextBlockFormat fmt;
	fmt.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	fmt.setTopMargin(0.0);
	fmt.setLeftMargin(0.0);
	fmt.setBottomMargin(0.0);
	fmt.setRightMargin(0.0);
	fmt.setIndent(0.0);
	fmt.setTextIndent(0.0);
	fmt.setNonBreakableLines(true);
	cursor.setBlockFormat(fmt);

	QTextCharFormat wtf;
	wtf.setForeground(IRCUserModel::nickColor(prefix));
	cursor.setCharFormat(wtf);

	cursor.insertText(prefix);

	wtf.setForeground(QColor(0, 255, 0));
	cursor.setCharFormat(wtf);
	cursor.insertText(" | ");

	cursor.movePosition(QTextCursor::NextCell);

	fmt.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	cursor.setCharFormat(QTextCharFormat());
	cursor.setBlockFormat(fmt);
	cursor.insertText(msg);
}
