#pragma once
#include <Scenario/Document/VerticalExtent.hpp>
#include <qcolor.h>
#include <qgraphicsitem.h>
#include <qpoint.h>
#include <qrect.h>

class QGraphicsSceneMouseEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class TimeNodePresenter;

class TimeNodeView final : public QGraphicsObject
{
        Q_OBJECT

    public:
        TimeNodeView(TimeNodePresenter& presenter,
                     QGraphicsObject* parent);
        ~TimeNodeView() = default;

        static constexpr int static_type()
        { return QGraphicsItem::UserType + 3; }
        int type() const override
        { return static_type(); }

        const TimeNodePresenter& presenter() const
        { return m_presenter;}

        void paint(QPainter* painter,
                   const QStyleOptionGraphicsItem* option,
                   QWidget* widget) override;

        // QGraphicsItem interface
        QRectF boundingRect() const override
        { return { -3., 0., 6., m_extent.bottom() - m_extent.top()}; }

        void setExtent(const VerticalExtent& extent);
        void setExtent(VerticalExtent&& extent);
        void addPoint(int newY);

        void setMoving(bool);
        void setSelected(bool selected);

        bool isSelected() const
        {
            return m_selected;
        }

    public slots:
        void changeColor(QColor);

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
        void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    private:
        TimeNodePresenter& m_presenter;
        VerticalExtent m_extent;

        QPointF m_clickedPoint {};
        QColor m_color;
        bool m_selected{};
};
