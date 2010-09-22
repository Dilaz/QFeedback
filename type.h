#ifndef TYPE_H
#define TYPE_H

#include <QString>
#include <QListWidgetItem>

// Constants
static const QString NEW_TYPE_NAME("New Type");

class TypeItem : public QListWidgetItem {
public:
    TypeItem(QListWidget *parent = 0);
    TypeItem(const QString &name, const QString &headerText = "",
               const QString &extraInfo = "", QListWidget *parent = 0);
    ~TypeItem();


    const QString &name() const;
    const QString &headerText() const;
    const QString &extraInfo() const;

    void setName(const QString &name);
    void setHeaderText(const QString &headerText);
    void setExtraInfo(const QString &extraInfo);

private:
    QString _name;
    QString _headerText;
    QString _extraInfo;
};

#endif // TYPE_H
