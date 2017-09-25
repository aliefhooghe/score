// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest/QtTest>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>

#include <Process/TimeValue.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <score/model/path/Path.hpp>

#include <chrono>

class SynchronizationModelTests : public QObject
{
  Q_OBJECT

public:
private slots:
  void AddEventTest()
  {
    SynchronizationModel model{Id<SynchronizationModel>(1),
                        TimeValue{std::chrono::milliseconds(1)}, 0.5, this};
  }
};

QTEST_MAIN(SynchronizationModelTests)
#include "SynchronizationModelTests.moc"
