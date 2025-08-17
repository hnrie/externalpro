#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>
#include <imgui.h>

#include "structures/structures.hpp"
#include "instance/instance.hpp"

struct player_info {
    std::string name;
    std::string display_name;
    Vector3 position;
};

namespace globals
{
    inline DWORD pid;
    inline HANDLE handle;
    inline HWND rbx;
    inline uintptr_t base;

    inline float walk_speed = 16.0f;
    inline float jump_power = 50.0f;
    inline bool sitting = false;

    inline Vector3 position(0, 0, 0);
    inline bool set_position = false;

    namespace esp
    {
        // i have not made the esp stuff only player stuff and the overlay and the fov settings.
        inline bool show_fov = true;
        inline bool esp_enabled = false;
        inline bool esp_names = false;
        inline bool esp_boxes = false;
        inline bool esp_health = false;
        inline bool esp_distance = false;
        inline bool fov_strobe = false;
        inline float esp_max_distance = 1000.0f;

        inline int fov_segments = 50.0f;
        inline float fov_size = 90.0f;
        inline float fov_thickness = 1.0f;
        inline float fov_strobe_speed = 1.0f;
        inline float fov_hue = 0.0f;
        inline float fov_transparency = 0.0f;

        inline ImVec4 esp_box_color = ImVec4(1.f, 0.f, 0.f, 1.f);
        inline ImVec4 esp_name_color = ImVec4(1.f, 1.f, 1.f, 1.f);
        inline ImVec4 fov_circle_color = ImVec4(0, 1, 0, 1);
    }

    inline uintptr_t workspace;
    inline uintptr_t players;
    inline uintptr_t datamodel;

    inline std::vector<player_info> player_list;
    inline bool updated = false;
}