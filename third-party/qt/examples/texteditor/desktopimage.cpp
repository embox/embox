#include "desktopimage.h"

#include <framework/mod/options.h>
#include <module/embox/arch/x86/boot/multiboot.h>

#include <stdio.h>

extern QMdiArea *emarea;

#define MBOOTMOD embox__arch__x86__boot__multiboot
// hack because emarea->heigth(), emarea->wigth() not suitable
#define WIDTH  OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_width)
#define HEIGHT OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_height)

extern void save_pref(char *wall, char *font, int font_pt);
extern void qtSetfont(const QString &fontFamily, int fontPt);

DesktopImageDialog::DesktopImageDialog(QStringList &images)
{
    createFilesTable();

    QVBoxLayout *vLayout = new QVBoxLayout;

    QFontDatabase fontDataBase;
    fontBox = new QComboBox();
    fontBox->addItems(fontDataBase.families());
    connect(fontBox, SIGNAL(currentIndexChanged(int)),this,SLOT(fontChanged(int)));

    QGridLayout *gLayout = new QGridLayout;
    gLayout->addWidget(filesTable, 3, 0, 1, 3);


    QPushButton *buttonOk = new QPushButton("Сохранить");
    connect(buttonOk, SIGNAL(released()), this, SLOT(handleOk()));

    fontPt = new QSpinBox();
    fontPt->setMinimum(6);
    fontPt->setMaximum(32);
    fontPt->setValue(10);
    connect(fontPt, SIGNAL(valueChanged(int)),this,SLOT(fontChanged(int)));

    vLayout->addLayout(gLayout);
    vLayout->addWidget(fontBox);
    vLayout->addWidget(buttonOk);
    vLayout->addWidget(fontPt);

    setLayout(vLayout);

    filesFoundLabel = new QLabel;

    showFiles(images);

    setWindowTitle(tr("Графические настройки"));
    resize(400, 300);

    subwindow = emarea->addSubWindow(this, windowType());
    selectedWallpaperRow = 0;
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
void DesktopImageDialog::handleOk()
 {

    save_pref(filesTable->item(selectedWallpaperRow, 0)->text().toAscii().data(),
		    fontBox->currentText().toAscii().data(), fontPt->value());

	emarea->setActiveSubWindow(subwindow);
	emarea->closeActiveSubWindow();
 }

void DesktopImageDialog::fontChanged(int)
{
    qtSetfont(fontBox->currentText(), fontPt->value());
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

void DesktopImageDialog::openFileOfItem(int row, int /* column */)
{
    selectedWallpaperRow = row;
    QTableWidgetItem *selectedWallpaper = filesTable->item(row, 0);

    QString imagePath = QString(":/images/").append(selectedWallpaper->text());
    QImage desktopImage = QImage(imagePath).convertToFormat(QImage::Format_RGB16)
    		.scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio);
    QPixmap bgPix = QPixmap::fromImage(desktopImage);
    emarea->setBackground(bgPix);


}
