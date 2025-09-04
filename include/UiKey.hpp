#ifndef NEXUS_KEYBOARD_OVERLAY_UIKEY_HPP
#define NEXUS_KEYBOARD_OVERLAY_UIKEY_HPP

#include <nexus/Nexus.h>

namespace Settings
{
extern int default_key_size;
}

struct Vec2
{
    int x;
    int y;
};

class UIKey
{
  public:
    UIKey() = default;
    explicit UIKey(const UINT virtual_code, const UINT scan_code, const float colors[4])
        : m_vk(virtual_code), m_scan_code(scan_code), m_pos({0, 0}),
          m_size({Settings::default_key_size, Settings::default_key_size}), m_pressed(false)
    {
        m_colors[0] = colors[0];
        m_colors[1] = colors[1];
        m_colors[2] = colors[2];
        m_colors[3] = colors[3];
    }
    [[nodiscard]] UINT virtual_code() const { return m_vk; }
    void set_virtual_code(const UINT vk) { m_vk = vk; }

    [[nodiscard]] UINT scan_code() const { return m_scan_code; }
    void set_scan_code(const UINT scan_code) { m_scan_code = scan_code; }

    [[nodiscard]] Vec2 position() const { return m_pos; }
    void set_position(const Vec2 &pos) { m_pos = pos; }

    [[nodiscard]] Vec2 size() const { return m_size; }
    void set_size(const Vec2 &size) { m_size = size; }

    [[nodiscard]] float *colors() { return m_colors; }
    [[nodiscard]] const float *colors() const { return m_colors; }
    void set_colors(const float colors[4])
    {
        m_colors[0] = colors[0];
        m_colors[1] = colors[1];
        m_colors[2] = colors[2];
        m_colors[3] = colors[3];
    }

    [[nodiscard]] bool pressed() const { return m_pressed; }
    void set_pressed(const bool pressed) { m_pressed = pressed; }

  private:
    UINT m_vk;
    UINT m_scan_code;
    Vec2 m_pos;
    Vec2 m_size;
    float m_colors[4];
    bool m_pressed;
};

#endif // NEXUS_KEYBOARD_OVERLAY_UIKEY_HPP
