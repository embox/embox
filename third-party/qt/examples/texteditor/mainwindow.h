#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QtGui>

#include "openfiledialog.h"
#include "createfiledialog.h"
#include "savefiledialog.h"

#define TEDIT_APP_TITLE "TextEditor"
#define TEDIT_DIR "/home/vita/"

class TextEditor : public QMainWindow
{
    Q_OBJECT

    public:
        TextEditor();

    private slots:
        void create();
        void open();
        void save();

    private:
        QTextEdit *textEdit;

        QAction *createAction;
        QAction *openAction;
        QAction *saveAction;
        QAction *exitAction;

        QMenu *fileMenu;
        QString fileName;

        OpenFileDialog *openDialog;
        CreateFileDialog *createDialog;
        SaveFileDialog *saveFile;
};

#endif // MAINWINDOW_H
