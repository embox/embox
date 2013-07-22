#ifndef DESKTOPIMAGE_H_
#define DESKTOPIMAGE_H_

#include <QtGui>

class DesktopImageDialog : public QWidget
{
    Q_OBJECT
public:
    explicit DesktopImageDialog(QStringList &images);

private slots:
    void openFileOfItem(int row, int column);
    void handleOk();
    void setFont(int);
private:
    void showFiles(const QStringList &files);
    QComboBox *createComboBox(const QString &text = QString());
    void createFilesTable();
    int selectedWallpaperRow;


    QComboBox *fontBox;

    QLabel *filesFoundLabel;
    QTableWidget *filesTable;
    QMdiSubWindow *subwindow;
};

#endif /* DESKTOPIMAGE_H_ */
