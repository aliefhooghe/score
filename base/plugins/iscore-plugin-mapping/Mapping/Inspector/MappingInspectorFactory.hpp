#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>
#include <Mapping/Inspector/MappingInspectorWidget.hpp>
#include <Mapping/MappingModel.hpp>

class MappingInspectorFactory final : public ProcessInspectorWidgetDelegateFactory
{
    public:
        MappingInspectorFactory() = default;

    private:
        ProcessInspectorWidgetDelegate* make(
                const Process& process,
                const iscore::DocumentContext& doc,
                QWidget* parent) const override
        {
            return new MappingInspectorWidget{
                static_cast<const MappingModel&>(process), doc, parent};

        }

        bool matches(const Process& process) const override
        {
            return dynamic_cast<const MappingModel*>(&process);
        }

};
