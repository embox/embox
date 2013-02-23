#include "mainwindow.h"


TextEditor::TextEditor()
{
    createAction = new QAction(tr("&New file"), this);
    openAction = new QAction(tr("&Open"), this);
    saveAction = new QAction(tr("&Save"), this);
    exitAction = new QAction(tr("E&xit"), this);

    connect(createAction, SIGNAL(triggered()), this, SLOT(create()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(createAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    textEdit = new QTextEdit;
    fileName = "";

    setCentralWidget(textEdit);
    setWindowTitle(tr(TEDIT_APP_TITLE));
    resize(640, 480);
}

void TextEditor::create()
{
    createDialog = new CreateFileDialog(textEdit, &fileName, this);
    createDialog->show();
}

void TextEditor::open()
{
    openDialog = new OpenFileDialog(textEdit, &fileName, this);
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
    saveFile = new SaveFileDialog(textEdit, &fileName, this);
    saveFile->show();
    }
}
