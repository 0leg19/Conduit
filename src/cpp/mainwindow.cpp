#include "mainwindow.h"
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setSpacing(20);
    layout->setContentsMargins(20, 20, 20, 20);

    // Status Label
    statusLabel = new QLabel("ველოდებით მანქანის მონაცემებს...", this);
    statusLabel->setStyleSheet("font-size: 32px; color: #7f8c8d; font-weight: bold;");
    statusLabel->setAlignment(Qt::AlignCenter);

    // Plate Label
    plateLabel = new QLabel("--", this);
    plateLabel->setStyleSheet("font-size: 90px; font-weight: bold; color: #2c3e50; border: 5px solid #2c3e50; padding: 20px;");
    plateLabel->setAlignment(Qt::AlignCenter);

    // Time Label
    timeLabel = new QLabel("დრო: 0 წთ", this);
    timeLabel->setStyleSheet("font-size: 60px; color: #e74c3c; font-weight: bold;");
    timeLabel->setAlignment(Qt::AlignCenter);

    // Amount Label
    amountLabel = new QLabel("თანხა: 0 ₾", this);
    amountLabel->setStyleSheet("font-size: 60px; color: #27ae60; font-weight: bold;");
    amountLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(statusLabel);
    layout->addStretch();
    layout->addWidget(plateLabel);
    layout->addWidget(timeLabel);
    layout->addWidget(amountLabel);
    layout->addStretch();

    setCentralWidget(centralWidget);
    setStyleSheet("background-color: #ecf0f1;");
    setWindowTitle("პარკირების კიოსკი");

    // Fullscreen
    showFullScreen();
}

MainWindow::~MainWindow() {}

void MainWindow::updateParkingDisplay(QString plate, int minutes, double amount)
{
    plateLabel->setText(plate);
    timeLabel->setText(QString("დრო: %1 წთ").arg(minutes));
    amountLabel->setText(QString("თანხა: %1 ₾").arg(amount, 0, 'f', 2));
    statusLabel->setText("მანქანის მონაცემები:");
}

void MainWindow::clearDisplay()
{
    plateLabel->setText("--");
    timeLabel->setText("დრო: 0 წთ");
    amountLabel->setText("თანხა: 0 ₾");
    statusLabel->setText("ველოდებით მანქანის მონაცემებს...");
}
