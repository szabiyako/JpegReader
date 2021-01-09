#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jpegreader.h"

#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select a file", QDir::currentPath(), "JPEG file (*.jpg *.jpeg)");
    if (filePath.isEmpty())
        return;
    bool hasError;
    QString errorMessage;
    JpegReader::read(filePath, hasError, errorMessage);
    if (hasError)
    {
        QMessageBox::warning(this, "Error", errorMessage);
        return;
    }
}
