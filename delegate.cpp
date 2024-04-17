// delegate.cpp
#include <QtGui>
#include "delegate.h"
#include <QApplication>
#include <QMouseEvent>

MyDelegate::MyDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

void MyDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton button;
    QRect rect = option.rect;
    int x = rect.left() + rect.width() - 30;
    int y = rect.top();
    int w = 30;
    int h = 30;
    button.rect = QRect(x, y, w, h);
    button.text = "Delete";
    button.state = QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool MyDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        int clickX = e->x();
        int clickY = e->y();

        QRect rect = option.rect;
        int x = rect.left() + rect.width() - 30;
        int y = rect.top();
        int w = 30;
        int h = 30;

        if (clickX > x && clickX < x + w && clickY > y && clickY < y + h)
        {
            model->removeRow(index.row());
            return true;
        }
    }

    return false;
}
