#include "type.h"

TypeItem::TypeItem(QListWidget *parent) : QListWidgetItem(parent) {
}

TypeItem::TypeItem(const QString &name, const QString &headerText,
           const QString &extraInfo, QListWidget *parent) :
QListWidgetItem(parent),
_name(name), _headerText(headerText), _extraInfo(extraInfo) {
    setText(_name);
}


TypeItem::~TypeItem() {
}


// Get-methods
const QString &TypeItem::name() const { return _name; }
const QString &TypeItem::headerText() const { return _headerText; }
const QString &TypeItem::extraInfo() const { return _extraInfo; }

// Set-methods
void TypeItem::setName(const QString &name) { _name = name; setText(_name); }
void TypeItem::setHeaderText(const QString &headerText) { _headerText = headerText; }
void TypeItem::setExtraInfo(const QString &extraInfo) { _extraInfo = extraInfo; }
