#include "appdata.h"
#include <QApplication>

AppData::AppData()
{
    settings = new QSettings(qApp->applicationDirPath() + "/app_data.ini", QSettings::IniFormat);
}

void AppData::setData(QString key, QVariant value){
    settings->setValue(key, value);
    settings->sync();
    return;
}

QVariant AppData::getData(QString key){
    QVariant value = settings->value(key);
    return value;
}
