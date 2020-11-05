#ifndef UWIHOME_H
#define UWIHOME_H

#include <QMainWindow>
#include "uwilogin.h"

#include <QDialog>
#include <QUrl>
#include <QNetworkAccessManager>

#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "commonservice.h"
#include "projects.h"

#include <QList>

#include "ui_uwihome.h"
#include "uwilogin.h"
#include <QTimer>
#include <QDebug>
#include "projects.h"
#include "projectdelegate.h"
#include <QGroupBox>
#include <QCheckBox>
#include "usersitem.h"

namespace Ui {
class UwiHome;
}

class UwiHome : public QMainWindow
{
    Q_OBJECT

public:
    explicit UwiHome(QWidget *parent = nullptr);
    ~UwiHome();

private slots:
    void allProjectsResponse(QNetworkReply* reply);
    void projectItemResponse(QNetworkReply* reply);
    void projectEditNameResponse(QNetworkReply* reply);

private:
    void getAllProjects(void);
    void getProjectItemInfo(quint64 id);

private:
    Ui::UwiHome *ui;
    UwiLogin *uwiLogin;
    ProjectDetails *projectEditItem;
    QList<ProjectDetails> projectsList;
    Projects *projects;
    AppData settings;
    bool isBearerTokenValid;
    QNetworkAccessManager *manager;
};

#endif // UWIHOME_H
