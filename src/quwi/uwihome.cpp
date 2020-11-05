#include "uwihome.h"

UwiHome::UwiHome(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UwiHome)
{
    ui->setupUi(this);
    ui->buttonProjects->setDisabled(true);
    ui->buttonLogout->setText(tr("LOGIN"));
    uwiLogin = new UwiLogin((QDialog*) this);
    projectEditItem = new ProjectDetails;

    manager = new QNetworkAccessManager(this);

    isBearerTokenValid = false;
    QVariant value = settings.getData(BEARER_TOKEN_KEY);
    if(!value.isNull() && value.isValid()){
        if(!value.toString().isEmpty()) {
            ui->buttonProjects->setDisabled(false);
            uwiLogin->setIsLoggedIn(true);
            ui->buttonLogout->setText("LOGOUT");
            isBearerTokenValid = true;
            uwiLogin->setBearerToken(value.toString());
        }
    }

    if(!isBearerTokenValid) {
        QTimer::singleShot(100, [=] () {
            uwiLogin->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
            uwiLogin->exec();
        });

        ui->buttonLogout->setText(tr("LOGIN"));
        ui->buttonProjects->setDisabled(true);
    } else {
        getAllProjects();
    }

    QObject::connect(ui->buttonLogout, &QPushButton::clicked, [=] () {
        if(!uwiLogin->isLoggedIn()) {
            QTimer::singleShot(100, [=] () {
                uwiLogin->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
                uwiLogin->exec();
            });
        } else {
            //handle logout
            QString serviceLink = QString(SERVICE_BASE_URL) + QString(SERVICE_LOGOUT_SUFFIX);
            QUrl serviceUrl = QUrl(serviceLink);
            QNetworkRequest request(serviceUrl);
            request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(
                                  QString("application/json")));
            QByteArray authorizationValue = QByteArray("Bearer ").append(settings.getData(BEARER_TOKEN_KEY).toString().toLocal8Bit());
            request.setRawHeader(QByteArray("Authorization"), authorizationValue);

            QJsonObject jsonObj;
            jsonObj["anywhere"] = false;

            QJsonDocument jsonDoc(jsonObj);
            QByteArray servicePostData = jsonDoc.toJson();

            manager->post(request, servicePostData);

            uwiLogin->setBearerToken("");
            settings.setData(BEARER_TOKEN_KEY, "");
            uwiLogin->setIsLoggedIn(false);
            ui->stackedWidget->setCurrentIndex(0);
            ui->buttonProjects->setDisabled(true);
            ui->buttonLogout->setText(tr("LOGIN"));

            ui->vLayoutProjectsList->removeWidget(projects);

            projects->clearAll();
            projectsList.clear();
            delete projects;
        }
    });

    QObject::connect(uwiLogin, &UwiLogin::login, [=] (bool loggedIn) {
        if(loggedIn) {
            ui->buttonProjects->setDisabled(false);
            ui->buttonLogout->setText(tr("LOGOUT"));
            getAllProjects();
        } else {
            ui->buttonProjects->setDisabled(true);
            ui->buttonLogout->setText(tr("LOGIN"));
        }
    });

    QObject::connect(ui->buttonProjects, &QPushButton::clicked, [=] () {
        QLayoutItem *child;
        while ((child = ui->gridLayout->takeAt(0)) != 0) {
            if (child->widget())
                child->widget()->setParent(NULL);

            delete child;
        }

        ui->stackedWidget->setCurrentIndex(0);
    });

    QObject::connect(ui->buttonActiveToggle, &QPushButton::clicked, this, [=] () {
        if(projectEditItem->is_active) {
            projectEditItem->is_active = 0;
            QPixmap pixmap2(":/images/toggle-off-48.png");
            QIcon ButtonIcon2(pixmap2);
            ui->buttonActiveToggle->setIcon(ButtonIcon2);
            ui->buttonActiveToggle->setIconSize(pixmap2.rect().size());
        } else {
            projectEditItem->is_active = 1;
            QPixmap pixmap(":/images/toggle-on-48.png");
            QIcon ButtonIcon(pixmap);
            ui->buttonActiveToggle->setIcon(ButtonIcon);
            ui->buttonActiveToggle->setIconSize(pixmap.rect().size());
        }
    });

    QObject::connect(ui->buttonOK, &QPushButton::clicked, this, [=] () {

       projectEditItem->name = ui->lineEditName->text();

        //handlle sending edit request to server
        QString serviceLink = QString(SERVICE_BASE_URL) + QString(SERVICE_PROJECT_PROPERTY_CHANGE) + QString("?id=") + QString::number(projectEditItem->id);
        QUrl serviceUrl = QUrl(serviceLink);
        QNetworkRequest request(serviceUrl);

        QByteArray authorizationValue = QByteArray("Bearer ").append(settings.getData(BEARER_TOKEN_KEY).toString().toLocal8Bit());
        request.setRawHeader(QByteArray("Authorization"), authorizationValue);

        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"name\""));
        textPart.setBody(projectEditItem->name.toLocal8Bit());

        multiPart->append(textPart);

        //QByteArray servicePostData;
        connect(manager, SIGNAL(finished(QNetworkReply*)), this,
                SLOT(projectEditNameResponse(QNetworkReply*)));

        manager->post(request, multiPart);
    });

    QObject::connect(ui->buttonWatcherToggle, &QPushButton::clicked, this, [=] () {
        if(projectEditItem->is_owner_watcher) {
            projectEditItem->is_owner_watcher = 0;
            QPixmap pixmap2(":/images/toggle-off-48.png");
            QIcon ButtonIcon2(pixmap2);
            ui->buttonWatcherToggle->setIcon(ButtonIcon2);
            ui->buttonWatcherToggle->setIconSize(pixmap2.rect().size());
        } else {
            projectEditItem->is_owner_watcher = 1;
            QPixmap pixmap(":/images/toggle-on-48.png");
            QIcon ButtonIcon(pixmap);
            ui->buttonWatcherToggle->setIcon(ButtonIcon);
            ui->buttonWatcherToggle->setIconSize(pixmap.rect().size());
        }
    });
}

UwiHome::~UwiHome()
{
    delete ui;
}

//private slots
void UwiHome::allProjectsResponse(QNetworkReply* reply){
    if(reply){
        if(reply->error() == QNetworkReply::NoError){

            QByteArray serviceResponse = reply->readAll();

            QJsonDocument jsonRespose = QJsonDocument::fromJson(serviceResponse);
            QJsonObject   jsonObject = jsonRespose.object();

            QJsonValue value = jsonObject.value("projects");
            QJsonArray projectsArray = value.toArray();

            foreach (const QJsonValue & element, projectsArray){
                ProjectDetails project;
                project.id = element.toObject().value("id").toInt();
                project.name = element.toObject().value("name").toString();
                project.uid = element.toObject().value("uid").toString();
                project.logo_url = element.toObject().value("logo_url").toString();
                project.position = element.toObject().value("position").toInt();
                project.is_active = element.toObject().value("is_active").toInt();
                project.is_owner_watcher = element.toObject().value("is_owner_watcher").toInt();
                project.dta_user_since = element.toObject().value("dta_user_since").toString();

                QJsonValue usersValue = element.toObject().value("users");
                QJsonArray usersArray = usersValue.toArray();
                foreach (const QJsonValue & userElement, usersArray){
                    UserDetails user;
                    user.id = userElement.toObject().value("id").toInt();
                    user.name = userElement.toObject().value("name").toString();
                    user.avatar_url = userElement.toObject().value("avatar_url").toString();
                    user.is_online = userElement.toObject().value("is_online").toInt();
                    user.dta_activity = userElement.toObject().value("dta_activity").toString();
                    user.dta_since = userElement.toObject().value("dta_since").toString();

                    //add to users list of the project
                    project.users.append(user);
                }

                //add to projects list
                projectsList.append(project);
            }

//            auto *projects = new Projects(this);
            projects = new Projects(this);

            foreach (ProjectDetails details, projectsList) {
                projects->addProject(details);
            }

            QObject::connect(projects, &Projects::clicked, [=] (const QModelIndex &index) {

                getProjectItemInfo(index.data(ProjectDelegate::idRole).toUInt());
            });

            ui->vLayoutProjectsList->addWidget(projects);
        }else{
            // Token error
            QByteArray serviceResponse = reply->readAll();

            QJsonDocument jsonRespose = QJsonDocument::fromJson(serviceResponse);
            QJsonObject   jsonObject = jsonRespose.object();

            TokenError tokenError;
            tokenError.name = jsonObject.value("name").toString();
            tokenError.message = jsonObject.value("message").toString();
            tokenError.code = jsonObject.value("code").toInt();
            tokenError.status = jsonObject.value("status").toInt();

            QString response =  tr("Error: %1 status: %2").arg(reply->errorString(), reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString());
            //qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Login Error :: -- "<< response;

            //token erro..now login
            uwiLogin->setBearerToken("");
            settings.setData(BEARER_TOKEN_KEY, "");
            uwiLogin->setIsLoggedIn(false);
            ui->buttonLogout->setText(tr("LOGIN"));
            uwiLogin->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
            uwiLogin->exec();
        }
    }else{
        // Response error
        //TODO : handle with showing proper dialog
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Request reply object is NULL !";
    }

    QObject::disconnect(manager, nullptr, nullptr, nullptr);
    return;
}

void UwiHome::projectItemResponse(QNetworkReply* reply){

    if(reply){
        if(reply->error() == QNetworkReply::NoError){

            QByteArray serviceResponse = reply->readAll();

            QJsonDocument jsonRespose = QJsonDocument::fromJson(serviceResponse);
            QJsonObject   jsonObject = jsonRespose.object();

            QJsonValue rootObj = jsonObject.value("project");

            projectEditItem->id = rootObj.toObject().value("id").toInt();
            projectEditItem->name = rootObj.toObject().value("name").toString();
            projectEditItem->uid = rootObj.toObject().value("uid").toString();
            projectEditItem->logo_url = rootObj.toObject().value("logo_url").toString();
            projectEditItem->position = rootObj.toObject().value("position").toInt();
            projectEditItem->is_active = rootObj.toObject().value("is_active").toInt();
            projectEditItem->is_owner_watcher = rootObj.toObject().value("is_owner_watcher").toInt();
            projectEditItem->dta_user_since = rootObj.toObject().value("dta_user_since").toString();

            QJsonValue usersValue = rootObj.toObject().value("users");
            QJsonArray usersArray = usersValue.toArray();

            QNetworkAccessManager *netwManager1 = new QNetworkAccessManager(this);
            QNetworkRequest request(projectEditItem->logo_url);

            QPixmap pixmap(":/images/project-96.png");
            ui->labelHolderProfileIcon->setPixmap(pixmap);

            QObject::connect(netwManager1, &QNetworkAccessManager::finished, this, [=] (QNetworkReply* reply) {

                QPixmap pixmap(":/images/project-96.png");
                ui->labelHolderProfileIcon->setPixmap(pixmap);

                if (reply->error() != QNetworkReply::NoError) {
                    netwManager1->deleteLater();
                    return;
                }

                QByteArray jpegData = reply->readAll();
                pixmap.loadFromData(jpegData);
                ui->labelHolderProfileIcon->setPixmap(pixmap);
                netwManager1->deleteLater();
            });

            netwManager1->get(request);

            static int i = 0;
            static int j = 0;

            j = 0;
            i = 0;
//            auto *grid = new QGridLayout;

            if(usersArray.size() <=0) {
                auto  noUsers= new UsersItem();
                noUsers->hideCheckAndIcon();
                noUsers->setName("No Users");

                ui->gridLayout->addWidget(noUsers, 0, 0);
            }
            foreach (const QJsonValue & userElement, usersArray){
                UserDetails user;
                user.id = userElement.toObject().value("id").toInt();
                user.name = userElement.toObject().value("name").toString();
                user.avatar_url = userElement.toObject().value("avatar_url").toString();
                user.is_online = userElement.toObject().value("is_online").toInt();
                user.dta_activity = userElement.toObject().value("dta_activity").toString();
                user.dta_since = userElement.toObject().value("dta_since").toString();

                //add to users list of the project
                projectEditItem->users.append(user);

                QNetworkAccessManager *netwManager = new QNetworkAccessManager(this);
                QNetworkRequest request(user.avatar_url);

                QObject::connect(netwManager, &QNetworkAccessManager::finished, this, [=] (QNetworkReply* reply) {
                    if (reply->error() != QNetworkReply::NoError) {
//                        static_cast<QStandardItemModel *>(model())->appendRow(item);

                        auto groupBoxUsers = new UsersItem();
                        groupBoxUsers->setMinimumSize(QSize(0, 196));

                        groupBoxUsers->setName(user.name);

                        ui->gridLayout->addWidget(groupBoxUsers, i, j);
                        j++;

                        if(j >= 3) {
                            j = 0;
                            i++;
                        }

                        netwManager->deleteLater();
                        return;
                    }

                    QByteArray jpegData = reply->readAll();
                    QPixmap pixmap;
                    pixmap.loadFromData(jpegData);
                    ui->labelHolderProfileIcon->setPixmap(pixmap);

                    auto groupBoxUsers = new UsersItem();
                    groupBoxUsers->setIcon(pixmap);
                    groupBoxUsers->setMinimumSize(QSize(0, 196));

                    groupBoxUsers->setName(user.name);


                    ui->gridLayout->addWidget(groupBoxUsers, i, j);
                    j++;
                    if(j >= 3) {
                        j = 0;
                        i++;
                    }
                    netwManager->deleteLater();
                });

                netwManager->get(request);                
            }

            //active toggle button handling
            ui->buttonActiveToggle->setAutoDefault(false);
            ui->buttonActiveToggle->setFlat(true);
            ui->buttonActiveToggle->setAutoFillBackground(true);
            if(projectEditItem->is_active) {
                QPixmap pixmap(":/images/toggle-on-48.png");
                QIcon ButtonIcon(pixmap);
                ui->buttonActiveToggle->setIcon(ButtonIcon);
                ui->buttonActiveToggle->setIconSize(pixmap.rect().size());
            } else {
                QPixmap pixmap2(":/images/toggle-off-48.png");
                QIcon ButtonIcon2(pixmap2);
                ui->buttonActiveToggle->setIcon(ButtonIcon2);
                ui->buttonActiveToggle->setIconSize(pixmap2.rect().size());
            }

            //name editing handling
            ui->lineEditName->setText(projectEditItem->name);

            //watcher toggle button handling
            ui->buttonWatcherToggle->setAutoDefault(false);
            ui->buttonWatcherToggle->setFlat(true);
            ui->buttonWatcherToggle->setAutoFillBackground(true);

            if(projectEditItem->is_owner_watcher) {
                QPixmap pixmap(":/images/toggle-on-48.png");
                QIcon ButtonIcon(pixmap);
                ui->buttonWatcherToggle->setIcon(ButtonIcon);
                ui->buttonWatcherToggle->setIconSize(pixmap.rect().size());
            } else {
                QPixmap pixmap2(":/images/toggle-off-48.png");
                QIcon ButtonIcon2(pixmap2);
                ui->buttonWatcherToggle->setIcon(ButtonIcon2);
                ui->buttonWatcherToggle->setIconSize(pixmap2.rect().size());
            }

            ui->stackedWidget->setCurrentIndex(1);
        }else{
            // Token error
            //TODO : handle with showing proper dialog

            QByteArray serviceResponse = reply->readAll();

            QJsonDocument jsonRespose = QJsonDocument::fromJson(serviceResponse);
            QJsonObject   jsonObject = jsonRespose.object();

            TokenError tokenError;
            tokenError.name = jsonObject.value("name").toString();
            tokenError.message = jsonObject.value("message").toString();
            tokenError.code = jsonObject.value("code").toInt();
            tokenError.status = jsonObject.value("status").toInt();

            QString response =  tr("Error: %1 status: %2").arg(reply->errorString(), reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString());
            //qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Login Error :: -- "<< response;

        }
    }else{
        // Response error
        //TODO : handle with showing proper dialog
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Request reply object is NULL !";
    }

    QObject::disconnect(manager, nullptr, nullptr, nullptr);
    return;
}

void UwiHome::projectEditNameResponse(QNetworkReply* reply){
    QByteArray serviceResponse = reply->readAll();

    if(reply){
        if(reply->error() == QNetworkReply::NoError){
            QObject::disconnect(manager, nullptr, nullptr, nullptr);
            ui->vLayoutProjectsList->removeWidget(projects);
            projects->clearAll();
            projectsList.clear();
            delete projects;
            getAllProjects();

            //remove items from users grid
            QLayoutItem *child;
            while ((child = ui->gridLayout->takeAt(0)) != 0) {
                if (child->widget())
                    child->widget()->setParent(NULL);

                delete child;
            }

            ui->stackedWidget->setCurrentIndex(0);
        } else {
            //handle error condition
        }
    }
}

//private
void UwiHome::getAllProjects(void)
{
    QString serviceLink = QString(SERVICE_BASE_URL) + QString(SERVICE_PROJECTS_LIST_SUFFIX);
    // Setup the webservice url
    QUrl serviceUrl = QUrl(serviceLink);
    QNetworkRequest request(serviceUrl);

    // Call the webservice
    QByteArray authorizationValue = QByteArray("Bearer ").append(uwiLogin->bearerToken().toLocal8Bit());
    request.setRawHeader(QByteArray("Authorization"), authorizationValue);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this,
            SLOT(allProjectsResponse(QNetworkReply*)));
    manager->get(request);
}

void UwiHome::getProjectItemInfo(quint64 id){
    uint projectsCount = projectsList.length();
    if(projectsCount > 0){
        QByteArray authorizationValue = QByteArray("Bearer ").append(uwiLogin->bearerToken().toLocal8Bit());

        connect(manager, SIGNAL(finished(QNetworkReply*)), this,
                SLOT(projectItemResponse(QNetworkReply*)));

        QString serviceLink = QString(SERVICE_BASE_URL) + QString("projects-manage/") + QString::number(id);
        // Setup the webservice url
        QUrl serviceUrl = QUrl(serviceLink);
        QNetworkRequest request(serviceUrl);
        request.setRawHeader(QByteArray("Authorization"), authorizationValue);
        // Call the webservice
        manager->get(request);
    }else{
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<< "Error : Projects count :" << QString::number(projectsCount);
    }
}
