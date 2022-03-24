/*  Mount Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <sstream>
#include "Common/Cpp/Exceptions.h"
#include "Common/Qt/ImageOpener.h"
#include "Kernels/ImageFilters/Kernels_ImageFilter_Basic.h"
#include "Kernels/Waterfill/Kernels_Waterfill.h"
#include "CommonFramework/Globals.h"
#include "CommonFramework/Tools/ErrorDumper.h"
#include "CommonFramework/ImageTools/ImageFilter.h"
#include "CommonFramework/ImageTools/BinaryImage_FilterRgb32.h"
#include "CommonFramework/ImageMatch/WaterfillTemplateMatcher.h"
#include "CommonFramework/ImageMatch/SubObjectTemplateMatcher.h"
#include "CommonFramework/Tools/VideoOverlaySet.h"
#include "PokemonLA/Inference/Objects/PokemonLA_ButtonDetector.h"
#include "PokemonLA_MountDetector.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{

using namespace Kernels;
using namespace Kernels::Waterfill;

class MountMatcher : public ImageMatch::WaterfillTemplateMatcher{
public:
    using WaterfillTemplateMatcher::WaterfillTemplateMatcher;

#if 0
    virtual double rmsd(const QImage& image, const WaterfillObject& object) const override{
        if (!check_aspect_ratio(object.width(), object.height())){
//            cout << "bad aspect ratio" << endl;
            return 99999.;
        }
        if (!check_area_ratio(object.area_ratio())){
//            cout << "bad area ratio" << endl;
            return 99999.;
        }

//        static int c = 0;
//        cout << c << endl;

        QImage filtered = extract_box(image, object);
        filter_rgb32(object.packed_matrix(), filtered, COLOR_BLACK, true);

        double rmsd = WaterfillTemplateMatcher::rmsd(filtered);

        cout << "rmsd  = " << rmsd << endl;

//        if (rmsd <= m_max_rmsd){
            static int c = 0;
            filtered.save("test-" + QString::number(c++) + "-" + QString::number(rmsd) + ".png");
//        }

        return rmsd;
    }
#endif
};


#if 1

QImage make_MountMatcher2Image(const char* path){
    QString qpath = RESOURCE_PATH() + path;
    QImage image = open_image(qpath);

    if (image.format() != QImage::Format_ARGB32){
        image = image.convertToFormat(QImage::Format_ARGB32);
    }

    PackedBinaryMatrix2 matrix = compress_rgb32_to_binary_range(image, 0xff808080, 0xffffffff);
    auto finder = make_WaterfillIterator(matrix, 50);

    WaterfillObject plus, arrowL, arrowR;

//    image.save("test.png");
//    static int c = 0;


    double rmsd_plus = 99999;
    double rmsd_arrowL = 99999;
    double rmsd_arrowR = 99999;


    WaterfillObject object;
    while (finder->find_next(object)){
        ConstImageRef cropped = extract_box_reference(image, object);
//        cropped.save("test-" + QString::number(c++) + ".png");

        double current_rmsd_plus = ButtonMatcher::Plus().rmsd_precropped(cropped, object);
        if (rmsd_plus > current_rmsd_plus){
            rmsd_plus = current_rmsd_plus;
            plus = object;
        }
        double current_rmsd_arrowL = ButtonMatcher::ArrowLeft().rmsd_precropped(cropped, object);
        if (rmsd_arrowL > current_rmsd_arrowL){
            rmsd_arrowL = current_rmsd_arrowL;
            arrowL = object;
        }
        double current_rmsd_arrowR = ButtonMatcher::ArrowRight().rmsd_precropped(cropped, object);
        if (rmsd_arrowR > current_rmsd_arrowR){
            rmsd_arrowR = current_rmsd_arrowR;
            arrowR = object;
            continue;
        }
    }

//    cout << "plus = " << rmsd_plus << ", arrowL = " << rmsd_arrowL << ", arrowR = " << rmsd_arrowR << endl;

    if (rmsd_plus > 80){
        throw FileException(nullptr, PA_CURRENT_FUNCTION, "Unable to find (+) button in resource. rmsd = " + std::to_string(rmsd_plus), path);
    }
    if (rmsd_arrowL > 180){
        throw FileException(nullptr, PA_CURRENT_FUNCTION, "Unable to find (<) button in resource. rmsd = " + std::to_string(rmsd_arrowL), path);
    }
    if (rmsd_arrowR > 180){
        throw FileException(nullptr, PA_CURRENT_FUNCTION, "Unable to find (>) button in resource. rmsd = " + std::to_string(rmsd_arrowR), path);
    }

    plus.merge_assume_no_overlap(arrowL);
    plus.merge_assume_no_overlap(arrowR);

    return extract_box(image, plus);
}

class MountMatcherButtons : public ImageMatch::ExactImageMatcher{
public:
    MountMatcherButtons(const char* path)
        : ExactImageMatcher(make_MountMatcher2Image(path))
    {}

};
#endif





class MountWyrdeerMatcher : public MountMatcher{
public:
    MountWyrdeerMatcher(bool on)
        : MountMatcher(
            on
                ? "PokemonLA/Mounts/MountOn-Wyrdeer-Template-1.png"
                : "PokemonLA/Mounts/MountOff-Wyrdeer-Template.png",
            Color(0xff808000), Color(0xffffffff), 100
        )
    {}
    static const MountWyrdeerMatcher& on(){
        static MountWyrdeerMatcher matcher(true);
        return matcher;
    }
    static const MountWyrdeerMatcher& off(){
        static MountWyrdeerMatcher matcher(false);
        return matcher;
    }
};
class MountUrsalunaMatcher : public MountMatcher{
public:
    MountUrsalunaMatcher(bool on)
        : MountMatcher(
            on
                ? "PokemonLA/Mounts/MountOn-Ursaluna-Template-1.png"
                : "PokemonLA/Mounts/MountOff-Ursaluna-Template.png",
            Color(0xff808000), Color(0xffffffff), 100
        )
    {}
    static const MountUrsalunaMatcher& on(){
        static MountUrsalunaMatcher matcher(true);
        return matcher;
    }
    static const MountUrsalunaMatcher& off(){
        static MountUrsalunaMatcher matcher(false);
        return matcher;
    }
};
class MountBasculegionMatcher : public MountMatcher{
public:
    MountBasculegionMatcher(bool on)
        : MountMatcher(
            on
                ? "PokemonLA/Mounts/MountOn-Basculegion-Template-1.png"
                : "PokemonLA/Mounts/MountOff-Basculegion-Template.png",
            Color(0xff808000), Color(0xffffffff), 100
        )
    {
        m_area_ratio_upper = 1.50;
    }
    static const MountBasculegionMatcher& on(){
        static MountBasculegionMatcher matcher(true);
        return matcher;
    }
    static const MountBasculegionMatcher& off(){
        static MountBasculegionMatcher matcher(false);
        return matcher;
    }
};
class MountSneaslerMatcher : public MountMatcher{
public:
    MountSneaslerMatcher(bool on)
        : MountMatcher(
            on
                ? "PokemonLA/Mounts/MountOn-Sneasler-Template-1.png"
                : "PokemonLA/Mounts/MountOff-Sneasler-Template.png",
            Color(0xff808000), Color(0xffffffff), 100
        )
    {}
    static const MountSneaslerMatcher& on(){
        static MountSneaslerMatcher matcher(true);
        return matcher;
    }
    static const MountSneaslerMatcher& off(){
        static MountSneaslerMatcher matcher(false);
        return matcher;
    }
};
class MountBraviaryMatcher : public MountMatcher{
public:
    MountBraviaryMatcher(bool on)
        : MountMatcher(
            on
                ? "PokemonLA/Mounts/MountOn-Braviary-Template-1.png"
                : "PokemonLA/Mounts/MountOff-Braviary-Template.png",
            Color(0xff808000), Color(0xffffffff), 100
        )
    {
        m_area_ratio_upper = 1.65;
    }
    static const MountBraviaryMatcher& on(){
        static MountBraviaryMatcher matcher(true);
        return matcher;
    }
    static const MountBraviaryMatcher& off(){
        static MountBraviaryMatcher matcher(false);
        return matcher;
    }
};





class MountWyrdeerMatcherButtons : public MountMatcherButtons{
public:
    MountWyrdeerMatcherButtons(bool on)
        : MountMatcherButtons(on
            ? "PokemonLA/Mounts/MountOn-Wyrdeer-Template.png"
            : "PokemonLA/Mounts/MountOff-Wyrdeer-Template.png"
        )
    {}
    static const MountWyrdeerMatcherButtons& on(){
        static MountWyrdeerMatcherButtons matcher(true);
        return matcher;
    }
    static const MountWyrdeerMatcherButtons& off(){
        static MountWyrdeerMatcherButtons matcher(false);
        return matcher;
    }
};
class MountUrsalunaMatcherButtons : public MountMatcherButtons{
public:
    MountUrsalunaMatcherButtons(bool on)
        : MountMatcherButtons(on
            ? "PokemonLA/Mounts/MountOn-Ursaluna-Template.png"
            : "PokemonLA/Mounts/MountOff-Ursaluna-Template.png"
        )
    {}
    static const MountUrsalunaMatcherButtons& on(){
        static MountUrsalunaMatcherButtons matcher(true);
        return matcher;
    }
    static const MountUrsalunaMatcherButtons& off(){
        static MountUrsalunaMatcherButtons matcher(false);
        return matcher;
    }
};
class MountBasculegionMatcherButtons : public MountMatcherButtons{
public:
    MountBasculegionMatcherButtons(bool on)
        : MountMatcherButtons(on
            ? "PokemonLA/Mounts/MountOn-Basculegion-Template.png"
            : "PokemonLA/Mounts/MountOff-Basculegion-Template.png"
        )
    {}
    static const MountBasculegionMatcherButtons& on(){
        static MountBasculegionMatcherButtons matcher(true);
        return matcher;
    }
//    static const MountBasculegionMatcherButtons& off(){
//        static MountBasculegionMatcherButtons matcher(false);
//        return matcher;
//    }
};
class MountSneaslerMatcherButtons : public MountMatcherButtons{
public:
    MountSneaslerMatcherButtons(bool on)
        : MountMatcherButtons(on
            ? "PokemonLA/Mounts/MountOn-Sneasler-Template.png"
            : "PokemonLA/Mounts/MountOff-Sneasler-Template.png"
        )
    {}
    static const MountSneaslerMatcherButtons& on(){
        static MountSneaslerMatcherButtons matcher(true);
        return matcher;
    }
    static const MountSneaslerMatcherButtons& off(){
        static MountSneaslerMatcherButtons matcher(false);
        return matcher;
    }
};
class MountBraviaryMatcherButtons : public MountMatcherButtons{
public:
    MountBraviaryMatcherButtons(bool on)
        : MountMatcherButtons(on
            ? "PokemonLA/Mounts/MountOn-Braviary-Template.png"
            : "PokemonLA/Mounts/MountOff-Braviary-Template.png"
        )
    {}
    static const MountBraviaryMatcherButtons& on(){
        static MountBraviaryMatcherButtons matcher(true);
        return matcher;
    }
    static const MountBraviaryMatcherButtons& off(){
        static MountBraviaryMatcherButtons matcher(false);
        return matcher;
    }
};




const char* MOUNT_STATE_STRINGS[] = {
    "No Detection",
    "Wrydeer Off",
    "Wrydeer On",
    "Ursaluna Off",
    "Ursaluna On",
    "Basculegion Off",
    "Basculegion On",
    "Sneasler Off",
    "Sneasler On",
    "Braviary Off",
    "Braviary On",
};



MountDetector::MountDetector(MountDetectorLogging logging)
    : m_box(0.905, 0.65, 0.08, 0.13)
    , m_logging(logging)
{}

struct MountCandiateTracker{
    double m_rmsd = 1000;
    MountState m_state = MountState::NOTHING;

    void add_filtered(double rmsd, MountState state){
        if (rmsd > 150 || m_rmsd <= rmsd){
            return;
        }
        m_rmsd = rmsd;
        m_state = state;
    }
    void add_direct(double rmsd, MountState state){
        if (rmsd > 80 || m_rmsd <= rmsd){
            return;
        }
        m_rmsd = rmsd;
        m_state = state;
    }
    void add_button_crop(double rmsd, MountState state){
        if (rmsd > 80 || m_rmsd <= rmsd){
            return;
        }
        m_rmsd = rmsd;
        m_state = state;
    }
};

void MountDetector::make_overlays(VideoOverlaySet& items) const{
    items.add(COLOR_RED, m_box);
}


struct MountDetectorFilteredImage{
    QImage image;
    PackedBinaryMatrix2 matrix;
};

std::vector<MountDetectorFilteredImage> run_filters(const ConstImageRef& image, const std::vector<std::pair<uint32_t, uint32_t>>& range){
    std::vector<MountDetectorFilteredImage> ret(range.size());
    size_t c = 0;
    for (; c + 3 < range.size(); c += 4){
        filter4_rgb32_range(
            image,
            ret[c + 0].image, range[c + 0].first, range[c + 0].second, COLOR_BLACK, false,
            ret[c + 1].image, range[c + 1].first, range[c + 1].second, COLOR_BLACK, false,
            ret[c + 2].image, range[c + 2].first, range[c + 2].second, COLOR_BLACK, false,
            ret[c + 3].image, range[c + 3].first, range[c + 3].second, COLOR_BLACK, false
        );
        compress4_rgb32_to_binary_range(
            image,
            ret[c + 0].matrix, range[c + 0].first, range[c + 0].second,
            ret[c + 1].matrix, range[c + 1].first, range[c + 1].second,
            ret[c + 2].matrix, range[c + 2].first, range[c + 2].second,
            ret[c + 3].matrix, range[c + 3].first, range[c + 3].second
        );
    }
    for (; c + 1 < range.size(); c += 2){
        filter2_rgb32_range(
            image,
            ret[c + 0].image, range[c + 0].first, range[c + 0].second, COLOR_BLACK, false,
            ret[c + 1].image, range[c + 1].first, range[c + 1].second, COLOR_BLACK, false
        );
        compress2_rgb32_to_binary_range(
            image,
            ret[c + 0].matrix, range[c + 0].first, range[c + 0].second,
            ret[c + 1].matrix, range[c + 1].first, range[c + 1].second
        );
    }
    if (c < range.size()){
        filter1_rgb32_range(
            image,
            ret[c].image, range[c].first, range[c].second, COLOR_BLACK, false
        );
        ret[c].matrix = compress_rgb32_to_binary_range(
            image, range[c].first, range[c].second
        );
    }
    return ret;
}


MountState MountDetector::detect(const QImage& screen) const{
    ConstImageRef image = extract_box_reference(screen, m_box);

    MountCandiateTracker candidates;
    WaterfillObject plus, arrowL, arrowR;

    //  Run direct-waterfill to detect all the off-mounts.
    double rmsd_plus = 99999;
    double rmsd_arrowL = 99999;
    double rmsd_arrowR = 99999;
    {
        std::vector<MountDetectorFilteredImage> filtered_images = run_filters(
            image,
            {
                {0xff808060, 0xffffffff},
                {0xff909070, 0xffffffff},
                {0xffa0a080, 0xffffffff},
                {0xffb0b090, 0xffffffff},
                {0xffc0c0a0, 0xffffffff},
                {0xffd0d0b0, 0xffffffff},
                {0xffe0e0c0, 0xffffffff},
                {0xfff0f0d0, 0xffffffff},
            }
        );
//        static int c = 0;
        for (MountDetectorFilteredImage& filtered : filtered_images){
//            cout << filtered.matrix.dump() << endl;
            auto finder = make_WaterfillIterator(filtered.matrix, 50);
            WaterfillObject object;
//            int c = 0;
            while (finder->find_next(object)){
//                c++;
                //  Skip anything that touches the borders.
                if (object.min_x == 0 || object.min_y == 0 ||
                    object.max_x - 1 == (size_t)image.width() ||
                    object.max_y - 1 == (size_t)image.height()
                ){
                    continue;
                }

                ConstImageRef cropped = extract_box_reference(image, object);

//                cout << "object = " << c << endl;
//                cropped.save("object-" + QString::number(c) + ".png");


                //  Read the buttons.
                double current_rmsd_plus = ButtonMatcher::Plus().rmsd_precropped(cropped, object);
                if (rmsd_plus > current_rmsd_plus){
                    rmsd_plus = current_rmsd_plus;
                    plus = object;
                }
//                cout << "Arrow (left)" << endl;
                double current_rmsd_arrowL = ButtonMatcher::ArrowLeft().rmsd_precropped(cropped, object);
                if (rmsd_arrowL > current_rmsd_arrowL){
                    rmsd_arrowL = current_rmsd_arrowL;
                    arrowL = object;
                }
//                cout << "Arrow (right)" << endl;
                double current_rmsd_arrowR = ButtonMatcher::ArrowRight().rmsd_precropped(cropped, object);
//                cout << "rmsd_arrowR = " << rmsd_arrowR << endl;
                if (rmsd_arrowR > current_rmsd_arrowR){
                    rmsd_arrowR = current_rmsd_arrowR;
                    arrowR = object;
                }

                //  Skip bad geometry.
                if (object.width() * 2 < image.width()){
                    continue;
                }
                if (object.height() * 3 < image.height()){
                    continue;
                }

                ConstImageRef filtered_cropped = extract_box_reference(filtered.image, object);
#if 1
                candidates.add_filtered(MountWyrdeerMatcher      ::off().rmsd_precropped(filtered_cropped, object), MountState::WYRDEER_OFF);
                candidates.add_direct  (MountWyrdeerMatcher      ::off().rmsd_precropped(cropped         , object), MountState::WYRDEER_OFF);
                candidates.add_filtered(MountUrsalunaMatcher     ::off().rmsd_precropped(filtered_cropped, object), MountState::URSALUNA_OFF);
                candidates.add_direct  (MountUrsalunaMatcher     ::off().rmsd_precropped(cropped         , object), MountState::URSALUNA_OFF);
                candidates.add_filtered(MountBasculegionMatcher  ::off().rmsd_precropped(filtered_cropped, object), MountState::BASCULEGION_OFF);
                candidates.add_direct  (MountBasculegionMatcher  ::off().rmsd_precropped(cropped         , object), MountState::BASCULEGION_OFF);
                candidates.add_filtered(MountSneaslerMatcher     ::off().rmsd_precropped(filtered_cropped, object), MountState::SNEASLER_OFF);
                candidates.add_direct  (MountSneaslerMatcher     ::off().rmsd_precropped(cropped         , object), MountState::SNEASLER_OFF);
                candidates.add_filtered(MountBraviaryMatcher     ::off().rmsd_precropped(filtered_cropped, object), MountState::BRAVIARY_OFF);
                candidates.add_direct  (MountBraviaryMatcher     ::off().rmsd_precropped(cropped         , object), MountState::BRAVIARY_OFF);
#endif

            }
        }
    }

//    cout << "plus   = " << plus.area << endl;
//    cout << "arrowL = " << arrowL.area << endl;
//    cout << "arrowR = " << arrowR.area << endl;

    //  No buttons detected means mounts aren't available or we're on Basculegion.
    bool buttons_detected = rmsd_plus < 120 && rmsd_arrowL < 180 && rmsd_arrowR < 180;
    if (!buttons_detected){
//        cout << "plus = " << rmsd_plus << ", arrowL = " << rmsd_arrowL << ", arrowR = " << rmsd_arrowR << endl;
    }

    //  If we detected buttons, run the detections that align to the buttons.
    if (buttons_detected){
        WaterfillObject arrows = std::move(plus);
        arrows.merge_assume_no_overlap(arrowL);
        arrows.merge_assume_no_overlap(arrowR);

        ConstImageRef cropped = extract_box_reference(image, arrows);

//        cout << "Start mounts" << endl;

#if 1
        candidates.add_button_crop(MountWyrdeerMatcherButtons    ::off().rmsd(cropped), MountState::WYRDEER_OFF);
//        candidates.add_button_crop(MountBasculegionMatcherButtons::off().rmsd(cropped), MountState::URSALUNA_OFF);
        candidates.add_button_crop(MountUrsalunaMatcherButtons   ::off().rmsd(cropped), MountState::BASCULEGION_OFF);
        candidates.add_button_crop(MountSneaslerMatcherButtons   ::off().rmsd(cropped), MountState::SNEASLER_OFF);
        candidates.add_button_crop(MountBraviaryMatcherButtons   ::off().rmsd(cropped), MountState::BRAVIARY_OFF);
        candidates.add_button_crop(MountWyrdeerMatcherButtons    ::on().rmsd(cropped), MountState::WYRDEER_ON);
        candidates.add_button_crop(MountBasculegionMatcherButtons::on().rmsd(cropped), MountState::URSALUNA_ON);
        candidates.add_button_crop(MountUrsalunaMatcherButtons   ::on().rmsd(cropped), MountState::BASCULEGION_ON);
        candidates.add_button_crop(MountSneaslerMatcherButtons   ::on().rmsd(cropped), MountState::SNEASLER_ON);
        candidates.add_button_crop(MountBraviaryMatcherButtons   ::on().rmsd(cropped), MountState::BRAVIARY_ON);
#endif
    }

#if 1
    //  Now run all the direct-waterfill to detect all the yellow mounts.
    {
        std::vector<MountDetectorFilteredImage> filtered_images = run_filters(
            image,
            {
                {0xff606000, 0xffffff7f},
                {0xff808000, 0xffffff6f},
                {0xffa0a000, 0xffffff5f},
                {0xffc0c000, 0xffffff4f},
//                {0xff606000, 0xffffffff},
//                {0xff808000, 0xffffffff},
//                {0xffa0a000, 0xffffffff},
//                {0xffc0c000, 0xffffffff},
            }
        );
//        int i = 0;
        for (MountDetectorFilteredImage& filtered : filtered_images){
            auto finder = make_WaterfillIterator(filtered.matrix, 50);
            WaterfillObject object;
            while (finder->find_next(object)){
                //  Skip anything that touches the borders.
                if (object.min_x == 0 || object.min_y == 0 ||
                    object.max_x - 1 == (size_t)image.width() ||
                    object.max_y - 1 == (size_t)image.height()
                ){
                    continue;
                }
                if (object.width() * 2 < (size_t)image.width()){
                    continue;
                }
                if (object.height() * 3 < (size_t)image.height()){
                    continue;
                }

                ConstImageRef cropped = extract_box_reference(image, object);
                ConstImageRef filtered_cropped = extract_box_reference(filtered.image, object);
#if 1
                candidates.add_filtered(MountWyrdeerMatcher      ::on().rmsd(filtered_cropped), MountState::WYRDEER_ON);
                candidates.add_direct  (MountWyrdeerMatcher      ::on().rmsd(cropped         ), MountState::WYRDEER_ON);
                candidates.add_filtered(MountUrsalunaMatcher     ::on().rmsd(filtered_cropped), MountState::URSALUNA_ON);
                candidates.add_direct  (MountUrsalunaMatcher     ::on().rmsd(cropped         ), MountState::URSALUNA_ON);
                candidates.add_filtered(MountBasculegionMatcher  ::on().rmsd(filtered_cropped), MountState::BASCULEGION_ON);
                candidates.add_direct  (MountBasculegionMatcher  ::on().rmsd(cropped         ), MountState::BASCULEGION_ON);
                candidates.add_filtered(MountSneaslerMatcher     ::on().rmsd(filtered_cropped), MountState::SNEASLER_ON);
                candidates.add_direct  (MountSneaslerMatcher     ::on().rmsd(cropped         ), MountState::SNEASLER_ON);
                candidates.add_filtered(MountBraviaryMatcher     ::on().rmsd(filtered_cropped), MountState::BRAVIARY_ON);
                candidates.add_direct  (MountBraviaryMatcher     ::on().rmsd(cropped         ), MountState::BRAVIARY_ON);
#endif
//                extract_box(image, object).save("test-" + QString::number(i) + ".png");
//                i++;
            }
        }
//        cout << "i = " << i << endl;
    }
#endif

//    cout << "Button: rmsd = " << candidates.m_rmsd << ", state = " << (int)candidates.m_state << endl;
//    if (candidates.m_state == MountState::BASCULEGION_ON){
//        screen.save("basculegion-detection.png");
//    }
    if (m_logging != MountDetectorLogging::NONE){
        std::cout << "MountDetector: " << MOUNT_STATE_STRINGS[(size_t)candidates.m_state]
                  << ", rmsd = " << candidates.m_rmsd << (buttons_detected ? " (button)" : " (no button)") << std::endl;
        if (m_logging == MountDetectorLogging::LOG_AND_DUMP_FAILURES && candidates.m_state == MountState::NOTHING){
            dump_image(global_logger_tagged(), ProgramInfo(), "MountDetection", screen);
        }
    }
    return candidates.m_state;
}





MountTracker::MountTracker(LoggerQt& logger, MountDetectorLogging logging)
    : VisualInferenceCallback("MountTracker")
    , m_logger(logger)
    , m_state(MountState::NOTHING)
    , m_detector(logging)
{}

void MountTracker::make_overlays(VideoOverlaySet& items) const{
    m_detector.make_overlays(items);
}

MountState MountTracker::state() const{
    return m_state.load(std::memory_order_acquire);
}

bool MountTracker::process_frame(
    const QImage& frame,
    std::chrono::system_clock::time_point timestamp
){
    MountState state = m_detector.detect(frame);
//    cout << "state = " << (int)state << endl;

//    SpinLockGuard lg(m_lock);

    //  Clear out old history.
    std::chrono::system_clock::time_point threshold = timestamp - std::chrono::seconds(1);
    while (!m_history.empty()){
        Sample& sample = m_history.front();
        if (m_history.front().timestamp >= threshold){
            break;
        }
        m_counts[sample.state]--;
        m_history.pop_front();
    }

    size_t& count = m_counts[state];
    m_history.emplace_back(Sample{timestamp, state});
    count++;

    //  Return most reported state in the last window.
    MountState best_state = MountState::NOTHING;
    size_t best_count = 0;
    for (const auto& item : m_counts){
        if (best_count < item.second){
            best_count = item.second;
            best_state = item.first;
        }
    }

    MountState last_state = this->state();
    if (last_state != best_state){
        m_logger.log(
            std::string("Mount changed from ") + MOUNT_STATE_STRINGS[(int)last_state] +
            " to " + MOUNT_STATE_STRINGS[(int)best_state] + ".",
            COLOR_PURPLE
        );
    }
    m_state.store(best_state, std::memory_order_release);

    return false;
}




void make_mount_template(){
    QImage image("MountOn-Braviary-Original.png");
    image = image.convertToFormat(QImage::Format_ARGB32);

    int width = image.width();
    int height = image.height();
    int plus_min_x = width - 29;
    int plus_max_x = width - 10;
    int plus_min_y = height - 23;
    int plus_max_y = height - 4;
    for (int r = 0; r < height; r++){
        for (int c = 0; c < width; c++){
            if (plus_min_x < c && c < plus_max_x && plus_min_y < r && r < plus_max_y){
                continue;
            }
            QRgb pixel = image.pixel(c, r);
            if (qRed(pixel) < 128 || qGreen(pixel) < 128){
                image.setPixel(c, r, 0);
            }
        }
    }

    image.save("MountOn-Braviary-Template.png");
}







}
}
}
