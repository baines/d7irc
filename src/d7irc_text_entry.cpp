#include "d7irc_qt.h"

IRCTextEntry::IRCTextEntry(QWidget*& w)
: QTextEdit(w)
, line_count(1)
{
	line_height = QFontMetrics(document()->defaultFont()).lineSpacing() + 1;
}

void IRCTextEntry::keyPressEvent(QKeyEvent* ev)
{
	switch(ev->key()){
		case Qt::Key_Return: {
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
		} break;

		case Qt::Key_Tab: {
			// nick completion
		} break;

		default: {
			QTextEdit::keyPressEvent(ev);
		}
	}
}

void IRCTextEntry::handleSubmit()
{
	//TODO: commands, encode formatting

	QString str = toPlainText();

	if(str.startsWith("/")){
		if(!str.startsWith("//")){
			emit command(str.mid(1));
			return;
		} else {
			str.remove(0, 1);
		}
	}

	// text
	emit textSubmit(str);
}
