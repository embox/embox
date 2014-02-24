#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <unistd.h>
#include <QtGui>

#include "openfiledialog.h"
#include "createfiledialog.h"
#include "savefiledialog.h"
#include "helpwindow.h"

#define TEDIT_APP_TITLE "TextEditor"
#define TEDIT_DIR "/tmp/"

extern void emboxQtShowDesktop(uid_t uid);
extern void emboxQtHideDesktop();

class TextEditor : public QMainWindow
{
    Q_OBJECT

    public slots:
        void quit();

    public:
	static void closeEditor(TextEditor *);
	static void closeAllEditors();
        TextEditor(QMdiArea *);

    private slots:
        void create();
        void open();
        void save();
        void help();
        void setFont(int);

    private:
	static QList<TextEditor*> textEditors;

	QMdiSubWindow *subwindow;

        QTextEdit *textEdit;

        QAction *createAction;
        QAction *openAction;
        QAction *saveAction;
        QAction *exitAction;
        QAction *helpAction;
        QAction *setFontAction;

        QMenu *fileMenu;
        QMenu *helpMenu;
        QString fileName;

        QComboBox *fontBox;

        OpenFileDialog *openDialog;
        CreateFileDialog *createDialog;
        SaveFileDialog *saveFile;
        EWisard *helpWindow;
};

class TextEditorSubWindow : public QMdiSubWindow
{
	Q_OBJECT
public:
	TextEditorSubWindow(TextEditor *);
protected:
        void closeEvent(QCloseEvent *closeEvent);
private:
	TextEditor *textEditor;
};

#endif // MAINWINDOW_H
