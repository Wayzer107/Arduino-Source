/*  Trade Routines
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonLA_TradeRoutines_H
#define PokemonAutomation_PokemonLA_TradeRoutines_H

#include "CommonFramework/Language.h"
#include "CommonFramework/VideoPipeline/VideoOverlay.h"
#include "CommonFramework/Tools/StatsTracking.h"
#include "CommonFramework/Tools/ConsoleHandle.h"
#include "CommonFramework/Tools/MultiConsoleErrors.h"

namespace PokemonAutomation{
    class BotBaseContext;
    class ProgramEnvironment;
namespace NintendoSwitch{
namespace PokemonLA{


struct TradeStats : public StatsTracker{
    TradeStats();
    std::atomic<uint64_t>& m_trades;
    std::atomic<uint64_t>& m_errors;
};


void trade_current_pokemon(
    ConsoleHandle& console, BotBaseContext& context,
    MultiConsoleErrorState& tracker,
    TradeStats& stats
);



class TradeNameReader{
public:
    TradeNameReader(LoggerQt& logger, VideoOverlay& overlay, Language language);

    std::string read(const QImage& screen) const;

private:
    LoggerQt& m_logger;
    Language m_language;
    InferenceBoxScope m_box;
    std::set<std::string> m_slugs;
};




}
}
}
#endif
