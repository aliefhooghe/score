#pragma once
#include <Scenario/Document/DisplayedElements/DisplayedElementsToolPalette/DisplayedElementsToolPaletteFactory.hpp>

#include <score/plugins/customfactory/FactoryFamily.hpp>

#include <ossia/detail/algorithms.hpp>

namespace Scenario
{
class DisplayedElementsToolPaletteFactoryList final
    : public score::MatchingFactory<DisplayedElementsToolPaletteFactory>
{
};
}
