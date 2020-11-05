#ifndef USERSITEM_H
#define USERSITEM_H

#include <QWidget>

namespace Ui {
class UsersItem;
}

class UsersItem : public QWidget
{
    Q_OBJECT

public:
    explicit UsersItem(QWidget *parent = 0);
    ~UsersItem();

    void isChecked(void) const;
    void setChecked(bool value);
    void setName(QString name);
    void setIcon(QPixmap &pixmap);
    void hideCheckAndIcon(void);

private:
    Ui::UsersItem *ui;
};

#endif // USERSITEM_H
