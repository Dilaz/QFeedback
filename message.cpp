#include "message.h"

Message::Message(QListWidget *parent)
    : QListWidgetItem(parent), _type(0) {
}


Message::~Message() {

}


// Get methods
const QString &Message::url() const { return _url; }
const QString &Message::extraInfo() const { return _extraInfo; }
TypeItem *Message::type() const { return _type; }

// Set methods
void Message::setUrl(const QString &url) { _url = url; }
void Message::setExtraInfo(const QString &info) { _extraInfo = info; }
void Message::setType(TypeItem *type) { _type = type; }
