#ifndef CONNECTION_H
#define CONNECTION_H
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

class Connection
{
public:
    QSqlDatabase db;
    Connection();
    bool createconnection();
    void closeconnection();
};

#endif // CONNECTION_H
