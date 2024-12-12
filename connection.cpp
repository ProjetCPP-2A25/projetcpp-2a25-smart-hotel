#include "connection.h"
#include <QSqlError>
#include <QDebug>

Connection::Connection() {}

bool Connection::createconnect() {
    bool test = false;

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    //db.setDatabaseName("Driver={Oracle in instantclient12_2};Dbq=localhost:1521/XE;Uid=oussama;Pwd=oussama;");
    db.setDatabaseName("Driver={Oracle in instantclient12_2};Dbq=localhost:1521/XE;Uid=amine;Pwd=esprit;");


    qDebug() << "Tentative de connexion à la base de données...";

    if (db.open()) {
        test = true;
        qDebug() << "Connexion à la base de données réussie.";
    } else {
        qDebug() << "Erreur de connexion à la base de données:";
        qDebug() << db.lastError().text();
        qDebug() << "Message d'erreur complet : " << db.lastError().driverText();
    }

    return test;
}
