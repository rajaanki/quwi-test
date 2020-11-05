#include "projects.h"

Projects::Projects(QWidget *parent) :
	QListView(parent)
{
    auto *delegate = new ProjectDelegate(this);
	QPalette p(palette());

	p.setBrush(QPalette::WindowText, QColor("#303030"));
	p.setBrush(QPalette::Base, QColor("#F0F1F2"));
	p.setBrush(QPalette::Light, QColor("#FFFFFF"));
	p.setBrush(QPalette::Midlight, QColor("#D3D6D8"));
	p.setBrush(QPalette::Mid, QColor("#C5C9Cb"));
	p.setBrush(QPalette::Dark, QColor("#9AA0A4"));
	p.setBrush(QPalette::Text, QColor("#616b71"));
	p.setBrush(QPalette::Highlight, QColor("#E2E4E5"));

	delegate->setContentsMargins(8, 8, 8, 8);
	delegate->setIconSize(32, 32);
    delegate->setHorizontalSpacing(36);
    delegate->setVerticalSpacing(16);
    delegate->setProjectItemWidth(600);

	setPalette(p);
	setFont(QFont("MSShellDlg 2", 12));
	setModel(new QStandardItemModel(this));
	setItemDelegate(delegate);
    this->setLayoutDirection(Qt::LeftToRight);
}

void Projects::addProject(ProjectDetails &details)
{
    auto *item = new QStandardItem(QIcon(details.logo_url), details.name);

    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setData(details.id, ProjectDelegate::idRole);
    item->setData(details.name, ProjectDelegate::nameRole);
    item->setData(details.uid, ProjectDelegate::uidRole);
    item->setData(details.logo_url, ProjectDelegate::logo_urlRole);
    item->setData(details.position, ProjectDelegate::positionRole);
    item->setData(details.is_active, ProjectDelegate::is_activeRole);
    item->setData(details.is_owner_watcher, ProjectDelegate::is_owner_watcherRole);
    item->setData(details.dta_user_since, ProjectDelegate::dta_user_since);
    item->setData(details.spent_time_week, ProjectDelegate::spent_time_weekRole);
    item->setData(details.spent_time_month, ProjectDelegate::spent_time_monthRole);
    item->setData(details.spent_time_all, ProjectDelegate::spent_time_allRole);

    QNetworkAccessManager *netwManager = new QNetworkAccessManager(this);
    QNetworkRequest request(details.logo_url);

    QObject::connect(netwManager, &QNetworkAccessManager::finished, this, [=] (QNetworkReply* reply) {
        if (reply->error() != QNetworkReply::NoError) {
            QPixmap pixmap(":/images/project-96.png");
            item->setIcon(QIcon(pixmap));
            static_cast<QStandardItemModel *>(model())->appendRow(item);
            netwManager->deleteLater();
            return;
        }

        QByteArray jpegData = reply->readAll();
        QPixmap pixmap;
        pixmap.loadFromData(jpegData);
        item->setIcon(QIcon(pixmap));
        static_cast<QStandardItemModel *>(model())->appendRow(item);
        netwManager->deleteLater();
    });

    netwManager->get(request);
}

void Projects::clearAll()
{
	static_cast<QStandardItemModel *>(model())->clear();
}
