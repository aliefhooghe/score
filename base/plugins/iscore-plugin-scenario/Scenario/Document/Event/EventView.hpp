#pragma once
#include <Scenario/Document/VerticalExtent.hpp>
#include <qcolor.h>
#include <qglobal.h>
#include <qgraphicsitem.h>
#include <qpoint.h>
#include <qrect.h>
#include <qstring.h>

#include "ExecutionStatus.hpp"

class ConditionView;
class EventPresenter;
class QGraphicsSceneDragDropEvent;
class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QMimeData;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class EventView final : public QGraphicsObject
{
        Q_OBJECT

    public:
        EventView(EventPresenter& presenter, QGraphicsObject* parent);
        virtual ~EventView() = default;

        static constexpr int static_type()
        { return QGraphicsItem::UserType + 1; }
        int type() const override
        { return static_type(); }

        const EventPresenter& presenter() const
        { return m_presenter; }

        QRectF boundingRect() const override
        { return {-5, -10., 10, qreal(m_extent.bottom() - m_extent.top() + 20)};  }

        void paint(QPainter* painter,
                   const QStyleOptionGraphicsItem* option,
                   QWidget* widget) override;

        void setSelected(bool selected);
        bool isSelected() const;

        void setCondition(const QString& cond);
        bool hasCondition() const;

        void setTrigger(const QString& trig);
        bool hasTrigger() const;

        void setExtent(const VerticalExtent& extent);
        void setExtent(VerticalExtent&& extent);

        void setStatus(ExecutionStatus s);

    signals:
        void eventHoverEnter();
        void eventHoverLeave();

        void dropReceived(const QPointF& pos, const QMimeData*);

    public slots:
        void changeColor(QColor);

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
        void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

        void hoverEnterEvent(QGraphicsSceneHoverEvent* h) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* h) override;

        void dropEvent(QGraphicsSceneDragDropEvent *event) override;

    private:
        EventPresenter& m_presenter;
        QString m_condition;
        QString m_trigger;
        QPointF m_clickedPoint;
        QColor m_color;

        ExecutionStatus m_status{ExecutionStatus::Editing};
        bool m_selected{};

        VerticalExtent m_extent;

        ConditionView* m_conditionItem{};
};

