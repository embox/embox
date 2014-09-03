#include <pwd.h>
#include <crypt.h>
#include "login.h"
#include "mainwindow.h"


LoginDialog::LoginDialog(QWidget *parent, QMdiArea *area) :
	QDialog(parent), emarea(area), closable(false) {
    setUpGUI();
    setWindowTitle( tr("Вход в систему") );
    setModal( true );
    subwindow = emarea->addSubWindow(this, windowType());
}

void LoginDialog::setUpGUI() {
    // set up the layout
    formGridLayout = new QGridLayout( this );

    editUsername = new QLineEdit( this );
    editPassword = new QLineEdit( this );
    editPassword->setEchoMode( QLineEdit::Password );

    // initialize the labels
    labelUsername = new QLabel( this );
    labelPassword = new QLabel( this );
    labelUsername->setText( tr( "Имя пользователя" ) );
    labelUsername->setBuddy( editUsername );
    labelPassword->setText( tr( "Пароль" ) );
    labelPassword->setBuddy( editPassword );

    // initialize buttons
    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok );
    buttons->button( QDialogButtonBox::Ok )->setText( tr("Login") );

    connect( buttons->button( QDialogButtonBox::Ok ),
             SIGNAL(clicked()),
             this,
             SLOT(slotAcceptLogin()) );

    errorLabel = new QLabel(this);
    errorLabel->setText( tr( "" ) );

    // place components into the dialog
    formGridLayout->addWidget( errorLabel, 0, 0 );
    formGridLayout->addWidget( labelUsername, 1, 0 );
    formGridLayout->addWidget( editUsername, 1, 1 );
    formGridLayout->addWidget( labelPassword, 2, 0 );
    formGridLayout->addWidget( editPassword, 2, 1 );
    formGridLayout->addWidget( buttons, 3, 0, 1, 2 );

    setLayout( formGridLayout );
}

void LoginDialog::showEvent(QShowEvent *) {
	QRect geo = subwindow->geometry();
	geo.moveCenter(emarea->parentWidget()->rect().center());
	subwindow->setGeometry(geo);
}

void LoginDialog::closeEvent(QCloseEvent *ev) {
	closable ? QDialog::closeEvent(ev) : ev->ignore();
}

void LoginDialog::slotAcceptLogin() {
    QString username = editUsername->text();
    QString password = editPassword->text();

    struct spwd *spwd = getspnam_f(username.toAscii().data());

    struct passwd *pwd = getpwnam(username.toAscii().data());

    if (NULL == spwd || NULL == pwd) {
    	errorLabel->setText("<font color='red'>Неверное имя пользователя</font>");
    	formGridLayout->addWidget( errorLabel, 0, 0);
    	return;
    }

    if (0 != strcmp(spwd->sp_pwdp, crypt(password.toAscii().data(), NULL))) {
    	errorLabel->setText("<font color='red'>Неверный пароль</font>");
    	formGridLayout->addWidget( errorLabel, 0, 0);
    	return;
    }

    emboxQtShowDesktop(pwd->pw_uid);

    // close this dialog
    closable = true;
    emarea->setActiveSubWindow(subwindow);
    emarea->closeActiveSubWindow();
}
