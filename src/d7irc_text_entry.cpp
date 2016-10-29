#include "d7irc_qt.h"

IRCTextEntry::IRCTextEntry(QWidget*& w)
: QTextEdit(w)
, line_count(1)
{
	line_height = QFontMetrics(document()->defaultFont()).lineSpacing() + 1;
}

void IRCTextEntry::keyPressEvent(QKeyEvent* ev)
{
	// TODO: parse commands in here

	if(ev->key() == Qt::Key_Return){
		if(!(ev->modifiers() & Qt::ShiftModifier)){
//			emit textSubmit();
			setMaximumHeight(minimumHeight());
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
