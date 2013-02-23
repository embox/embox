#include <QApplication>
#include <QTextEdit>
#include <QtGui>
#include "mainwindow.h"

int main(int argv, char **args)
{
    QApplication app(argv, args);

    TextEditor textEditor;
    textEditor.show();

    return app.exec();
}
