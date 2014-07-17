#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSocketNotifier>
#include <QThread>

#include <streambuf>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int pipefd[2];
    //int oldstd[2];
    std::streambuf *oldstd[2];
    QScopedPointer<QSocketNotifier> notifier;
    QScopedPointer<QThread> thread;
    QString brok;
    friend class QpidWorker;

private slots:
    void onStdout();
    void onFinished();
    void on_lineEdit_returnPressed();
    void on_pushButton_pressed();
};

class QpidWorker : public QObject {
    Q_OBJECT
public:
    QpidWorker(MainWindow *mw) : mw(mw) {;}
private:
    MainWindow *mw;
private slots:
    void doQPIDHello();
};

#endif // MAINWINDOW_H
