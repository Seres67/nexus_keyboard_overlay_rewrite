#include "settings.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <globals.hpp>

#include <nexus/Nexus.h>

using json = nlohmann::json;

void from_json(const nlohmann::json &j, UIKey &key)
{
    key.set_virtual_code(j.at("virtual_code").get<UINT>());
    key.set_scan_code(j.at("scan_code").get<UINT>());
    const float r = j.at("r").get<float>();
    const float g = j.at("g").get<float>();
    const float b = j.at("b").get<float>();
    const float a = j.at("a").get<float>();
    const float colors[4] = {r, g, b, a};
    key.set_colors(colors);
    const int x = j.at("x").get<int>();
    const int y = j.at("y").get<int>();
    key.set_position({x, y});
    const int width = j.at("width").get<int>();
    const int height = j.at("height").get<int>();
    key.set_size({width, height});
}

void to_json(nlohmann::json &j, const UIKey &key)
{
    j = json{
        {"virtual_code", key.virtual_code()},
        {"scan_code", key.scan_code()},
        {"r", key.colors()[0]},
        {"g", key.colors()[1]},
        {"b", key.colors()[2]},
        {"a", key.colors()[3]},
        {"x", key.position().x},
        {"y", key.position().y},
        {"width", key.size().x},
        {"height", key.size().y},
    };
}

namespace Settings
{
json json_settings;
std::mutex mutex;
std::filesystem::path settings_path;

std::unordered_map<unsigned int, UIKey> keys;
float background_color[4] = {0.075, 0.086, 0.11, 0.933};
int default_key_size = 42;
bool disable_while_in_chat = true;
bool disable_when_map_open = false;
void load()
{
    json_settings = json::object();
    if (!std::filesystem::exists(settings_path)) {
        return;
    }

    {
        std::lock_guard lock(mutex);
        try {
            if (std::ifstream file(settings_path); file.is_open()) {
                json_settings = json::parse(file);
                file.close();
            }
        } catch (json::parse_error &ex) {
            api->Log(ELogLevel_WARNING, addon_name, "settings.json could not be parsed.");
            api->Log(ELogLevel_WARNING, addon_name, ex.what());
        }
    }
    if (!json_settings["Keys"].is_null())
        json_settings["Keys"].get_to(keys);
    if (!json_settings["BackgroundColor"].is_null())
        json_settings["BackgroundColor"].get_to(background_color);
    if (!json_settings["DefaultKeySize"].is_null())
        json_settings["DefaultKeySize"].get_to(default_key_size);
    if (!json_settings["DisableWhileInChat"].is_null())
        json_settings["DisableWhileInChat"].get_to(disable_while_in_chat);
    if (!json_settings["DisableWhenMapOpen"].is_null())
        json_settings["DisableWhenMapOpen"].get_to(disable_when_map_open);
    api->Log(ELogLevel_INFO, addon_name, "settings loaded!");
}

void save()
{
    if (!std::filesystem::exists(settings_path.parent_path())) {
        std::filesystem::create_directories(settings_path.parent_path());
    }
    {
        std::lock_guard lock(mutex);
        if (std::ofstream file(settings_path); file.is_open()) {
            file << json_settings.dump(1, '\t') << std::endl;
            file.close();
        }
        api->Log(ELogLevel_INFO, addon_name, "settings saved!");
    }
}
} // namespace Settings
