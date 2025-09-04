#include <UiKey.hpp>
#include <globals.hpp>
#include <gui.hpp>
#include <imgui/imgui.h>
#include <settings.hpp>
#include <utils.hpp>

bool tmp_open = true;
void render_window()
{
    if (Settings::disable_when_map_open && (!nexus_link->IsGameplay || mumble_link->Context.IsMapOpen))
        return;

    ImGui::SetNextWindowPos(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, {Settings::background_color[0], Settings::background_color[1],
                                              Settings::background_color[2], Settings::background_color[3]});
    if (ImGui::Begin("Keyboard Overlay##KeyboardOverlayMainWindow", &tmp_open, flags)) {
        ImGui::Text("%u, %s", pressed_vk, pressed_key.c_str());
        for (auto &val : Settings::keys | std::views::values) {
            if (val.pressed()) {
                ImGui::PushStyleColor(ImGuiCol_Button,
                                      {val.colors()[0], val.colors()[1], val.colors()[2], val.colors()[3]});
                // ImGui::TextColored(ImVec4(val.colors()[0], val.colors()[1], val.colors()[2], val.colors()[3]),
                // "%u: pressed", val.virtual_code());
                ImGui::Button(key_to_string(val.virtual_code(), val.scan_code()).c_str(),
                              {static_cast<float>(val.size().x), static_cast<float>(val.size().y)});
                ImGui::PopStyleColor();
            } else {
                ImGui::Button(key_to_string(val.virtual_code(), val.scan_code()).c_str(),
                              {static_cast<float>(val.size().x), static_cast<float>(val.size().y)});
                // ImGui::Text("%u: released", val.virtual_code());
            }
        }
        ImGui::End();
    }
    ImGui::PopStyleColor();
}

float add_key_colors[4] = {1, 1, 1, 0.933};
void render_options()
{
    if (ImGui::BeginPopupModal("Add Key Modal##KeyboardOverlayAddKeyModal")) {
        if (!recording_keypress) {
            if (ImGui::Button("Record Key##KeyboardOverlayRecordNewKey")) {
                recording_keypress = true;
                virtual_key_to_add = 0;
                scan_code_to_add = 0;
            }
        }
        if (recording_keypress && virtual_key_to_add == 0) {
            ImGui::Button("Press key to add...##KeyboardOverlayPressKey");
        }
        if (virtual_key_to_add != 0) {
            const std::string key_str = key_to_string(virtual_key_to_add, scan_code_to_add);
            ImGui::Text("Adding key %s/%u", key_str.c_str(), virtual_key_to_add);
            recording_keypress = false;
        }
        // TODO: add a way to upload textures
        ImGui::ColorEdit4("Color when key is pressed##KeyboardOverlayKeyColorPressed", add_key_colors,
                          ImGuiColorEditFlags_NoInputs);
        if (ImGui::Button("Cancel##KeyboardOverlayAddKeyModalCancel")) {
            adding_key = false;
            virtual_key_to_add = 0;
            scan_code_to_add = 0;
            recording_keypress = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Save##KeyboardOverlayAddKeyModalSave") || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
            if (virtual_key_to_add == 0)
                return;
            Settings::keys[virtual_key_to_add] = UIKey(virtual_key_to_add, scan_code_to_add, add_key_colors);
            adding_key = false;
            virtual_key_to_add = 0;
            scan_code_to_add = 0;
            recording_keypress = false;
            Settings::json_settings["Keys"] = Settings::keys;
            Settings::save();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    if (ImGui::Button("Add key##KeyboardOverlayOpenAddKeyModal")) {
        adding_key = true;
        ImGui::OpenPopup("Add Key Modal##KeyboardOverlayAddKeyModal");
    }
    if (ImGui::ColorEdit4("Background Color##KeyboardOverlayBackgroundColor", Settings::background_color,
                          ImGuiColorEditFlags_NoInputs)) {
        Settings::json_settings["BackgroundColor"] = Settings::background_color;
        Settings::save();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset background color##KeyboardOverlayResetBackgroundColor")) {
        Settings::background_color[0] = 0.075;
        Settings::background_color[1] = 0.086;
        Settings::background_color[2] = 0.11;
        Settings::background_color[3] = 0.933;
        Settings::json_settings["BackgroundColor"] = Settings::background_color;
        Settings::save();
    }
    if (ImGui::InputInt("Default Key Size", &Settings::default_key_size)) {
        Settings::json_settings["DefaultKeySize"] = Settings::default_key_size;
        Settings::save();
    }
    if (ImGui::Checkbox("Disable while typing in chat", &Settings::disable_while_in_chat)) {
        Settings::json_settings["DisableWhileInChat"] = Settings::disable_while_in_chat;
        Settings::save();
    }
    if (ImGui::Checkbox("Disable when map is open", &Settings::disable_when_map_open)) {
        Settings::json_settings["DisableWhenMapOpen"] = Settings::disable_when_map_open;
        Settings::save();
    }
    if (ImGui::CollapsingHeader("Keys##KeyboardOverlayKeysHeader")) {
        for (auto key = Settings::keys.begin(); key != Settings::keys.end();) {
            std::string key_str = key_to_string(key->second.virtual_code(), key->second.scan_code());
            if (key->second.pressed()) {
                ImGui::TextColored(ImVec4(key->second.colors()[0], key->second.colors()[1], key->second.colors()[2],
                                          key->second.colors()[3]),
                                   "%s", key_str.c_str());
            } else {
                ImGui::Text("%s", key_str.c_str());
            }
            ImGui::SameLine();
            if (ImGui::ColorEdit4(
                    ("Key color##KeyboardOverlayKeyColor" + std::to_string(key->second.virtual_code())).c_str(),
                    key->second.colors(), ImGuiColorEditFlags_NoInputs)) {
                Settings::json_settings["Keys"] = Settings::keys;
                Settings::save();
            }
            ImGui::SameLine();
            if (ImGui::Button(
                    ("Delete##KeyboardOverlayDeleteKey" + std::to_string(key->second.virtual_code())).c_str())) {
                key = Settings::keys.erase(key);
                Settings::json_settings["Keys"] = Settings::keys;
                Settings::save();
            } else {
                ++key;
            }
        }
    }
}