/*  Raw Text Recognition
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_OCR_RawOCR_H
#define PokemonAutomation_OCR_RawOCR_H

#include "CommonFramework/Language.h"
#include "CommonFramework/ImageTypes/ImageReference.h"

namespace PokemonAutomation{
namespace OCR{


bool language_available(Language language);

QString ocr_read(Language language, const ConstImageRef& image);



}
}
#endif
