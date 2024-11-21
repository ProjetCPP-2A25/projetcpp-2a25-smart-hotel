#ifndef CONNECTION_H
#define CONNECTION_H
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

class Connection
{
    QSqlDatabase db;
public:
    Connection();
    bool createconnect();
    void closeConnection();
};

#endif // CONNECTION_H
