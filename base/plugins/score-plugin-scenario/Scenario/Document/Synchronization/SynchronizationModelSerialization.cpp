// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <QDataStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QtGlobal>
#include <algorithm>
#include <score/tools/std/Optional.hpp>

#include "SynchronizationModel.hpp"
#include <Process/TimeValue.hpp>
#include <Scenario/Document/VerticalExtent.hpp>
#include <State/Expression.hpp>
#include <score/model/ModelMetadata.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONValueVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <score/model/Identifier.hpp>
#include <score/model/tree/TreeNode.hpp>

template <typename T>
class Reader;
template <typename T>
class Writer;
template <typename model>
class IdentifiedObject;


template <>
SCORE_PLUGIN_SCENARIO_EXPORT void
DataStreamReader::read(const Scenario::SynchronizationModel& synchronization)
{
  m_stream << synchronization.m_date << synchronization.m_events << synchronization.m_extent
           << synchronization.m_active << synchronization.m_expression;

  insertDelimiter();
}


template <>
SCORE_PLUGIN_SCENARIO_EXPORT void
DataStreamWriter::write(Scenario::SynchronizationModel& synchronization)
{
  m_stream >> synchronization.m_date >> synchronization.m_events >> synchronization.m_extent
      >> synchronization.m_active >> synchronization.m_expression;


  checkDelimiter();
}


template <>
SCORE_PLUGIN_SCENARIO_EXPORT void
JSONObjectReader::read(const Scenario::SynchronizationModel& synchronization)
{
  obj[strings.Date] = toJsonValue(synchronization.date());
  obj[strings.Events] = toJsonArray(synchronization.m_events);
  obj[strings.Extent] = toJsonValue(synchronization.m_extent);

  QJsonObject trig;
  trig[strings.Active] = synchronization.m_active;
  trig[strings.Expression] = toJsonObject(synchronization.m_expression);
  obj[strings.Trigger] = trig;
}


template <>
SCORE_PLUGIN_SCENARIO_EXPORT void
JSONObjectWriter::write(Scenario::SynchronizationModel& synchronization)
{
  synchronization.m_date = fromJsonValue<TimeVal>(obj[strings.Date]);
  synchronization.m_extent = fromJsonValue<Scenario::VerticalExtent>(obj[strings.Extent]);

  fromJsonValueArray(obj[strings.Events].toArray(), synchronization.m_events);

  State::Expression t;
  const auto& trig_obj = obj[strings.Trigger].toObject();
  fromJsonObject(trig_obj[strings.Expression], t);
  synchronization.m_expression = std::move(t);
  synchronization.m_active = trig_obj[strings.Active].toBool();
}
