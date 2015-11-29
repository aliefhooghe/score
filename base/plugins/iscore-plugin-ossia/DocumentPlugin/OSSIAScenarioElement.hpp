#pragma once
#include <Editor/TimeEvent.h>
#include <boost/optional/optional.hpp>
#include <iscore/tools/IdentifiedObjectMap.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <map>
#include <memory>

#include "OSSIAProcessElement.hpp"
#include "Scenario/Document/Constraint/ConstraintModel.hpp"

class DeviceList;
class EventModel;
class OSSIAConstraintElement;
class OSSIAEventElement;
class OSSIAStateElement;
class OSSIATimeNodeElement;
class Process;
class QObject;
class StateModel;
class TimeNodeModel;
namespace OSSIA {
class TimeProcess;
}  // namespace OSSIA
namespace Scenario {
class ScenarioModel;
}  // namespace Scenario

namespace OSSIA
{
    class Scenario;
}

// TODO see if this can be used for the base scenario model too.
class OSSIAScenarioElement final : public OSSIAProcessElement
{
    public:
        OSSIAScenarioElement(
                OSSIAConstraintElement& parentConstraint,
                Scenario::ScenarioModel& element,
                QObject* parent);

        std::shared_ptr<OSSIA::TimeProcess> OSSIAProcess() const override;
        std::shared_ptr<OSSIA::Scenario> scenario() const;

        const auto& states() const
        { return m_ossia_states; }
        const auto& constraints() const
        { return m_ossia_constraints; }
        const auto& events() const
        { return m_ossia_timeevents; }
        const auto& timeNodes() const
        { return m_ossia_timenodes; }

        Process& iscoreProcess() const override;

        void stop() override;

        void recreate() override;
        void clear() override;

    private:
        void on_constraintCreated(const ConstraintModel&);
        void on_stateCreated(const StateModel&);
        void on_eventCreated(const EventModel&);
        void on_timeNodeCreated(const TimeNodeModel&);

        void on_constraintRemoved(const ConstraintModel&);
        void on_stateRemoved(const StateModel&);
        void on_eventRemoved(const EventModel&);
        void on_timeNodeRemoved(const TimeNodeModel&);

        void startConstraintExecution(const Id<ConstraintModel>&);
        void stopConstraintExecution(const Id<ConstraintModel>&);

        void eventCallback(
                OSSIAEventElement& ev,
                OSSIA::TimeEvent::Status newStatus);

    private:
        // TODO use IdContainer
        std::map<Id<ConstraintModel>, OSSIAConstraintElement*> m_ossia_constraints;
        std::map<Id<StateModel>, OSSIAStateElement*> m_ossia_states;
        std::map<Id<TimeNodeModel>, OSSIATimeNodeElement*> m_ossia_timenodes;
        std::map<Id<EventModel>, OSSIAEventElement*> m_ossia_timeevents;
        std::shared_ptr<OSSIA::Scenario> m_ossia_scenario;
        Scenario::ScenarioModel& m_iscore_scenario;

        IdContainer<ConstraintModel> m_executingConstraints;

        const DeviceList& m_deviceList;
};
