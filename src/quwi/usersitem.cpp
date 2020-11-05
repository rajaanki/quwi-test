#include "usersitem.h"
#include "ui_usersitem.h"

UsersItem::UsersItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UsersItem)
{
    ui->setupUi(this);
    QPixmap pixmap(":/images/test-profile-24.png");
    ui->icon->setPixmap(pixmap);
}

UsersItem::~UsersItem()
{
    delete ui;
}

void UsersItem::isChecked(void) const
{
    ui->checkBox->isChecked();
}

void UsersItem::setChecked(bool value)
{
    ui->checkBox->setChecked(value);
}

void UsersItem::setName(QString name)
{
    ui->name->setText(name);
}

void UsersItem::setIcon(QPixmap &pixmap)
{
    ui->icon->setPixmap(pixmap);
}

void UsersItem::hideCheckAndIcon(void)
{
    ui->icon->hide();
    ui->checkBox->hide();
}
