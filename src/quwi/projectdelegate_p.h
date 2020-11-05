#ifndef PROJECTDELEGATE_P_H
#define PROJECTDELEGATE_P_H

#include <qglobal.h>

class ProjectDelegatePrivate
{
    ProjectDelegatePrivate();

    inline QRect statusBox(const QStyleOptionViewItem &option,
							  const QModelIndex &index) const;
    inline qreal statusFontPointSize(const QFont &f) const;
    inline qreal timeFontPointSize(const QFont &f) const;

    inline QRect nameBox(const QStyleOptionViewItem &option) const;
    inline QRect timeBox(const QStyleOptionViewItem &option,
                              const QModelIndex &index, const int role) const;
    inline QRect timeBox(const QStyleOptionViewItem &option,
                              const QString &text) const;

	QSize iconSize;
	QMargins margins;
	int spacingHorizontal;
	int spacingVertical;
    int minWidth;

    friend class ProjectDelegate;
};

#endif // PROJECTDELEGATE_P_H
