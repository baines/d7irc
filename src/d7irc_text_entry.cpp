#include "d7irc_qt.h"

IRCTextEntry::IRCTextEntry(QWidget*& w)
: QTextEdit(w)
, line_count(1)
{
	line_height = QFontMetrics(document()->defaultFont()).lineSpacing() + 1;
}

void IRCTextEntry::keyPressEvent(QKeyEvent* ev)
{
	if(ev->key() == Qt::Key_Return){
		if(!(ev->modifiers() & Qt::ShiftModifier)){
			setMaximumHeight(minimumHeight());
			handleSubmit();
			clear();
			line_count = 1;
		} else {
			if(++line_count < 16){
				setMaximumHeight(maximumHeight() + line_height);
			}
			QTextEdit::keyPressEvent(ev);
		}
	} else {
		QTextEdit::keyPressEvent(ev);
	}
}

void IRCTextEntry::handleSubmit()
{
	//TODO: commands, encode formatting

	QString str = toPlainText();

	if(str.startsWith("/")){
		if(!str.startsWith("//")){
			// command
			return;
		} else {
			str.remove(0, 1);
		}
	}

	// text
	emit textSubmit(str);
}
