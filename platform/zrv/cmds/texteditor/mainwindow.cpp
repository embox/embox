#include "mainwindow.h"

static QList<TextEditor*> TextEditor::textEditors;

TextEditorSubWindow::TextEditorSubWindow(TextEditor *editor)
{
	textEditor = editor;
}

void TextEditorSubWindow::closeEvent(QCloseEvent *closeEvent)
{
	TextEditor::closeEditor(textEditor);
	QMdiSubWindow::closeEvent(closeEvent);
}

void TextEditor::closeEditor(TextEditor *ed) {
	for (int i = 0; i < textEditors.size(); i++) {
		if (textEditors.at(i) == ed) {
			textEditors.removeAt(i);
		}
	}
}
void TextEditor::closeAllEditors() {
	for (int i = textEditors.size() - 1; i >= 0; i--) {
		textEditors.at(i)->quit();
	}
}

TextEditor::TextEditor(QMdiArea *area)
{
    //extern QGraphicsScene *emscene;
    createAction = new QAction(tr("&Новый файл"), this);
    openAction = new QAction(tr("&Открыть"), this);
    saveAction = new QAction(tr("&Сохранить"), this);
    exitAction = new QAction(tr("&Выход"), this);
    helpAction = new QAction(tr("&О редакторе"), this);
    setFontAction = new QAction(this);

    connect(createAction, SIGNAL(triggered()), this, SLOT(create()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

    fileMenu = new QMenu(tr("&Файл"), this);
    //emscene->addWidget(fileMenu)->setZValue(99);
    //fileMenu->setVisible(false);

    menuBar()->addMenu(fileMenu);
    fileMenu->addAction(createAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(helpAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    textEdit = new QTextEdit(this);
    fileName = "";

    QFontDatabase fontDataBase;
    fontBox = new QComboBox();
    fontBox->addItems(fontDataBase.families());
    fontBox->addAction(setFontAction);
    connect(fontBox , SIGNAL(currentIndexChanged(int)),this,SLOT(setFont(int)));

    QGroupBox *groupBox = new QGroupBox;
    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(fontBox);
    layout->addWidget(textEdit);
    groupBox->setLayout(layout);

    setCentralWidget(groupBox);
    setWindowTitle(tr(TEDIT_APP_TITLE));
    resize(640, 480);

    subwindow = new TextEditorSubWindow(this);
    subwindow->setWidget(this);
    subwindow->setAttribute(Qt::WA_DeleteOnClose);
    area->addSubWindow(subwindow, this->windowType());

    TextEditor::textEditors << this;
}

void TextEditor::create()
{
	createDialog = new CreateFileDialog(textEdit, &fileName, this);
	createDialog->show();
}

void TextEditor::quit() {
	subwindow->mdiArea()->setActiveSubWindow(subwindow);
	subwindow->mdiArea()->closeActiveSubWindow();
}

void TextEditor::help() {
    helpWindow = new EWisard();
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

void TextEditor::setFont(int)
{
    textEdit->setFontFamily(fontBox->currentText());
}
