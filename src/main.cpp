#include <UiKey.hpp>
#include <globals.hpp>
#include <gui.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <nexus/Nexus.h>
#include <settings.hpp>
#include <utils.hpp>

void addon_load(AddonAPI *api_p);
void addon_unload();
void addon_render();
void addon_options();
UINT wnd_proc(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param);

BOOL APIENTRY dll_main(const HMODULE hModule, const DWORD ul_reason_for_call, [[maybe_unused]] LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        self_module = hModule;
        break;
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        break;
    }
    return TRUE;
}

// NOLINTNEXTLINE(readability-identifier-naming)
extern "C" __declspec(dllexport) AddonDefinition *GetAddonDef()
{
    addon_def.Signature = -918234798;
    addon_def.APIVersion = NEXUS_API_VERSION;
    addon_def.Name = addon_name;
    addon_def.Version.Major = 0;
    addon_def.Version.Minor = 9;
    addon_def.Version.Build = 0;
    addon_def.Version.Revision = 0;
    addon_def.Author = "Seres67";
    addon_def.Description = "Adds a modular keyboard overlay to the game!";
    addon_def.Load = addon_load;
    addon_def.Unload = addon_unload;
    addon_def.Flags = EAddonFlags_None;
    addon_def.Provider = EUpdateProvider_GitHub;
    addon_def.UpdateLink = "https://github.com/Seres67/nexus_keyboard_overlay";

    return &addon_def;
}

void addon_load(AddonAPI *api_p)
{
    api = api_p;

    ImGui::SetCurrentContext(static_cast<ImGuiContext *>(api->ImguiContext));
    ImGui::SetAllocatorFunctions(static_cast<void *(*)(size_t, void *)>(api->ImguiMalloc),
                                 static_cast<void (*)(void *, void *)>(api->ImguiFree)); // on imgui 1.80+

    mumble_link = static_cast<Mumble::Data *>(api->DataLink.Get("DL_MUMBLE_LINK"));
    nexus_link = static_cast<NexusLinkData *>(api->DataLink.Get("DL_NEXUS_LINK"));

    api->Renderer.Register(ERenderType_Render, addon_render);
    api->Renderer.Register(ERenderType_OptionsRender, addon_options);
    api->WndProc.Register(wnd_proc);

    Settings::settings_path = api->Paths.GetAddonDirectory("keyboard_overlay\\settings.json");
    Settings::load();

    // TODO: convert existing keys in `AllKeybindings` to UIKeys in `keys`
    api->Log(ELogLevel_INFO, addon_name, "addon loaded!");
}

void addon_unload()
{
    api->Log(ELogLevel_INFO, addon_name, "unloading addon...");
    api->Renderer.Deregister(addon_render);
    api->Renderer.Deregister(addon_options);
    api->WndProc.Deregister(wnd_proc);
    nexus_link = nullptr;
    mumble_link = nullptr;
    api->Log(ELogLevel_INFO, addon_name, "addon unloaded!");
    api = nullptr;
}

void addon_render() { render_window(); }

void addon_options() { render_options(); }

UINT get_mouse_button(const UINT u_msg, const WPARAM w_param)
{
    UINT virtual_key = 0;
    switch (u_msg) {
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
        virtual_key = VK_LBUTTON;
        break;
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
        virtual_key = VK_RBUTTON;
        break;
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
        virtual_key = VK_MBUTTON;
        break;
    case WM_XBUTTONUP:
    case WM_XBUTTONDBLCLK:
    case WM_XBUTTONDOWN: {
        const WORD btn = HIWORD(w_param);
        if (btn == XBUTTON1)
            virtual_key = VK_XBUTTON1;
        else if (btn == XBUTTON2)
            virtual_key = VK_XBUTTON2;
        break;
    }
    default:
        virtual_key = 0;
        break;
    }
    return virtual_key;
}

UINT wnd_proc(HWND__ *h_wnd, const UINT u_msg, const WPARAM w_param, const LPARAM l_param)
{
    if (!game_handle)
        game_handle = h_wnd;
    if (Settings::disable_while_in_chat && mumble_link->Context.IsTextboxFocused)
        return u_msg;
    if (u_msg == WM_KEYDOWN || u_msg == WM_SYSKEYDOWN) {
        const UINT virtual_key = static_cast<UINT>(w_param);
        const UINT scan_code = l_param >> 16 & 0xFF;
        if (recording_keypress) {
            virtual_key_to_add = virtual_key;
            scan_code_to_add = scan_code;
        }
        if (Settings::keys.contains(virtual_key))
            Settings::keys[virtual_key].set_pressed(true);
        const std::string key = key_to_string(virtual_key, scan_code);
        pressed_vk = virtual_key;
        pressed_key = key;
#ifndef NDEBUG
        api->Log(ELogLevel_DEBUG, addon_name, (key + " " + std::to_string(virtual_key)).c_str());
#endif
    }
    if (u_msg == WM_KEYUP || u_msg == WM_SYSKEYUP) {
        const UINT virtual_key = static_cast<UINT>(w_param);
        if (Settings::keys.contains(virtual_key))
            Settings::keys[virtual_key].set_pressed(false);
    }
    if (u_msg == WM_LBUTTONDOWN || u_msg == WM_MBUTTONDOWN || u_msg == WM_RBUTTONDOWN || u_msg == WM_XBUTTONDOWN ||
        u_msg == WM_LBUTTONDBLCLK || u_msg == WM_RBUTTONDBLCLK || u_msg == WM_MBUTTONDBLCLK ||
        u_msg == WM_XBUTTONDBLCLK) {
        const UINT virtual_key = get_mouse_button(u_msg, w_param);
        if (recording_keypress) {
            virtual_key_to_add = virtual_key;
            scan_code_to_add = 0;
        }
        if (Settings::keys.contains(virtual_key))
            Settings::keys[virtual_key].set_pressed(true);
        pressed_vk = virtual_key;
        pressed_key = key_to_string(virtual_key, 0);
        api->Log(ELogLevel_DEBUG, addon_name, pressed_key.c_str());
    }
    if (u_msg == WM_LBUTTONUP || u_msg == WM_MBUTTONUP || u_msg == WM_RBUTTONUP || u_msg == WM_XBUTTONUP) {
        const UINT virtual_key = get_mouse_button(u_msg, w_param);
        if (Settings::keys.contains(virtual_key))
            Settings::keys[virtual_key].set_pressed(false);
    }
    return u_msg;
}