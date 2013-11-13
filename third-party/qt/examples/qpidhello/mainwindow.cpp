#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <unistd.h>
#include <fcntl.h>

#include <QScrollBar>

#include <streambuf>
#include <iostream>

/*
class fdoutbuf : public std::streambuf {
  protected:
    int fd;    // file descriptor
  public:
    // constructor
    fdoutbuf (int _fd) : fd(_fd) {
    }
  protected:
    // write one character
    virtual int_type overflow (int_type c) {
        if (c != EOF) {
            char z = c;
            if (write (fd, &z, 1) != 1) {
                return EOF;
            }
        }
        return c;
    }
    // write multiple characters
    virtual
    std::streamsize xsputn (const char* s,
                            std::streamsize num) {
        return write(fd,s,num);
    }
};
*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int flags;

    pipe(pipefd);
    flags = fcntl(pipefd[0], F_GETFL, 0);
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(pipefd[1], F_GETFL, 0);
    fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK);

    oldstd[0] = dup(STDOUT_FILENO);
    oldstd[1] = dup(STDERR_FILENO);
    ::close(STDOUT_FILENO);
    dup(pipefd[1]);
    ::close(STDERR_FILENO);
    dup(pipefd[1]);
    //std::cout.rdbuf(new fdoutbuf(pipefd[1]));

    notifier.reset(new QSocketNotifier(pipefd[0], QSocketNotifier::Read));
    connect(notifier.data(), SIGNAL(activated(int)), this, SLOT(onStdout()));

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    ::close(STDOUT_FILENO);
    dup(oldstd[0]);
    ::close(STDERR_FILENO);
    dup(oldstd[1]);
    ::close(oldstd[0]);
    ::close(oldstd[1]);

    ::close(pipefd[0]);
    ::close(pipefd[1]);

    delete ui;
}

void MainWindow::onStdout()
{
    char buf[256];
    ssize_t read;

    do {
        read = ::read(pipefd[0], buf, 256);
        ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText().append(QByteArray(buf,read)));
    } while (read>0);

    QScrollBar *sb = ui->plainTextEdit->verticalScrollBar();
    sb->setSliderPosition(sb->maximum());
}

void MainWindow::on_pushButton_pressed()
{
    on_lineEdit_returnPressed();
}

void MainWindow::on_lineEdit_returnPressed()
{
    printf("qpidapp:>>> %s\n", ui->lineEdit->text().toAscii().data());
    fflush(stdout);
    //std::cout << "qpidapp:>>> " << ui->lineEdit->text().toAscii().data() << std::endl;
}

