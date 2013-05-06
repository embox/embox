#include "desktopimage.h"

#include <framework/mod/options.h>
#include <module/embox/arch/x86/boot/multiboot.h>

#include <stdio.h>

extern QMdiArea *emarea;

#define MBOOTMOD embox__arch__x86__boot__multiboot
// hack because emarea->heigth(), emarea->wigth() not suitable
#define WIDTH  OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_width)
#define HEIGHT OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_height)

DesktopImageDialog::DesktopImageDialog(QStringList &images)
{
    createFilesTable();

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(filesTable, 3, 0, 1, 3);
    setLayout(mainLayout);

    filesFoundLabel = new QLabel;

    showFiles(images);

    setWindowTitle(tr("Обои на рабочий стол"));
    resize(400, 300);

    subwindow = emarea->addSubWindow(this, windowType());
}

void DesktopImageDialog::createFilesTable()
 {
     filesTable = new QTableWidget(0, 1);
     filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

     QStringList labels;
     labels << tr("Имя файла");
     filesTable->setHorizontalHeaderLabels(labels);
     filesTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
     filesTable->verticalHeader()->hide();
     filesTable->setShowGrid(false);

     connect(filesTable, SIGNAL(cellActivated(int,int)),
             this, SLOT(openFileOfItem(int,int)));
 }

void DesktopImageDialog::showFiles(const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
    }
    filesFoundLabel->setText(tr("Два клика левой кнопкой мыши, чтобы сменить обои"));
}

extern void save_pref(char *buf, int buflen);

void DesktopImageDialog::openFileOfItem(int row, int /* column */)
{
    QTableWidgetItem *item = filesTable->item(row, 0);

    //char *imageName = item->text().toAscii().data();

    QString imagePath = QString(":/images/").append(item->text());
    QImage desktopImage = QImage(imagePath).convertToFormat(QImage::Format_RGB16)
    		.scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio);
    QPixmap bgPix = QPixmap::fromImage(desktopImage);
    emarea->setBackground(bgPix);

    save_pref(item->text().toAscii().data(), strlen(item->text().toAscii().data()) + 1);

	emarea->setActiveSubWindow(subwindow);
	emarea->closeActiveSubWindow();
}
