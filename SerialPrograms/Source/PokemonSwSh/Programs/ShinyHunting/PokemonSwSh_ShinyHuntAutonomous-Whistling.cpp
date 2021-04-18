/*  Shiny Hunt Autonomous - Whistling
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Clientside/PrettyPrint.h"
#include "Common/SwitchFramework/FrameworkSettings.h"
#include "Common/SwitchFramework/Switch_PushButtons.h"
#include "Common/PokemonSwSh/PokemonSettings.h"
#include "Common/PokemonSwSh/PokemonSwShGameEntry.h"
#include "Common/PokemonSwSh/PokemonSwShDateSpam.h"
#include "CommonFramework/PersistentSettings.h"
#include "PokemonSwSh/Inference/PokemonSwSh_StartBattleDetector.h"
#include "PokemonSwSh/Inference/PokemonSwSh_BattleMenuDetector.h"
#include "PokemonSwSh/Inference/ShinyDetection/PokemonSwSh_ShinyEncounterDetector.h"
#include "PokemonSwSh_EncounterTracker.h"
#include "PokemonSwSh_ShinyHuntAutonomous-Whistling.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{


ShinyHuntAutonomousWhistling::ShinyHuntAutonomousWhistling()
    : SingleSwitchProgram(
        FeedbackType::REQUIRED, PABotBaseLevel::PABOTBASE_12KB,
        "Shiny Hunt Autonomous - Whistling",
        "SerialPrograms/ShinyHuntAutonomous-Whistling.md",
        "Stand in one place and whistle. Shiny hunt everything that attacks you using video feedback."
    )
    , GO_HOME_WHEN_DONE(
        "<b>Go Home when Done:</b><br>After finding a shiny, go to the Switch Home menu to idle. (turn this off for unattended streaming)",
        false
    )
    , TIME_ROLLBACK_HOURS(
        "<b>Time Rollback (in hours):</b><br>Periodically roll back the time to keep the weather the same. If set to zero, this feature is disabled.",
        1, 0, 11
    )
    , m_advanced_options(
        "<font size=4><b>Advanced Options:</b> You should not need to touch anything below here.</font>"
    )
    , EXIT_BATTLE_MASH_TIME(
        "<b>Exit Battle Time:</b><br>After running, wait this long to return to overworld.",
        "6 * TICKS_PER_SECOND"
    )
    , VIDEO_ON_SHINY(
        "<b>Video Capture:</b><br>Take a video of the encounter if it is shiny.",
        true
    )
    , RUN_FROM_EVERYTHING(
        "<b>Run from Everything:</b><br>Run from everything - even if it is shiny. (For testing only.)",
        false
    )
{
    m_options.emplace_back(&GO_HOME_WHEN_DONE, "GO_HOME_WHEN_DONE");
    m_options.emplace_back(&TIME_ROLLBACK_HOURS, "TIME_ROLLBACK_HOURS");
    m_options.emplace_back(&m_advanced_options, "");
    m_options.emplace_back(&EXIT_BATTLE_MASH_TIME, "EXIT_BATTLE_MASH_TIME");
    if (settings.developer_mode){
        m_options.emplace_back(&VIDEO_ON_SHINY, "VIDEO_ON_SHINY");
        m_options.emplace_back(&RUN_FROM_EVERYTHING, "RUN_FROM_EVERYTHING");
    }
}



struct ShinyHuntAutonomousWhistling::Stats : public ShinyHuntTracker{
    Stats()
        : ShinyHuntTracker(true)
        , m_timeouts(m_stats["Timeouts"])
        , m_unexpected_battles(m_stats["Unexpected Battles"])
    {
        m_display_order.insert(m_display_order.begin() + 1, Stat("Timeouts"));
        m_display_order.insert(m_display_order.begin() + 2, Stat("Unexpected Battles"));
    }
    uint64_t& m_timeouts;
    uint64_t& m_unexpected_battles;
};
std::unique_ptr<StatsTracker> ShinyHuntAutonomousWhistling::make_stats() const{
    return std::unique_ptr<StatsTracker>(new Stats());
}





void ShinyHuntAutonomousWhistling::program(SingleSwitchProgramEnvironment& env) const{
    grip_menu_connect_go_home();
    resume_game_back_out(TOLERATE_SYSTEM_UPDATE_MENU_FAST, 200);

    const uint32_t PERIOD = (uint32_t)TIME_ROLLBACK_HOURS * 3600 * TICKS_PER_SECOND;
    uint32_t last_touch = system_clock();

    Stats& stats = env.stats<Stats>();
    StandardEncounterTracker tracker(
        stats, env.console,
        false,
        EXIT_BATTLE_MASH_TIME,
        VIDEO_ON_SHINY,
        RUN_FROM_EVERYTHING
    );

    while (true){
        env.update_stats();

        //  Touch the date.
        if (TIME_ROLLBACK_HOURS > 0 && system_clock() - last_touch >= PERIOD){
            pbf_press_button(BUTTON_HOME, 10, GAME_TO_HOME_DELAY_SAFE);
            rollback_hours_from_home(TIME_ROLLBACK_HOURS, SETTINGS_TO_HOME_DELAY);
            resume_game_no_interact(TOLERATE_SYSTEM_UPDATE_MENU_FAST);
            last_touch += PERIOD;
        }

        env.console.botbase().wait_for_all_requests();
        {
            StandardBattleMenuDetector menu(env.console);
            StartBattleDetector detector(env.console, std::chrono::seconds(0));

            //  Detect start of battle.
            bool unexpected = false;
            QImage screen;
            do{
                screen = env.console.video().snapshot();
                if (menu.detect(screen)){
                    env.log("ScreenChangeDetector: Unexpected battle menu.", Qt::red);
                    stats.m_unexpected_battles++;
                    unexpected = true;
                    break;
                }
                pbf_mash_button(BUTTON_LCLICK, 10);
                pbf_move_right_joystick(192, 128, 10, 0);
                env.console.botbase().wait_for_all_requests();
            }while (!detector.detect(screen));

            if (unexpected){
                pbf_mash_button(BUTTON_B, TICKS_PER_SECOND);
                tracker.run_away();
                continue;
            }
            pbf_mash_button(BUTTON_B, 5 * TICKS_PER_SECOND);
        }

        //  Detect shiny.
        ShinyDetection detection = detect_shiny_battle(
            env, env.console,
            SHINY_BATTLE_REGULAR,
            std::chrono::seconds(30)
        );

        if (tracker.process_result(detection)){
            break;
        }
        if (detection == ShinyDetection::NO_BATTLE_MENU){
            stats.m_timeouts++;
            pbf_mash_button(BUTTON_B, TICKS_PER_SECOND);
            tracker.run_away();
        }
    }

    env.update_stats();

    if (GO_HOME_WHEN_DONE){
        pbf_press_button(BUTTON_HOME, 10, GAME_TO_HOME_DELAY_SAFE);
    }

    end_program_callback();
    end_program_loop();
}



}
}
}

