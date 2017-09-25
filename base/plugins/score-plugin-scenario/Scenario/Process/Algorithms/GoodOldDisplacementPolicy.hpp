#pragma once

#include <QString>
#include <QVector>
#include <Scenario/Process/Algorithms/StandardDisplacementPolicy.hpp>
#include <algorithm>
#include <vector>

#include <Process/TimeValue.hpp>

#include <score/model/Identifier.hpp>

namespace Scenario
{
struct ElementsProperties;
class SynchronizationModel;
class ProcessModel;
class GoodOldDisplacementPolicy
{
public:
  static void init(
      Scenario::ProcessModel& scenario,
      const QVector<Id<SynchronizationModel>>& draggedElements)
  {
  }

  static void computeDisplacement(
      Scenario::ProcessModel& scenario,
      const QVector<Id<SynchronizationModel>>& draggedElements,
      const TimeVal& deltaTime,
      ElementsProperties& elementsProperties);

  static void getRelatedSynchronizations(
      Scenario::ProcessModel& scenario,
      const Id<SynchronizationModel>& firstSynchronizationMovedId,
      std::vector<Id<SynchronizationModel>>& translatedSynchronizations);

  static QString name()
  {
    return QString{"Old way"};
  }

  template <typename... Args>
  static void updatePositions(Args&&... args)
  {
    CommonDisplacementPolicy::updatePositions(std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void revertPositions(Args&&... args)
  {
    CommonDisplacementPolicy::revertPositions(std::forward<Args>(args)...);
  }
};
}
