#include "savefiledialog.h"
#include "mainwindow.h"

extern QMdiArea *emarea;

SaveFileDialog::SaveFileDialog(QTextEdit *textEdit, QString *fileName, QMainWindow *mainWindow)
{
    this->textEdit = textEdit;
    this->fileName = fileName;
    this->mainWindow = mainWindow;

    fileNameEdit = new QLineEdit;
    fileNameLabel = new QLabel(tr("Имя файла:"));

    okButton = createButton(tr("&Ок"), SLOT(save()));
    cancelButton = createButton(tr("&Cancel"), SLOT(close()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(fileNameLabel, 0, 0, 1, 3);
    mainLayout->addWidget(fileNameEdit, 1, 0, 1, 3);
    mainLayout->addLayout(buttonsLayout, 2, 0, 1, 3);
    setLayout(mainLayout);

    setWindowTitle(tr("Сохранить"));
    resize(200, 100);
    subwindow = emarea->addSubWindow(this, windowType());
}

QPushButton *SaveFileDialog::createButton(const QString &text, const char *member)
 {
     QPushButton *button = new QPushButton(text);
     connect(button, SIGNAL(clicked()), this, member);
     return button;
 }

void SaveFileDialog::save()
{
    currentDir = QDir(QLatin1String(TEDIT_DIR));
    currentDir.setNameFilters(QStringList() << QLatin1String("*.txt"));
    currentDir.refresh();

    QStringList files;
    files = currentDir.entryList(QStringList("*"),
                                      QDir::Files | QDir::NoSymLinks);
    if (files.contains(fileNameEdit->text() + ".txt")){
        //TODO: error
    }

    if (fileNameEdit->text() != "") {
    *fileName = TEDIT_DIR + fileNameEdit->text() + ".txt";
    QFile file(*fileName);
    if (!file.open(QIODevice::WriteOnly)) {
                //TODO: error message
    }
    else {
        QTextStream stream(&file);
        stream << textEdit->toPlainText();
        stream.flush();
        file.close();
        mainWindow->setWindowTitle(*fileName + " - " + TEDIT_APP_TITLE);
        close();
        }
    }
}

void SaveFileDialog::close() {
	//hide();
	emarea->setActiveSubWindow(subwindow);
	emarea->closeActiveSubWindow();
}
