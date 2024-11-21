#include "reservations.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <utility>
#include<QRegularExpression>

// Default constructor
Reservations::Reservations()
    : Id_Res(0),              // Initialize Id_Res to a default value
      Nom_Client(""),        // Initialize Nom_Client to an empty string
      Date_Arr(00,00,0000),            // Calls the default constructor of datee
      Date_Dep(00,00,000),            // Calls the default constructor of datee
      Type_Res(""),          // Initialize Type_Res to an empty string
      Mode_Paiement(""),     // Initialize Mode_Paiement to an empty string
      Montant_Total(0),    // Initialize Montant_Total to "0" or any default value
      Nbr_perso(0),             // Initialize Id_emp to a default value
      Id_emp(0),
      Id_Client(0)
{
    // Additional initialization if needed
}

// Parameterized constructor
Reservations::Reservations(int id, QString name, datee dateArr, datee dateDep, QString typeRes, QString modePai, int montant,int nbr_pers , int idEmp, int idCl)
    : Id_Res(id),              // Initialize Id_Res with id
      Nom_Client(name),        // Initialize Nom_Client with name
      Date_Arr(dateArr),      // Initialize Date_Arr with dateArr
      Date_Dep(dateDep),      // Initialize Date_Dep with dateDep
      Type_Res(typeRes),      // Initialize Type_Res with typeRes
      Mode_Paiement(modePai), // Initialize Mode_Paiement with modePai
      Montant_Total(montant), // Initialize Montant_Total with montant
      Nbr_perso(nbr_pers),
      Id_emp(idEmp),          // Initialize Id_emp with idEmp
      Id_Client(idCl)         // Initialize Id_Client with idCl
{
    // Additional initialization if needed
}

// Create
bool Reservations::ajouter() {
    QString idStr = QString::number(Id_Res);  // Convert Id_Res (int) to QString

        // Regular expression for ID validation (8 digits only)
        QRegularExpression idPattern("^[0-9]{8}$");
        QRegularExpressionMatch idMatch = idPattern.match(idStr);

        // Check if ID is valid (8 digits)
        if (!idMatch.hasMatch()) {
            qDebug() << "Invalid ID. It must contain 8 digits.";
            return false;
        }

        // Regular expression for Name validation (alphabets only)
        QRegularExpression namePattern("^[A-Za-z]+$");
        QRegularExpressionMatch nameMatch = namePattern.match(Nom_Client);

        // Check if Name is valid (only alphabets)
        if (!nameMatch.hasMatch()) {
            qDebug() << "Invalid Name. It must contain only alphabets.";
            return false;
        }
        QRegularExpression typePattern("^[A-Za-z]+$");
        QRegularExpressionMatch typeMatch = typePattern.match(Type_Res);

        // Check if Name is valid (only alphabets)
        if (!typeMatch.hasMatch()) {
            qDebug() << "Invalid type. It must contain only alphabets.";
            return false;
        }
        QString montantStr = QString::number(Montant_Total);
        QRegularExpression montantPattern("^\\d{2,4}$"); // 2 à 4 chiffres seulement
        QRegularExpressionMatch montantMatch = montantPattern.match(montantStr);

        // Vérifier si le montant est valide
        if (!montantMatch.hasMatch()) {
            qDebug() << "Montant invalide. Il doit contenir entre 2 et 4 chiffres.";
            return false;
        }

    QSqlQuery query;    
    query.prepare("INSERT INTO Reservations (ID_RES, NOM_CLIENT, DATE_ARRIVE, DATE_DEPART, TYPE_RES, MODE_PAIEMENT, MONTANT_TOTAL, NBR_PERSO) "
                  "VALUES (:Id_Res, :Nom_Client, TO_DATE(:Date_Arr, 'YYYY-MM-DD'), TO_DATE(:Date_Dep, 'YYYY-MM-DD'), :Type_Res, :Mode_Paiement, :Montant, :Nbr_perso)");

    // Binding values
    query.bindValue(":Id_Res", Id_Res);
    query.bindValue(":Nom_Client", Nom_Client);
    query.bindValue(":Date_Arr", QString("%1-%2-%3").arg(Date_Arr.getaaaa()).arg(Date_Arr.getmm()).arg(Date_Arr.getjj()));
    query.bindValue(":Date_Dep", QString("%1-%2-%3").arg(Date_Dep.getaaaa()).arg(Date_Dep.getmm()).arg(Date_Dep.getjj()));
    query.bindValue(":Type_Res", Type_Res);
    query.bindValue(":Mode_Paiement", Mode_Paiement);
    query.bindValue(":Montant", Montant_Total); // Change this to :Montant
    query.bindValue(":Nbr_perso", Nbr_perso);

    return query.exec(); // Execute the query
}


// Read
QSqlQueryModel* Reservations::afficher() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM Reservations");
    return model;
}


bool Reservations::modifier(int id, const QString& Nom_Client, const datee& Date_Arr, const datee& Date_Dep, const QString& Type_Res, const QString& Mode_Paiement, int Montant_Total,int Nbr_perso) {
    QSqlQuery query;
    query.prepare("UPDATE Reservations SET NOM_CLIENT = :Nom_Client, DATE_ARRIVE = :Date_Arr, "
                  "DATE_DEPART = :Date_Dep, TYPE_RES = :Type_Res, MODE_PAIEMENT = :Mode_Paiement, "
                  "MONTANT_TOTAL = :Montant_Total, NBR_PESO = :Nbr_perso WHERE ID_RES = :Id_Res");

    query.bindValue(":Id_Res", id);
    query.bindValue(":Nom_Client", Nom_Client);

    // Use QDate methods to get year, month, and day
    query.bindValue(":Date_Arr",QDate(Date_Arr.getaaaa(),Date_Arr.getmm(),Date_Arr.getjj()));
    query.bindValue(":Date_Dep",QDate(Date_Dep.getaaaa(),Date_Dep.getmm(),Date_Dep.getjj()));

    query.bindValue(":Type_Res", Type_Res);
    query.bindValue(":Mode_Paiement", Mode_Paiement);
    query.bindValue(":Montant_Total", Montant_Total);
    query.bindValue(":Nbr_perso", Nbr_perso);
    // Execute the query and check for success
    if (!query.exec()) {
        qDebug() << "Error updating reservation:" << query.lastError().text();
        return false; // Return false if the update fails
    }

    return true; // Return true if the update is successful
}





bool Reservations::supprimer(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM Reservations WHERE ID_RES = :id"); // Correct the table name and ID column if needed
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error deleting reservation:" << query.lastError().text();
        return false;
    }
    return true;
}


QSqlQueryModel* Reservations::rechercher(int id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM Reservations WHERE ID_RES = :id"); // Ensure placeholder matches bindValue key
    query.bindValue(":id", id);

    // Execute the query
    if (!query.exec()) {
        // Handle the error if the query execution fails
        qDebug() << "Query execution failed:" << query.lastError().text();
        return nullptr;
    }

    // Create a new model to hold the results
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(query);

    return model;
}
QSqlQueryModel* Reservations::sortByColumn(const QString& column) {
    QSqlQueryModel* model = new QSqlQueryModel();

    QString queryStr = QString("SELECT * FROM Reservations ORDER BY %1").arg(column);
    model->setQuery(queryStr);

    if (model->lastError().isValid()) {
        qDebug() << "Error sorting reservations:" << model->lastError().text();
        delete model;
        return nullptr;
    }

    return model;
}






