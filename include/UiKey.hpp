#ifndef NEXUS_KEYBOARD_OVERLAY_UIKEY_HPP
#define NEXUS_KEYBOARD_OVERLAY_UIKEY_HPP

#include <nexus/Nexus.h>

namespace Settings
{
extern float default_key_size;
}

class UIKey
{
  public:
    UIKey() = default;
    UIKey(const UINT virtual_code, const UINT scan_code, const float released_colors[4], const float pressed_colors[4])
        : m_vk(virtual_code), m_scan_code(scan_code), m_pressed(false)
    {
        m_pos[0] = 0;
        m_pos[1] = 0;
        m_size[0] = Settings::default_key_size;
        m_size[1] = Settings::default_key_size;
        m_released_colors[0] = released_colors[0];
        m_released_colors[1] = released_colors[1];
        m_released_colors[2] = released_colors[2];
        m_released_colors[3] = released_colors[3];
        m_pressed_colors[0] = pressed_colors[0];
        m_pressed_colors[1] = pressed_colors[1];
        m_pressed_colors[2] = pressed_colors[2];
        m_pressed_colors[3] = pressed_colors[3];
    }

    UIKey(const UINT virtual_code, const UINT scan_code, const float pos[2], const float size[2],
          const float released_colors[4], const float pressed_colors[4])
        : m_vk(virtual_code), m_scan_code(scan_code), m_pressed(false)
    {
        m_pos[0] = pos[0];
        m_pos[1] = pos[1];
        m_size[0] = size[0];
        m_size[1] = size[1];
        m_released_colors[0] = released_colors[0];
        m_released_colors[1] = released_colors[1];
        m_released_colors[2] = released_colors[2];
        m_released_colors[3] = released_colors[3];
        m_pressed_colors[0] = pressed_colors[0];
        m_pressed_colors[1] = pressed_colors[1];
        m_pressed_colors[2] = pressed_colors[2];
        m_pressed_colors[3] = pressed_colors[3];
    }
    [[nodiscard]] UINT virtual_code() const { return m_vk; }
    void set_virtual_code(const UINT vk) { m_vk = vk; }

    [[nodiscard]] UINT scan_code() const { return m_scan_code; }
    void set_scan_code(const UINT scan_code) { m_scan_code = scan_code; }

    [[nodiscard]] const float *position() const { return m_pos; }
    [[nodiscard]] float *position() { return m_pos; }
    void set_position(const float pos[2])
    {
        m_pos[0] = pos[0];
        m_pos[1] = pos[1];
    }

    [[nodiscard]] const float *size() const { return m_size; }
    [[nodiscard]] float *size() { return m_size; }
    void set_size(const float size[2])
    {
        m_size[0] = size[0];
        m_size[1] = size[1];
    }

    [[nodiscard]] float *released_colors() { return m_released_colors; }
    [[nodiscard]] const float *released_colors() const { return m_released_colors; }
    void set_released_colors(const float colors[4])
    {
        m_released_colors[0] = colors[0];
        m_released_colors[1] = colors[1];
        m_released_colors[2] = colors[2];
        m_released_colors[3] = colors[3];
    }

    [[nodiscard]] float *pressed_colors() { return m_pressed_colors; }
    [[nodiscard]] const float *pressed_colors() const { return m_pressed_colors; }
    void set_pressed_colors(const float colors[4])
    {
        m_pressed_colors[0] = colors[0];
        m_pressed_colors[1] = colors[1];
        m_pressed_colors[2] = colors[2];
        m_pressed_colors[3] = colors[3];
    }

    [[nodiscard]] bool pressed() const { return m_pressed; }
    void set_pressed(const bool pressed) { m_pressed = pressed; }

  private:
    UINT m_vk{};
    UINT m_scan_code;
    float m_pos[2];
    float m_size[2];
    float m_released_colors[4];
    float m_pressed_colors[4];
    bool m_pressed;
};

#endif // NEXUS_KEYBOARD_OVERLAY_UIKEY_HPP
