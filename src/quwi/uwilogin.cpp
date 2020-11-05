#include "uwilogin.h"
#include "ui_uwilogin.h"
#include <QDebug>

UwiLogin::UwiLogin(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::Login),
    loggedIn(false)
{
    ui->setupUi(this);

    ui->buttonLogin->setDisabled(true);
    ui->labelInfo->clear();
    ui->labelInfo->hide();
    ui->labelInfo->setStyleSheet("QLabel { color : red; }");

    ui->lineEditUserName->setText("vitaliibondtest@gmail.com");
    ui->lineEditPassword->setText("vitaliibondtest");

    QObject::connect(ui->buttonLogin, &QPushButton::clicked, this, [=] () {
        ui->labelInfo->clear();
        ui->labelInfo->setStyleSheet("QLabel { color : green; }");
        ui->labelInfo->setText("Logging InProgress ...");
        ui->labelInfo->show();
        ui->buttonLogin->setEnabled(false);
        QString serviceLink = QString(SERVICE_BASE_URL) + QString(SERVICE_LOGIN_SUFFIX);
        // Setup the webservice url
        QUrl serviceUrl = QUrl(serviceLink);

        QJsonObject jsonObj;
        jsonObj[EMAIL] = ui->lineEditUserName->text();
        jsonObj[PASSWORD] = ui->lineEditPassword->text();

        QJsonDocument jsonDoc(jsonObj);
        QByteArray servicePostData = jsonDoc.toJson();

        // Call the webservice
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);

        //QNetworkRequest::setHeader();
        QNetworkRequest request(serviceUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(
                              QString("application/json")));
        request.setRawHeader(QByteArray("Client-Timezone-Offset"), QByteArray("60"));
        request.setRawHeader(QByteArray("Client-Language"), QByteArray("ru-RU"));
        request.setRawHeader(QByteArray("Client-Company"), QByteArray("udimiteam"));
        request.setRawHeader(QByteArray("Client-Device"), QByteArray("desktop"));

        connect(manager, SIGNAL(finished(QNetworkReply*)), this,
                SLOT(loginResponse(QNetworkReply*)));
        manager->post(request, servicePostData);

    }, Qt::AutoConnection);

    QObject::connect(ui->lineEditUserName, &QLineEdit::textChanged, this, [=] () {

        if(ui->lineEditUserName->text().contains("@") && ui->lineEditUserName->text().contains(".") &&
                !ui->lineEditPassword->text().isEmpty()) {
            ui->buttonLogin->setDisabled(false);
        } else {
            ui->buttonLogin->setDisabled(true);
        }

    }, Qt::AutoConnection);

    QObject::connect(ui->lineEditPassword, &QLineEdit::textChanged, this, [=] () {

        if(ui->lineEditUserName->text().contains("@") && ui->lineEditUserName->text().contains(".") &&
                !ui->lineEditPassword->text().isEmpty()) {
            ui->buttonLogin->setDisabled(false);
        } else {
            ui->buttonLogin->setDisabled(true);
        }

    }, Qt::AutoConnection);

}

UwiLogin::~UwiLogin()
{
    delete ui;
}

QString UwiLogin::bearerToken(void) const
{
    return token;
}
void UwiLogin::setBearerToken(QString bearerToken)
{
    this->token = bearerToken;
}

void UwiLogin::loginResponse(QNetworkReply* reply){
    if(reply){
        if(reply->error() == QNetworkReply::NoError){
            QByteArray serviceResponse = reply->readAll();

            QJsonDocument jsonRespose = QJsonDocument::fromJson(serviceResponse);
            QJsonObject   jsonObject = jsonRespose.object();

            QString bearerToken = jsonObject["token"].toString();

            setBearerToken(bearerToken);
            settings.setData(BEARER_TOKEN_KEY, QVariant(bearerToken));

            loggedIn = true;

            ui->labelInfo->hide();
            emit login(true);
            close();
        }else{
            // Login error
            //TODO : handle with showing proper dialog

            ui->lineEditPassword->clear();

            QString response =  tr("Error: %1 status: %2").arg(reply->errorString(), reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString());
            //qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Login Error :: -- "<< response;

            QByteArray errorResponse = reply->readAll();
            //qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Login Error Response :: -- "<< errorResponse;
            if(response.contains(DATA_VALIDATION_FAILED)){
                ui->labelInfo->setStyleSheet("QLabel { color : red; }");
                ui->labelInfo->setText("Email or Password Wrong\nPlease check.");
            }else if(response.contains(NETWORK_UNREACHABLE) || errorResponse.isEmpty()){
                ui->labelInfo->setStyleSheet("QLabel { color : red; }");
                ui->labelInfo->setText("Network Unreachable\nPlease check.");
            }

            ui->labelInfo->show();
            loggedIn = false;
            emit login(false);
        }
    }else{
        // Login error
        //TODO : handle with showing proper dialog
        ui->lineEditPassword->clear();
        ui->labelInfo->setText("Server Error\nPlease check.");
        ui->labelInfo->show();
        loggedIn = false;
        emit login(false);
    }
}

bool UwiLogin::isLoggedIn(void) const
{
    return loggedIn;
}

void UwiLogin::setIsLoggedIn(bool value)
{
    loggedIn = value;
}

