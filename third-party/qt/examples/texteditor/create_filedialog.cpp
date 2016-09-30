#include "createfiledialog.h"

TRY::TRY()
{
    fileNameEdit = new QLineEdit;

    fileNameLabel = new QLabel(tr("File name:"));

    okButton = createButton(tr("&Ok"), SLOT(createFile()));
    cancelButton = createButton(tr("&Cancel"), SLOT(close()));

         QHBoxLayout *buttonsLayout = new QHBoxLayout;
         buttonsLayout->addStretch();
         buttonsLayout->addWidget(okButton);
         buttonsLayout->addWidget(cancelButton);

    QGridLayout *mainLayout = new QGridLayout;
         //mainLayout->addWidget(browseButton, 2, 2);
    mainLayout->addWidget(fileNameLabel, 3, 0, 1, 3);
    mainLayout->addWidget(fileNameEdit, 4, 0, 1, 3);
    mainLayout->addLayout(buttonsLayout, 5, 0, 1, 3);
    setLayout(mainLayout);

    setWindowTitle(tr("Find Files"));
    resize(700, 300);
}

void OpenFileDialog::openFileOfItem(int row, int /* column */)
{
    QTableWidgetItem *item = filesTable->item(row, 0);

    *fileName = currentDir.absoluteFilePath(item->text());
    QFile file(*fileName);

    file.open(QFile::ReadOnly | QFile::Text);

    QTextStream ReadFile(&file);
    textEdit->setText(ReadFile.readAll());
    file.close();
    this->close();
}
