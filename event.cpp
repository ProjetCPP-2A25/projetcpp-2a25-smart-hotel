#include "event.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QPdfWriter>
#include <QFileDialog>


Event::Event() {}

Event::Event(int id, QString name, QDate date, int capacity, QString type, int budget, float price,QString rentabilite,QString danger,QString salle)
: id_event(id), name(name), date_event(date), capacity(capacity), type(type), budget(budget), price(price), rentabilite(rentabilite),danger(danger),salle(salle) {}

bool Event::addEvent() {
QSqlQuery query;
query.prepare("INSERT INTO EVENTS (ID_EVENT, NAME, DATE_EVENT, CAPACITY, TYPE, BUDGET, PRICE, RENTABILITE,DANGER,SALLE) "
          "VALUES (:id, :name, :date, :capacity, :type, :budget, :price, :rentabilite, :danger, :salle)");
query.bindValue(":id", id_event);
query.bindValue(":name", name);
query.bindValue(":date", date_event);
query.bindValue(":capacity", capacity);
query.bindValue(":type", type);
query.bindValue(":budget", budget);
query.bindValue(":price", price);
query.bindValue(":rentabilite", rentabilite);
query.bindValue(":danger","pas encore testee");
query.bindValue(":salle", salle);


if (query.exec()) {
qDebug() << "Événement ajouté avec succès.";
return true;
} else {
qDebug() << "Erreur lors de l'ajout de l'événement:" << query.lastError().text();
return false;
}
}
bool Event::verifGaz(int gaz, int id)
{
    // Vérification de la connexion à la base de données
    if (!QSqlDatabase::database().isOpen()) {
        qDebug() << "La base de données n'est pas ouverte.";
        return false;
    }

    // Vérification si l'ID existe dans la table EVENTS
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM EVENTS WHERE ID_EVENT = :id");
    checkQuery.bindValue(":id", id);
    if (!checkQuery.exec()) {
        qDebug() << "Erreur de vérification de l'ID:" << checkQuery.lastError().text();
        return false;
    }



    // Préparation de la requête de mise à jour
    QSqlQuery query;
    if (gaz > 500) {
        query.prepare("UPDATE EVENTS SET DANGER = :danger WHERE ID_EVENT = :id");
        query.bindValue(":id", id);
        query.bindValue(":danger", "Danger critique");
    } else {
        query.prepare("UPDATE EVENTS SET DANGER = :danger WHERE ID_EVENT = :id");
        query.bindValue(":id", id);
        query.bindValue(":danger", " pas danger ");
    }

    qDebug() << "Requête SQL : " << query.lastQuery();  // Affichage de la requête pour vérification

    if (query.exec()) {
        qDebug() << "Événement mis à jour avec succès.";
        return true;
    } else {
        qDebug() << "Erreur lors de la mise à jour de l'événement:" << query.lastError().text();
        return false;
    }
}


bool Event::verifSalle(QDate date,QString salle)
{
    QSqlQueryModel* model = new QSqlQueryModel();

       QSqlQuery query;
       query.prepare("SELECT * FROM EVENTS WHERE DATE_EVENT = :date");
       query.bindValue(":date", date);
       if (query.exec()) {
           model->setQuery(query);
       } else {
           qDebug() << "Erreur lors de la recherche de l'événement:" << query.lastError().text();
           return false;
       }
       for (int row = 0; row < model->rowCount(); ++row) {
           QVariant salleEvent = model->data(model->index(row, 9));
           if (salleEvent.toString() == salle) {
               return false;
           }
       }
       return true;
}
bool Event::modifyEvent(int id) {
QSqlQuery query;
query.prepare("UPDATE EVENTS SET NAME = :name, DATE_EVENT = :date, CAPACITY = :capacity, "
             "TYPE = :type, BUDGET = :budget, PRICE = :price WHERE ID_EVENT = :id");
query.bindValue(":id", id);
query.bindValue(":name", name);
query.bindValue(":date", date_event);
query.bindValue(":capacity", capacity);
query.bindValue(":type", type);  // Type de l'événement
query.bindValue(":budget", budget);  // Durée de l'événement
query.bindValue(":price", price);  // Prix de l'événement

// Exécution de la requête
if (query.exec()) {
   qDebug() << "Événement mis à jour avec succès.";
   return true;  // Retourne true si la mise à jour a réussi
} else {
   qDebug() << "Erreur lors de la mise à jour de l'événement:" << query.lastError().text();
   return false;  // Retourne false en cas d'erreur
}
}
QSqlQueryModel* Event::displayEvents() {
QSqlQueryModel* model = new QSqlQueryModel();
model->setQuery("SELECT * FROM EVENTS");
return model;
}
bool Event::deleteEvent(int id) {
QSqlQuery query;
query.prepare("DELETE FROM EVENTS WHERE ID_EVENT = :id");
query.bindValue(":id", id);

if (query.exec()) {
return true;
} else {
qDebug() << "Erreur SQL lors de la suppression de l'événement:" << query.lastError().text();
return false;
}
}
QSqlQueryModel* Event::Recherche(int id) {
QSqlQueryModel* model = new QSqlQueryModel();

QSqlQuery query;
query.prepare("SELECT * FROM EVENTS WHERE ID_EVENT = :id");
query.bindValue(":id", id);  // Lier la valeur de :id à l'argument id

// Exécution de la requête et assignation du modèle
if (query.exec()) {
model->setQuery(query);
} else {
qDebug() << "Erreur lors de la recherche de l'événement:" << query.lastError().text();
}

return model;
}
QSqlQueryModel* Event::triEvents(int column)
{
QSqlQueryModel* model = new QSqlQueryModel();
switch (column) {
   case 0:
       model->setQuery("SELECT * FROM EVENTS ORDER BY ID_EVENT ASC");
       break;
   case 1:
       model->setQuery("SELECT * FROM EVENTS ORDER BY NAME ASC");
       break;
   case 2:
       model->setQuery("SELECT * FROM EVENTS ORDER BY DATE_EVENT ASC");  // Assurez-vous d'utiliser le bon nom de colonne
       break;
   case 3:
       model->setQuery("SELECT * FROM EVENTS ORDER BY CAPACITY ASC");
       break;
   case 4:
       model->setQuery("SELECT * FROM EVENTS ORDER BY TYPE ASC");
       break;
   case 5:
        model->setQuery("SELECT * FROM EVENTS ORDER BY BUDGET ASC");
       break;
   case 6:
        model->setQuery("SELECT * FROM EVENTS ORDER BY PRICE ASC");
        break;
   case 7:
        model->setQuery("SELECT * FROM EVENTS ORDER BY RENTABILITE ASC");

   default:
       qDebug() << "Colonne invalide pour le tri.";
       break;
}
return model;


}
QVector<int> Event::getPercent()
{
    QVector<int> percents;
    QSqlQuery query("SELECT COUNT(*) AS TOTAL, COUNT(CASE WHEN TYPE = 'Prive' THEN 1 END) AS TYPE_1, "
                    "COUNT(CASE WHEN TYPE = 'Professionnel' THEN 1 END) AS TYPE_2, "
                    "COUNT(CASE WHEN TYPE = 'Saisionnier' THEN 1 END) AS TYPE_3, "
                    "COUNT(CASE WHEN TYPE = 'Culturel' THEN 1 END) AS TYPE_4, "
                    "COUNT(CASE WHEN TYPE = 'Sportif' THEN 1 END) AS TYPE_5 FROM EVENTS");
    if (query.next()) {
        // Récupération des valeurs
        int total = query.value("TOTAL").toInt();
        int type1 = query.value("TYPE_1").toInt();
        int type2 = query.value("TYPE_2").toInt();
        int type3 = query.value("TYPE_3").toInt();
        int type4 = query.value("TYPE_4").toInt();
        int type5 = query.value("TYPE_5").toInt();

        qDebug() << "Total: " << total;
        qDebug() << "PRIVE: " << type1;
        qDebug() << "PRO: " << type2;
        qDebug() << "Saisionnier: " << type3;
        qDebug() << "CULTUREL: " << type4;
        qDebug() << "SPORTIF: " << type5;

        percents.append(total);
        percents.append(type1);
        percents.append(type2);
        percents.append(type3);
        percents.append(type4);
        percents.append(type5);
    }
    return percents;
}

bool Event::checkRentabilite(int prix,int nbrPlace,int budget)
{
if(prix*nbrPlace>budget)
return true;
else
return false;
}



int Event::getId() const { return id_event; }
void Event::setId(int id) { id_event = id; }

QString Event::getName() const { return name; }
void Event::setName(const QString &name) { this->name = name; }

QDate Event::getDate() const { return date_event; }
void Event::setDate(const QDate &date) { date_event = date; }

int Event::getCapacity() const { return capacity; }
void Event::setCapacity(int capacity) { this->capacity = capacity; }

QString Event::getType() const { return type; }
void Event::setType(const QString &type) { this->type = type; }

int Event::getBugdet() const { return budget; }
void Event::setBudget(const int &budget) { this->budget = budget; }

float Event::getPrice() const { return price; }
void Event::setPrice(float price) { this->price = price; }
QString Event::getRentabilite() const { return rentabilite; }
void Event::setRentabilite(const QString &rentabilite) {
this->rentabilite = rentabilite;
}
QString Event::getSalle() const { return salle; }

void Event::setSalle(const QString &salle) {
    this->salle = salle;
}
void Event::setDanger(const QString &danger) {
    this->danger = danger;
}
QString Event::getDanger() const { return danger; }


