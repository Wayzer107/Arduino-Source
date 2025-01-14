/*  Settings from JSON File.
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <memory>
#include <vector>
#include "Options/ConfigItem.h"
#include "ConfigSet.h"

namespace PokemonAutomation{


class Settings_JsonFile : public ConfigSet{
public:
    Settings_JsonFile(QString category, const QString& filepath);
    Settings_JsonFile(QString category, const QJsonObject& obj);

    virtual QString check_validity() const override;
    virtual void restore_defaults() override;

    virtual QJsonArray options_json() const override;
    virtual std::string options_cpp() const override;

    virtual QWidget* make_options_body(QWidget& parent) override;

private:
    std::vector<std::unique_ptr<ConfigItem>> m_options;
};


}
