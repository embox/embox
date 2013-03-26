#include <QApplication>
#include <QTextEdit>
#include <QtGui>
#include "mainwindow.h"

int main(int argv, char **args)
{
	Q_INIT_RESOURCE(texteditor);

    QApplication app(argv, args);

    QGraphicsScene scene(-350, -350, 700, 700);

    QPixmap bgPix(":/desktopImage.jpg");
    QGraphicsView *view = new QGraphicsView(&scene);
    view->setBackgroundBrush(bgPix);
    view->show();

    //TextEditor textEditor;
    //textEditor.show();

    return app.exec();
}
