#include <globals.hpp>
#include <gui.hpp>
#include <imgui/imgui.h>
#include <settings.hpp>
#include <utils.hpp>

bool tmp_open = true;
void render_window()
{
    ImGui::SetNextWindowPos(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, {Settings::background_color[0], Settings::background_color[1],
                                              Settings::background_color[2], Settings::background_color[3]});
    if (ImGui::Begin("Keyboard Overlay##KeyboardOverlayMainWindow", &tmp_open, flags)) {
        ImGui::Text("%u, %s", pressed_vk, pressed_key.c_str());
        for (auto &val : Settings::keys | std::views::values) {
            if (val.pressed()) {
                ImGui::TextColored(ImVec4(val.colors()[0], val.colors()[1], val.colors()[2], val.colors()[3]),
                                   "%u: pressed", val.virtual_code());
            } else {
                ImGui::Text("%u: released", val.virtual_code());
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
            ImGui::PushItemWidth(200);
            if (ImGui::Button("Record Key##KeyboardOverlayRecordNewKey")) {
                recording_keypress = true;
                virtual_key_to_add = 0;
                scan_code_to_add = 0;
            }
            ImGui::PopItemWidth();
        }
        if (recording_keypress && virtual_key_to_add == 0) {
            ImGui::PushItemWidth(200);
            ImGui::Button("Press key to add...##KeyboardOverlayPressKey");
            ImGui::PopItemWidth();
        }
        if (virtual_key_to_add != 0) {
            const std::string key_str = get_latest_keypress(virtual_key_to_add, scan_code_to_add);
            ImGui::Text("Adding key %s/%u", key_str.c_str(), virtual_key_to_add);
            recording_keypress = false;
        }
        // TODO: add a way to upload textures
        ImGui::PushItemWidth(200);
        ImGui::ColorEdit4("Color when key is pressed##KeyboardOverlayKeyColorPressed", add_key_colors);
        if (ImGui::Button("Cancel##KeyboardOverlayAddKeyModalCancel")) {
            adding_key = false;
            virtual_key_to_add = 0;
            scan_code_to_add = 0;
            recording_keypress = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopItemWidth();
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
    // ImGui::PushItemWidth(200);
    if (ImGui::ColorEdit4("Background Color##KeyboardOverlayBackgroundColor", Settings::background_color)) {
        Settings::json_settings["BackgroundColor"] = Settings::background_color;
        Settings::save();
    }
    // ImGui::PopItemWidth();
    if (ImGui::Button("Reset background color##KeyboardOverlayResetBackgroundColor")) {
        Settings::background_color[0] = 0.075;
        Settings::background_color[1] = 0.086;
        Settings::background_color[2] = 0.11;
        Settings::background_color[3] = 0.933;
        Settings::json_settings["BackgroundColor"] = Settings::background_color;
        Settings::save();
    }
    if (ImGui::CollapsingHeader("Keys##KeyboardOverlayKeysHeader")) {
        for (auto key = Settings::keys.begin(); key != Settings::keys.end();) {
            std::string key_str = get_latest_keypress(key->second.virtual_code(), key->second.scan_code());
            if (key->second.pressed()) {
                ImGui::TextColored(ImVec4(key->second.colors()[0], key->second.colors()[1], key->second.colors()[2],
                                          key->second.colors()[3]),
                                   "%s/%u", key_str.c_str(), key->second.virtual_code());
            } else {
                ImGui::Text("%s/%u", key_str.c_str(), key->second.virtual_code());
            }
            ImGui::SameLine();
            // ImGui::PushItemWidth(50);
            if (ImGui::ColorEdit4(
                    ("Key color##KeyboardOverlayKeyColor" + std::to_string(key->second.virtual_code())).c_str(),
                    key->second.colors())) {
                Settings::json_settings["Keys"] = Settings::keys;
                Settings::save();
            }
            // ImGui::PopItemWidth();
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