#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDate>

class Event {
public:
    Event();
    Event(int id, QString name, QDate date, int capacity, QString type, int budget, float price, QString rentabilite,QString danger,QString salle);

    bool addEvent();
    QSqlQueryModel* displayEvents();
    QSqlQueryModel* triEvents(int column);
    bool modifyEvent(int id);
    bool deleteEvent(int id);
    QSqlQueryModel* Recherche(int id);
    int getId() const;
    void setId(int id);
    QString getName() const;
    void setName(const QString &name);
    bool checkRentabilite(int prix,int nbrPlace,int budget);
    static QVector<int> getPercent();
    QDate getDate() const;
    void setDate(const QDate &date);
    bool verifGaz(int gaz,int id);
    int getCapacity() const;
    void setCapacity(int capacity);

    QString getType() const;
    void setType(const QString &type);
    void exportToPdf();
    int getBugdet() const;
    void setBudget(const int &budget);
    bool verifSalle(QDate date,QString salle);
    float getPrice() const;
    void setPrice(float price);
    QString getRentabilite() const;
    void setRentabilite(const QString &rentabilite);
    QString getSalle() const;
    void setSalle(const QString &salle);
    void setDanger(const QString &danger);
    QString getDanger() const;


private:
    int id_event;
    QString name;
    QDate date_event;
    int capacity;
    QString type;
    int budget;
    int price;
    QString rentabilite;
    QString danger;
    QString salle;
};

#endif // EVENT_H
