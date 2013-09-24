#include <pwd.h>
#include "login.h"
#include "mainwindow.h"


LoginDialog::LoginDialog(QWidget *parent, QMdiArea *area) :
	QDialog(parent), emarea(area)
{
    setUpGUI();
    setWindowTitle( tr("Вход в систему") );
    setModal( true );
    subwindow = emarea->addSubWindow(this, windowType());
}

void LoginDialog::setUpGUI(){
    // set up the layout
    formGridLayout = new QGridLayout( this );

    // initialize the username combo box so that it is editable
    editUsername = new QLineEdit( this );
    //comboUsername->setEditable( true );
    // initialize the password field so that it does not echo
    // characters
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
    buttons->addButton( QDialogButtonBox::Cancel );
    buttons->button( QDialogButtonBox::Ok )->setText( tr("Login") );
    buttons->button( QDialogButtonBox::Cancel )->setText( tr("Abort") );

    // connects slots
    connect( buttons->button( QDialogButtonBox::Cancel ),
             SIGNAL(clicked()),
             this,
             SLOT(close())
             );

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

void LoginDialog::showEvent(QShowEvent *)
{
	QRect geo = subwindow->geometry();
	geo.moveCenter(emarea->parentWidget()->rect().center());
	subwindow->setGeometry(geo);
}

void LoginDialog::slotAcceptLogin(){
    QString username = editUsername->text();
    QString password = editPassword->text();

    struct spwd *spwd = getspnam_f(username.toAscii().data());

    struct passwd *pwd = getpwnam(username.toAscii().data());

    if (NULL == spwd || NULL == pwd) {
    	errorLabel->setText("<font color='red'>Неверное имя пользователя</font>");
    	formGridLayout->addWidget( errorLabel, 0, 0);
    	return;
    }

    if (0 != strcmp(spwd->sp_pwdp, password.toAscii().data())) {
    	errorLabel->setText("<font color='red'>Неверный пароль</font>");
    	formGridLayout->addWidget( errorLabel, 0, 0);
    	return;
    }

    emboxQtShowDesktop(pwd->pw_uid);

    // close this dialog
    close();
    emarea->setActiveSubWindow(subwindow);
    emarea->closeActiveSubWindow();
}

void LoginDialog::close() {
	//emarea->setActiveSubWindow(subwindow);
	//emarea->closeActiveSubWindow();
}
