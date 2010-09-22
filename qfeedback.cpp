#include "qfeedback.h"
#include "ui_qfeedback.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), somethingChanged(false) {
    ui->setupUi(this);
    initSlots();

    // Load settings
    settings = new QSettings("settings.ini", QSettings::IniFormat, this);

    // Restores splitter states
    settings->beginGroup("GUI");
    ui->messagesSplitter->restoreState(settings->value("MessagesSplitterSize").toByteArray());
    ui->typesSplitter->restoreState(settings->value("TypesSplitterSize").toByteArray());
    settings->endGroup();

    // Load the types
    loadTypes();

    // Load header & footer texts
    loadHeader();
    loadFooter();
}

MainWindow::~MainWindow() {
    // Delete message items
    for (int row = 0; row < ui->messageList->count(); row++) {
        // Delete item
        QListWidgetItem *item = ui->messageList->item(row);
        ui->messageList->removeItemWidget(item);
        delete item;
    }

    // Delete type items
    for (int row = 0; row < ui->typesList->count(); row++) {
        // Delete item
        QListWidgetItem *item = ui->typesList->item(row);
        ui->typesList->removeItemWidget(item);
        delete item;
    }

    // Delete ui
    delete ui;

    // Delete settings
    settings->deleteLater();
}


void MainWindow::closeEvent(QCloseEvent *e) {
    if (QMessageBox::question(this, tr("Quit"), tr("Are you sure?"),
                              QMessageBox::Yes | QMessageBox::No)
        == QMessageBox::No) {
        e->ignore();
    }
    else {
        e->accept();
    }
}

void MainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void MainWindow::initSlots() {
    // Menu
    QObject::connect(ui->actionExit, SIGNAL(triggered()),
                     this, SLOT(close()));

    // Splitters
    QObject::connect(ui->messagesSplitter, SIGNAL(splitterMoved(int,int)),
                     this, SLOT(saveSplitterSizes()));
    QObject::connect(ui->typesSplitter, SIGNAL(splitterMoved(int,int)),
                     this, SLOT(saveSplitterSizes()));

    // Messages
    QObject::connect(ui->messageList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
                     this, SLOT(refreshMessageInfo(QListWidgetItem *, QListWidgetItem *)));
    QObject::connect(ui->messageNewButton, SIGNAL(clicked()),
                     this, SLOT(addNewMessage()));
    QObject::connect(ui->messageTypeSelect, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(updateCurrentMessageName()));
    QObject::connect(ui->messageUrlEdit, SIGNAL(textChanged(QString)),
                     this, SLOT(updateCurrentMessageName()));

    // Types
    QObject::connect(ui->typesSaveButton, SIGNAL(clicked()),
                     this, SLOT(saveTypes()));
    QObject::connect(ui->typesList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
                     this, SLOT(refreshTypesInfo(QListWidgetItem *, QListWidgetItem *)));
    QObject::connect(ui->typesNameEdit, SIGNAL(textChanged(QString)),
                     this, SLOT(checkTypesStatus()));
    QObject::connect(ui->typesHeaderTextEdit, SIGNAL(textChanged()),
                     this, SLOT(checkTypesStatus()));
    QObject::connect(ui->typesNameEdit, SIGNAL(textChanged(QString)),
                     this, SLOT(updateCurrentTypeName()));
    QObject::connect(ui->typesNewButton, SIGNAL(clicked()),
                     this, SLOT(addNewType()));
    QObject::connect(ui->messageRemoveButton, SIGNAL(clicked()),
                     this, SLOT(deleteCurrentMessage()));
    QObject::connect(ui->typesRemoveButton, SIGNAL(clicked()),
                     this, SLOT(deleteCurrentType()));


    // Finished message
    QObject::connect(ui->messageExportHeaderEdit, SIGNAL(textChanged()),
                     this, SLOT(saveHeader()));
    QObject::connect(ui->messageExportFooterEdit, SIGNAL(textChanged()),
                     this, SLOT(saveFooter()));
    QObject::connect(ui->messageExportCopyButton, SIGNAL(clicked()),
                     this, SLOT(copyMessage()));
}


//
// PUBLIC SLOTS
//

void MainWindow::loadTypes() {
    // Read the settings
    settings->beginGroup("Types");
    int size = settings->beginReadArray("types");
    for (int pos = 0; pos < size; pos++) {
        settings->setArrayIndex(pos);
        TypeItem *type = new TypeItem(
                settings->value("Name", "").toString(),
                settings->value("HeaderText", "").toString(),
                settings->value("ExtraInfo", "").toString(),
                ui->typesList
                );

        ui->typesList->addItem(type);
    }
    settings->endArray();
    settings->endGroup();

    // Reload types
    reloadMessagesTypeSelect();
}

void MainWindow::reloadMessagesTypeSelect() {
    // Clear the combo-box
    ui->messageTypeSelect->clear();

    // Add types to the combobox
    ui->messageTypeSelect->addItem(tr("<Select>"));
    for (int pos = 0; pos < ui->typesList->count(); pos++) {
        ui->messageTypeSelect->addItem(
                dynamic_cast<TypeItem *>(ui->typesList->item(pos))->name());
    }
}

void MainWindow::saveTypeClass(QListWidgetItem *previous) {
	if (ui->typesList->count()) {
		TypeItem *currentType = dynamic_cast<TypeItem *>(previous);
		currentType->setName(ui->typesNameEdit->text());
		currentType->setHeaderText(ui->typesHeaderTextEdit->toPlainText());
		currentType->setExtraInfo(ui->typesExtraInfoEdit->toPlainText());
	}
}

void MainWindow::saveTypes() {
    saveTypeClass(ui->typesList->currentItem());

    // Write to the settings-file
    settings->beginGroup("Types");
    settings->beginWriteArray("types");
    for (int pos = 0; pos < ui->typesList->count(); pos++) {
        TypeItem *item = dynamic_cast<TypeItem *>(ui->typesList->item(pos));

        // Save to settings
        settings->setArrayIndex(pos);
        settings->setValue("Name", item->name());
        settings->setValue("HeaderText", item->headerText());
        settings->setValue("ExtraInfo", item->extraInfo());
    }
    settings->endArray();
    settings->endGroup();

    // Reload types
    reloadMessagesTypeSelect();
}


//
// PRIVATE SLOTS
//

void MainWindow::saveSplitterSizes() {
    settings->beginGroup("GUI");
    settings->setValue("MessagesSplitterSize", ui->messagesSplitter->saveState());
    settings->setValue("TypesSplitterSize", ui->typesSplitter->saveState());
    settings->endGroup();
}


void MainWindow::addNewMessage() {
    if (!ui->typesList->count()) {
        QMessageBox::information(this, tr("New message"),
        tr("Please add some types first from the Types tab"),
        QMessageBox::Ok);
        return;
    }

    // Is there "New message" already?
    QList<QListWidgetItem *> newMessages =
            ui->messageList->findItems(NEW_MESSAGE_NAME, Qt::MatchExactly);

    // Create new Message
    if (newMessages.empty()) {
        Message *msg = new Message(ui->messageList);
        msg->setText(NEW_MESSAGE_NAME);

        ui->messageList->addItem(msg);
        ui->messageList->setCurrentItem(msg);
    }

    // Set current index to the existing one
    else {
        ui->typesList->setCurrentItem(newMessages.first());
    }

	// Enable copy-button
	ui->messageExportCopyButton->setEnabled(true);
}

void MainWindow::deleteCurrentMessage() {
    Message *currentMessage = dynamic_cast<Message *>(ui->messageList->currentItem());

    if (ui->messageList->currentRow() == -1
    || QMessageBox::question(this, tr("Delete"),
    tr("Are you sure you want to delete this message"),
    QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    // Delete type from the list
    delete ui->messageList->takeItem(ui->messageList->row(currentMessage));

	// Disable copy-button if it was the last message
	ui->messageExportCopyButton->setDisabled(ui->messageList->count() == 0);
}

void MainWindow::saveMessageClass(QListWidgetItem *previous) {
    Message *currentMessage = dynamic_cast<Message *>(previous);

    currentMessage->setUrl(ui->messageUrlEdit->text());
    currentMessage->setExtraInfo(ui->messageInfoEdit->toPlainText());
    if (ui->messageTypeSelect->currentIndex() == 0) {
        currentMessage->setType(0);
    }
    else {
        currentMessage->setType(dynamic_cast<TypeItem *>(ui->typesList->findItems(ui->messageTypeSelect->currentText(), Qt::MatchExactly).first()));
    }
}

void MainWindow::updateCurrentMessageName() {
    if (ui->messageList->currentRow() != -1) {
        if (ui->messageTypeSelect->currentIndex() == 0
            || ui->messageUrlEdit->text().isEmpty()) {
            ui->messageList->currentItem()->setText(NEW_MESSAGE_NAME);
        }
        else {
            QRegExp rx("(/[^/?& ]*)([ ?&]|$)");
            QString name = ui->messageTypeSelect->currentText();
            rx.indexIn(ui->messageUrlEdit->text());
            name += ": ";
            name += rx.cap(1);

            ui->messageList->currentItem()->setText(name);
        }
    }
}

void MainWindow::refreshMessageInfo(QListWidgetItem *current, QListWidgetItem *previous) {
    if (previous) {
        saveMessageClass(previous);
    }
    // Clear texts
    clearMessagesTexts();

    // Set boxes disabled if there's no current type
    if (!current) {
        ui->messageTypeSelect->setDisabled(true);
        ui->messageUrlEdit->setDisabled(true);
        ui->messageInfoEdit->setDisabled(true);
        return;
    }

    // Enable boxes
    ui->messageTypeSelect->setEnabled(true);
    ui->messageUrlEdit->setEnabled(true);
    ui->messageInfoEdit->setEnabled(true);

    // Get current type
    Message *currentMessage = dynamic_cast<Message *>(current);

    // Insert the new data
    ui->messageUrlEdit->setText(currentMessage->url());
    ui->messageInfoEdit->setPlainText(currentMessage->extraInfo());
    if (!currentMessage->type()) {
        ui->messageTypeSelect->setCurrentIndex(0);
    }
    else {
        ui->messageTypeSelect->setCurrentIndex(
                ui->messageTypeSelect->findText(currentMessage->type()->name()));
    }
}

void MainWindow::checkTypesStatus() {
    ui->typesSaveButton->setDisabled(
            ui->typesNameEdit->text().isEmpty()
            || ui->typesHeaderTextEdit->toPlainText().isEmpty()
            );
}

void MainWindow::updateCurrentTypeName() {
    if (ui->typesList->currentRow() != -1)
        ui->typesList->currentItem()->setText(ui->typesNameEdit->text());
}

void MainWindow::addNewType() {
    // Is there a new type already?
    QList<QListWidgetItem *> newTypes =
            ui->typesList->findItems(NEW_TYPE_NAME, Qt::MatchExactly);

    // Create new type
    if (newTypes.empty()) {
        TypeItem *type = new TypeItem(NEW_TYPE_NAME, "", "", ui->typesList);
        ui->typesList->addItem(type);
        ui->typesList->setCurrentItem(type);
    }

    // Set current index to the existing one
    else {
        ui->typesList->setCurrentItem(newTypes.first());
    }
}

void MainWindow::deleteCurrentType() {
    TypeItem *currentType = dynamic_cast<TypeItem *>(ui->typesList->currentItem());

    if (ui->typesList->currentRow() == -1
    || QMessageBox::question(this, tr("Delete"), tr("Are you sure you want to delete ")
    + currentType->name(), QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

	// Delete messages with current type
	for (int i = 0; i < ui->messageList->count(); i++) {
		Message *message = dynamic_cast<Message *>(ui->messageList->item(i));

		// Check if message's type is current type
		if (message->type() == currentType) {
			// Delete message
			ui->messageList->takeItem(ui->messageList->row(message));
		}
	}

    // Delete type from the list
    delete ui->typesList->takeItem(ui->typesList->row(currentType));

    // Save the new list
    saveTypes();
}

void MainWindow::clearTypesTexts() {
    ui->typesNameEdit->clear();
    ui->typesHeaderTextEdit->clear();
    ui->typesExtraInfoEdit->clear();
}

void MainWindow::clearMessagesTexts() {
    ui->messageUrlEdit->clear();
    ui->messageInfoEdit->clear();
}

void MainWindow::refreshTypesInfo(QListWidgetItem *current, QListWidgetItem *previous) {
    // Save texts
    if (previous) {
        saveTypeClass(previous);
    }

    // Clear the old texts
    clearTypesTexts();

    // Get current type
    TypeItem *currentType = dynamic_cast<TypeItem *>(current);

    // Insert the new data
    if (currentType) {
        ui->typesNameEdit->setEnabled(true);
        ui->typesHeaderTextEdit->setEnabled(true);
        ui->typesExtraInfoEdit->setEnabled(true);

        ui->typesNameEdit->setText(currentType->name());
        ui->typesHeaderTextEdit->setPlainText(currentType->headerText());
        ui->typesExtraInfoEdit->setPlainText(currentType->extraInfo());
    }
    else {
        ui->typesNameEdit->setDisabled(true);
        ui->typesHeaderTextEdit->setDisabled(true);
        ui->typesExtraInfoEdit->setDisabled(true);
    }
}

void MainWindow::loadHeader() {
    settings->beginGroup("Export");
    ui->messageExportHeaderEdit->setPlainText(settings->value("HeaderText", QString()).toString());
    settings->endGroup();
}

void MainWindow::saveHeader() {
    settings->beginGroup("Export");
    settings->setValue("HeaderText", ui->messageExportHeaderEdit->toPlainText());
    settings->endGroup();
}

void MainWindow::loadFooter() {
    settings->beginGroup("Export");
    ui->messageExportFooterEdit->setPlainText(settings->value("FooterText", QString()).toString());
    settings->endGroup();
}

void MainWindow::saveFooter() {
    settings->beginGroup("Export");
    settings->setValue("FooterText", ui->messageExportFooterEdit->toPlainText());
    settings->endGroup();
}

void MainWindow::copyMessage() {
    TypeItem *currentType = 0;
    QString message = ui->messageExportHeaderEdit->toPlainText();

	if (!message.isEmpty()) {
		message += ENDL;
		message += ENDL;
	}

    // Save current message
    saveMessageClass(ui->messageList->currentItem());

    // Loop messages
    for (int pos = 0; pos < ui->messageList->count(); pos++) {
        // Get message
        Message *currentMessage = dynamic_cast<Message *>(ui->messageList->item(pos));

        // Does it have a type
        if (!currentMessage->type()) {
            continue;
        }

        // Check type
        if (!currentType) {
            currentType = currentMessage->type();
            message += currentType->name();
            message += ":" + ENDL;
            message += currentType->headerText();
            message += ENDL;
        }
        else if (currentType->name() != currentMessage->type()->name()) {
            message += currentType->extraInfo();
            message += ENDL;
            message += ENDL;

            // Change current type
            currentType = currentMessage->type();
            message += currentType->name();
            message += ":" + ENDL;
            message += currentType->headerText();
            message += ENDL;
        }

        // Add info from message
        message += currentMessage->url();
        message += ENDL;
        if (!currentMessage->extraInfo().isEmpty()) {
            message += "\t";
            message += currentMessage->extraInfo();
            message += ENDL;
        }
    }

    // Last type's extra info
    message += currentType->extraInfo();
    message += ENDL;
    message += ENDL;

    // Add footer
	if (!ui->messageExportFooterEdit->toPlainText().isEmpty()) {
		message += "--";
		message += ENDL;
		message += ui->messageExportFooterEdit->toPlainText();
	}

    // Copy message to clipboard
    QApplication::clipboard()->setText(message);
}
