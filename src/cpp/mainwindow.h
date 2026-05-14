#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void updateParkingDisplay(QString plate, int minutes, double amount);
    void clearDisplay();

private:
    QLabel *plateLabel;
    QLabel *timeLabel;
    QLabel *amountLabel;
    QLabel *statusLabel;
};

#endif // MAINWINDOW_H
