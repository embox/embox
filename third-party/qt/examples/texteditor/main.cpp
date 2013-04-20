#include <QApplication>
#include <QTextEdit>
#include <QtGui>
#include <QTextCodec>
#include "mainwindow.h"
#include "login.h"

#include <framework/mod/options.h>
#include <module/embox/arch/x86/boot/multiboot.h>

#define MBOOTMOD embox__arch__x86__boot__multiboot

#define WIDTH  OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_width)
#define HEIGHT OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_height)
#define BPP    OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_depth)

TextEditor *textEditor;
QMdiArea *emarea;
QMdiSubWindow *emEditorSubWindow;

class EmboxRootWindow : public QMainWindow
{
    Q_OBJECT

    public:
    EmboxRootWindow() {
    	QMenu *fileMenu = new QMenu(QString("&Пуск"), this);
    	menuBar()->addMenu(fileMenu);

    	textEditorAction = new QAction(QString("&Текстовый редактор"), this);
    	fileMenu->addAction(textEditorAction);
    	connect(textEditorAction, SIGNAL(triggered()), this, SLOT(textEditorRun()));
    }

    private slots:
    	void textEditorRun() {
    		textEditor = new TextEditor();
    		emEditorSubWindow = emarea->addSubWindow(textEditor, textEditor->windowType());
    		textEditor->show();
    	}

    private:
        QAction *textEditorAction;
};

int main(int argv, char **args)
{
    //Q_INIT_RESOURCE(texteditor);

    QApplication app(argv, args);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QImage desktopImage = QImage(":/default.png").convertToFormat(QImage::Format_RGB16).scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio);
    QPixmap bgPix = QPixmap::fromImage(desktopImage);

    EmboxRootWindow *emroot = new EmboxRootWindow();

    emarea = new QMdiArea();
    emarea->setBackground(bgPix);
    emarea->resize(WIDTH, HEIGHT);
    emarea->show();

    emroot->setCentralWidget(emarea);
    emroot->resize(WIDTH, HEIGHT);
    emroot->show();

    LoginDialog* loginDialog = new LoginDialog();
    //connect(loginDialog,
                     //SIGNAL(acceptLogin(QString&,QString&,int&)),
                     //this,
                     //SLOT(slotAcceptUserLogin(QString&,QString&)));
    //loginDialog->move(200, 200);
    loginDialog->subwindow->setGeometry(WIDTH/2 - 150, HEIGHT/2 - 75, 300, 150);
    loginDialog->show();

    return app.exec();
}

#include "main.moc"
