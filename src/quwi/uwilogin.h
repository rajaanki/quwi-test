#ifndef UWILOGIN_H
#define UWILOGIN_H

#include <QDialog>
#include <QUrl>
#include <QNetworkAccessManager>

#include <QNetworkReply>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "commonservice.h"
#include "appdata.h"

namespace Ui {
class Login;
}

class UwiLogin : public QDialog
{
    Q_OBJECT

public:
    explicit UwiLogin(QDialog *parent = nullptr);
    ~UwiLogin();

    QString bearerToken(void) const;
    void setBearerToken(QString bearerToken);

public slots:
    void loginResponse(QNetworkReply* reply);
    bool isLoggedIn(void) const;
    void setIsLoggedIn(bool value);

signals:
    void login(bool status);

private:
    Ui::Login *ui;
    bool loggedIn;
    AppData settings;
    QString token;
};

#endif // UWILOGIN_H
