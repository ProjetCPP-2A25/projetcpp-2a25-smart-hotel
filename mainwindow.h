#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "reservations.h" // Include your Reservations class

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addButton_clicked(); // Slot for handling add button clicks
    void on_updateButton_clicked();
    void on_deleteButton_clicked();
    void on_searchButton_clicked();
    void on_sauvegarder_clicked();
    void on_exportButton_clicked();
    void on_statistique_clicked();
    void on_comboBoxIndexChanged(int index);
    void on_generateTrendsButton_clicked();
    void checkDepartures();
private:
    Ui::MainWindow *ui;
    QMediaPlayer *musicPlayer; // Lecteur de musique
    QAudioOutput *audioOutput; // Sortie audio

    void refreshTable(); // Function to refresh the table after adding a reservation
};

#endif // MAINWINDOW_H
