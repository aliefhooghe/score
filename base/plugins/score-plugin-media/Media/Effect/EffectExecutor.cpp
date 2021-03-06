#include "EffectExecutor.hpp"

#include <Process/ExecutionContext.hpp>
#include <Process/ExecutionSetup.hpp>

#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/port.hpp>
#include <fmt/format.h>

namespace fmt
{
template <>
struct formatter<ossia::graph_node>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const ossia::graph_node& e, FormatContext &ctx) {
    return fmt::format_to(ctx.begin(), "{} ({})", e.label(), (void*) &e);
  }
};

template <>
struct formatter<ossia::graph_edge>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const ossia::graph_edge& e, FormatContext &ctx) {
    return fmt::format_to(ctx.begin(), "{}[{}] -> {}[{}]",
                          *e.out_node, std::distance(ossia::find(e.out_node->outputs(), e.out), e.out_node->outputs().begin()),
                          *e.in_node, std::distance(ossia::find(e.in_node->inputs(), e.in), e.in_node->inputs().begin()));
  }
};
}

namespace Media
{

static
void check_exec_validity(const ossia::graph_interface& g, const ossia::node_chain_process& proc)
{
  for(auto& node : proc.nodes)
  {
    SCORE_ASSERT(ossia::contains(g.get_nodes(), node.get()));
  }

  for(auto it = proc.nodes.begin(); it != proc.nodes.end(); ++it)
  {
    if(it + 1 == proc.nodes.end())
      break;

    auto& outputs = it->get()->outputs();
    SCORE_ASSERT(outputs.size() >= 1);

    auto& inputs = (it+1)->get()->inputs();
    SCORE_ASSERT(inputs.size() >= 1);

    SCORE_ASSERT(outputs.front()->targets.size() == 1);
    SCORE_ASSERT(inputs.front()->sources.size() == 1);

    SCORE_ASSERT(outputs.front()->targets[0] == inputs.front()->sources[0]);
    auto edge = outputs.front()->targets[0];
    SCORE_ASSERT(edge->out_node.get() == it->get());
    SCORE_ASSERT(edge->in_node.get() == (it+1)->get());
  }
}
static std::vector<ossia::node_ptr> get_nodes(const std::vector<std::pair<Id<Process::ProcessModel>, EffectProcessComponentBase::RegisteredEffect>>& fx)
{
  std::vector<ossia::node_ptr> v;
  for(const auto& f : fx)
    v.push_back(f.second.node());
  return v;
}
static
void check_exec_order(const std::vector<ossia::node_ptr>&g, const ossia::node_chain_process& proc)
{
  SCORE_ASSERT(proc.nodes == g);
}
static
void check_last_validity(const ossia::graph_interface& g, const ossia::node_chain_process& proc)
{
  if(!proc.nodes.empty())
    SCORE_ASSERT(proc.node == proc.nodes.back());
  else
    SCORE_ASSERT(proc.node == std::shared_ptr<ossia::graph_node>{});
}
EffectProcessComponentBase::EffectProcessComponentBase(
    Media::Effect::ProcessModel& element, const Execution::Context& ctx,
    const Id<score::Component>& id, QObject* parent)
    : Execution::ProcessComponent_T<
          Media::Effect::ProcessModel, ossia::node_chain_process>{
          element, ctx, id, "Executor::EffectComponent", parent}
{
  m_ossia_process = std::make_shared<ossia::node_chain_process>();
  element.effects().orderChanged.connect<&EffectProcessComponentBase::on_orderChanged>(*this);
}

EffectProcessComponentBase::~EffectProcessComponentBase()
{
  process().effects().orderChanged.disconnect<&EffectProcessComponentBase::on_orderChanged>(*this);
}

static auto move_edges(
    ossia::inlet& old_in, ossia::inlet_ptr new_in,
    std::shared_ptr<ossia::graph_node> new_node, ossia::graph_interface& g)
{
  auto old_sources = old_in.sources;
  for (ossia::graph_edge* e : old_sources)
  {
    g.connect(ossia::make_edge(e->con, e->out, new_in, e->out_node, new_node));
    g.disconnect(e);
  }
}
static auto move_edges(
    ossia::outlet& old_out, ossia::outlet_ptr new_out,
    std::shared_ptr<ossia::graph_node> new_node, ossia::graph_interface& g)
{
  auto old_targets = old_out.targets;
  for (ossia::graph_edge* e : old_targets)
  {
    g.connect(ossia::make_edge(e->con, new_out, e->in, new_node, e->in_node));
    g.disconnect(e);
  }
}


void EffectProcessComponentBase::unregister_old_first_node(
    std::pair<Id<Process::ProcessModel>, EffectProcessComponentBase::RegisteredEffect>& old_first,
    std::vector<Execution::ExecutionCommand>& commands)
{
  if (old_first.second)
  {
    unreg(old_first.second);
    // Take all the incoming cables and keep them
    old_first.second.registeredInlets
        = process().effects().at(old_first.first).inlets();
    reg(old_first.second, commands);
  }
}
void EffectProcessComponentBase::register_new_first_node(
    std::pair<Id<Process::ProcessModel>, EffectProcessComponentBase::RegisteredEffect>& new_first,
    std::vector<Execution::ExecutionCommand>& commands)
{
  if (new_first.second)
  {
    unreg(new_first.second);
    if (new_first.second.node())
    {
      new_first.second.registeredInlets
          = process().effects().at(new_first.first).inlets();
      new_first.second.registeredInlets[0] = process().inlet.get();
      reg(new_first.second, commands);
    }
  }
}

void EffectProcessComponentBase::unregister_old_last_node(
    std::pair<Id<Process::ProcessModel>, EffectProcessComponentBase::RegisteredEffect>& old_last,
    std::vector<Execution::ExecutionCommand>& commands)
{
  if (old_last.second)
  {
    unreg(old_last.second);
    old_last.second.registeredOutlets
        = process().effects().at(old_last.first).outlets();
    reg(old_last.second, commands);
  }
}
void EffectProcessComponentBase::register_new_last_node(
    std::pair<Id<Process::ProcessModel>, EffectProcessComponentBase::RegisteredEffect>& new_last,
    std::vector<Execution::ExecutionCommand>& commands)
{
  if (new_last.second)
  {
    unreg(new_last.second);
    if (new_last.second.node())
    {
      new_last.second.registeredOutlets
          = process().effects().at(new_last.first).outlets();
      new_last.second.registeredOutlets[0] = process().outlet.get();
      reg(new_last.second, commands);

      auto old = m_ossia_process->node;
      commands.push_back([proc = m_ossia_process, node = new_last.second.node()] {
        proc->node = node;
      });
      nodeChanged(old, new_last.second.node(), commands);
    }
  }
}

Execution::ProcessComponent* EffectProcessComponentBase::make(
    const Id<score::Component>& id,
    Execution::ProcessComponentFactory& factory, Process::ProcessModel& effect)
{
  if (process().badChaining())
    return nullptr;
  const Execution::Context& ctx = system();
  const auto& echain = std::dynamic_pointer_cast<ossia::node_chain_process>(m_ossia_process);
  std::vector<Execution::ExecutionCommand> commands;
  commands.reserve(4);

  std::shared_ptr<ProcessComponent> fx
      = factory.make(effect, system(), id, this);
  if (fx)
  {
    SCORE_ASSERT(fx->node);
    QObject::connect(
        &effect.selection, &Selectable::changed, fx.get(),
        [this, n = fx->node](bool ok) {
          in_exec([=] { n->set_logging(ok); });
        });

    const auto idx_ = process().effectPosition(effect.id());
    SCORE_ASSERT(idx_ != -1);
    const std::size_t idx = idx_;
    if (m_fxes.size() < (idx + 1))
    {
      m_fxes.resize(idx + 1);
      m_fxes[idx] = std::make_pair(effect.id(), RegisteredEffect{fx, {}, {}});
    }
    else
    {
      m_fxes.insert(m_fxes.begin() + idx, std::make_pair(effect.id(), RegisteredEffect{fx, {}, {}}));
    }

    commands.push_back([idx, proc=echain,n=fx->node] {
      proc->add_node(idx, n);
    });

    auto& this_fx = m_fxes[idx].second;

    SCORE_ASSERT(this_fx.node()->inputs().size() > 0);
    SCORE_ASSERT(this_fx.node()->outputs().size() > 0);
    this_fx.registeredInlets = effect.inlets();
    this_fx.registeredOutlets = effect.outlets();

    // TODO this could be glitchy : there's no guarantee there won't be another
    // tick between all the submitted commands
    if (idx == 0)
    {
      // unregister and re-register previous first node if any
      if (m_fxes.size() > 1)
      {
        unregister_old_first_node(m_fxes[1], commands);
      }

      // register new first node ; first inlet is process's ; last inlets are
      // node's
      this_fx.registeredInlets[0] = process().inlet.get();

      // connect out to next in
      if (m_fxes.size() > 1)
      {
        // there's an effect after
        reg(this_fx, commands);
        commands.push_back(
            [g = ctx.execGraph, n1 = fx->node, n2 = m_fxes[1].second.node()] {
              move_edges(*n2->inputs()[0], n1->inputs()[0], n1, *g);

              auto edge = ossia::make_edge(
                  ossia::immediate_strict_connection{}, n1->outputs()[0],
                  n2->inputs()[0], n1, n2);
              g->connect(std::move(edge));

            });
      }
      else
      {
        // only effect, goes to end of process
        this_fx.registeredOutlets[0] = process().outlet.get();
        reg(this_fx, commands);

        // set as process node
        auto old = m_ossia_process->node;
        m_ossia_process->node = fx->node;
        nodeChanged(old, m_ossia_process->node, commands);
      }
    }
    else if (idx == (process().effects().size() - 1))
    {
      // unregister previous last node if any
      if (idx >= 1)
      {
        unregister_old_first_node(m_fxes[idx - 1], commands);
      }

      // register new last node
      this_fx.registeredOutlets[0] = process().outlet.get();

      // connect in to prev out
      if (m_fxes.size() > 1)
      {
        reg(this_fx, commands);

        // there's an effect before
        auto& old_last_comp = m_fxes[idx - 1];
        if (old_last_comp.second)
        {
          commands.push_back([g = ctx.execGraph, n1 = old_last_comp.second.node(),
                   n2 = this_fx.node()] {
            move_edges(*n1->outputs()[0], n2->outputs()[0], n2, *g);
            auto edge = ossia::make_edge(
                ossia::immediate_strict_connection{}, n1->outputs()[0],
                n2->inputs()[0], n1, n2);
            g->connect(std::move(edge));
          });
        }
      }
      else
      {
        // only effect
        this_fx.registeredInlets[0] = process().inlet.get();
        reg(this_fx, commands);
      }

      // set as process node
      auto old = m_ossia_process->node;
      commands.push_back(
          [proc = m_ossia_process, node = fx->node] { proc->node = node; });
      nodeChanged(old, fx->node, commands);
    }
    else
    {
      // register node with effect's inlets
      reg(this_fx, commands);

      // unlink before and after and link this one in-between
      auto& prev = m_fxes[idx - 1];
      if (prev.second)
      {
        commands.push_back(
            [g = ctx.execGraph, n1 = prev.second.node(), n2 = this_fx.node()] {
              auto& o_prev = n1->outputs()[0];
              if (!o_prev->targets.empty())
              {
                auto cbl = o_prev->targets[0];
                g->disconnect(cbl);
              }

              auto edge = ossia::make_edge(
                  ossia::immediate_strict_connection{}, n1->outputs()[0],
                  n2->inputs()[0], n1, n2);
              g->connect(std::move(edge));
            });
      }

      if (m_fxes.size() > (idx + 1))
      {
        auto& next = m_fxes[idx + 1];

        if (next.second)
        {
          commands.push_back([g = ctx.execGraph, n2 = this_fx.node(),
                   n3 = next.second.node()] {
            auto& i_next = n3->inputs()[0];
            if (!i_next->sources.empty())
            {
              auto cbl = i_next->sources[0];
              g->disconnect(cbl);
            }
            auto edge = ossia::make_edge(
                ossia::immediate_strict_connection{}, n2->outputs()[0],
                n3->inputs()[0], n2, n3);
            g->connect(std::move(edge));
          });
        }
      }
    }
  }

  if(!commands.empty())
  {
#if !defined(NDEBUG)
    in_exec([f=std::move(commands), g=ctx.execGraph, proc=echain, test_fx=get_nodes(m_fxes)
            ] {
      for(auto& cmd : f)
        cmd();
      check_exec_validity(*g, *proc);
      check_exec_order(test_fx,*proc);
    });
#else
    in_exec([f=std::move(commands),g=ctx.execGraph,proc=echain] {
      for(auto& cmd : f)
        cmd();
    });
#endif
  }
  return fx.get();
}

void EffectProcessComponentBase::added(ProcessComponent& e)
{
}


std::function<void()> EffectProcessComponentBase::removing(
    const Process::ProcessModel& e, ProcessComponent& c)
{
  if (process().badChaining())
    return {};

  const Execution::Context& ctx = system();

  std::vector<Execution::ExecutionCommand> commands;

  auto it = ossia::find_if(
      m_fxes, [&](const auto& v) { return v.first == e.id(); });
  if (it == m_fxes.end())
    return {};
  std::size_t idx = std::distance(m_fxes.begin(), it);
  auto& this_fx = it->second;

  // Remove all the chaining
  auto echain = std::dynamic_pointer_cast<ossia::node_chain_process>(m_ossia_process);

  commands.push_back([g = system().execGraph, n = this_fx.node(), echain] {
    ossia::remove_one(echain->nodes, n);
    if(echain->nodes.empty())
      echain->node.reset();
    n->clear();
    g->remove_node(n);
  });

  unreg(this_fx);

  if (idx == 0)
  {
    if (m_fxes.size() > 1)
    {
      // Link start with next idx
      register_new_first_node(m_fxes[1], commands);
    }
    else if(m_fxes.size() == 1)
    {
      nodeChanged(m_fxes.front().second.node(), {}, commands);
    }
  }
  else if (idx == (m_fxes.size() - 1))
  {
    if (m_fxes.size() > 1)
    {
      register_new_last_node(m_fxes[idx - 1], commands);
    }
    else if(m_fxes.size() == 1)
    {
      nodeChanged(m_fxes.front().second.node(), {}, commands);
    }
  }
  else
  {
    if (m_fxes.size() > (idx + 1))
    {
      auto& prev = m_fxes[idx - 1];
      auto& next = m_fxes[idx + 1];
      if (prev.second && next.second)
      {
        commands.push_back([g = system().execGraph, n1 = prev.second.node(),
                 n2 = next.second.node()] {
          auto edge = ossia::make_edge(
              ossia::immediate_strict_connection{}, n1->outputs()[0],
              n2->inputs()[0], n1, n2);
          g->connect(std::move(edge));
        });
      }
    }
  }

  if(!commands.empty())
  {
#if !defined(NDEBUG)
    auto test_fx=get_nodes(m_fxes);
    ossia::remove_erase(test_fx, this_fx.node());
    in_exec([f=std::move(commands),g=ctx.execGraph,proc=echain, test_fx] {
      for(auto& cmd : f)
        cmd();
      check_exec_validity(*g, *proc);
      check_last_validity(*g, *proc);
      check_exec_order(test_fx,*proc);
    });
#else
    in_exec([f=std::move(commands),g=ctx.execGraph,proc=echain] {
      for(auto& cmd : f)
        cmd();
    });
#endif
  }

  this_fx.comp->node.reset();
  return [=] { m_fxes.erase(it); };
}

void EffectProcessComponentBase::on_orderChanged()
{
  if (process().badChaining())
  {
    // TODO how to handle this ?
    return;
  }

  if(process().effects().empty() || m_fxes.empty())
    return;

  // TODO it would be less expensive to swap things starting from i = 0
  std::vector<std::pair<Id<Process::ProcessModel>, RegisteredEffect>> fxes;
  std::vector<std::shared_ptr<ossia::graph_node>> nodes;

  for(auto& f : process().effects())
  {
    auto it = ossia::find_if(m_fxes, [&] (const auto& pair) { return pair.first == f.id(); });
    SCORE_ASSERT(it != m_fxes.end());

    fxes.push_back(*it);
    if(const auto& comp = it->second.comp; comp->node)
    {
      nodes.push_back(comp->node);
    }
  }
  auto old_first = m_fxes.front();
  bool first_changed = (fxes.front() != m_fxes.front());

  auto old_last = m_fxes.back();
  bool last_changed = (fxes.back() != m_fxes.back());

  m_fxes = std::move(fxes);

  const Execution::Context& ctx = system();

  std::vector<Execution::ExecutionCommand> commands;

  if (first_changed && old_first.second.node())
  {
    unreg(old_first.second);
    old_first.second.registeredInlets = process().effects().at(old_first.first).inlets();
    reg(old_first.second, commands);

    register_new_first_node(m_fxes.front(), commands);
  }

  if (last_changed && old_last.second.node())
  {
    unreg(old_last.second);
    old_last.second.registeredOutlets = process().effects().at(old_last.first).outlets();
    reg(old_last.second, commands);

    register_new_last_node(m_fxes.back(), commands);
  }


  const auto& echain = std::dynamic_pointer_cast<ossia::node_chain_process>(m_ossia_process);
  commands.push_back([g=ctx.execGraph,
                      proc=echain,
                      f=std::move(nodes)] () mutable {
    // 1. Disconnect existing cables
    for(auto it = proc->nodes.begin(); it != proc->nodes.end(); ++it)
    {
      if(it+1 == proc->nodes.end())
        break;
      auto cable = it->get()->outputs()[0]->targets[0];
      g->disconnect(cable);
    }

    // 2. Change node order
    proc->nodes = std::move(f);

    // 3. Connect new cables
    for(auto it = proc->nodes.begin(); it != proc->nodes.end(); ++it)
    {
      if(it+1 == proc->nodes.end())
        break;
      const auto& n1 = *it;
      const auto& n2 = *(it+1);
      auto edge = ossia::make_edge(
          ossia::immediate_strict_connection{},
            n1->outputs()[0], n2->inputs()[0], n1, n2);
      g->connect(std::move(edge));
    }
  });

  if(!commands.empty())
  {
#if !defined(NDEBUG)
    in_exec([f=std::move(commands),g=ctx.execGraph,proc=echain, test_fx=get_nodes(m_fxes)] {
      for(auto& cmd : f)
        cmd();

      check_exec_validity(*g, *proc);
      check_last_validity(*g, *proc);
      check_exec_order(test_fx,*proc);
    });
#else
    in_exec([f=std::move(commands),g=ctx.execGraph,proc=echain] {
      for(auto& cmd : f)
        cmd();
    });
#endif
  }
}

void EffectProcessComponentBase::unreg(
    const EffectProcessComponentBase::RegisteredEffect& fx)
{
  system().setup.unregister_node_soft(
        fx.registeredInlets, fx.registeredOutlets, fx.node());
}

void EffectProcessComponentBase::reg(
    const RegisteredEffect& fx, std::vector<Execution::ExecutionCommand>& vec)
{
  system().setup.register_node(
      fx.registeredInlets, fx.registeredOutlets, fx.node(), vec);
}

void EffectProcessComponent::cleanup()
{
  clear();
  ProcessComponent::cleanup();
}

EffectProcessComponent::~EffectProcessComponent()
{
}
}
