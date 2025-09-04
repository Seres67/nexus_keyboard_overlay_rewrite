#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <mumble/Mumble.h>
#include <nexus/Nexus.h>
#include <string>

// handle to self hmodule
extern HMODULE self_module;
// addon definition
extern AddonDefinition addon_def;
// addon api
extern AddonAPI *api;

extern char addon_name[];

extern HWND game_handle;

extern Mumble::Data *mumble_link;
extern NexusLinkData *nexus_link;

extern UINT pressed_vk;
extern std::string pressed_key;

extern bool adding_key;
extern bool recording_keypress;
extern UINT virtual_key_to_add;
extern UINT scan_code_to_add;
#endif // GLOBALS_HPP
