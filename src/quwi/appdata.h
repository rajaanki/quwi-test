#ifndef APPDATA_H
#define APPDATA_H

#include <QSettings>
#include <QDir>


class AppData
{
public:
    AppData();
    void setData(const QString key, QVariant value);
    QVariant getData(const QString key);

private:
    QSettings* settings;
};

#endif // APPDATA_H
