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
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar;
    if (Settings::lock_window)
        window_flags |= ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, {Settings::background_color[0], Settings::background_color[1],
                                              Settings::background_color[2], Settings::background_color[3]});
    if (ImGui::Begin("Keyboard Overlay##KeyboardOverlayMainWindow", &tmp_open, window_flags)) {
#ifndef NDEBUG
        ImGui::Text("%u, %s", pressed_vk, pressed_key.c_str());
#endif
        for (auto &val : Settings::keys | std::views::values) {
            ImGui::SetCursorPos({val.position()[0], val.position()[1]});
            if (val.pressed()) {
                ImGui::PushStyleColor(ImGuiCol_Button,
                                      {val.colors()[0], val.colors()[1], val.colors()[2], val.colors()[3]});
                ImGui::Button(key_to_string(val.virtual_code(), val.scan_code()).c_str(),
                              {(val.size()[0]), (val.size()[1])});
                ImGui::PopStyleColor();
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button,
                                      {0.247, 0.302, 0.396, 0.933}); // TODO: make it a setting, maybe even per-key
                ImGui::Button(key_to_string(val.virtual_code(), val.scan_code()).c_str(),
                              {(val.size()[0]), (val.size()[1])});
                ImGui::PopStyleColor();
            }
            if (Settings::edit_mode && ImGui::IsItemActive()) {
                const float pos[2] = {val.position()[0] + ImGui::GetIO().MouseDelta.x,
                                      val.position()[1] + ImGui::GetIO().MouseDelta.y};
                val.set_position(pos);
            }
            if (Settings::edit_mode && ImGui::IsItemDeactivated()) {
                Settings::json_settings["Keys"] = Settings::keys;
                Settings::save();
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
    ImGui::PushItemWidth(400);
    if (ImGui::SliderFloat("Default Key Size", &Settings::default_key_size, 1.f, 1000.f)) {
        Settings::json_settings["DefaultKeySize"] = Settings::default_key_size;
        Settings::save();
    }
    ImGui::PopItemWidth();
    if (ImGui::Checkbox("Disable while typing in chat", &Settings::disable_while_in_chat)) {
        Settings::json_settings["DisableWhileInChat"] = Settings::disable_while_in_chat;
        Settings::save();
    }
    if (ImGui::Checkbox("Disable when map is open", &Settings::disable_when_map_open)) {
        Settings::json_settings["DisableWhenMapOpen"] = Settings::disable_when_map_open;
        Settings::save();
    }
    if (ImGui::Checkbox("Edit Mode", &Settings::edit_mode)) {
        Settings::json_settings["EditMode"] = Settings::edit_mode;
        Settings::save();
    }
    if (ImGui::Checkbox("Lock Window", &Settings::lock_window)) {
        Settings::json_settings["LockWindow"] = Settings::lock_window;
        Settings::save();
    }
    if (ImGui::CollapsingHeader("Keys##KeyboardOverlayKeysHeader")) {
        if (ImGui::BeginTable("KeysTable##KeyboardOverlayKeysTable", 5,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 80.f);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Position", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 50.f);
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 50.f);
            ImGui::TableHeadersRow();

            for (auto key = Settings::keys.begin(); key != Settings::keys.end();) {
                UIKey &k = key->second;
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                std::string key_str = key_to_string(k.virtual_code(), k.scan_code());
                if (k.pressed()) {
                    ImGui::TextColored(ImVec4(k.colors()[0], k.colors()[1], k.colors()[2], k.colors()[3]), "%s",
                                       key_str.c_str());
                } else {
                    ImGui::Text("%s", key_str.c_str());
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::PushItemWidth(-1);
                if (ImGui::SliderFloat2(("##Size" + std::to_string(k.virtual_code())).c_str(), k.size(), 1.0f,
                                        1000.0f)) {
                    Settings::json_settings["Keys"] = Settings::keys;
                    Settings::save();
                }
                ImGui::PopItemWidth();

                ImGui::TableSetColumnIndex(2);
                ImGui::PushItemWidth(-1);
                if (ImGui::SliderFloat2(("##Position" + std::to_string(k.virtual_code())).c_str(), k.position(), 0.0f,
                                        5000.0f)) {
                    Settings::json_settings["Keys"] = Settings::keys;
                    Settings::save();
                }
                ImGui::PopItemWidth();

                ImGui::TableSetColumnIndex(3);
                if (ImGui::ColorEdit4(("##Color" + std::to_string(k.virtual_code())).c_str(), k.colors(),
                                      ImGuiColorEditFlags_NoInputs)) {
                    Settings::json_settings["Keys"] = Settings::keys;
                    Settings::save();
                }

                ImGui::TableSetColumnIndex(4);
                if (ImGui::Button(("Delete##" + std::to_string(k.virtual_code())).c_str())) {
                    key = Settings::keys.erase(key);
                    Settings::json_settings["Keys"] = Settings::keys;
                    Settings::save();
                    continue;
                }

                ++key;
            }
            ImGui::EndTable();
        }
    }
}