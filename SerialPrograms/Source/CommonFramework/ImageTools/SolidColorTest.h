/*  Solid Color Test
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_CommonFramework_SolidColorTest_H
#define PokemonAutomation_CommonFramework_SolidColorTest_H

#include "CommonFramework/ImageTypes/ImageReference.h"
#include "ImageStats.h"

//#include <iostream>
//using std::cout;
//using std::endl;

namespace PokemonAutomation{


bool is_white(
    const ImageStats& stats,
    double min_rgb_sum = 500,
    double max_stddev_sum = 10
);
bool is_black(
    const ImageStats& stats,
    double max_rgb_sum = 100,
    double max_stddev_sum = 10
);
bool is_grey(
    const ImageStats& stats,
    double min_rgb_sum, double max_rgb_sum,
    double max_stddev_sum = 10
);

// expected_color_ratio: ratio of color channels to match, e.g. if a color is (127, 127, 254), it's color ratio is (0.25, 0.25, 0.5)
bool is_solid(
    const ImageStats& stats,
    const FloatPixel& expected_color_ratio,
    double max_euclidean_distance = 0.15,
    double max_stddev_sum = 10
);


inline bool is_white(
    const ConstImageRef& image,
    double min_rgb_sum = 500,
    double max_stddev_sum = 10
){
    return is_white(image_stats(image), min_rgb_sum, max_stddev_sum);
}
inline bool is_black(
    const ConstImageRef& image,
    double max_rgb_sum = 100,
    double max_stddev_sum = 10
){
    ImageStats stats = image_stats(image);
//    cout << stats.average << stats.stddev << endl;
    return is_black(stats, max_rgb_sum, max_stddev_sum);
}
inline bool is_grey(
    const ConstImageRef& image,
    double min_rgb_sum, double max_rgb_sum,
    double max_stddev_sum = 10
){
    return is_grey(image_stats(image), min_rgb_sum, max_rgb_sum, max_stddev_sum);
}

// expected_color_ratio: ratio of color channels to match, e.g. if a color is (127, 127, 254), it's color ratio is (0.25, 0.25, 0.5)
inline bool is_solid(
    const ConstImageRef& image,
    const FloatPixel& expected_color_ratio,
    double max_euclidean_distance = 0.15,
    double max_stddev_sum = 10
){
    return is_solid(image_stats(image), expected_color_ratio, max_euclidean_distance, max_stddev_sum);
}


}
#endif
