#ifndef CREATE_FILEDIALOG_H
#define CREATE_FILEDIALOG_H

#include <QtGui>

class TRY : public QWidget
{
    Q_OBJECT
public:
    explicit TRY();

private slots:
    void createFile(QString name);

private:
    QLineEdit fileNameEdit;

    QLabel *fileNameLabel;

    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // CREATEFILEDIALOG_H
