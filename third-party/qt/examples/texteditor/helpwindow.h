/*
 * helpwindow.h
 *
 *  Created on: 2013 3 29
 *      Author: alexander
 */

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class QTextEdit;

class EditorHelpWindow : public QWidget
{
    Q_OBJECT

public:
    EditorHelpWindow(QWidget *parent = 0);
    //void setHelp(const QString &topic);

private:
    QTextEdit *m_textEdit;
};

QT_END_NAMESPACE

#endif
