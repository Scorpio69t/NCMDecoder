#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QTableWidget>

using namespace Utils;

#define FORMAT_STRING "当前文件夹共%1个文件，已勾选%2个"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), m_selected(0) {
    ui->setupUi(this);
    m_ncmdump = std::make_unique<NcmDump>();
    m_threadPool = std::make_unique<ThreadPool>(10);
    if (!m_ncmdump->loadLibrary("liblibncmdump.dll")) {
        qDebug() << "加载库失败";
        return;
    }
    qDebug() << "加载库成功";

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"是否选中", "文件名", "文件大小 (MB)", "操作"});
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow() {
    delete ui;
}

// void MainWindow::on_pushButton_clicked()
// {
//     void* ptr = m_ncmdump->CreateNeteaseCrypt("AGA - 小问题.ncm");
//     if(ptr == nullptr) {
//         QMessageBox::warning(this, tr("警告"), tr("解析库对象创建失败"));
//         return;
//     }

//     int ret = m_ncmdump->Dump(ptr);
//     if (ret != 0) {
//         qDebug() << "dump ret:" << ret;
//     }

//     m_ncmdump->FixMetadata(ptr);

//     m_ncmdump->DestroyNeteaseCrypt(ptr);
//     QMessageBox::information(this, tr("提示"), tr("解析库测试通过"));
// }

void MainWindow::on_openDirPushButton_clicked() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("选择目录"), "",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        m_files.clear();
        ui->inDirlLineEdit->setText(dir);
        // ui->outDirPushButton->setEnabled(true);
        ui->clearSelectPushButton->setEnabled(true);
        ui->startDecodePushButton->setEnabled(true);
        m_inDir = dir;
        m_outDir = dir;
        ui->outDirlLineEdit->setText(m_outDir);
        listFiles(m_inDir);
        ui->openDirPushButton->setDisabled(true);
        m_selected = 0;
    } else {
        QMessageBox::warning(this, tr("警告"), tr("该文件夹为空，没有文件"));
    }
    updateLabel();
}

void MainWindow::listFiles(const QString &dirPath) {
    QDir dir(dirPath);
    QFileInfoList fileList = dir.entryInfoList(QStringList() << "*.ncm", QDir::Files, QDir::Name);
    if (fileList.empty()) {
        QMessageBox::warning(this, tr("警告"), tr("当前目录没有ncm文件"));
        return;
    }

    for (const QFileInfo &fileInfo: fileList) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem());
        ui->tableWidget->item(row, 0)->setCheckState(Qt::Unchecked);
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(fileInfo.fileName()));
        double fileSize = fileInfo.size() / 1024.0 / 1024.0;
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(fileSize)));

        QPushButton *transcodeButton = new QPushButton("开始解码", this);
        connect(transcodeButton, &QPushButton::clicked, this, [this, fileInfo, transcodeButton]() {
            transcodeFileWithButton(fileInfo.absoluteFilePath(), transcodeButton);
        });
        ui->tableWidget->setCellWidget(row, 3, transcodeButton);
        m_files[row] = fileInfo.absoluteFilePath();
    }
}

int MainWindow::transcodeFile(const QString &filePath) {
    // 调用库函数进行解码
    // transcodeNCMToMP3(fileInfo.absoluteFilePath(), outputDir);
    int res = -1;
    qDebug() << "调用库函数进行解码, 文件名:" << filePath;
    void *ptr = m_ncmdump->CreateNeteaseCrypt(filePath.toStdString().c_str());
    if (ptr == nullptr) {
        qDebug() << "解析库对象创建失败";
        return res;
    }

    res = m_ncmdump->Dump(ptr);
    if (res != 0) {
        qDebug() << "dump ret:" << res;
        return res;
    }

    m_ncmdump->FixMetadata(ptr);
    QString filename = filePath.mid(filePath.lastIndexOf('/') + 1);
    qDebug() << filename << "解码成功!";

    m_ncmdump->DestroyNeteaseCrypt(ptr);

    return res;
}

int MainWindow::transcodeFileWithButton(const QString &filePath, QPushButton *btn) {
    // 调用库函数进行解码
    int res = -1;
    qDebug() << "调用库函数进行解码, 文件名:" << filePath;
    void *ptr = m_ncmdump->CreateNeteaseCrypt(filePath.toStdString().c_str());
    if (ptr == nullptr) {
        qDebug() << "解析库对象创建失败";
        return res;
    }

    res = m_ncmdump->Dump(ptr);
    if (res != 0) {
        qDebug() << "dump ret:" << res;
        return res;
    }

    m_ncmdump->FixMetadata(ptr);
    btn->setDisabled(true);
    btn->setText("已解码");
    QString filename = filePath.mid(filePath.lastIndexOf('/') + 1);
    qDebug() << filename << "解码成功!";

    m_ncmdump->DestroyNeteaseCrypt(ptr);

    return res;
}

void MainWindow::updateLabel() {

    QString str = QString(FORMAT_STRING).arg(ui->tableWidget->rowCount()).arg(m_selected);
    ui->label->setText(str);
}

void MainWindow::on_tableWidget_cellClicked(int row, int column) {
    qDebug() << "row:" << row << "column:" << column;
    if (column != 0) {
        return;
    }

    int rowCount = ui->tableWidget->rowCount();
    m_selected = 0;
    for (int i = 0; i < rowCount; i++) {
        if (ui->tableWidget->item(i, 0)->checkState() == Qt::Checked) {
            m_selected++;
        }
    }

    updateLabel();
}


void MainWindow::on_checkAllPushButton_clicked() {
    int rowCount = ui->tableWidget->rowCount();
    if (ui->checkAllPushButton->text() == QString("全选")) {
        for (int i = 0; i < rowCount; i++) {
            ui->tableWidget->item(i, 0)->setCheckState(Qt::Checked);
        }
        m_selected = rowCount;
        ui->checkAllPushButton->setText(tr("取消全选"));
    } else {
        for (int i = 0; i < rowCount; i++) {
            ui->tableWidget->item(i, 0)->setCheckState(Qt::Unchecked);
        }
        m_selected = 0;
        ui->checkAllPushButton->setText(tr("全选"));
    }
    updateLabel();
}

void MainWindow::on_clearSelectPushButton_clicked() {
    m_inDir = "";
    m_outDir = "";
    ui->inDirlLineEdit->setText(m_inDir);
    ui->outDirlLineEdit->setText(m_outDir);
    int rowCount = ui->tableWidget->rowCount();
    for (int i = 0; i < rowCount; i++) {
        ui->tableWidget->removeRow(i);
    }
    m_selected = 0;
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"是否选中", "文件名", "文件大小 (MB)", "操作"});
    ui->clearSelectPushButton->setDisabled(true);
    ui->startDecodePushButton->setDisabled(true);
    ui->openDirPushButton->setEnabled(true);
    updateLabel();
}

void MainWindow::on_startDecodePushButton_clicked() {
    bool flag = false;
    int rowCount = ui->tableWidget->rowCount();
    std::vector<std::future<int>> futures;

    for (int i = 0; i < rowCount; i++) {
        if (ui->tableWidget->item(i, 0)->checkState() == Qt::Checked) {
            auto filePath = m_files[i];
            auto *btn = reinterpret_cast<QPushButton *>(ui->tableWidget->cellWidget(i, 3));
            if (btn->text() == QString("已解码")) {
                continue;
            }
            // 将任务添加到线程池执行
            futures.emplace_back(m_threadPool->addTask([this, filePath]() -> int { return transcodeFile(filePath); }));
            btn->setDisabled(true);
            btn->setText("已解码");
        }
    }

    // 收集执行结果
    std::vector<int> results;
    for (auto &f: futures) {
        results.push_back(f.get());
    }

    int count = 0;
    for(auto &r : results) {
        if (r == 0) {
            flag = true;
            count++;
        }
    }

    if (!flag) {
        QMessageBox::warning(this, tr("警告"), tr("未选择任何文件"));
    } else {
        QString str = tr("%1个文件解码成功").arg(count);
        QMessageBox::information(this, tr("提示"), str);
    }
}
