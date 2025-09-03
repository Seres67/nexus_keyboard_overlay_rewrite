#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "UiKey.hpp"

#include <mutex>
#include <nlohmann/json.hpp>

void from_json(const nlohmann::json &j, UIKey &key);
void to_json(nlohmann::json &j, const UIKey &key);
namespace Settings
{

void load();
void save();

extern nlohmann::json json_settings;
extern std::filesystem::path settings_path;
extern std::mutex mutex;

extern std::unordered_map<unsigned int, UIKey> keys;
extern float background_color[4];
} // namespace Settings

#endif // SETTINGS_HPP
