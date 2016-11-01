#include "d7irc_qt.h"
#include "d7irc_data.h"
#include <qnetworkreply.h>

void IRCExternalDownloader::checkMessage(const QString& msg, IRCBuffer* buf){

	QRegExp regex("https?://i\\.imgur\\.com/.*\\.[a-zA-Z]{3,4}");
	if(!msg.contains(regex)) return;

	QNetworkRequest req;
	req.setUrl(regex.cap(0));
	req.setRawHeader("User-Agent", "SamuraIRC");

	QNetworkReply* reply = net.get(req);
	connect(reply, &QNetworkReply::finished, [=](void){
		if(reply->error()){
			puts("ERROR");
			return;
		}

		QImage img = QImage::fromData(reply->readAll());
		if(img.isNull()){
			puts("null img");
			return;
		}

		if(img.height() > 250){
			img = img.scaledToHeight(250, Qt::SmoothTransformation);
		}

		buf->addImage(img);
	});
}
