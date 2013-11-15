#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <unistd.h>
#include <fcntl.h>

#include <QScrollBar>

#include <iostream>

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

#ifdef __EMBOX__
extern
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
extern
int pthread_rwlock_init(pthread_rwlock_t * rwlock,
    const pthread_rwlockattr_t * attr);
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
extern
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
extern
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
extern
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
extern
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
#endif //__EMBOX__
#include <qpid/log/Logger.h>
#include <qpid/log/Options.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int flags;

    qpid::log::Options logopt;
    const char *argv[]={"qpidhello","--log-to-stdout=yes"};
    logopt.parse(sizeof(argv)/sizeof(char*), argv);
    logopt.trace = true;
    qpid::log::Logger::instance().configure(logopt);
    //::setenv("QPID_LOG_ENABLE","trace+",0);
    //::setenv("QPID_LOG_OUTPUT","stdout",0);
    //::setenv("QPID_LOG_TO_FILE","stdout",0);

    pipe(pipefd);
    flags = fcntl(pipefd[0], F_GETFL, 0);
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
    //flags = fcntl(pipefd[1], F_GETFL, 0);
    //fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK);

    /*
    oldstd[0] = dup(STDOUT_FILENO);
    oldstd[1] = dup(STDERR_FILENO);
    ::close(STDOUT_FILENO);
    dup(pipefd[1]);
    ::close(STDERR_FILENO);
    dup(pipefd[1]);
    */
    fdoutbuf *ob = new fdoutbuf(pipefd[1]);
    oldstd[1] = std::cerr.rdbuf(ob);
    oldstd[0] = std::cout.rdbuf(ob);

    notifier.reset(new QSocketNotifier(pipefd[0], QSocketNotifier::Read));
    connect(notifier.data(), SIGNAL(activated(int)), this, SLOT(onStdout()));

    ui->setupUi(this);

    ui->lineEdit->setFocus();
    ui->lineEdit->selectAll();
}

MainWindow::~MainWindow()
{
    /*
    ::close(STDOUT_FILENO);
    dup(oldstd[0]);
    ::close(STDERR_FILENO);
    dup(oldstd[1]);
    ::close(oldstd[0]);
    ::close(oldstd[1]);
    */
    std::streambuf *ob = std::cout.rdbuf(oldstd[0]);
    std::cerr.rdbuf(oldstd[1]);
    delete ob;

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
    if (!thread.isNull()) {
        return;
    }
    brok = ui->lineEdit->text();
    brok.toAscii().data();
    ui->plainTextEdit->clear();

    thread.reset(new QThread(this));
    QpidWorker *w = new QpidWorker(this);
    connect(thread.data(), SIGNAL(started()), w, SLOT(doQPIDHello()));
    connect(thread.data(), SIGNAL(finished()), this, SLOT(onFinished()));
    w->moveToThread(thread.data());
    thread->start();
}

void MainWindow::onFinished()
{
    onStdout();
    std::cout << std::endl << "Finished.";
    delete thread.take();
}

static int qpidhello(char *br);

void QpidWorker::doQPIDHello()
{
    std::cout << "Started with broker: " << mw->brok.toAscii().data() << "." << std::endl << std::endl;

    if (mw->brok.length())
        qpidhello(mw->brok.toAscii().data());
    else
        qpidhello(NULL);

    mw->thread->exit();
}

#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>

using namespace qpid::messaging;

static int qpidhello(char *br) {
    std::string broker = br ? br : "localhost:5672";
    std::string address = "amq.topic";
    std::string connectionOptions = "";

    Connection connection(broker, connectionOptions);
    try {
        connection.open();
        Session session = connection.createSession();

        Receiver receiver = session.createReceiver(address);
        Sender sender = session.createSender(address);

        sender.send(Message("Hello world!"));

        Message message = receiver.fetch(Duration::SECOND * 1);
        std::cout << message.getContent() << std::endl;
        session.acknowledge();

        connection.close();
        return 0;
    } catch(const std::exception& error) {
        std::cerr << error.what() << std::endl;
        connection.close();
        return 1;
    }
}
