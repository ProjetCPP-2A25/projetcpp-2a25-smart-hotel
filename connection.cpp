#include "connection.h"
#include <QDebug>

Connection::Connection()
{

}

bool Connection::createconnect()
{bool test=false;
QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
db.setDatabaseName("source25");//inserer le nom de la source de données
db.setUserName("aziz");//inserer nom de l'utilisateur
db.setPassword("esprit2024");//inserer mot de passe de cet utilisateur

if (db.open())
test=true;






    return  test;
}
void Connection::closeConnection(){
    db.close();
}
