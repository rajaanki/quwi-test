#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SERVICE_BASE_URL                "https://api.quwi.com/v2/"
#define SERVICE_LOGIN_SUFFIX            "auth/login"
#define SERVICE_LOGOUT_SUFFIX           "auth/logout"
#define SERVICE_PROJECTS_LIST_SUFFIX    "projects-manage/index"
#define SERVICE_PROJECT_PROPERTY_CHANGE "projects-manage/update"

#define EMAIL                           "email"
#define PASSWORD                        "password"

#define LOGIN_ERROR_KEY                 "first_errors"

#define DATA_VALIDATION_FAILED          "Data Validation Failed"
#define NETWORK_UNREACHABLE             "Network unreachable"

#define BEARER_TOKEN_KEY                "Bearer_Token"

#include <QtGlobal>

typedef struct _TokenError {
    QString name;
    QString message;
    int code;
    int status;
}TokenError;

typedef struct _UserDetails {
    quint64 id;
    QString name;
    QString avatar_url;
    quint8 is_online;
    QString dta_activity;
    QString dta_since;

}UserDetails;

typedef struct _ProjectDetails{
    quint64 id;
    QString name;
    QString uid;
    QString logo_url;
    quint64 position;
    quint8 is_active;
    quint8 is_owner_watcher;
    QString dta_user_since;
    QString spent_time_week;
    QString spent_time_month;
    QString spent_time_all;
    QList<UserDetails> users;
}ProjectDetails;

#endif // CONSTANTS_H
