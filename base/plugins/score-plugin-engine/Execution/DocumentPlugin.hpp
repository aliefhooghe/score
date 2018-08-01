#pragma once
#include "BaseScenarioComponent.hpp"
#include <wobjectdefs.h>

#include <ossia/dataflow/bench_map.hpp>
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>

#include <Execution/ExecutorContext.hpp>
#include <Execution/ExecutorSetup.hpp>
#include <Process/Dataflow/Port.hpp>
#include <memory>
#include <score/plugins/documentdelegate/plugin/DocumentPlugin.hpp>
#include <score/tools/Metadata.hpp>

namespace ossia
{
class audio_protocol;
struct bench_map;
}
namespace score
{
class DocumentModel;
}
namespace Scenario
{
class BaseScenario;
class IntervalModel;
}
namespace Dataflow
{
class AudioDevice;
}
namespace Execution
{
class SCORE_PLUGIN_ENGINE_EXPORT DocumentPlugin final
    : public score::DocumentPlugin
{
  W_OBJECT(DocumentPlugin)
public:
  DocumentPlugin(
      const score::DocumentContext& ctx,
      Id<score::DocumentPlugin>,
      QObject* parent);

  ~DocumentPlugin() override;
  void reload(Scenario::IntervalModel& doc);
  void clear();

  void on_documentClosing() override;
  const BaseScenarioElement& baseScenario() const;
  BaseScenarioElement& baseScenario();

  bool isPlaying() const;

  const Context& context() const
  {
    return m_ctx;
  }
  ossia::audio_protocol& audioProto();

  void runAllCommands() const;

  const Execution::Settings::Model& settings;

  std::shared_ptr<ossia::graph_interface> execGraph;
  std::shared_ptr<ossia::execution_state> execState;

  QPointer<Dataflow::AudioDevice> audio_device{};


public:
  void finished() W_SIGNAL(finished);
  void sig_bench(ossia::bench_map arg_1, int64_t ns) W_SIGNAL(sig_bench, arg_1, ns);
public:
  void slot_bench(ossia::bench_map, int64_t ns); W_SLOT(slot_bench);

private:

  mutable ExecutionCommandQueue m_execQueue;
  mutable ExecutionCommandQueue m_editionQueue;
  Context m_ctx;
  SetupContext m_setup_ctx;
  BaseScenarioElement m_base;
  std::atomic_bool m_created{};

  void on_finished();

  void timerEvent(QTimerEvent* event) override;
  int m_tid{};
  void makeGraph();
};

}