#ifndef QFEEDBACK_H
#define QFEEDBACK_H

#include <QMainWindow>
#include <QSettings>
#include <QMessageBox>
#include <QDir>
#include <QCloseEvent>
#include <QClipboard>

#include "message.h"
#include "type.h"


namespace Ui {
    class MainWindow;
}

// Constants
static const QString ENDL = "\n";

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

private:
    Ui::MainWindow *ui;
    QSettings *settings;
    QDir *typeDir;
    bool somethingChanged;

    void initSlots();

private slots:
    void saveSplitterSizes();

    // Types
    void loadTypes();
    void saveTypes();
    void saveTypeClass(QListWidgetItem *previous);
    void checkTypesStatus();
    void refreshTypesInfo(QListWidgetItem *current, QListWidgetItem *previous);
    void updateCurrentTypeName();
    void addNewType();
    void deleteCurrentType();
    void clearTypesTexts();
    void reloadMessagesTypeSelect();

    // Messages
    void refreshMessageInfo(QListWidgetItem *current, QListWidgetItem *previous);
    void updateCurrentMessageName();
    void addNewMessage();
    void deleteCurrentMessage();
    void clearMessagesTexts();
    void saveMessageClass(QListWidgetItem *previous);

    // Export
    void loadHeader();
    void saveHeader();
    void loadFooter();
    void saveFooter();
    void copyMessage();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);
};

#endif // MAINWINDOW_H
