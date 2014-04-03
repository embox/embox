#include "openfiledialog.h"
#include "mainwindow.h"

extern QMdiArea *emarea;

OpenFileDialog::OpenFileDialog(QTextEdit *textEdit, QString *fileName, QMainWindow *mainWindow)
{
    this->textEdit = textEdit;
    this->fileName = fileName;
    this->mainWindow = mainWindow;

    fileLabel = new QLabel(tr("Имя файла:"));
    textLabel = new QLabel(tr("Содержит текст:"));
    directoryLabel = new QLabel(tr("В папке:"));
    filesFoundLabel = new QLabel;

    createFilesTable();
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(filesTable, 3, 0, 1, 3);
    mainLayout->addWidget(filesFoundLabel, 4, 0, 1, 3);
    setLayout(mainLayout);

    find();

    setWindowTitle(tr("Открыть файл"));
    resize(700, 300);

    subwindow = emarea->addSubWindow(this, windowType());
}

void OpenFileDialog::createFilesTable()
 {
     filesTable = new QTableWidget(0, 2);
     filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

     QStringList labels;
     labels << tr("Имя файла") << tr("Размер");
     filesTable->setHorizontalHeaderLabels(labels);
     filesTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
     filesTable->verticalHeader()->hide();
     filesTable->setShowGrid(false);

     connect(filesTable, SIGNAL(cellActivated(int,int)),
             this, SLOT(openFileOfItem(int,int)));
 }

void OpenFileDialog::showFiles(const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(currentDir.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                             .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, sizeItem);
    }
    filesFoundLabel->setText(tr("Два клика левой кнопкой мыши, чтобы открыть файла"));
}

void OpenFileDialog::find()
 {
     filesTable->setRowCount(0);

     currentDir = QDir(TEDIT_DIR);
     currentDir.setNameFilters(QStringList() << QLatin1String("*.txt"));
     currentDir.refresh();

     QStringList files;
     files = currentDir.entryList(QStringList("*"), QDir::Files | QDir::NoSymLinks);

     showFiles(files);
 }


void OpenFileDialog::openFileOfItem(int row, int /* column */)
{
    QTableWidgetItem *item = filesTable->item(row, 0);

    *fileName = currentDir.absoluteFilePath(item->text());
    QFile file(*fileName);

    if (file.open(QFile::ReadOnly | QFile::Text)){
        QTextStream ReadFile(&file);
        textEdit->setText(ReadFile.readAll());
        mainWindow->setWindowTitle(*fileName + " - " + TEDIT_APP_TITLE);
        file.close();
    }
    else{
        //TODO: error message
    }
	emarea->setActiveSubWindow(subwindow);
	emarea->closeActiveSubWindow();
}
