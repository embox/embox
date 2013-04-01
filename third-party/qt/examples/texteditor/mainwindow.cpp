#include "mainwindow.h"


TextEditor::TextEditor()
{
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

    fileMenu = menuBar()->addMenu(tr("&Файл"));
    fileMenu->addAction(createAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    helpMenu = menuBar()->addMenu(tr("&Справка"));
    helpMenu->addAction(helpAction);

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

void TextEditor::quit() {
	extern QGraphicsView *emboxView;
	extern int desktopRepaint;
	hide();
	desktopRepaint = 1;
	emboxView->repaint();
}

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

void TextEditor::help() {
	helpWindow = new QWizard();
	helpWindow->addPage(createIntroPage());
	helpWindow->setWindowTitle("Справка");
	helpWindow->resize(600, 300);
	helpWindow->show();
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

