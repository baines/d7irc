#include "d7irc_qt.h"
#include "d7irc_data.h"
#include <qtexttable.h>
#include <qdebug.h>

static QColor colors[16] = {
	{ 0xff, 0xff, 0xff },
	{ 0x00, 0x00, 0x00 },
	{ 0x42, 0x7F, 0xED },
	{ 0x4E, 0x9A, 0x06 },
	{ 0xEF, 0x29, 0x29 },
	{ 0xCC, 0x00, 0x00 },
	{ 0x8A, 0x60, 0x9D },
	{ 0xC4, 0xA0, 0x00 },
	{ 0xFC, 0xE9, 0x4F },
	{ 0x8A, 0xE2, 0x34 },
	{ 0x06, 0x98, 0x9A },
	{ 0x34, 0xE2, 0xE2 },
	{ 0x72, 0x9F, 0xCF },
	{ 0xAD, 0x7F, 0xA8 },
	{ 0x55, 0x57, 0x53 },
	{ 0xD3, 0xD7, 0xCF },
};

static int set_formatting(const char32_t* str, QTextCharFormat& fmt){
	int skip = 0;

	switch(*str){
		case 0x02: {
			fmt.setFontWeight(fmt.fontWeight() == QFont::Normal ? QFont::Bold : QFont::Normal);
			skip = 1;
		} break;

		case 0x03: {
			unsigned int fg = 0, bg = 0;
			bool change_fg, change_bg;
			int i;

			for(i = 1; i < 3 && str[i]; ++i){
				if(str[i] >= '0' && str[i] <= '9'){
					change_fg = true;
					fg *= 10;
					fg += str[i] - '0';
				} else {
					break;
				}
			}

			if(str[i] == ','){
				++i;
				for(int j = 0; j < 2 && str[i]; ++i,++j){
					if(str[i] >= '0' && str[i] <= '9'){
						change_bg = true;
						bg *= 10;
						bg += str[i] - '0';
					} else {
						break;
					}
				}
			}

			if(change_fg && fg < 16) fmt.setForeground(colors[fg]);
			if(change_bg && bg < 16) fmt.setBackground(colors[bg]);

			skip = i;
		} break;

		case 0x0F: {
			fmt.setFontWeight(QFont::Normal);
			fmt.setFontUnderline(false);
			fmt.setFontItalic(false);
			fmt.clearForeground();
			fmt.clearBackground();
			skip = 1;
		} break;

		case 0x16: {
			// TODO reverse video
			skip = 1;
		} break;

		case 0x1D: {
			fmt.setFontItalic(!fmt.fontItalic());
			skip = 1;
		} break;

		case 0x1F: {
			fmt.setFontUnderline(!fmt.fontUnderline());
			skip = 1;
		} break;

		// TODO: on various word boundaries, look for the start of a link, i.e. "https?://"
		// and set the anchor format, or end it.
		/*
		case ' ':
		case '(':
		case '<':
		*/
	}

	return skip;
}

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

	char_fmt = QTextCharFormat();
	cursor.setCharFormat(char_fmt);
	cursor.setBlockFormat(block_fmt);

#if 0
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
#endif

	std::u32string str = msg.toStdU32String();
	for(int i = 0; i < str.size(); /**/){
		int skip = set_formatting(str.data() + i, char_fmt);
		if(skip == 0){
			QChar c(str[i]);
			cursor.insertText(QString(&c, 1));
			++i;
		} else {
			cursor.setCharFormat(char_fmt);
			i += skip;
		}
	}

	cursor.endEditBlock();
}

void IRCBuffer::addImage(const QImage& img){
	cursor.currentTable()->appendRows(1);
	cursor.movePosition(QTextCursor::NextRow);
	cursor.insertImage(img);
}
