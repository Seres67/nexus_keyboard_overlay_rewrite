#include <globals.hpp>

#include <unordered_map>
#include <utils.hpp>

std::string get_latest_keypress(const UINT virtual_key, const UINT scan_code)
{
    if (virtual_key >= VK_F1 && virtual_key <= VK_F24) {
        return "F" + std::to_string(virtual_key - VK_F1 + 1);
    }

    static const std::unordered_map<UINT, std::string> special_keys = {
        {VK_OEM_3, "`"},           {VK_SPACE, "Space"},        {VK_RETURN, "Enter"},    {VK_TAB, "Tab"},
        {VK_CAPITAL, "Caps Lock"}, {VK_SHIFT, "Shift"},        {VK_CONTROL, "Control"}, {VK_MENU, "Alt"},
        {VK_ESCAPE, "Escape"},     {VK_BACK, "Backspace"},     {VK_APPS, "Menu"},       {VK_UP, "Up"},
        {VK_DOWN, "Down"},         {VK_LEFT, "Left"},          {VK_RIGHT, "Right"},     {VK_INSERT, "Insert"},
        {VK_DELETE, "Delete"},     {VK_HOME, "Home"},          {VK_END, "End"},         {VK_NEXT, "Page Down"},
        {VK_PRIOR, "Page Up"},     {VK_SCROLL, "Scroll Lock"}, {VK_PAUSE, "Pause"}};

    if (const auto it = special_keys.find(virtual_key); it != special_keys.end()) {
        return it->second;
    }

    BYTE keyboard_state[256];
    if (!GetKeyboardState(keyboard_state))
        return {};

    // we do this so that ToUnicodeEx returns base keys and not shifted keys etc.
    keyboard_state[VK_SHIFT] = 0;        // ignore Shift
    keyboard_state[VK_CAPITAL] = 0;      // ignore Caps Lock
    keyboard_state[VK_LCONTROL] &= 0x7F; // ignore Ctrl
    keyboard_state[VK_RCONTROL] &= 0x7F;
    keyboard_state[VK_LMENU] &= 0x7F; // ignore Alt
    keyboard_state[VK_RMENU] &= 0x7F;

    HKL__ *layout = GetKeyboardLayout(0);

    WCHAR buff[8];
    const int result = ToUnicodeEx(virtual_key, scan_code, keyboard_state, buff, 8, 0, layout);
    if (result > 0) {
        const int key_size = WideCharToMultiByte(CP_UTF8, 0, buff, result, nullptr, 0, nullptr, nullptr);
        if (key_size > 0) {
            std::string key(key_size, '\0');
            WideCharToMultiByte(CP_UTF8, 0, buff, result, key.data(), key_size, nullptr, nullptr);
            return key;
        }
    }
    return {};
}