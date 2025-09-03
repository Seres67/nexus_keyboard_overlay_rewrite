#ifndef NEXUS_KEYBOARD_OVERLAY_UIKEY_HPP
#define NEXUS_KEYBOARD_OVERLAY_UIKEY_HPP
#include <imgui/imgui.h>
#include <nexus/Nexus.h>

struct Position
{
    UINT x;
    UINT y;
};

class UIKey
{
  public:
    UIKey() = default;
    explicit UIKey(const UINT virtual_code, const UINT scan_code, float colors[4])
        : m_vk(virtual_code), m_scan_code(scan_code), m_pos({0, 0}), m_pressed(false)
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

    [[nodiscard]] Position position() const { return m_pos; }
    void set_position(const Position &pos) { m_pos = pos; }

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
    Position m_pos;
    float m_colors[4];
    bool m_pressed;
};

#endif // NEXUS_KEYBOARD_OVERLAY_UIKEY_HPP
