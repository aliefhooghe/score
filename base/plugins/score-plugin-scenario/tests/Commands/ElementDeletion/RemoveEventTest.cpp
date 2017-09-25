// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest/QTest>

#include <Scenario/Commands/Scenario/Creations/CreateEvent.hpp>
#include <Scenario/Commands/Scenario/Deletions/RemoveEvent.hpp>

#include <Scenario/Document/Event/EventData.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

using namespace score;
using namespace Scenario::Command;

class RemoveEventTest : public QObject
{
  Q_OBJECT
private slots:
  void RemoveEventAndSynchronizationTest()
  {
    // only one event on a synchronization
    // the synchronization will be deleted too

    Scenario::ProcessModel* scenar = new ScenarioModel(
        std::chrono::seconds(15), Id<ProcessModel>{0}, qApp);

    EventData data{};
    data.dDate.setMSecs(10);
    data.relativeY = 0.4;
    data.endSynchronizationId = Id<SynchronizationModel>(-1);

    CreateEvent eventCmd(
        {
            {"ScenarioModel", {0}},
        },
        data);
    eventCmd.redo(ctx);

    auto eventCreated = scenar->event(eventCmd.createdEvent());
    auto event_id = eventCreated->id();
    auto tn_id = eventCreated->synchronization();

    int nbOfEvent = 3;
    int nbOfSynchronizations = 3;

    RemoveEvent removeCmd(
        {
            {"ScenarioModel", {0}},
        },
        eventCreated);

    removeCmd.redo(ctx);
    QCOMPARE((int)scenar->events().size(), nbOfEvent - 1);
    QCOMPARE((int)scenar->synchronizations().size(), nbOfSynchronizations - 1);
    try
    {
      scenar->event(event_id);
      QFAIL("Event call did not throw!");
    }
    catch (...)
    {
    }
    try
    {
      scenar->synchronization(tn_id);
      QFAIL("Synchronization call did not throw!");
    }
    catch (...)
    {
    }

    removeCmd.undo(ctx);
    QCOMPARE((int)scenar->events().size(), nbOfEvent);
    QCOMPARE((int)scenar->synchronizations().size(), nbOfSynchronizations);
    QCOMPARE(scenar->event(event_id)->heightPercentage(), 0.4);

    removeCmd.redo(ctx);
    QCOMPARE((int)scenar->events().size(), nbOfEvent - 1);
    QCOMPARE((int)scenar->synchronizations().size(), nbOfSynchronizations - 1);

    try
    {
      scenar->event(event_id);
      QFAIL("Event call did not throw!");
    }
    catch (...)
    {
    }
    try
    {
      scenar->synchronization(tn_id);
      QFAIL("Synchronization call did not throw!");
    }
    catch (...)
    {
    }

    delete scenar;
  }

  void RemoveOnlyEventTest()
  {
    // two events on a same synchronization
    // test removing just one of them : the synchronization stay

    Scenario::ProcessModel* scenar = new ScenarioModel(
        std::chrono::seconds(15), Id<ProcessModel>{0}, qApp);

    EventData data{};
    data.dDate.setMSecs(10);
    data.relativeY = 0.8;
    data.endSynchronizationId = Id<SynchronizationModel>(-1);

    CreateEvent eventCmd(
        {
            {"ScenarioModel", {0}},
        },
        data);
    eventCmd.redo(ctx);

    data.endSynchronizationId = eventCmd.createdSynchronization();
    data.relativeY = 0.4;

    CreateEvent event2Cmd(
        {
            {"ScenarioModel", {0}},
        },
        data);
    event2Cmd.redo(ctx);

    auto event_id = event2Cmd.createdEvent();
    auto eventCreated = scenar->event(event_id);

    int prevIntervalCount = 1;
    QCOMPARE(eventCreated->previousIntervals().size(), prevIntervalCount);

    auto prevIntervals = eventCreated->previousIntervals();

    int nbOfEvent = 4;
    int nbOfSynchronizations = 3;

    RemoveEvent removeCmd(
        {
            {"ScenarioModel", {0}},
        },
        eventCreated);

    removeCmd.redo(ctx);
    QCOMPARE((int)scenar->events().size(), nbOfEvent - 1);
    QCOMPARE((int)scenar->synchronizations().size(), nbOfSynchronizations);
    try
    {
      scenar->event(event_id);
      QFAIL("Event call did not throw!");
    }
    catch (...)
    {
    }

    removeCmd.undo(ctx);
    QCOMPARE((int)scenar->events().size(), nbOfEvent);
    QCOMPARE((int)scenar->synchronizations().size(), nbOfSynchronizations);
    QCOMPARE(scenar->event(event_id)->heightPercentage(), 0.4);
    QCOMPARE(
        scenar->event(event_id)->previousIntervals().size(),
        prevIntervalCount);
    QCOMPARE(
        scenar->event(event_id)->previousIntervals().first(),
        prevIntervals[0]);

    removeCmd.redo(ctx);
    QCOMPARE((int)scenar->events().size(), nbOfEvent - 1);
    QCOMPARE((int)scenar->synchronizations().size(), nbOfSynchronizations);

    try
    {
      scenar->event(event_id);
      QFAIL("Event call did not throw!");
    }
    catch (...)
    {
    }

    delete scenar;
  }
};

QTEST_MAIN(RemoveEventTest)
#include "RemoveEventTest.moc"
