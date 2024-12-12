#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtCharts>
#include <QSerialPortInfo>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addButton_clicked();
    void on_refreshButton_clicked();
    void on_modifyButton_clicked();
    void on_deleteButton_clicked();

    void on_eventTableWidget_cellClicked(int row, int column);
    void readArduinoData();

    void on_Recherche_clicked();
    void exportToPdf();
    void on_eventTableWidget_cellDoubleClicked(int row, int column);

    void updateStatisticsChart();
    void on_gazDetect_pressed();

    void on_gazDetect_clicked();

    void on_exporter_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *arduino;
    QString arduinoPortName;
    QString gazValueString;
    void connectToArduino();    // Méthode pour initialiser la connexion Arduino

};

#endif // MAINWINDOW_H
