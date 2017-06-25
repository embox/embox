#ifndef OPENFILEDIALOG_H
#define OPENFILEDIALOG_H

#include <QtGui>

class OpenFileDialog : public QWidget
{
    Q_OBJECT
public:
    explicit OpenFileDialog(QTextEdit *textEdit, QString *fileName, QMainWindow *mainWindow);

private slots:
    void find();
    void openFileOfItem(int row, int column);
private:
    QStringList findFiles(const QStringList &files, const QString &text);
    void showFiles(const QStringList &files);
    //QPushButton *createButton(const QString &text, const char *member);
    QComboBox *createComboBox(const QString &text = QString());
    void createFilesTable();

    QLabel *fileLabel;
    QLabel *textLabel;
    QLabel *directoryLabel;
    QLabel *filesFoundLabel;
    QPushButton *browseButton;
    QPushButton *findButton;
    QTableWidget *filesTable;
    QTextEdit *textEdit;

    QDir currentDir;
    QString *fileName;
    QMainWindow *mainWindow;
    QMdiSubWindow *subwindow;
};

#endif // OPENFILEDIALOG_H
