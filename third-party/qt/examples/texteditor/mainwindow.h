#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QtGui>

#include "openfiledialog.h"
#include "createfiledialog.h"
#include "savefiledialog.h"
#include "helpwindow.h"

#define TEDIT_APP_TITLE "TextEditor"
#define TEDIT_DIR "/tmp/"

class TextEditor : public QMainWindow
{
    Q_OBJECT

    public:
        TextEditor();

    private slots:
        void create();
        void open();
        void save();
        void quit();
        void help();

    private:
        QTextEdit *textEdit;

        QAction *createAction;
        QAction *openAction;
        QAction *saveAction;
        QAction *exitAction;
        QAction *helpAction;

        QMenu *fileMenu;
        QMenu *helpMenu;
        QString fileName;

        OpenFileDialog *openDialog;
        CreateFileDialog *createDialog;
        SaveFileDialog *saveFile;
        QMessageBox *helpWindow;
};

#endif // MAINWINDOW_H
