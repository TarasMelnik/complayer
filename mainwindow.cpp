#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial = new QSerialPort();
    packet_tmr = new QTimer();

    connect(packet_tmr, SIGNAL(timeout()), this, SLOT(updateTime()));

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
                ui->comboBox->addItem(info.portName());

    ui->comboBox_2->addItem("2400");
    ui->comboBox_2->addItem("9600");
    ui->comboBox_2->addItem("115200");

}

MainWindow::~MainWindow()
{
    if(serial->isOpen() == true) {
        serial->close();
    }
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open file"), "",
            tr("All Files (*)"));

    file = new QFile(this);
    file->setFileName(fileName);
    if (!file->open(QIODevice::ReadOnly)) {
                QMessageBox::information(this, tr("Unable to open file"),
                    file->errorString());
                return;
    }
}

void MainWindow::updateTime()
{
    char array[512];

    if(file->read(array, packet_size) == packet_size) {
        serial->write(array, packet_size);
    }else{
        if(ui->checkBox->isChecked() == true) {
            file->seek(0);
        }
    }

}

void MainWindow::on_pushButton_clicked()
{
    packet_size = ui->spinBox->value();

    if(packet_size <= 0) {
        ui->label_5->setText("Set packet size");
        return;
    }

    if(file->isOpen() == false) {
        ui->label_5->setText("Open file");
        return;
    }

    if(serial->isOpen() == false) {

        QString serial_name = ui->comboBox->currentText();

        int baud_rate = ui->comboBox_2->currentIndex();

        switch (baud_rate) {
        case 0:
            serial->setBaudRate(QSerialPort::Baud2400);
            break;
        case 1:
            serial->setBaudRate(QSerialPort::Baud9600);
            break;
        case 2:
            serial->setBaudRate(QSerialPort::Baud115200);
            break;
        default:
            break;
        }

        serial->setPortName(serial_name);
        serial->open(QSerialPort::WriteOnly);
    }

    int timeout = ui->spinBox_2->value();
    packet_tmr->setInterval(timeout);
    packet_tmr->start();
}
