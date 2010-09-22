#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QListWidgetItem>

class TypeItem;

// Constants
static const QString NEW_MESSAGE_NAME("New Message");

class Message : public QListWidgetItem {
public:
    Message(QListWidget *parent = 0);
    ~Message();

    // Const methods
    const QString &url() const;
    const QString &extraInfo() const;
    TypeItem *type() const;

    // Set methods
    void setUrl(const QString &url);
    void setExtraInfo(const QString &info);
    void setType(TypeItem *type);

private:
    QString _url;
    QString _extraInfo;
    TypeItem *_type;
};

#endif // MESSAGE_H
