#ifndef PROJECTDELEGATE_H
#define PROJECTDELEGATE_H

#include <QStyledItemDelegate>

class ProjectDelegatePrivate;

class ProjectDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
    enum roles {
        idRole = Qt::UserRole + 1,
        nameRole,
        uidRole,
        logo_urlRole,
        positionRole,
        is_activeRole,
        is_owner_watcherRole,
        dta_user_since,
        spent_time_weekRole,
        spent_time_monthRole,
        spent_time_allRole
    };

    explicit ProjectDelegate(QObject *parent = nullptr);
    ~ProjectDelegate();

	QSize iconSize() const;
	void setIconSize(int width, int height);

	QMargins contentsMargins() const;
	void setContentsMargins(int left, int top, int right, int bottom);

	int horizontalSpacing() const;
	void setHorizontalSpacing(int spacing);

	int verticalSpacing() const;
	void setVerticalSpacing(int spacing);

    int projectItemWidth() const;
    void setProjectItemWidth(int width);

//    int ProjectDelegate::projectItemMinWidth()
//    {
//        return m_ptr->minWidth;
//    }

//    int ProjectDelegate::setProjectItemMinWidth(int width)
//    {
//        m_ptr->minWidth = width;
//    }

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
			   const QModelIndex &index) const override;
	QSize sizeHint(const QStyleOptionViewItem &option,
				   const QModelIndex &index) const override;

private:
    ProjectDelegatePrivate *m_ptr;
};

#endif // PROJECTDELEGATE_H
