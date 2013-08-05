
#include <QApplication>
#include <QTextEdit>
#include <QtGui>
#include <QTextCodec>
#include "mainwindow.h"
#include "login.h"
#include "desktopimage.h"

#include <fcntl.h>
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

static QApplication *__qt_app;
static QMenu *__qt_menu;

class EmboxRootWindow : public QMainWindow
{
    Q_OBJECT

    public:

    QAction *closeAllEditorsAction;
    EmboxRootWindow() {
    	fileMenu = new QMenu(QString("&Пуск"), this);
    	menuBar()->addMenu(fileMenu);

	__qt_menu = fileMenu;

    	textEditorAction = new QAction(QString("&Текстовый редактор"), this);
    	fileMenu->addAction(textEditorAction);
    	connect(textEditorAction, SIGNAL(triggered()), this, SLOT(textEditorRun()));

    	closeAllEditorsAction = new QAction(QString("&Закрыть все окна"), this);
    	fileMenu->addAction(closeAllEditorsAction);
    	connect(closeAllEditorsAction, SIGNAL(triggered()), this, SLOT(closeAllEditors()));

    	logoutAction = new QAction(QString("&Завершение сеанса"), this);
    	fileMenu->addAction(logoutAction);
    	connect(logoutAction, SIGNAL(triggered()), this, SLOT(logout()));

    	saveAction = new QAction(QString("&Сохранить конфигурацию"), this);
    	fileMenu->addAction(saveAction);
    	connect(saveAction, SIGNAL(triggered()), this, SLOT(savedefault()));

    	wallpaperAction = new QAction(QString("&Графические настройки"), this);
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

    	void savedefault() {

    	}

    private:
    	QAction *logoutAction;
        QAction *textEditorAction;
        QAction *wallpaperAction;
        QAction *saveAction;
        QMenu *fileMenu;
};

EmboxRootWindow *emroot;

#define FSIZE 64

static int curuid;

#define SEP ':'

static char *parse(char **buf) {
	char *ret = *buf;
	char *ch;

	if ((ch = strchr(*buf, SEP))) {
		*ch++ = '\0';
	}

	*buf = ch;

	return ret;
}

void qtSetfont(const QString &fontFamily, int fontPt) {
	QFont serifFont(fontFamily, fontPt);
	__qt_app->setFont(serifFont);
	__qt_menu->setFont(serifFont);
}

static void load_pref(void) {
	int fd, ret;
	char cbuf[FSIZE];
	const char *wall = "default.png";
	const char *font = "Times";
	int font_pt = 10;

	snprintf(cbuf, FSIZE, "/mnt/pref_%d", curuid);
	fd = open(cbuf, O_RDONLY, 0755);

	lseek(fd, 0, 0);
	if (0 < (ret = read(fd, cbuf, FSIZE))) {
		char *buf = cbuf;

		buf[ret] = '\0';

		wall = parse(&buf);
		font = parse(&buf);
		font_pt = atoi(parse(&buf));
	}
	close(fd);

	QString imagePath = QString(":/images/").append(QString(wall));
	QImage desktopImage = QImage(imagePath).convertToFormat(QImage::Format_RGB16).scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio);
	QPixmap bgPix = QPixmap::fromImage(desktopImage);
	emarea->setBackground(bgPix);

	qtSetfont(font, font_pt);

}

void save_pref(char *wall, char *font, int font_pt) {
	int fd, len;
	char cbuf[FSIZE];

	snprintf(cbuf, FSIZE, "/mnt/pref_%d", curuid);

	fd = creat(cbuf, 0755);

	len = snprintf(cbuf, FSIZE, "%s:%s:%d", wall, font, font_pt);

	write(fd, cbuf, len);

	ftruncate(fd, len);

	close(fd);
}

void emboxShowDesktop(uid_t uid) {
	int fd = -1;
	bool enabled = uid ? false : true;

	curuid = uid;
	emroot->menuBar()->show();
	emroot->closeAllEditorsAction->setEnabled(enabled);

	load_pref();
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

    __qt_app = &app;

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
