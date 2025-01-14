/*  Frozen Screen Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 *  Detect if the entire screen is frozen.
 */

#ifndef PokemonAutomation_CommonFramework_FrozenImageDetector_H
#define PokemonAutomation_CommonFramework_FrozenImageDetector_H

#include <QImage>
#include "Common/Cpp/Color.h"
#include "CommonFramework/ImageTools/ImageBoxes.h"
#include "CommonFramework/InferenceInfra/VisualInferenceCallback.h"

namespace PokemonAutomation{


class FrozenImageDetector : public VisualInferenceCallback{
public:
    FrozenImageDetector(std::chrono::milliseconds timeout, double rmsd_threshold);
    FrozenImageDetector(
        Color color, const ImageFloatBox& box,
        std::chrono::milliseconds timeout, double rmsd_threshold
    );

    virtual void make_overlays(VideoOverlaySet& items) const override;
    virtual bool process_frame(const QImage& frame, WallClock timestamp) override;

private:
    Color m_color;
    ImageFloatBox m_box;
    std::chrono::milliseconds m_timeout;
    double m_rmsd_threshold;
    WallClock m_timestamp;
    QImage m_last_delta;
};



}
#endif
