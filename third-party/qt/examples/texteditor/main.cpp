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

static void emboxShowLoginForm();

class EmboxRootWindow : public QMainWindow
{
    Q_OBJECT

    public:
    EmboxRootWindow() {
    	fileMenu = new QMenu(QString("&Пуск"), this);
    	menuBar()->addMenu(fileMenu);

    	textEditorAction = new QAction(QString("&Текстовый редактор"), this);
    	fileMenu->addAction(textEditorAction);
    	connect(textEditorAction, SIGNAL(triggered()), this, SLOT(textEditorRun()));

    	logoutAction = new QAction(QString("&Завершение сеанса"), this);
    	fileMenu->addAction(logoutAction);
    	connect(logoutAction, SIGNAL(triggered()), this, SLOT(logout()));
    }

    private slots:
    	void textEditorRun() {
    		textEditor = new TextEditor();
    		emEditorSubWindow = emarea->addSubWindow(textEditor, textEditor->windowType());
    		textEditor->show();
    	}

    	void logout() {
    		emboxHideDesktop();
    		emboxShowLoginForm();
    	}

    private:
    	QAction *logoutAction;
        QAction *textEditorAction;
        QMenu *fileMenu;
};

EmboxRootWindow *emroot;

void emboxShowDesktop() {
	emroot->menuBar()->show();
}

void emboxHideDesktop() {
	emarea->closeAllSubWindows();
	emroot->menuBar()->hide();
}

static void emboxShowLoginForm() {
    LoginDialog *loginDialog = new LoginDialog();
    loginDialog->subwindow->setGeometry(WIDTH/2 - 150, HEIGHT/2 - 75, 300, 150);
    loginDialog->show();
}

int main(int argv, char **args)
{
    //Q_INIT_RESOURCE(texteditor);

    QApplication app(argv, args);

    QFont serifFont("Times", 10);
    app.setFont(serifFont);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QImage desktopImage = QImage(":/default.png").convertToFormat(QImage::Format_RGB16).scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio);
    QPixmap bgPix = QPixmap::fromImage(desktopImage);

    emarea = new QMdiArea();
    emarea->setBackground(bgPix);
    emarea->resize(WIDTH, HEIGHT);
    emarea->show();

    emroot = new EmboxRootWindow();
    emroot->setCentralWidget(emarea);
    emroot->resize(WIDTH, HEIGHT);
    emroot->show();

    emboxHideDesktop();

    emboxShowLoginForm();

    return app.exec();
}

#include "main.moc"
