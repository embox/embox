#ifndef TEXTEDITOR_MDI_BACKGROUND_H
#define TEXTEDITOR_MDI_BACKGROUND_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QStringList>
#include <QDebug>
#include <QtGui>

#include <QtGui>

class QMdiAreaWBackground : public QMdiArea
{
	Q_OBJECT
public:
        QMdiAreaWBackground(QString& str);
        void setBackgroundPath(QString& str);
protected:
        void resizeEvent(QResizeEvent *resizeEvent);
private:
        void resize(QSize& size);
        QImage i;
        QSize size;
};

#endif /* TEXTEDITOR_MDI_BACKGROUND_H */
