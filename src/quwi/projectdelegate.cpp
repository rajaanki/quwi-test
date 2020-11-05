#include "projectdelegate.h"
#include "projectdelegate_p.h"
#include <QPainter>
#include <QDebug>

ProjectDelegate::ProjectDelegate(QObject *parent) :
	QStyledItemDelegate(parent),
	m_ptr(new ProjectDelegatePrivate)
{

}

ProjectDelegate::~ProjectDelegate()
{
	delete m_ptr;
}

QSize ProjectDelegate::iconSize() const
{
	return m_ptr->iconSize;
}

void ProjectDelegate::setIconSize(int width, int height)
{
	m_ptr->iconSize = QSize(width, height);
}

QMargins ProjectDelegate::contentsMargins() const
{
	return m_ptr->margins;
}

void ProjectDelegate::setContentsMargins(int left, int top, int right, int bottom)
{
	m_ptr->margins = QMargins(left, top, right, bottom);
}

int ProjectDelegate::horizontalSpacing() const
{
	return m_ptr->spacingHorizontal;
}

void ProjectDelegate::setHorizontalSpacing(int spacing)
{
	m_ptr->spacingHorizontal = spacing;
}

int ProjectDelegate::verticalSpacing() const
{
	return m_ptr->spacingVertical;
}

void ProjectDelegate::setVerticalSpacing(int spacing)
{
	m_ptr->spacingVertical = spacing;
}

int ProjectDelegate::projectItemWidth() const
{
    return m_ptr->minWidth;
}

void ProjectDelegate::setProjectItemWidth(int width)
{
    m_ptr->minWidth = width;
}

void ProjectDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
					 const QModelIndex &index) const
{
	QStyleOptionViewItem opt(option);
	initStyleOption(&opt, index);

	const QPalette &palette(opt.palette);
	const QRect &rect(opt.rect);
	const QRect &contentRect(rect.adjusted(m_ptr->margins.left(),
											   m_ptr->margins.top(),
											   -m_ptr->margins.right(),
											   -m_ptr->margins.bottom()));
	const bool lastIndex = (index.model()->rowCount() - 1) == index.row();
	const bool hasIcon = !opt.icon.isNull();
	const int bottomEdge = rect.bottom();
	QFont f(opt.font);

    f.setPointSize(m_ptr->statusFontPointSize(opt.font));

	painter->save();
	painter->setClipping(true);
	painter->setClipRect(rect);
	painter->setFont(opt.font);

	// Draw background
	painter->fillRect(rect, opt.state & QStyle::State_Selected ?
						  palette.highlight().color() :
						  palette.light().color());

	// Draw bottom line
	painter->setPen(lastIndex ? palette.dark().color()
							  : palette.mid().color());
	painter->drawLine(lastIndex ? rect.left() : m_ptr->margins.left(),
					  bottomEdge, rect.right(), bottomEdge);

    // Draw projectitem icon
	if (hasIcon)
        painter->drawPixmap(contentRect.left(), contentRect.top() + verticalSpacing() / 2,
							opt.icon.pixmap(m_ptr->iconSize));

    // Draw project name text
    QRect messageRect(m_ptr->nameBox(opt));

    messageRect.moveTo(m_ptr->margins.left() + m_ptr->iconSize.width()
                       + m_ptr->spacingHorizontal,
                       contentRect.top() + m_ptr->spacingVertical);

    painter->setFont(opt.font);
    painter->setPen(palette.windowText().color());
    painter->drawText(messageRect, Qt::TextSingleLine, opt.text);

    // Draw status
    QRect statusRect(m_ptr->statusBox(opt, index));

    statusRect.moveTo(m_ptr->margins.left() + m_ptr->minWidth/2,
                         contentRect.top() + m_ptr->spacingVertical);

    painter->setFont(f);
    if(index.data(is_activeRole).toBool()) {
        painter->setPen(QColor("#298A08"));

        painter->drawText(statusRect, Qt::TextSingleLine,
                          QString("Active"));
    } else {
        painter->setPen(palette.text().color());

        painter->drawText(statusRect, Qt::TextSingleLine,
                          QString("Inactive"));
    }


    // Draw weektime Text
    QRect weekTimeRect(m_ptr->timeBox(opt, QString("this week time")));

    weekTimeRect.moveTo(m_ptr->margins.left() + m_ptr->minWidth * 3/4,
                         contentRect.top() - m_ptr->spacingVertical / 2);

    f.setPointSizeF(m_ptr->timeFontPointSize(option.font));
    painter->setFont(f);

    painter->setPen(palette.windowText().color());

    painter->drawText(weekTimeRect, Qt::TextSingleLine,
                      QString("this week time"));

    // Draw monthtime text
    QRect monthTimeRect(m_ptr->timeBox(opt, QString("this month time")));

    monthTimeRect.moveTo(m_ptr->margins.left() + m_ptr->minWidth * 3/4,
                         contentRect.top() + m_ptr->spacingVertical);

    painter->drawText(monthTimeRect, Qt::TextSingleLine,
                      QString("this month time"));

    // Draw totaltime
    QRect totalTimeRect(m_ptr->timeBox(opt, QString("total time")));

    totalTimeRect.moveTo(m_ptr->margins.left() + m_ptr->minWidth * 3/4,
                         contentRect.bottom() - m_ptr->spacingVertical);

    painter->drawText(totalTimeRect, Qt::TextSingleLine,
                      QString("total time"));

    // Draw weektime value
    QRect weekTimeValueRect(m_ptr->timeBox(opt, index, spent_time_weekRole));

    weekTimeValueRect.moveTo(m_ptr->margins.left() + m_ptr->minWidth,
                         contentRect.top() - m_ptr->spacingVertical / 2);

    painter->drawText(weekTimeValueRect, Qt::TextSingleLine,
                      index.data(spent_time_weekRole).toString());

    // Draw monthtime value
    QRect monthTimeValueRect(m_ptr->timeBox(opt, index, spent_time_monthRole));

    monthTimeValueRect.moveTo(m_ptr->margins.left() + m_ptr->minWidth,
                         contentRect.top() + m_ptr->spacingVertical);

    painter->drawText(monthTimeValueRect, Qt::TextSingleLine,
                      index.data(spent_time_monthRole).toString());

    // Draw totaltime value
    QRect totalTimeValueRect(m_ptr->timeBox(opt, index, spent_time_allRole));

    totalTimeValueRect.moveTo(m_ptr->margins.left() + m_ptr->minWidth,
                         contentRect.bottom() - m_ptr->spacingVertical);

    painter->drawText(totalTimeValueRect, Qt::TextSingleLine,
                      index.data(spent_time_allRole).toString());

	painter->restore();
}

QSize ProjectDelegate::sizeHint(const QStyleOptionViewItem &option,
						 const QModelIndex &index) const
{
	QStyleOptionViewItem opt(option);
	initStyleOption(&opt, index);

    int textHeight = m_ptr->statusBox(opt, index).height()
            + m_ptr->spacingVertical + m_ptr->nameBox(opt).height();
	int iconHeight = m_ptr->iconSize.height();
	int h = textHeight > iconHeight ? textHeight : iconHeight;

	return QSize(opt.rect.width(), m_ptr->margins.top() + h
				 + m_ptr->margins.bottom());
}

ProjectDelegatePrivate::ProjectDelegatePrivate() :
    iconSize(16, 16),
	margins(0, 0, 0, 0),
	spacingHorizontal(0),
    spacingVertical(0)
{

}

QRect ProjectDelegatePrivate::statusBox(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
	QFont f(option.font);

    f.setPointSizeF(statusFontPointSize(option.font));

    if(index.data(ProjectDelegate::is_activeRole).toBool()) {
        return QFontMetrics(f).boundingRect(QString("Active"))
                .adjusted(0, 0, 1, 1);
    }

    return QFontMetrics(f).boundingRect(QString("Inactive"))
            .adjusted(0, 0, 1, 1);
}

qreal ProjectDelegatePrivate::statusFontPointSize(const QFont &f) const
{
    return 1*f.pointSize();
}

qreal ProjectDelegatePrivate::timeFontPointSize(const QFont &f) const
{
    return 0.85*f.pointSize();
}

QRect ProjectDelegatePrivate::nameBox(const QStyleOptionViewItem &option) const
{
	return option.fontMetrics.boundingRect(option.text).adjusted(0, 0, 1, 1);
}

QRect ProjectDelegatePrivate::timeBox(const QStyleOptionViewItem &option,
                             const QModelIndex &index, const int role) const
{
    QFont f(option.font);

    f.setPointSizeF(timeFontPointSize(option.font));

    return QFontMetrics(f).boundingRect(index.data(role).toString())
            .adjusted(0, 0, 1, 1);
}

QRect ProjectDelegatePrivate::timeBox(const QStyleOptionViewItem &option, const QString &text) const
{
    QFont f(option.font);

    f.setPointSizeF(timeFontPointSize(option.font));

    return QFontMetrics(f).boundingRect(text)
            .adjusted(0, 0, 1, 1);
}
