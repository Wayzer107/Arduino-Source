/*  Video Widget
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_VideoPipeline_VideoWidget_H
#define PokemonAutomation_VideoPipeline_VideoWidget_H

#include <vector>
#include <QWidget>

class QImage;

namespace PokemonAutomation{

// Base class for the widget that loads the video content.
class VideoWidget : public QWidget{
public:
    using QWidget::QWidget;
    virtual ~VideoWidget() = default;

    //  These will be called from the UI thread.
    virtual QSize resolution() const = 0;
    virtual std::vector<QSize> resolutions() const = 0;
    virtual void set_resolution(const QSize& size) = 0;

    //  This snapshot function will be called asynchronously from many threads
    //  at a very high rate. So it needs to be thread-safe with both itself
    //  and all the functions above and should cache the image if there has been
    //  no new frames since the previous call.
    //  This function will never be called on the UI thread.
    virtual QImage snapshot() = 0;
};



}
#endif
