#pragma once
#include <Process/TimeValue.hpp>
#include <score/model/Identifier.hpp>

namespace Scenario
{
class IntervalModel;
class EventModel;
class StateModel;
class CommentBlockModel;

class SynchronizationModel;
class ProcessModel;
struct VerticalExtent;
template <typename T>
class ScenarioCreate;
template <>
class ScenarioCreate<CommentBlockModel>
{
public:
  static void undo(const Id<CommentBlockModel>& id, Scenario::ProcessModel& s);

  static CommentBlockModel& redo(
      const Id<CommentBlockModel>& id,
      const TimeVal& date,
      double y,
      Scenario::ProcessModel& s);
};

template <>
class ScenarioCreate<SynchronizationModel>
{
public:
  static void undo(const Id<SynchronizationModel>& id, Scenario::ProcessModel& s);

  static SynchronizationModel& redo(
      const Id<SynchronizationModel>& id,
      const VerticalExtent& extent,
      const TimeVal& date,
      Scenario::ProcessModel& s);
};

template <>
class ScenarioCreate<EventModel>
{
public:
  static void undo(const Id<EventModel>& id, Scenario::ProcessModel& s);

  static EventModel& redo(
      const Id<EventModel>& id,
      SynchronizationModel& synchronization,
      const VerticalExtent& extent,
      Scenario::ProcessModel& s);
};

template <>
class ScenarioCreate<StateModel>
{
public:
  static void undo(const Id<StateModel>& id, Scenario::ProcessModel& s);

  static StateModel& redo(
      const Id<StateModel>& id,
      EventModel& ev,
      double y,
      Scenario::ProcessModel& s);
};

template <>
class ScenarioCreate<IntervalModel>
{
public:
  static void undo(const Id<IntervalModel>& id, Scenario::ProcessModel& s);

  static IntervalModel& redo(
      const Id<IntervalModel>& id,
      StateModel& sst,
      StateModel& est,
      double ypos,
      Scenario::ProcessModel& s);
};

} // namespace Scenario
