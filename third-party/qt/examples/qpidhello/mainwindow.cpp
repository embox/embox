#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <unistd.h>
#include <fcntl.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int flags;

    oldstd[0] = dup(1);
    oldstd[1] = dup(2);
    pipe(pipefd);
    flags = fcntl(pipefd[0], F_GETFL, 0);
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(pipefd[1], F_GETFL, 0);
    fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK);
    ::close(1);
    dup(pipefd[0]);
    ::close(2);
    dup(pipefd[0]);

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
