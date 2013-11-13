#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSocketNotifier>

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
    int oldstd[2];
    QScopedPointer<QSocketNotifier> notifier;

private slots:
    void onStdout();
    void on_lineEdit_returnPressed();
    void on_pushButton_pressed();
};

#endif // MAINWINDOW_H
