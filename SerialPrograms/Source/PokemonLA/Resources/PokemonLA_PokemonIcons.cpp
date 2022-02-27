/*  Pokemon LA Icons
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Cpp/Exception.h"
#include "Common/Qt/QtJsonTools.h"
#include "CommonFramework/Globals.h"
#include "CommonFramework/ImageMatch/ImageCropper.h"
#include "PokemonLA_PokemonIcons.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{


std::map<std::string, QIcon> load_pokemon_icons(){
    QImage sprites(RESOURCE_PATH() + "PokemonLA/PokemonSprites.png");
    QJsonObject json = read_json_file(
        RESOURCE_PATH() + "PokemonLA/PokemonSprites.json"
    ).object();

    int width = json.find("spriteWidth")->toInt();
    int height = json.find("spriteHeight")->toInt();
    if (width <= 0){
        PA_THROW_ParseException("Invalid width.");
    }
    if (height <= 0){
        PA_THROW_ParseException("Invalid height.");
    }

    std::map<std::string, QIcon> map;
    QJsonObject locations = json.find("spriteLocations")->toObject();
    for (auto iter = locations.begin(); iter != locations.end(); ++iter){
        std::string slug = iter.key().toStdString();
        QJsonObject obj = iter.value().toObject();
        int y = obj.find("top")->toInt();
        int x = obj.find("left")->toInt();

        QImage sprite = sprites.copy(x, y, width, height);
        QPixmap pixmap = QPixmap::fromImage(ImageMatch::trim_image_alpha(sprite));
        map.emplace(std::move(slug), pixmap);
    }
    return map;
}


const std::map<std::string, QIcon>& ALL_POKEMON_ICONS(){
    static std::map<std::string, QIcon> icons = load_pokemon_icons();
    return icons;
}



}
}
}