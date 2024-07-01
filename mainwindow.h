#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QHash>
#include "ncmdump.h"
#include "ThreadPool.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    // void on_pushButton_clicked();

    void on_openDirPushButton_clicked();

    void on_tableWidget_cellClicked(int row, int column);

    void on_checkAllPushButton_clicked();

    void on_clearSelectPushButton_clicked();

    void on_startDecodePushButton_clicked();

private:
    void listFiles(const QString &dirPath);

    int transcodeFile(const QString &filePath);

    int transcodeFileWithButton(const QString &filePath, QPushButton *btn);

    void updateLabel();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<NcmDump> m_ncmdump;
    std::unique_ptr<Utils::ThreadPool> m_threadPool;
    QString m_inDir;
    QString m_outDir;
    QHash<int, QString> m_files;
    int m_selected;
};

#endif // MAINWINDOW_H
