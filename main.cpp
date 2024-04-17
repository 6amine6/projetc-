//main.cpp

#include "inviteclass.h"
#include <QApplication>
#include <QCoreApplication>
#include <QtSql>
#include <QApplication>

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
    // Set up the database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC"); // Use "QOCI" for Oracle
    db.setHostName("localhost"); // Replace with your hostname
    db.setPort(1521); // Default Oracle port
    db.setDatabaseName("Source_Projet2A"); // Replace with your database name
    db.setUserName("AmineAdmin"); // Replace with your username
    db.setPassword("oracle666"); // Replace with your password

    // Try to open the database
    if (db.open()) {
        qDebug() << "Connected to database!";
    } else {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
    }

    inviteclass w;
    w.show();
    return a.exec();
}


