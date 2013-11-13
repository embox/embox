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
    ::close(STDOUT_FILENO);
    dup(pipefd[0]);
    ::close(STDERR_FILENO);
    dup(pipefd[0]);

    notifier.reset(new QSocketNotifier(pipefd[1], QSocketNotifier::Read));
    connect(notifier.data(), SIGNAL(activated(int)), this, SLOT(onStdout()));
    notifier.data()->setEnabled(true);

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStdout()
{
    char buf[256];
    ssize_t read;

    do {
        read = ::read(pipefd[1], buf, 256);
        ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText().append(QByteArray(buf,read)));
    } while (read>0);
}

void MainWindow::on_lineEdit_returnPressed()
{
    printf("qpidapp:>>> %s\n", ui->lineEdit->text().toAscii().data());
    fflush(stdout);
}

void MainWindow::on_pushButton_pressed()
{
    on_lineEdit_returnPressed();
}
