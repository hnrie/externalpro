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
    inline float gravity = 196.2f;

    inline bool highlight = true;
    inline float highlight_intensity = 0.2;

    inline Vector3 position(0, 0, 0);
    inline bool set_position = false;
    inline int fps = 240;
    inline bool lock_fps = false;

    inline float time_of_day = 12.f;
    inline float brightness = 2.f;
    inline ImVec4 ambient = ImVec4(0.5f, 0.5f, 0.5f, 1.f);
    inline float fog_end = 100000.f;
    inline float fog_start = 0.f;
    inline ImVec4 fog_color = ImVec4(0.5f, 0.5f, 0.5f, 1.f);

    inline bool noclip = false;
    inline float hip_height = 7.f;

    inline bool wallhack_enabled = false;
    inline float wallhack_transparency = 0.5f;
    inline std::map<uintptr_t, float> original_transparencies;

    namespace esp
    {
        inline bool enabled = false;
        inline bool aimbot_enabled = false;
        inline player_info target;
        inline bool show_fov = false;
        inline bool esp_enabled = true;
        inline bool esp_names = true;
        inline bool esp_boxes = true;
        inline bool esp_health = true;
        inline bool esp_tracers = true;
        inline bool esp_distance = false;
        inline bool fov_strobe = false;
        inline float esp_max_distance = 1000.0f;

        inline ImVec4 esp_tracer_color = ImVec4(1.f, 1.f, 1.f, 1.f);

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