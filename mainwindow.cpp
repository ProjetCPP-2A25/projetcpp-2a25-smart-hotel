#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include "event.h"
#include "connection.h"
#include <QTableView>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QPainter>
#include <event.h>
#include <QSerialPortInfo>
#include <QSerialPort>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
        ui->setupUi(this);

        arduino = new QSerialPort(this);
                connectToArduino();  // Connexion à l'Arduino
                connect(ui->gazDetect, &QPushButton::clicked, this, &MainWindow::on_gazDetect_clicked);  // Connexion du signal du bouton

                // Logique pour la détection des ports série
                qDebug() << "Nombre de ports:" << QSerialPortInfo::availablePorts().length();
                foreach (const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts()) {
                    qDebug() << "Nom du port:" << portInfo.portName();
                    qDebug() << "Description:" << portInfo.description();
                    qDebug() << "Fabricant:" << portInfo.manufacturer();
                    qDebug() << "Emplacement système:" << portInfo.systemLocation();
                }

        connect(ui->eventTableWidget, &QTableWidget::cellClicked, this, &MainWindow::on_eventTableWidget_cellClicked);
        Connection connection;
        if (!connection.createconnect()) {
            QMessageBox::critical(this, "Erreur de connexion", "La connexion à la base de données a échoué.");
            return;
        }






    on_refreshButton_clicked();
}

MainWindow::~MainWindow() {
    if (arduino->isOpen()) {
          arduino->close();
      }
    connect(ui->gazDetect, &QPushButton::pressed, this, &MainWindow::on_gazDetect_pressed);

    delete ui;
}
void MainWindow::updateStatisticsChart() {
    // Vérifier si chartContainer possède un layout, sinon en créer un
    QLayout *existingLayout = ui->chartContainer->layout();
    if (existingLayout) {
        // Supprimer tous les widgets existants dans le layout
        QLayoutItem *item;
        while ((item = existingLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();  // Libérer les widgets
            }
            delete item;  // Libérer l'élément de mise en page
        }
    } else {
        // Si aucun layout n'existe, en créer un
        ui->chartContainer->setLayout(new QVBoxLayout());
    }

    // Création de la connexion à la base de données
    Connection connection;
    if (!connection.createconnect()) {
        QMessageBox::critical(this, "Erreur de connexion", "La connexion à la base de données a échoué.");
        return;
    }

    // Récupération des statistiques via l'objet Event
    Event event;
    QVector<int> percents = Event::getPercent();

    if (percents.isEmpty() || percents[0] == 0) {
        QMessageBox::information(this, "Statistiques", "Aucune donnée disponible pour les statistiques.");
        return;
    }

    // Création de la série de données pour le graphique
    QPieSeries *series = new QPieSeries();
    series->append("PRIVE", percents[1] * 100 / percents[0]);
    series->append("PROFESSIONNEL", percents[2] * 100 / percents[0]);
    series->append("SAISIONNIER", percents[3] * 100 / percents[0]);
    series->append("CULTUREL", percents[4] * 100 / percents[0]);
    series->append("SPORTIF", percents[5] * 100 / percents[0]);

    // Personnaliser les couleurs et les labels des tranches
    series->slices().at(0)->setBrush(QColor(255, 99, 132));  // PRIVE (rose)
    series->slices().at(1)->setBrush(QColor(54, 162, 235));  // PRO (bleu)
    series->slices().at(2)->setBrush(QColor(75, 192, 192));  // SAISONNIER (vert)
    series->slices().at(3)->setBrush(QColor(153, 102, 255)); // CULTUREL (violet)
    series->slices().at(4)->setBrush(QColor(255, 159, 64));  // SPORTIF (orange)

    for (int i = 0; i < series->slices().count(); ++i) {
        QPieSlice *slice = series->slices().at(i);
        QString name = series->slices().at(i)->label(); // Nom de la tranche
        double percentage = slice->percentage() * 100;  // Calcul du pourcentage
        slice->setLabel(QString("%1: %2%").arg(name).arg(percentage, 0, 'f', 1));  // Nom + Pourcentage
        slice->setLabelVisible(true);
    }

    // Création et configuration du graphique
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des événements");
    chart->legend()->hide();

    // Création de la vue du graphique
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Ajouter le graphique au conteneur
    ui->chartContainer->layout()->addWidget(chartView);
}


void MainWindow::on_addButton_clicked() {
    int id = ui->idLineEdit->text().toInt();
    QString name = ui->nameLineEdit->text();
    QDate date = ui->dateLineEdit->date();
    int capacity = ui->capacityLineEdit->text().toInt();
    QString type = ui->typeLineEdit->currentText();
    int budget = ui->durationLineEdit->text().toInt();
    float price = ui->priceLineEdit->text().toFloat();
    QString salle = ui->salleLineEdit->currentText();
    if (id <= 0) {
        QMessageBox::warning(this, "Donnez un entier" ,"positif pour l'id");
        return;
    }
    if (name.isEmpty() || !date.isValid() || type.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "All fields must be filled.");
        return;
    }
    if (date <= QDate::currentDate()) {
        QMessageBox::warning(this, "Invalid Date", "The date must be in the future.");
        return;
    }
    Event event;
    event.setId(id);
    event.setName(name);
    event.setDate(date);
    event.setCapacity(capacity);
    event.setType(type);
    event.setBudget(budget);
    event.setPrice(price);
    event.setSalle(salle);
    if (event.checkRentabilite(price, capacity, budget)) {
        event.setRentabilite("gagnant");
    }
    else
        event.setRentabilite("perdant");


    if(event.verifSalle(date,salle))
    {
        if (event.addEvent()) {
            QMessageBox::information(this, "Success", "Event added successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Failed.");
        }
    }
    else
        QMessageBox::critical(this, "Error", "Cette salle est deja reservee pour cette date, Veuillez choisir une autre s'il vous plait.");
    on_refreshButton_clicked();


}

void MainWindow::on_refreshButton_clicked() {

    ui->eventTableWidget->clearContents();
    ui->eventTableWidget->setRowCount(0);
    Event event;
    QSqlQueryModel* model = event.displayEvents();

    if (!model) {
        qDebug() << "Erreur : le modèle retourné par displayEvents() est null.";
        return;
    }
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();
    ui->eventTableWidget->setRowCount(rowCount);
    ui->eventTableWidget->setColumnCount(columnCount);
    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            QVariant cellData = model->data(model->index(row, col)); // Obtenir la donnée directement

            QString displayData;
            if (col == 2) {
                if (cellData.canConvert<QDate>()) {
                    QDate date = cellData.toDate();
                    displayData = date.toString("dd/MM/yyyy");
                } else if (cellData.canConvert<QDateTime>()) {
                    QDateTime dateTime = cellData.toDateTime();
                    displayData = dateTime.date().toString("dd/MM/yyyy");
                }
            } else {
                displayData = cellData.toString();
            }

            ui->eventTableWidget->setItem(row, col, new QTableWidgetItem(displayData));
        }
    }


    ui->eventTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    

    qDebug() << "Table des événements rafraîchie avec succès.";
    updateStatisticsChart();

}


void MainWindow::on_modifyButton_clicked() {
    int id = ui->idLineEdit->text().toInt();
    QString name = ui->nameLineEdit->text();
    QDate date = ui->dateLineEdit->date();
    int capacity = ui->capacityLineEdit->text().toInt();
    QString type = ui->typeLineEdit->currentText();
    int budget = ui->durationLineEdit->text().toInt();
    float price = ui->priceLineEdit->text().toFloat();

    if (id <= 0) {
        QMessageBox::warning(this, "Donnez un entier", "positif pour l'id");
        return;
    }
    if (name.isEmpty() || !date.isValid() || type.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "All fields must be filled.");
        return;
    }
    if (date <= QDate::currentDate()) {
        QMessageBox::warning(this, "Invalid Date", "The date must be in the future.");
        return;
    }
    Event event;
    event.setId(id);
    event.setName(name);
    event.setDate(date);
    event.setCapacity(capacity);
    event.setType(type);
    event.setBudget(budget);
    event.setPrice(price);

    if (event.modifyEvent(id)) {
        QMessageBox::information(this, "Success", "Event modified successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Failed to modify the event. Check logs for details.");
    }

    on_refreshButton_clicked();
}


void MainWindow::on_deleteButton_clicked() {
    QTableWidgetItem* currentItem = ui->eventTableWidget->currentItem();

    if (!currentItem) {
        QMessageBox::warning(this, "No Selection", "Please select a cell to delete the event.");
        return;
    }

    int row = currentItem->row();
    int id = ui->eventTableWidget->item(row, 0)->text().toInt();  // ID dans la première colonne

    if (id <= 0) {
        QMessageBox::warning(this, "Invalid ID", "Please select a valid event to delete.");
        return;
    }
    Event event;
    bool success = event.deleteEvent(id);  // Supposons que cette méthode gère la requête SQL et renvoie true si réussi

    if (success) {
        QMessageBox::information(this, "Success", "Event deleted successfully.");
        qDebug() << "Événement supprimé avec succès.";

        // Supprimer la ligne du QTableWidget
        ui->eventTableWidget->removeRow(row);
    } else {
        QMessageBox::critical(this, "Error", "Failed to delete the event. Check logs for details.");

    }

    // Rafraîchir la table après suppression
    on_refreshButton_clicked();
}



void MainWindow::on_Recherche_clicked()
{
    ui->eventTableWidget->clearContents();
    ui->eventTableWidget->setRowCount(0);


    int id = ui->idRecherche->text().toInt();

    if (id <= 0) {
        QMessageBox::warning(this, "Invalid ID", "Please select a valid event to delete.");
        return;
    }

    // Créer un objet Event et appeler la méthode deleteEvent()
    Event event;
    QSqlQueryModel* model = event.Recherche(id);
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();
    ui->eventTableWidget->setRowCount(rowCount);
    ui->eventTableWidget->setColumnCount(columnCount);

    // Remplir le QTableWidget avec les données du modèle
    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            QString cellData = model->data(model->index(row, col)).toString();
            ui->eventTableWidget->setItem(row, col, new QTableWidgetItem(cellData));
        }
    }

    // Ajuster la taille des colonnes pour remplir l'espace
    ui->eventTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    qDebug() << "Table des événements rafraîchie avec succès.";



}

void MainWindow::on_eventTableWidget_cellClicked(int row, int column)
{


}

void MainWindow::on_eventTableWidget_cellDoubleClicked(int row, int column)
{
    qDebug() << row,column;
    if(row == 2)
    {
        ui->eventTableWidget->clearContents();
        ui->eventTableWidget->setRowCount(0);
        Event event;
        QSqlQueryModel* model = event.triEvents(column);

        if (!model) {
            qDebug() << "Erreur : le modèle retourné par displayEvents() est null.";
            return;
        }
        int rowCount = model->rowCount();
        int columnCount = model->columnCount();
        ui->eventTableWidget->setRowCount(rowCount);
        ui->eventTableWidget->setColumnCount(columnCount);
        for (int row = 0; row < rowCount; ++row) {
            for (int col = 0; col < columnCount; ++col) {
                QVariant cellData = model->data(model->index(row, col));

                QString displayData;
                if (col == 2) {
                    if (cellData.canConvert<QDate>()) {
                        QDate date = cellData.toDate();
                        displayData = date.toString("dd/MM/yyyy");
                    } else if (cellData.canConvert<QDateTime>()) {
                        QDateTime dateTime = cellData.toDateTime();
                        displayData = dateTime.date().toString("dd/MM/yyyy");
                    }
                } else {
                    displayData = cellData.toString();
                }

                ui->eventTableWidget->setItem(row, col, new QTableWidgetItem(displayData));
            }
        }
        ui->eventTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        qDebug() << "Table des événements rafraîchie avec succès.";

    }
}


QString serialBuffer;  // Tampon pour assembler les données série

void MainWindow::readArduinoData() {
    QByteArray data = arduino->readAll();  // Lire les données reçues de l'Arduino
    serialBuffer += QString(data);  // Ajouter les données au tampon

    // Vérification et traitement des données reçues
    while (serialBuffer.contains("\n")) {
        int index = serialBuffer.indexOf("\n");  // Trouver la fin d'une ligne
        QString completeLine = serialBuffer.left(index).trimmed();  // Extraire la ligne complète
        serialBuffer.remove(0, index + 1);  // Retirer la ligne traitée du tampon

        // Afficher la valeur reçue
        qDebug() << "Données complètes Arduino reçues :" << completeLine;
        // Si les données reçues sont un nombre (ex: valeur du gaz)
        bool ok;
        int gazValue = completeLine.toInt(&ok);  // Convertir la ligne en entier
        if (ok) {
             gazValueString = QString::number(gazValue);  // Stocker la valeur du gaz sous forme de chaîne
        }
    }
}
void MainWindow::connectToArduino() {
    // Détection des ports série disponibles
    foreach (const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts()) {
        if (portInfo.description().contains("Arduino")) {
            arduinoPortName = portInfo.portName();
            break;
        }
    }

    if (arduinoPortName.isEmpty()) {
        QMessageBox::warning(this, "Arduino non trouvé", "Aucun appareil Arduino détecté.");
        return;
    }

    arduino->setPortName(arduinoPortName);
    arduino->setBaudRate(QSerialPort::Baud9600);
    arduino->setDataBits(QSerialPort::Data8);
    arduino->setParity(QSerialPort::NoParity);
    arduino->setStopBits(QSerialPort::OneStop);
    arduino->setFlowControl(QSerialPort::NoFlowControl);

    if (arduino->open(QIODevice::ReadOnly)) {
        connect(arduino, &QSerialPort::readyRead, this, &MainWindow::readArduinoData);  // Connexion pour lire les données
        qDebug() << "Arduino connecté sur le port :" << arduinoPortName;
    } else {
        QMessageBox::critical(this, "Erreur de connexion", "Impossible d'ouvrir le port série.");
    }
}

    void MainWindow::on_gazDetect_pressed() {


    }

    void MainWindow::on_gazDetect_clicked() {
        if (! gazValueString.isEmpty()) {
            Event event;
            int id = ui->idRecherche->text().toInt();
            if(bool success = event.verifGaz(gazValueString.toInt(),id))
                QMessageBox::warning(this, "ça marche", "bien.");


            else
                QMessageBox::warning(this, "Erreur", "Aucune donnée de gaz reçue.");

        } else {
            QMessageBox::warning(this, "Erreur", "Aucune donnée de gaz reçue.");
        }
        on_refreshButton_clicked();

    }
    void MainWindow::exportToPdf()
    {
        // Ouvre une boîte de dialogue pour choisir l'emplacement et le nom du fichier PDF
        QString filePath = QFileDialog::getSaveFileName(
            this, "Save Event Data as PDF", "", "PDF Files (*.pdf)");

        // Si un chemin de fichier valide est sélectionné
        if (!filePath.isEmpty()) {
            // Ajouter l'extension .pdf si elle n'est pas présente
            if (!filePath.endsWith(".pdf")) {
                filePath += ".pdf";
            }

            Event event;
            QSqlQueryModel* model = event.displayEvents();

            if (model == nullptr) {
                QMessageBox::critical(this, "Erreur", "Erreur lors de la récupération des données.");
                return;
            }

            QVector<Event> events;
            for (int row = 0; row < model->rowCount(); ++row) {
                Event event;
                event.setId(model->data(model->index(row, 0)).toInt());
                event.setName(model->data(model->index(row, 1)).toString());
                event.setDate(model->data(model->index(row, 2)).toDate());
                event.setType(model->data(model->index(row, 4)).toString());
                event.setPrice(model->data(model->index(row, 6)).toInt());
                event.setSalle(model->data(model->index(row, 9)).toString());
                event.setBudget(model->data(model->index(row, 5)).toInt());
                event.setCapacity(model->data(model->index(row, 3)).toInt());
                event.setRentabilite(model->data(model->index(row, 7)).toString());
                event.setDanger(model->data(model->index(row, 8)).toString());

                events.append(event);
            }

            QFont font("Arial", 8); // Police légèrement agrandie
            QPdfWriter pdfWriter(filePath);
            pdfWriter.setPageSize(QPageSize(QPageSize::A3));
            pdfWriter.setTitle("Export Events");
            pdfWriter.setCreator("Event Management System");

            QPainter painter;
            if (!painter.begin(&pdfWriter)) {
                QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier PDF.");
                return;
            }

            painter.setFont(font);
            QPen pen;
            pen.setColor(Qt::black);
            pen.setWidth(3);                   // Épaisseur des traits (ajustable)
             painter.setPen(pen);

            int margin = 100;        // Marge autour du tableau
            int y = margin;          // Position verticale initiale
            int rowHeight = 1000;    // Hauteur de chaque ligne
            int headerHeight = 1000; // Hauteur des en-têtes
            int columnWidth = 1200;  // Largeur des colonnes
            int textMargin = 200;    // Marge intérieure pour le texte



            // Dessiner les en-têtes du tableau
            QStringList headers = {
                "ID", "Nom", "Date", "Capacite", "Type", "Budget",
                "Prix", "Rentabilite", "Danger", "Salle"
            };

            for (int i = 0; i < headers.size(); ++i) {
                painter.drawRect(margin + i * columnWidth, y, columnWidth, headerHeight);
                painter.drawText(
                    margin + i * columnWidth + textMargin,
                    y + headerHeight / 2 + painter.fontMetrics().ascent() / 2,
                    headers[i]
                );
            }

            y += headerHeight;

            // Dessiner les données
            for (const Event& event : events) {
                QStringList data = {
                    QString::number(event.getId()),
                    event.getName(),
                    event.getDate().toString("yyyy-MM-dd"),
                    QString::number(event.getCapacity()),
                    event.getType(),
                    QString::number(event.getBugdet()),
                    QString::number(event.getPrice()),
                    event.getRentabilite(),
                    event.getDanger(),
                    event.getSalle()
                };

                for (int i = 0; i < data.size(); ++i) {
                    painter.drawRect(margin + i * columnWidth, y, columnWidth, rowHeight);
                    painter.drawText(
                        margin + i * columnWidth + textMargin,
                        y + rowHeight / 2 + painter.fontMetrics().ascent() / 2,
                        data[i]
                    );
                }

                y += rowHeight;

                // Gérer le passage à une nouvelle page
                if (y > pdfWriter.height() - margin - rowHeight) {
                    pdfWriter.newPage();
                    y = margin;

                    // Redessiner les en-têtes
                    for (int i = 0; i < headers.size(); ++i) {
                        painter.drawRect(margin + i * columnWidth, y, columnWidth, headerHeight);
                        painter.drawText(
                            margin + i * columnWidth + textMargin,
                            y + headerHeight / 2 + painter.fontMetrics().ascent() / 2,
                            headers[i]
                        );
                    }

                    y += headerHeight;
                }
            }

            painter.end();

            // Demander si l'utilisateur veut afficher le PDF
            int response = QMessageBox::question(this, "PDF", "Afficher le PDF ?", QMessageBox::Yes | QMessageBox::No);
            if (response == QMessageBox::Yes) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
            }
        }
    }


void MainWindow::on_exporter_clicked()
{
    exportToPdf();
}
