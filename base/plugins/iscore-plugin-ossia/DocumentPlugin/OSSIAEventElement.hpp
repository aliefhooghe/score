#pragma once
#include <State/Expression.hpp>
#include <qobject.h>
#include <memory>

class DeviceList;
class EventModel;
class QEvent;

namespace OSSIA
{
    class TimeEvent;
}
class OSSIAEventElement final : public QObject
{
    public:
        OSSIAEventElement(
                std::shared_ptr<OSSIA::TimeEvent> event,
                const EventModel& element,
                const DeviceList& deviceList,
                QObject* parent);

        std::shared_ptr<OSSIA::TimeEvent> OSSIAEvent() const;
        const EventModel& iscoreEvent() const
        { return m_iscore_event; }


        bool event(QEvent* ev) override { return QObject::event(ev); }

        void recreate();
        void clear();
    private:
        void on_conditionChanged(const iscore::Condition& c);

        const EventModel& m_iscore_event;
        std::shared_ptr<OSSIA::TimeEvent> m_ossia_event;
        const DeviceList& m_deviceList;
};
