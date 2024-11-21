#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "reservations.h"
#include "connection.h"
#include <QMessageBox>
#include "mainwindow.h"
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QDateTime>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QKeyEvent>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QVBoxLayout>
#include <QtCharts/QSplineSeries>
#include <QDate>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPropertyAnimation>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    musicPlayer(new QMediaPlayer(this)),
    audioOutput(new QAudioOutput(this))
{
    ui->setupUi(this);
    musicPlayer->setAudioOutput(audioOutput);
    musicPlayer->setSource(QUrl::fromLocalFile("C://Users//ghrib//Documents//reservation//cool-jazz-loops-2641.mp3")); // Remplacez le chemin par votre fichier audio
    musicPlayer->setLoops(QMediaPlayer::Infinite);
    musicPlayer->play();
    QTimer::singleShot(500, this, [this]() {
            if (QSqlDatabase::database().isOpen()) {
                checkDepartures();
                qDebug() << "checkDepartures() exécutée après délai.";
            } else {
                qDebug() << "La connexion à la base de données n'est toujours pas ouverte après délai.";
            }
        });
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::checkDepartures);
    timer->start(3600000); // Toutes les heures (3600000 ms)
    connect(ui->updateButton, &QPushButton::clicked, this, &MainWindow::on_updateButton_clicked); // Connect only once here
    connect(ui->trierLine, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &::MainWindow::on_comboBoxIndexChanged);
    connect(ui->exportButton, &QPushButton::clicked, this, &MainWindow::on_exportButton_clicked);
    connect(ui->generateTrendsButton, &QPushButton::clicked, this, &MainWindow::on_generateTrendsButton_clicked);
}

void MainWindow::on_generateTrendsButton_clicked()
{
    // Requête SQL pour calculer les revenus trimestriels
    QString quarterlyQuery =
        "SELECT "
        "  CASE "
        "    WHEN EXTRACT(MONTH FROM DATE_ARRIVE) IN (1, 2, 3) THEN 'Trimestre 1' "
        "    WHEN EXTRACT(MONTH FROM DATE_ARRIVE) IN (4, 5, 6) THEN 'Trimestre 2' "
        "    WHEN EXTRACT(MONTH FROM DATE_ARRIVE) IN (7, 8, 9) THEN 'Trimestre 3' "
        "    WHEN EXTRACT(MONTH FROM DATE_ARRIVE) IN (10, 11, 12) THEN 'Trimestre 4' "
        "  END AS trimestre, "
        "  SUM(MONTANT_TOTAL) AS total_trimestre "
        "FROM RESERVATIONS "
        "GROUP BY "
        "  CASE "
        "    WHEN EXTRACT(MONTH FROM DATE_ARRIVE) IN (1, 2, 3) THEN 'Trimestre 1' "
        "    WHEN EXTRACT(MONTH FROM DATE_ARRIVE) IN (4, 5, 6) THEN 'Trimestre 2' "
        "    WHEN EXTRACT(MONTH FROM DATE_ARRIVE) IN (7, 8, 9) THEN 'Trimestre 3' "
        "    WHEN EXTRACT(MONTH FROM DATE_ARRIVE) IN (10, 11, 12) THEN 'Trimestre 4' "
        "  END "
        "ORDER BY trimestre";
    QSqlQuery query;
    if (!query.exec(quarterlyQuery)) {
        qDebug() << "Erreur lors de l'exécution de la requête trimestrielle :" << query.lastError().text();
        return;
    }
    // Création d'une table pour afficher les résultats
    QTableWidget *tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Trimestres" << "Revenus (DT)");
    // Ajout des données au tableau
    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString())); // Trimestre
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(query.value(1).toDouble(), 'f', 2))); // Revenus
        row++;
    }

    // Ajustement des colonnes et affichage
    tableWidget->resizeColumnsToContents();

    // Afficher la table dans une boîte de dialogue
    QDialog *tableDialog = new QDialog(this);
    tableDialog->setWindowTitle("Tableau des Revenus Trimestriels");
    QVBoxLayout *layout = new QVBoxLayout(tableDialog);
    layout->addWidget(tableWidget);
    tableDialog->setLayout(layout);
    tableDialog->exec();
}


void MainWindow::on_statistique_clicked() {
    // Créer un QSqlQuery pour récupérer les données
    QSqlQuery query;

    // Requête pour obtenir les réservations groupées par mois
    query.prepare(
        "SELECT EXTRACT(MONTH FROM DATE_ARRIVE) AS mois, COUNT(*) AS total "
        "FROM RESERVATIONS "
        "GROUP BY EXTRACT(MONTH FROM DATE_ARRIVE) "
        "ORDER BY mois ASC"
    );

    // Vérifiez si la requête s'exécute correctement
    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête :" << query.lastError().text();
        return;
    }

    // Créer un ensemble de barres pour stocker les données
    QBarSet *set = new QBarSet("Reservations");

    // Liste pour stocker les noms des mois
    QStringList months;

    // Parcourir les résultats de la requête
    while (query.next()) {
        int mois = query.value("mois").toInt();
        int total = query.value("total").toInt();

        // Ajouter les données au set
        *set << total;

        // Ajouter le mois correspondant à la liste (remplacez par des noms de mois si nécessaire)
        switch (mois) {
            case 1: months << "Janvier"; break;
            case 2: months << "Février"; break;
            case 3: months << "Mars"; break;
            case 4: months << "Avril"; break;
            case 5: months << "Mai"; break;
            case 6: months << "Juin"; break;
            case 7: months << "Juillet"; break;
            case 8: months << "Août"; break;
            case 9: months << "Septembre"; break;
            case 10: months << "Octobre"; break;
            case 11: months << "Novembre"; break;
            case 12: months << "Décembre"; break;
        }
    }

    // Trouver la valeur maximale pour définir l'axe Y
    double maxValue = 0;
    for (int i = 0; i < set->count(); ++i) {
        double value = set->at(i);
        if (value > maxValue) {
            maxValue = value;
        }
    }

    // Créer une série de barres et y ajouter le set
    QBarSeries *series = new QBarSeries();
    series->append(set);

    // Créer le graphique et y ajouter la série
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des Réservations");

    // Créer l'axe Y basé sur la valeur maximale
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, maxValue + 5); // Ajouter une marge au-dessus de la valeur max
    axisY->setTitleText("Nombre de Réservations");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Créer l'axe X avec les noms des mois
    QCategoryAxis *axisX = new QCategoryAxis();
    for (int i = 0; i < months.size(); ++i) {
        axisX->append(months[i], i);
    }
    axisX->setTitleText("Mois");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Créer la vue du graphique
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Créer une nouvelle boîte de dialogue pour afficher le graphique
    QDialog *chartDialog = new QDialog(this);
    chartDialog->setWindowTitle("Statistiques des Réservations");

    // Ajouter le graphique à la boîte de dialogue
    QVBoxLayout *layout = new QVBoxLayout(chartDialog);
    layout->addWidget(chartView);
    chartDialog->setLayout(layout);

    // Ajuster la taille et afficher la boîte de dialogue
    chartDialog->resize(600, 400);
    chartDialog->exec();
}

void MainWindow::on_exportButton_clicked() {
    connect(ui->exportButton, &QPushButton::clicked, this, &MainWindow::on_exportButton_clicked);
    qDebug() << "Export Button clicked";

    // 1. Choose a location and file name for the PDF
    QString listofcomplaints = QFileDialog::getSaveFileName(this, "Save PDF", "", "*.pdf");
    if (listofcomplaints.isEmpty()) {
        return; // User canceled the dialog
    }

    // Add ".pdf" if the user did not specify it
    if (!listofcomplaints.endsWith(".pdf", Qt::CaseInsensitive)) {
        listofcomplaints += ".pdf";
    }

    // 2. Create a QPdfWriter instance
    QPdfWriter pdfWriter(listofcomplaints);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageMargins(QMargins(30, 30, 30, 30)); // Adjust margins as needed

    // 3. Set up QPainter for drawing on the PDF
    QPainter painter(&pdfWriter);
    painter.setPen(Qt::black);

    // Optional: Add a title or header
    QFont titleFont("Arial", 16, QFont::Bold);
    painter.setFont(titleFont);
    painter.drawText(100, 100, "La Liste Des Reservations"); // Position (x, y) and text content

    // Optional: Add a timestamp
    QFont dateFont("Arial", 10);
    painter.setFont(dateFont);
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    painter.drawText(4000, 100, "Generated on: " + timestamp);

    // 4. Draw the table data (example for QTableView)
    QFont dataFont("Arial", 10);
    painter.setFont(dataFont);

    int rowHeight = 300;
    int xPos = 100;
    int yPos = 300;

    // Example: Loop through rows and columns of QTableV iew and write to PDF
    QAbstractItemModel *model = ui->tableWidget->model();
    for (int row = 0; row < model->rowCount(); ++row) {
        xPos = 100;
        for (int col = 0; col < model->columnCount(); ++col) {
            QString cellData = model->data(model->index(row, col)).toString();
            painter.drawText(xPos, yPos, cellData);
            xPos += 1000; // Adjust column width as needed
        }
        yPos += rowHeight; // Move to the next row
    }

    // 5. End the painting and save the PDF
    painter.end();

    // Optional: Confirm successful export
    QMessageBox::information(this, "Export Successful", "PDF file has been successfully created.");
}

// Add a reservation
void MainWindow::on_addButton_clicked() {
    Reservations res;
    res.setID_Res(ui->lineID->text().toInt());
    res.setNom_client(ui->lineName->text());
    res.setDate_Arr(datee(ui->datearr->date().day(), ui->datearr->date().month(), ui->datearr->date().year()));
    res.setDate_Dep(datee(ui->datedep->date().day(), ui->datedep->date().month(), ui->datedep->date().year()));
    res.setType_Res(ui->typeLine->text());
    res.setNbr_perso(ui->nbr_perso->text().toInt());
    res.setMode_Paiement(ui->modePaiLine->currentText());
    res.setMontant_Total(ui->montantLine->text().toInt());

    if (res.ajouter()) {
        QMessageBox::information(this, "Success", "Reservation added successfully!");
        refreshTable();
        ui->lineID->clear();
        ui->lineName->clear();
        ui->typeLine->clear();
        ui->nbr_perso->clear();
        ui->montantLine->clear();
    } else {
        QMessageBox::warning(this, "Error", "Failed to add reservation.");
    }
}

// Refresh the table
void MainWindow::refreshTable() {
    Reservations res;
    QSqlQueryModel *model = res.afficher();

    ui->tableWidget->clearContents(); // Keep headers intact
    ui->tableWidget->setRowCount(0);

    if (model) {
        ui->tableWidget->setRowCount(model->rowCount());
        ui->tableWidget->setColumnCount(model->columnCount());

        for (int row = 0; row < model->rowCount(); ++row) {
            for (int column = 0; column < model->columnCount(); ++column) {
                QTableWidgetItem *newItem = new QTableWidgetItem(model->data(model->index(row, column)).toString());
                ui->tableWidget->setItem(row, column, newItem);
            }
        }
        delete model;
    }
}

void MainWindow::on_updateButton_clicked() {
    int id = ui->seearchLine->text().toInt();

    Reservations res;
    QSqlQueryModel* model = res.rechercher(id);

    if (model && model->rowCount() > 0) {
        ui->lineID->setText(model->data(model->index(0, 0)).toString());
        ui->lineName->setText(model->data(model->index(0, 1)).toString());
        ui->typeLine->setText(model->data(model->index(0, 4)).toString());
        ui->modePaiLine->setCurrentText(model->data(model->index(0, 5)).toString());
        ui->montantLine->setText(model->data(model->index(0, 6)).toString());
        ui->nbr_perso->setText(model->data(model->index(0, 7)).toString());
        QString dateArrStr = model->data(model->index(0, 2)).toString();
        QDate dateArr = QDate::fromString(dateArrStr, "yyyy-MM-dd");
        ui->datearr->setDate(dateArr);

        QString dateDepStr = model->data(model->index(0, 3)).toString();
        QDate dateDep = QDate::fromString(dateDepStr, "yyyy-MM-dd");
        ui->datedep->setDate(dateDep);

        delete model;
    } else {
        QMessageBox::warning(this, "Not Found", "No reservation found with that ID.");
        delete model;
    }
}

void MainWindow::on_sauvegarder_clicked() {
    int id = ui->lineID->text().toInt();
    QString nomClient = ui->lineName->text();
    QDate dateArr = ui->datearr->date();
    QDate dateDep = ui->datedep->date();
    QString typeRes = ui->typeLine->text();
    int Nbr_perso = ui->nbr_perso->text().toInt();
    QString modePaiement = ui->modePaiLine->currentText();
    float montantTotal = ui->montantLine->text().toFloat();

    Reservations res;

    // Corrected the datee function calls and fixed the if condition syntax
    if (res.modifier(id, nomClient,
                     datee(dateArr.day(), dateArr.month(), dateArr.year()),
                     datee(dateDep.day(), dateDep.month(), dateDep.year()),
                     typeRes, modePaiement, montantTotal, Nbr_perso)) {
        QMessageBox::information(this, "Success", "Reservation updated successfully.");
        refreshTable();
        ui->lineID->clear();
        ui->lineName->clear();
        ui->typeLine->clear();
        ui->montantLine->clear();
        ui->nbr_perso->clear();
    } else {
        QMessageBox::warning(this, "Update Failed", "Failed to update reservation.");
    }
}


void MainWindow::on_deleteButton_clicked() {
    int id = ui->seearchLine->text().toInt();
    Reservations res;
    if (res.supprimer(id)) {
        QMessageBox::information(this, "Success", "Reservation deleted successfully!");
        refreshTable();
    } else {
        QMessageBox::warning(this, "Error", "Failed to delete reservation.");
    }
}

void MainWindow::on_searchButton_clicked() {
    int id = ui->seearchLine->text().toInt();
    Reservations res;
    QSqlQueryModel* model = res.rechercher(id);

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    if (model) {
        ui->tableWidget->setRowCount(model->rowCount());
        ui->tableWidget->setColumnCount(model->columnCount());

        for (int row = 0; row < model->rowCount(); ++row) {
            for (int column = 0; column < model->columnCount(); ++column) {
                QTableWidgetItem *newItem = new QTableWidgetItem(model->data(model->index(row, column)).toString());
                ui->tableWidget->setItem(row, column, newItem);
            }
        }
        delete model;
    } else {
        QMessageBox::information(this, "Not Found", "No reservation found with that ID.");
    }
}

MainWindow::~MainWindow() {
    delete musicPlayer;
    delete audioOutput;
    delete ui;

}
void MainWindow::on_comboBoxIndexChanged(int index) {
    Reservations res;
    QSqlQueryModel* model = nullptr;

    // Check the selected index and sort accordingly
    if (index == 1) { // Assuming "Sort by Date" is at index 0
        model = res.sortByColumn("DATE_ARRIVE");
    }

    if (model) {
        ui->tableWidget->clearContents(); // Keep headers intact
        ui->tableWidget->setRowCount(model->rowCount());
        ui->tableWidget->setColumnCount(model->columnCount());

        for (int row = 0; row < model->rowCount(); ++row) {
            for (int column = 0; column < model->columnCount(); ++column) {
                QTableWidgetItem* newItem = new QTableWidgetItem(model->data(model->index(row, column)).toString());
                ui->tableWidget->setItem(row, column, newItem);
            }
        }
        delete model;
    } else {
        QMessageBox::warning(this, "Error", "Failed to sort reservations.");
    }
}
void MainWindow::checkDepartures()
{
    // Créer un objet de requête SQL
    QSqlQuery query1;

    // Préparer la requête SQL pour récupérer les départs d'aujourd'hui
    query1.prepare("SELECT ID_RES, NOM_CLIENT, DATE_DEPART FROM RESERVATIONS WHERE TRUNC(DATE_DEPART) = TRUNC(SYSDATE)");

    // Exécuter la requête
    if (query1.exec()) {
        bool found = false;

        // Parcourir les résultats de la requête
        while (query1.next()) {
            int reservationId = query1.value("ID_RES").toInt();
            QString clientName = query1.value("NOM_CLIENT").toString();
            QString departureDate = query1.value("DATE_DEPART").toString();


            // Afficher un message de rappel pour chaque départ trouvé
            departureDate = departureDate.split("T").first();
            QMessageBox::information(this, "Rappel : Départ Aujourd'hui",
                                     QString("Le client %1 doit partir aujourd'hui (%2).")
                                     .arg(clientName, departureDate));
            found = true;
            QSqlQuery deleteQuery;
            deleteQuery.prepare("DELETE FROM RESERVATIONS WHERE ID_RES = :id");
            deleteQuery.bindValue(":id", reservationId);

            if (!deleteQuery.exec()) {
                qDebug() << "Erreur lors de la suppression de la réservation : " << deleteQuery.lastError().text();
            } else {
                qDebug() << "Réservation supprimée pour le client : " << clientName;
            }
        }

        // Si aucun départ n'est trouvé, afficher un message dans la console
        if (!found) {
            qDebug() << "Aucun départ trouvé pour aujourd'hui.";
        }
    } else {
        // Afficher une erreur si la requête échoue
        qDebug() << "Erreur lors de la récupération des données : " << query1.lastError().text();
    }
}


