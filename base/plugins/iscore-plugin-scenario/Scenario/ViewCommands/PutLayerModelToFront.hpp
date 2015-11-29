#pragma once
#include <iscore/tools/ModelPath.hpp>

class LayerModel;
class SlotModel;
template <typename tag, typename impl> class id_base_t;

class PutLayerModelToFront
{
    public:
        PutLayerModelToFront(
                Path<SlotModel>&& slotPath,
                const Id<LayerModel>& pid);

        void redo() const;

    private:
        Path<SlotModel> m_slotPath;
        const Id<LayerModel>& m_pid;
};
