/*  Curry Hunter
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "CommonFramework/Globals.h"
#include "CommonFramework/InferenceInfra/InferenceSession.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_Device.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "NintendoSwitch/NintendoSwitch_Settings.h"
#include "PokemonSwSh/PokemonSwSh_Settings.h"
#include "PokemonSwSh/Commands/PokemonSwSh_Commands_GameEntry.h"
#include "PokemonSwSh/ShinyHuntTracker.h"
#include "PokemonSwSh/Inference/PokemonSwSh_ReceivePokemonDetector.h"
//#include "PokemonSwSh/Inference/ShinyDetection/PokemonSwSh_ShinySparkleDetector.h"
#include "PokemonSwSh_CurryHunter.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{


CurryHunter_Descriptor::CurryHunter_Descriptor()
    : RunnableSwitchProgramDescriptor(
        "PokemonSwSh:CurryHunter",
        STRING_POKEMON + " SwSh", "Curry Hunter",
        "ComputerControl/blob/master/Wiki/Programs/PokemonSwSh/CurryHunter.md",
        "Cooks curry to attract " + STRING_POKEMON + " to your camp. This is a beta version. "
        "<font color=\"red\">(This program cannot detect shinies. You must check manually or with " + STRING_POKEMON + " HOME.)</font>",
        FeedbackType::OPTIONAL_, false,
        PABotBaseLevel::PABOTBASE_12KB
    )
{}


struct CurryHunter::Stats : public ShinyHuntTracker{
    Stats()
        : ShinyHuntTracker(true)
        , m_attempts(m_stats["Attempts"])
    {
        for (auto& item : m_display_order){
            item.omit_if_zero = true;
        }
        m_display_order.insert(m_display_order.begin(), Stat("Attempts"));
    }
    std::atomic<uint64_t>& m_attempts;
};
std::unique_ptr<StatsTracker> CurryHunter::make_stats() const{
    return std::unique_ptr<StatsTracker>(new Stats());
}


CurryHunter::CurryHunter(const CurryHunter_Descriptor& descriptor)
    : SingleSwitchProgramInstance(descriptor)
    , WALK_UP_DELAY(
        "<b>Walk up Delay:</b><br>Wait this long for the " + STRING_POKEMON + " to walk up to you.",
        "2 * TICKS_PER_SECOND"
    )
    , TAKE_VIDEO(
        "<b>Take Videos:</b><br>Take a video after each cooking iteration. This will spam your album with videos.",
        false
    )
    , ITERATIONS(
        "<b>Iterations:</b><br># of cooking attempts.",
        999
    )
{
    PA_ADD_OPTION(WALK_UP_DELAY);
    PA_ADD_OPTION(START_IN_GRIP_MENU);
    PA_ADD_OPTION(TAKE_VIDEO);
    PA_ADD_OPTION(ITERATIONS);
}



void CurryHunter::program(SingleSwitchProgramEnvironment& env, BotBaseContext& context){
    if (START_IN_GRIP_MENU){
        grip_menu_connect_go_home(context);
        resume_game_no_interact(context, ConsoleSettings::instance().TOLERATE_SYSTEM_UPDATE_MENU_FAST);
    }else{
        pbf_press_button(context, BUTTON_R, 5, 50);
    }


    Stats& stats = env.stats<Stats>();
    env.update_stats();


    //  Select the cooking option.
    pbf_press_button(context, BUTTON_X, 5, 125);
    pbf_press_dpad(context, DPAD_RIGHT, 5, 100);
    pbf_press_button(context, BUTTON_A, 5, 125);
    pbf_press_button(context, BUTTON_A, 5, 300);


    for (uint32_t it = 0; it < ITERATIONS; it++){
        //  Which ingredient do you want to use?
        pbf_press_button(context, BUTTON_A, 5, 75);     //  Get rid of the pop-up menu.
        pbf_press_button(context, BUTTON_A, 5, 150);    //  75. english text is longer.
        pbf_press_button(context, BUTTON_A, 5, 100);    //  Ingredient is now selected.

        //  Which berries do you want to use?
        pbf_press_button(context, BUTTON_A, 5, 75);     //  Get rid of the pop-up menu.
        pbf_press_button(context, BUTTON_A, 5, 75);
        pbf_press_button(context, BUTTON_A, 5, 125);
        pbf_press_button(context, BUTTON_A, 5, 75);
        pbf_press_button(context, BUTTON_A, 5, 125);
        pbf_press_button(context, BUTTON_A, 5, 75);
        pbf_press_button(context, BUTTON_A, 5, 125);
        pbf_press_button(context, BUTTON_A, 5, 75);
        pbf_press_button(context, BUTTON_A, 5, 125);
        pbf_press_button(context, BUTTON_A, 5, 75);
        pbf_press_button(context, BUTTON_A, 5, 125);
        pbf_press_button(context, BUTTON_A, 5, 75);
        pbf_press_button(context, BUTTON_A, 5, 125);
        pbf_press_button(context, BUTTON_A, 5, 75);
        pbf_press_button(context, BUTTON_A, 5, 125);
        pbf_press_button(context, BUTTON_A, 5, 75);
        pbf_press_button(context, BUTTON_A, 5, 125);
        pbf_press_button(context, BUTTON_A, 5, 75);
        pbf_press_button(context, BUTTON_A, 5, 125);
        pbf_press_button(context, BUTTON_A, 5, 75);
        pbf_press_button(context, BUTTON_A, 5, 125);
        pbf_press_button(context, BUTTON_PLUS, 5, 200); //  125. english text is longer.
        pbf_press_button(context, BUTTON_A, 5, 1000);   //  Berries are now selected as well.


        //  Around 17 seconds of A mashing for the first curry cooking phase.
        for (uint16_t c = 0; c < 2100; c = c + 10){
            pbf_press_button(context, BUTTON_A, 5, 5);
        }

        //  Slowing down to not burn the curry.
        for (uint16_t c = 0; c < 300; c = c + 25){
            pbf_press_button(context, BUTTON_A, 5, 20);
        }
        pbf_wait(context, 170);


        //  Do circles with the joystick. Each circle has ten positions.
        for (uint16_t i = 0; i < 2350; i = i + 50){
            pbf_move_right_joystick(context, 128, 255, 5, 0);
            pbf_move_right_joystick(context, 202, 231, 5, 0);
            pbf_move_right_joystick(context, 249, 167, 5, 0);
            pbf_move_right_joystick(context, 249, 88, 5, 0);
            pbf_move_right_joystick(context, 202, 24, 5, 0);
            pbf_move_right_joystick(context, 128, 0, 5, 0);
            pbf_move_right_joystick(context, 53, 24, 5, 0);
            pbf_move_right_joystick(context, 6, 88, 5, 0);
            pbf_move_right_joystick(context, 6, 167, 5, 0);
            pbf_move_right_joystick(context, 53, 231, 5, 0);
        }


        //  Last step for the curry cooking part.
        pbf_wait(context, 425);
        pbf_press_button(context, BUTTON_A, 5, 2250);

        //  Press A when it shows your curry, and its class.
        pbf_press_button(context, BUTTON_A, 5, 1500);
        pbf_press_button(context, BUTTON_A, 5, 500);
        // You are now back to the camping with your 6 Pokemon, and hopefully a curry Pokemon.

        {
            context.wait_for_all_requests();

            ReceivePokemonDetector receive_detector;
//            ShinySparkleDetector shiny_detector(
//                env.console, env.console,
//                ImageFloatBox(0.1, 0.01, 0.8, 0.77)
//            );
            InferenceSession inference(
                context, env.console,
                {
                    {receive_detector},
//                    {shiny_detector},
                }
            );

            //  Different implementation of the "attract curry Pokemon" routine. DEFAULT
            pbf_move_left_joystick(context, 0x80, 0x00, 40, 5);     //  Move up a bit to avoid talking to your pokemon.
            pbf_press_button(context, BUTTON_A, 5, 5);
            pbf_move_left_joystick(context, 0xff, 0x80, 55, 0);     //  Right
            for (uint16_t i = 0;  i< 2; i++){
                pbf_move_left_joystick(context, 0x00, 0x80, 27, 0); //  Left
                pbf_press_button(context, BUTTON_A, 5, 5);
                pbf_move_left_joystick(context, 0x00, 0x80, 27, 0); //  Left
                pbf_press_button(context, BUTTON_A, 5, 5);
                pbf_move_left_joystick(context, 0x00, 0x80, 27, 0); //  Left
                pbf_press_button(context, BUTTON_A, 5, 5);
                pbf_move_left_joystick(context, 0x00, 0x80, 27, 0); //  Left
                pbf_press_button(context, BUTTON_A, 5, 5);
                pbf_move_left_joystick(context, 0xff, 0x80, 27, 0); //  Right
                pbf_press_button(context, BUTTON_A, 5, 5);
                pbf_move_left_joystick(context, 0xff, 0x80, 27, 0); //  Right
                pbf_press_button(context, BUTTON_A, 5, 5);
                pbf_move_left_joystick(context, 0xff, 0x80, 27, 0); //  Right
                pbf_press_button(context, BUTTON_A, 5, 5);
                pbf_move_left_joystick(context, 0xff, 0x80, 27, 0); //  Right
                pbf_press_button(context, BUTTON_A, 5, 5);
            }


            //  Give the pokemon the time to come to us.
            pbf_wait(context, WALK_UP_DELAY);

            //  Record the encounter.
            if (TAKE_VIDEO){
                pbf_press_button(context, BUTTON_CAPTURE, 250, 500);
            }else{
                pbf_wait(context, 750);
            }


            //  At this point, let's assume we have attracted the curry pokemon.
            //  Now we need to add it to our party while making sure this won't break anything if there is
            //  No curry encounter.
            //  This sequence seems to work because when you whistle at a curry encounter, your camera
            //  Automatically focuses on it, while it doesn't do that for your pokemons.

            //  "a [wild pokemon] came to your camp !"
            pbf_press_button(context, BUTTON_A, 5, 250);

            //  "[wild pokemon] seems to want to come with you!"
            pbf_press_button(context, BUTTON_A, 5, 200);

            //  "do you want to adopt it ?"
            pbf_press_button(context, BUTTON_A, 5, 200);

            //  "you adopted [wild pokemon]!"
            pbf_press_button(context, BUTTON_A, 5, 200);

            //  "[wild pokemon] entered into a poke ball"
            pbf_press_button(context, BUTTON_A, 5, 800);

            //  "[wild pokemon] has been sent to the PC"
            pbf_press_button(context, BUTTON_A, 5, 375);


            context.wait_for_all_requests();
            ShinyType shininess = ShinyType::NOT_SHINY;
            if (inference.triggered_ptr() != nullptr){
//                shininess = shiny_detector.results();
#if 1
                stats.add_non_shiny();
#else
                switch (shininess){
                case ShinyType::UNKNOWN:
                case ShinyType::NOT_SHINY:
                    stats.add_non_shiny();
                    break;
                case ShinyType::UNKNOWN_SHINY:
                    stats.add_unknown_shiny();
                    break;
                case ShinyType::STAR_SHINY:
                    stats.add_star_shiny();
                    break;
                case ShinyType::SQUARE_SHINY:
                    stats.add_square_shiny();
                    break;

                }
#endif
            }

            stats.m_attempts++;
            env.update_stats();
            if (shininess != ShinyType::NOT_SHINY){
                pbf_press_button(context, BUTTON_CAPTURE, 250, 500);
            }
        }


        //  If you talked to the curry Pokemon too early, you can end up talking to one of your Pokemon
        //  Once you reach this part of the program. The following sequence ensures that you
        //  Are in a correct state to cook again.
        pbf_move_left_joystick(context, 0xff, 0x80, 125, 5);
        pbf_move_left_joystick(context, 0x80, 0x00, 125, 5);
        pbf_move_left_joystick(context, 0x80, 0x00, 5, 50);
        pbf_press_button(context, BUTTON_A, 5, 250);    //  Wait 2 seconds to be sure the following X press won't be dropped.

        //  And now we cook another curry.
        pbf_press_button(context, BUTTON_X, 5, 125);
        pbf_press_button(context, BUTTON_A, 5, 125);
        pbf_press_button(context, BUTTON_A, 5, 300);
    }


    //  Not really relevant here, but for programs that finish, go to
    //  Switch home to idle.
    pbf_press_button(context, BUTTON_HOME, 10, GameSettings::instance().GAME_TO_HOME_DELAY_SAFE);
}




}
}
}
