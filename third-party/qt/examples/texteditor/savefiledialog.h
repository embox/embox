#ifndef SAVEFILEDIALOG_H
#define SAVEFILEDIALOG_H

#include <QtGui>

class SaveFileDialog : public QWidget
{
    Q_OBJECT
public:
    explicit SaveFileDialog(QTextEdit *textEdit, QString *fileName, QMainWindow *mainWindow);

signals:

private slots:
    void save();
    void close();

private:
    QPushButton *createButton(const QString &text, const char *member);

    QLineEdit *fileNameEdit;

    QLabel *fileNameLabel;

    QPushButton *okButton;
    QPushButton *cancelButton;
    QDir currentDir;
    QString *fileName;
    QMainWindow *mainWindow;
    QTextEdit *textEdit;
    QMdiSubWindow *subwindow;
};

#endif // SAVEFILEDIALOG_H
