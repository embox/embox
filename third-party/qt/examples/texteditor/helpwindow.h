/*
 * helpwindow.h
 *
 *  Created on: 2013 3 29
 *      Author: alexander
 */

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QtGui>

class EWisard : public QWizard {
    Q_OBJECT

    public:
    	EWisard();

    	void setVisible(bool visible);

    private:
        QMdiSubWindow *subwindow;
};

QT_END_NAMESPACE

#endif
