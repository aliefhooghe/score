#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <qbytearray.h>

class DataStreamInput;
class DataStreamOutput;
class StateModel;

namespace Scenario
{
    namespace Command
    {
        class ClearState final : public iscore::SerializableCommand
        {
                ISCORE_COMMAND_DECL(ScenarioCommandFactoryName(), ClearState, "Clear a state")
            public:
                ClearState(Path<StateModel>&& path);
                void undo() const override;
                void redo() const override;

            protected:
                void serializeImpl(DataStreamInput&) const override;
                void deserializeImpl(DataStreamOutput&) override;

            private:
                Path<StateModel> m_path;

                QByteArray m_serializedStates;
        };
    }
}
