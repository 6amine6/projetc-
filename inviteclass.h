// inviteclass.h
#ifndef INVITECLASS_H
#define INVITECLASS_H

#include <QMainWindow>
#include <QTableWidgetItem>

namespace Ui {
class inviteclass;
}

class inviteclass : public QMainWindow
{
    Q_OBJECT
public slots:
    void on_pushButton_ExportPDF_clicked();
public:
    explicit inviteclass(QWidget *parent = nullptr);
    ~inviteclass();
    void extractAsPdf();
       void extractAsWord(const QString &filename);
       void extractAsCsv(const QString &filename);
       void exportTableToPDF(const QString &fileName);

private slots:
    void on_pushButton_clicked();
    void on_editButton_clicked(int row);
    void populateTableFromDatabase();
    void sortTable(const QString &columnName);
    void clearTable(); // Function declaration for clearing the table
    void on_comboBox_2_currentIndexChanged(int index);

private:
    static bool tablePopulated; // Static variable to track if the table has been populated
    Ui::inviteclass *ui;
    void addToTable(const QString &nom, const QString &id, const QString &prenom,
                    const QString &sexe, const QString &age, const QString &NTA,const QString &montant,const QString &phone,const QDate &DDP);
    void addEditButton(int row);
    void addDeleteButton(int row);
    void exportTable(const QString &fileName, const QString &fileFormat); // Export table function
    void calculateAgeStatistics();
    QString getAgeRange(int age);
void calculateSexStatistics();
void calculateMntTotalStatistics();
void calculateNTAStatistics();
void addemailButton(int row);


};

#endif // INVITECLASS_H
