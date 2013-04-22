
#include <QApplication>
#include <QTextEdit>
#include <QtGui>
#include <QTextCodec>
#include "mainwindow.h"
#include "login.h"
#include "desktopimage.h"

#include <framework/mod/options.h>
#include <module/embox/arch/x86/boot/multiboot.h>

#define MBOOTMOD embox__arch__x86__boot__multiboot

#define WIDTH  OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_width)
#define HEIGHT OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_height)
#define BPP    OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_depth)

static QList<TextEditor*> textEditors;
TextEditor *textEditor;
QMdiArea *emarea;
QMdiSubWindow *emEditorSubWindow;
static DesktopImageDialog *wallpaperDialog;
static QStringList desktopImagesList;

void textEditorClosed(TextEditor *ed) {
	for (int i = 0; i < textEditors.size(); i++) {
		if (textEditors.at(i) == ed) {
			textEditors.removeAt(i);
		}
	}
}

static void emboxShowLoginForm();

class EmboxRootWindow : public QMainWindow
{
    Q_OBJECT

    public:

    QAction *closeAllEditorsAction;
    EmboxRootWindow() {
    	fileMenu = new QMenu(QString("&Пуск"), this);
    	menuBar()->addMenu(fileMenu);

    	textEditorAction = new QAction(QString("&Текстовый редактор"), this);
    	fileMenu->addAction(textEditorAction);
    	connect(textEditorAction, SIGNAL(triggered()), this, SLOT(textEditorRun()));

    	closeAllEditorsAction = new QAction(QString("&Закрыть все окна"), this);
    	fileMenu->addAction(closeAllEditorsAction);
    	connect(closeAllEditorsAction, SIGNAL(triggered()), this, SLOT(closeAllEditors()));

    	logoutAction = new QAction(QString("&Завершение сеанса"), this);
    	fileMenu->addAction(logoutAction);
    	connect(logoutAction, SIGNAL(triggered()), this, SLOT(logout()));

    	wallpaperAction = new QAction(QString("&Обои на рабочий стол"), this);
    	fileMenu->addAction(wallpaperAction);
    	connect(wallpaperAction, SIGNAL(triggered()), this, SLOT(wallpaper()));
    }

    private slots:
    	void textEditorRun() {
    		textEditor = new TextEditor();
		textEditors << textEditor;
    		emEditorSubWindow = emarea->addSubWindow(textEditor, textEditor->windowType());
		textEditor->subwindow = emEditorSubWindow;
    		textEditor->show();
    	}

    	void closeAllEditors() {
		for (int i = 0; i < textEditors.size(); i++) {
			textEditors.at(i)->subwindow->close();
		}
	}

    	void logout() {
    		emboxHideDesktop();
    		emboxShowLoginForm();
    	}

    	void wallpaper() {
    		wallpaperDialog = new DesktopImageDialog(desktopImagesList);
    		wallpaperDialog->show();
    	}

    private:
    	QAction *logoutAction;
        QAction *textEditorAction;
        QAction *wallpaperAction;
        QMenu *fileMenu;
};

EmboxRootWindow *emroot;

void emboxShowDesktop(uid_t uid) {
	bool enabled = uid ? false : true;
	emroot->menuBar()->show();
	emroot->closeAllEditorsAction->setEnabled(enabled);
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

    desktopImagesList << "cats.jpg" << "default.png";

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QImage desktopImage = QImage(":/images/default.png").convertToFormat(QImage::Format_RGB16).scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio);
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
