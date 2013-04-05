#include "helpwindow.h"

extern QMdiArea *emarea;

QWizardPage *createIntroPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("О редакторе");

    QLabel *label = new QLabel("Текстовый редактор ZarjaEditor версии 1.0.");
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    page->setLayout(layout);

    return page;
}

EWisard::EWisard() {
    addPage(createIntroPage());
    setWindowTitle("Справка");
    resize(600, 300);
    subwindow = emarea->addSubWindow(this, windowType());
}

void EWisard::setVisible(bool visible) {
	if (!visible) {
		emarea->setActiveSubWindow(subwindow);
		emarea->closeActiveSubWindow();
	} else {
	    restart();
	}
	QDialog::setVisible(visible);
}
