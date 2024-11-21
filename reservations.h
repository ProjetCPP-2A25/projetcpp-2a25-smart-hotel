#ifndef RESERVATIONS_H
#define RESERVATIONS_H
#include"connection.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QString>
#include <QDate>
class datee {
private:
    int jj; // Day
    int mm; // Month
    int aaaa; // Year
public:
    // Constructor with parameters
    datee(int jj, int mm, int aaaa) : jj(jj), mm(mm), aaaa(aaaa) {}
    // Getter methods
    QString toString(const QString& format) const {
        // Format the date based on the desired output
        if (format == "yyyy-MM-dd") {
            return QString("%1-%2-%3").arg(aaaa).arg(mm, 2, 10, QChar('0')).arg(jj, 2, 10, QChar('0'));
        }
        // Add other formats if necessary
        return QString();
    }
    int getjj() const { return jj; }
    int getmm() const { return mm; }
    int getaaaa() const { return aaaa; }

    // Setter methods
    void setjj(int jj) { this->jj = jj; }
    void setmm(int mm) { this->mm = mm; }
    void setaaaa(int a) { this->aaaa = a; }
};


class Reservations {
    int Id_Res;
    QString Nom_Client;
    datee Date_Arr;
    datee Date_Dep;
    QString Type_Res;
    QString Mode_Paiement;
    int Montant_Total;
    int Nbr_perso;
    int Id_emp;
    int Id_Client;

public:
    Reservations();
    Reservations(int, QString, datee, datee, QString, QString, int, int, int, int);
    int getID_Res() const { return Id_Res; }
    QString getNom_Client() const { return Nom_Client; }
    QString getType_Res() const { return Type_Res; }
    QString getMode_Paiement() const { return Mode_Paiement; }
    int getMontant_Total() const { return Montant_Total; }
    datee getDate_Arr() const { return Date_Arr; }
    int getNbr_perso() const {return Nbr_perso;}

    datee getDate_Dep() const { return Date_Dep; }
    void setNom_client(const QString &Nom_Cl) { Nom_Client = Nom_Cl; }
    void setType_Res(const QString &TypeRes) { Type_Res = TypeRes; }
    void setMode_Paiement(const QString &ModePai) { Mode_Paiement = ModePai; }
    void setMontant_Total(int Mont) { this->Montant_Total = Mont; }
    void setID_Res(int Id) { this->Id_Res = Id; }
    void setID_Client(int Id_Cl) { this->Id_Client = Id_Cl; }
    void setNbr_perso(int Nbr_per){this->Nbr_perso= Nbr_per;}
    void setID_emp(int Id_em) { this->Id_emp = Id_em; }
    void setDate_Arr(const datee &DateArr) { Date_Arr = DateArr; }
    void setDate_Dep(const datee &DateDep) { Date_Dep = DateDep; }
    bool ajouter();
    QSqlQueryModel * afficher();
    // In reservations.h
    bool modifier(int id, const QString& Nom_Client, const datee& Date_Arr, const datee& Date_Dep, const QString& Type_Res, const QString& Mode_Paiement, int Montant_Total,int Nbr_perso);
    bool supprimer(int);
    QSqlQueryModel* rechercher(int);
    QSqlQueryModel* sortByColumn(const QString& column);

};

#endif // RESERVATIONS_H
