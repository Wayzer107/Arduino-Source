/*  Mount Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonLA_MountDetector_H
#define PokemonAutomation_PokemonLA_MountDetector_H

#include <deque>
#include <map>
#include <QString>
#include "Common/Cpp/SpinLock.h"
#include "CommonFramework/Logging/LoggerQt.h"
#include "CommonFramework/ImageTools/ImageBoxes.h"
#include "CommonFramework/InferenceInfra/VisualInferenceCallback.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{


enum class MountState{
    NOTHING,
    WYRDEER_OFF,
    WYRDEER_ON,
    URSALUNA_OFF,
    URSALUNA_ON,
    BASCULEGION_OFF,
    BASCULEGION_ON,
    SNEASLER_OFF,
    SNEASLER_ON,
    BRAVIARY_OFF,
    BRAVIARY_ON,
};
extern const char* MOUNT_STATE_STRINGS[];
extern const std::map<QString, MountState> MOUNT_STATE_MAP;


enum class MountDetectorLogging{
    NONE,
    LOG_ONLY,
    LOG_AND_DUMP_FAILURES,
};


class MountDetector{
public:
    MountDetector(MountDetectorLogging logging = MountDetectorLogging::NONE);

    void make_overlays(VideoOverlaySet& items) const;
    MountState detect(const QImage& screen) const;

private:
    ImageFloatBox m_box;
    MountDetectorLogging m_logging;
};



class MountTracker : public VisualInferenceCallback{
public:
    MountTracker(LoggerQt& logger, MountDetectorLogging logging = MountDetectorLogging::NONE);

    virtual void make_overlays(VideoOverlaySet& items) const override;

    MountState state() const;

    virtual bool process_frame(const QImage& frame, WallClock timestamp) override;

private:
    struct Sample{
        WallClock timestamp;
        MountState state;
    };

private:
    LoggerQt& m_logger;
    std::atomic<MountState> m_state;

    SpinLock m_lock;
    MountDetector m_detector;

    std::deque<Sample> m_history;
    std::map<MountState, size_t> m_counts;
};



}
}
}
#endif
