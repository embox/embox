#include "mainwindow.h"

QWizardPage *createIntroPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("О редакторе");

    QLabel *label = new QLabel("Текстовый редактор ZarjaEditor версии 1.0.");
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    page->setLayout(layout);

    return page;
}


TextEditor::TextEditor()
{
    extern QGraphicsScene *emscene;
    createAction = new QAction(tr("&Новый файл"), this);
    openAction = new QAction(tr("&Открыть"), this);
    saveAction = new QAction(tr("&Сохранить"), this);
    exitAction = new QAction(tr("&Выход"), this);
    helpAction = new QAction(tr("&О редакторе"), this);

    connect(createAction, SIGNAL(triggered()), this, SLOT(create()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

    fileMenu = new QMenu(tr("&Файл"), this);
    emscene->addWidget(fileMenu)->setZValue(99);
    fileMenu->setVisible(false);

    menuBar()->addMenu(fileMenu);
    fileMenu->addAction(createAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(helpAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    textEdit = new QTextEdit;
    fileName = "";

    setCentralWidget(textEdit);
    setWindowTitle(tr(TEDIT_APP_TITLE));
    resize(640, 480);

    helpWindow = new QWizard();
    helpWindow->addPage(createIntroPage());
    helpWindow->setWindowTitle("Справка");
    helpWindow->resize(600, 300);
    emscene->addWidget(helpWindow,helpWindow->windowType())->setZValue(100);
    helpWindow->hide();

    createDialog = new CreateFileDialog(textEdit, &fileName, this);
    openDialog = new OpenFileDialog(textEdit, &fileName, this);
    saveFile = new SaveFileDialog(textEdit, &fileName, this);
    emscene->addWidget(createDialog, createDialog->windowType())->setZValue(100);
    createDialog->hide();
    emscene->addWidget(openDialog, openDialog->windowType())->setZValue(100);
    openDialog->hide();
}

extern QGraphicsScene *emscene;

void TextEditor::create()
{
    createDialog->show();
}

void TextEditor::quit() {
	hide();
}

void TextEditor::help() {
	helpWindow->show();
}

void TextEditor::open()
{
    openDialog->show();
}

void TextEditor::save()
{
    if (fileName != "") {
    QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
                    //TODO: error message
        }
        else {
	    QTextStream stream(&file);
	    stream << textEdit->toPlainText();
	    stream.flush();
	    file.close();
        }
    }
    else{
	saveFile->show();
    }
}

