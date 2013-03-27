#include <QApplication>
#include <QTextEdit>
#include <QtGui>
#include "mainwindow.h"

class Button : public QGraphicsWidget
{
    Q_OBJECT
public:
    Button(const QPixmap &pixmap, QGraphicsItem *parent = 0)
        : QGraphicsWidget(parent), _pix(pixmap)
    {
        setAcceptHoverEvents(true);
        setCacheMode(DeviceCoordinateCache);
    }

    QRectF boundingRect() const
    {
        return QRectF(-65, -65, 130, 130);
    }

    QPainterPath shape() const
    {
        QPainterPath path;
        path.addEllipse(boundingRect());
        return path;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
    {
        bool down = option->state & QStyle::State_Sunken;
        QRectF r = boundingRect();
        QLinearGradient grad(r.topLeft(), r.bottomRight());
        grad.setColorAt(down ? 1 : 0, option->state & QStyle::State_MouseOver ? Qt::white : Qt::lightGray);
        grad.setColorAt(down ? 0 : 1, Qt::darkGray);
        painter->setPen(Qt::darkGray);
        painter->setBrush(grad);
        //painter->drawEllipse(r);
        painter->drawRect(r);
        QLinearGradient grad2(r.topLeft(), r.bottomRight());
        grad.setColorAt(down ? 1 : 0, Qt::darkGray);
        grad.setColorAt(down ? 0 : 1, Qt::lightGray);
        painter->setPen(Qt::NoPen);
        painter->setBrush(grad);
        if (down)
            painter->translate(2, 2);
        painter->drawRect(r.adjusted(5, 5, -5, -5));
        painter->drawPixmap(-_pix.width()/2, -_pix.height()/2, _pix);
    }

signals:
    void pressed();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *)
    {
        //emit pressed();
        //update();
        TextEditor textEditor;
        textEditor.show();
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *)
    {
        //update();
    }

private:
    QPixmap _pix;
};

int main(int argv, char **args)
{
	Q_INIT_RESOURCE(texteditor);

    QApplication app(argv, args);

    QGraphicsScene scene(-350, -350, 700, 700);

    QPixmap bgPix(":/desktopImage.jpg");
    QGraphicsView *view = new QGraphicsView(&scene);
    view->setBackgroundBrush(bgPix);

    QGraphicsItem *buttonParent = new QGraphicsRectItem;
    Button *ellipseButton = new Button(QPixmap(":/icon.png"), buttonParent);

    ellipseButton->setPos(-100, -100);

    scene.addItem(buttonParent);
    buttonParent->scale(0.75, 0.75);
    buttonParent->setPos(-100, -50);
    buttonParent->setZValue(65);

    view->show();

    return app.exec();
}

#include "main.moc"
