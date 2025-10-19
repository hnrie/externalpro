#include "overlay.hpp"
#include "string"
#include "../instance/instance.hpp"

#include <DirectXMath.h>

using namespace DirectX;

Vector2 WorldToScreen(const Vector3& world_pos, const Matrix4& view_matrix, int screen_width, int screen_height)
{
    Vector4 clip_coords;
    clip_coords.x = world_pos.x * view_matrix.m[0][0] + world_pos.y * view_matrix.m[1][0] + world_pos.z * view_matrix.m[2][0] + view_matrix.m[3][0];
    clip_coords.y = world_pos.x * view_matrix.m[0][1] + world_pos.y * view_matrix.m[1][1] + world_pos.z * view_matrix.m[2][1] + view_matrix.m[3][1];
    clip_coords.z = world_pos.x * view_matrix.m[0][2] + world_pos.y * view_matrix.m[1][2] + world_pos.z * view_matrix.m[2][2] + view_matrix.m[3][2];
    clip_coords.w = world_pos.x * view_matrix.m[0][3] + world_pos.y * view_matrix.m[1][3] + world_pos.z * view_matrix.m[2][3] + view_matrix.m[3][3];

    if (clip_coords.w < 0.1f)
        return Vector2(-1, -1);

    Vector3 ndc;
    ndc.x = clip_coords.x / clip_coords.w;
    ndc.y = clip_coords.y / clip_coords.w;
    ndc.z = clip_coords.z / clip_coords.w;

    Vector2 screen_pos;
    screen_pos.x = (screen_width / 2.0f * ndc.x) + (screen_width / 2.0f);
    screen_pos.y = -(screen_height / 2.0f * ndc.y) + (screen_height / 2.0f);

    return screen_pos;
}

bool overlay_manager::create_d3d_device(HWND window_handle)
{
    DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.BufferDesc.Width = 0;
    swap_chain_desc.BufferDesc.Height = 0;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = window_handle;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.Windowed = TRUE;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    const UINT creation_flags = 0;
    D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    D3D_FEATURE_LEVEL selected_feature_level;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creation_flags,
        feature_levels,
        _countof(feature_levels),
        D3D11_SDK_VERSION,
        &swap_chain_desc,
        &swap_chain,
        &d3d_device,
        &selected_feature_level,
        &d3d_context);

    if (hr == DXGI_ERROR_UNSUPPORTED)
    {
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            creation_flags,
            feature_levels,
            _countof(feature_levels),
            D3D11_SDK_VERSION,
            &swap_chain_desc,
            &swap_chain,
            &d3d_device,
            &selected_feature_level,
            &d3d_context);
    }

    if (hr != S_OK)
        return false;

    create_render_target_view();
    return true;
}

void overlay_manager::cleanup_d3d_device()
{
    cleanup_render_target_view();

    if (swap_chain)
    {
        swap_chain->Release();
        swap_chain = nullptr;
    }
    if (d3d_context)
    {
        d3d_context->Release();
        d3d_context = nullptr;
    }
    if (d3d_device)
    {
        d3d_device->Release();
        d3d_device = nullptr;
    }
}

void overlay_manager::create_render_target_view()
{
    ID3D11Texture2D* back_buffer = nullptr;
    if (swap_chain && swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)) == S_OK)
    {
        d3d_device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
        back_buffer->Release();
    }
}

void overlay_manager::cleanup_render_target_view()
{
    if (render_target_view)
    {
        render_target_view->Release();
        render_target_view = nullptr;
    }
}

LRESULT CALLBACK overlay_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (overlay_instance->d3d_device && wparam != SIZE_MINIMIZED)
        {
            overlay_instance->cleanup_render_target_view();
            overlay_instance->swap_chain->ResizeBuffers(2, LOWORD(lparam), HIWORD(lparam), DXGI_FORMAT_UNKNOWN, 0);
            overlay_instance->create_render_target_view();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wparam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

struct drawing_manager
{
    ImDrawList* draw_list;

    drawing_manager()
    {
        draw_list = nullptr;
    }

    void begin()
    {
        draw_list = ImGui::GetForegroundDrawList();
    }

    void add_circle(float x, float y, float radius, ImU32 color, int segments = 64, float thickness = 1.0f)
    {
        if (!draw_list) return;
        draw_list->AddCircle(ImVec2(x, y), radius, color, segments, thickness);
    }

    void add_square(float x1, float y1, float x2, float y2, ImU32 color, float thickness = 1.0f)
    {
        if (!draw_list) return;
        draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), color, 0.0f, 0, thickness);
    }

    void add_line(float x1, float y1, float x2, float y2, ImU32 color, float thickness = 1.0f)
    {
        if (!draw_list) return;
        draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);
    }

    void add_text(float x, float y, ImU32 color, const char* text)
    {
        if (!draw_list) return;
        draw_list->AddText(ImVec2(x, y), color, text);
    }
};

POINT get_mouse_pos(HWND overlay_window)
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(overlay_window, &pt);
    return pt;
}

instance selected_instance;

void render_dex(const instance& inst, bool highlight = false) {
    if (!globals::highlight)
        highlight = false;

    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
        | ImGuiTreeNodeFlags_SpanAvailWidth
        | ImGuiTreeNodeFlags_FramePadding;

    if (inst.children().empty())
        node_flags |= ImGuiTreeNodeFlags_Leaf;

    if (highlight)
        node_flags |= ImGuiTreeNodeFlags_Selected;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 3));

    if (highlight) {
        ImVec4 subtle = ImGui::GetStyleColorVec4(ImGuiCol_Header);
        subtle.w = globals::highlight_intensity;
        ImGui::PushStyleColor(ImGuiCol_Header, subtle);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, subtle);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, subtle);
    }

    bool node_open = ImGui::TreeNodeEx(
        (void*)(intptr_t)inst.address,
        node_flags,
        "%s  [%s]",
        inst.name().c_str(),
        inst.class_name().c_str()
    );

    if (highlight)
        ImGui::PopStyleColor(3);

    ImGui::PopStyleVar();

    bool hovered = ImGui::IsItemHovered();

    if (ImGui::IsItemClicked())
        selected_instance = inst;

    if (node_open) {
        for (const auto& child : inst.children()) {
            render_dex(child, globals::highlight && (hovered || highlight));
        }
        ImGui::TreePop();
    }
}

int get_refresh_rate()
{
    DEVMODE dev_mode = {};
    dev_mode.dmSize = sizeof(dev_mode);

    if (EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dev_mode))
        return dev_mode.dmDisplayFrequency;

    return 0;
}

bool overlay_manager::initialize_overlay()
{
    ImGui_ImplWin32_EnableDpiAwareness();

    WNDCLASSEXA wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc = overlay_wndproc;
    wc.hInstance = GetModuleHandleA(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(CreateSolidBrush(RGB(0, 0, 0)));
    wc.lpszClassName = "skibidi ohio";
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    RegisterClassExA(&wc);

    HWND overlay_window = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        wc.lpszClassName,
        "rizzler",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr);

    SetLayeredWindowAttributes(overlay_window, 0, 255, LWA_ALPHA);

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(overlay_window, &margins);

    if (!create_d3d_device(overlay_window))
    {
        cleanup_d3d_device();
        UnregisterClassA(wc.lpszClassName, wc.hInstance);
        return false;
    }

    ShowWindow(overlay_window, SW_SHOW);
    UpdateWindow(overlay_window);

    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;

    ImGui_ImplWin32_Init(overlay_window);
    ImGui_ImplDX11_Init(d3d_device, d3d_context);

    ImVec4 clear_color = ImVec4(0, 0, 0, 0);

    bool done = false;
    bool show_ui = true;

    while (!done)
    {
        MSG msg{};
        while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }

        if (done)
            break;

        if (GetAsyncKeyState(VK_INSERT) & 1)
            show_ui = !show_ui;

        HWND target = FindWindowA(nullptr, "Roblox");
        HWND foreground = GetForegroundWindow();

        if (target != foreground && overlay_window != foreground)
        {
            MoveWindow(overlay_window, 0, 0, 0, 0, true);
        }
        else
        {
            RECT target_rect{};
            GetWindowRect(target, &target_rect);

            int width = target_rect.right - target_rect.left;
            int height = target_rect.bottom - target_rect.top;

            bool fullscreen = false;
            MONITORINFO monitor_info{ sizeof(MONITORINFO) };
            if (GetMonitorInfoA(MonitorFromWindow(target, MONITOR_DEFAULTTOPRIMARY), &monitor_info))
            {
                RECT window_rect{};
                if (GetWindowRect(target, &window_rect))
                {
                    fullscreen = window_rect.left == monitor_info.rcMonitor.left &&
                        window_rect.right == monitor_info.rcMonitor.right &&
                        window_rect.top == monitor_info.rcMonitor.top &&
                        window_rect.bottom == monitor_info.rcMonitor.bottom;
                }
            }

            if (fullscreen)
            {
                width += 16;
                height -= 24;
            }
            else
            {
                height -= 63;
                target_rect.left += 8;
                target_rect.top += 31;
            }

            MoveWindow(overlay_window, target_rect.left, target_rect.top, width, height, TRUE);
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (show_ui)
        {
            static ImVec2 window_size = ImVec2(600, 350);
            static bool first_init = false;

            if (!first_init)
            {
                ImGui::SetNextWindowSize(window_size);
                first_init = true;
            }

            ImGui::Begin("pro external", nullptr, ImGuiWindowFlags_None);

            if (ImGui::BeginTabBar("main_tabs"))
            {
                if (ImGui::BeginTabItem("esp"))
                {
                    ImGui::Text("ESP Settings");
                    ImGui::Checkbox("Enable ESP", &globals::esp::esp_enabled);
                    ImGui::Checkbox("Show Names", &globals::esp::esp_names);
                    ImGui::Checkbox("Show Boxes", &globals::esp::esp_boxes);
                    ImGui::Checkbox("Show Health", &globals::esp::esp_health);
                    ImGui::Checkbox("Show Tracers", &globals::esp::esp_tracers);
                    ImGui::Checkbox("Show Distance", &globals::esp::esp_distance);
                    ImGui::SliderFloat("ESP Max Distance", &globals::esp::esp_max_distance, 0.f, 5000.f);
                    ImGui::ColorEdit4("ESP Box Color", (float*)&globals::esp::esp_box_color);
                    ImGui::ColorEdit4("ESP Name Color", (float*)&globals::esp::esp_name_color);
                    ImGui::ColorEdit4("ESP Tracer Color", (float*)&globals::esp::esp_tracer_color);
                    ImGui::Text("FOV Settings");
                    ImGui::Checkbox("Show FOV Circle", &globals::esp::show_fov);
                    ImGui::SliderFloat("FOV Size", &globals::esp::fov_size, 10.0f, 900.0f);
                    if (globals::esp::show_fov)
                    {
                        ImGui::ColorEdit4("FOV Circle Color", (float*)&globals::esp::fov_circle_color);
                        ImGui::Checkbox("FOV Strobe", &globals::esp::fov_strobe);
                        if (globals::esp::fov_strobe)
                            ImGui::SliderFloat("Strobe Speed", &globals::esp::fov_strobe_speed, 1.0f, 50.0f);
                        ImGui::SliderFloat("FOV Thickness", &globals::esp::fov_thickness, 0.1f, 20.0f);
                        ImGui::SliderInt("FOV Segments", &globals::esp::fov_segments, 3, 50);
                        ImGui::SliderFloat("FOV Transparency", &globals::esp::fov_transparency, 0.0f, 1.0f);
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Aimbot"))
                {
                    ImGui::Checkbox("ESP Enabled", &globals::esp::enabled);
                    ImGui::Checkbox("Aimbot Enabled", &globals::esp::aimbot_enabled);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("World"))
                {
                    ImGui::Text("World Settings");
                    ImGui::Checkbox("Wallhack", &globals::wallhack_enabled);
                    ImGui::SliderFloat("Wallhack Transparency", &globals::wallhack_transparency, 0.f, 1.f);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Environment"))
                {
                    ImGui::Text("Lighting Settings");
                    ImGui::SliderFloat("Time of Day", &globals::time_of_day, 0.f, 24.f);
                    ImGui::SliderFloat("Brightness", &globals::brightness, 0.f, 10.f);
                    ImGui::ColorEdit3("Ambient", (float*)&globals::ambient);
                    ImGui::SliderFloat("FogEnd", &globals::fog_end, 0.f, 100000.f);
                    ImGui::SliderFloat("FogStart", &globals::fog_start, 0.f, 100000.f);
                    ImGui::ColorEdit3("FogColor", (float*)&globals::fog_color);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("local player"))
                {
                    ImGui::SliderFloat("JumpPower", &globals::jump_power, 16.0f, 1000.0f);
                    ImGui::SliderFloat("WalkSpeed", &globals::walk_speed, 50.0f, 320.0f);
                    ImGui::SliderFloat("Gravity", &globals::gravity, 0.0f, 2000.0f);
                    if (ImGui::Button("Reset Gravity"))
                        globals::gravity = 196.2f;
                    ImGui::Checkbox("Sitting", &globals::sitting);
                    ImGui::InputFloat3("Position", &globals::position.x);
                    if (ImGui::Button("Set Position"))
                        globals::set_position = true;

                    ImGui::Checkbox("No-clip", &globals::noclip);
                    ImGui::SliderFloat("HipHeight", &globals::hip_height, 0.f, 100.f);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("explorer")) {
                    ImGui::Text("dex explorer");
                    ImGui::Separator();
                    render_dex(instance(globals::datamodel));
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("settings")) {
                    ImGui::Text("Explorer Settings");
                    ImGui::Checkbox("Group Highlight", &globals::highlight);
                    ImGui::SliderFloat("Group Highlight Intensity", &globals::highlight_intensity, 0.01f, 1.0f);

                    ImGui::Text("FPS Settings");
                    if (ImGui::Checkbox("Lock FPS To Screen HZ", &globals::lock_fps)) {
                        if (globals::lock_fps)
                            globals::fps = get_refresh_rate();
                    }

                    if (!globals::lock_fps)
                        ImGui::SliderInt("FPS Cap", &globals::fps, 1, 240);
                }

                ImGui::EndTabBar();
            }

            ImGui::End();
        }

        POINT mouse_pos = get_mouse_pos(overlay_window);

        drawing_manager drawing;
        drawing.begin();

        if (globals::esp::enabled)
        {
            auto local_player_ptr = memory::read<uintptr_t>(globals::players + offsets::local_player);
            instance local_player(local_player_ptr);
            auto camera = instance(globals::workspace).find_first_descendant_of_class("Camera");

            if (local_player.address && camera.address)
            {
                Matrix4 view_matrix = memory::read<Matrix4>(camera.address + offsets::view_matrix);
                RECT rect{};
                GetClientRect(overlay_window, &rect);
                int screen_width = rect.right - rect.left;
                int screen_height = rect.bottom - rect.top;

                float closest_distance = FLT_MAX;
                player_info closest_player;

                for (const auto& player : instance(globals::players).get_children())
                {
                    if (player.address == local_player.address) continue;

                    auto character = player.find_first_child_of_class("Model");
                    if (!character.address) continue;

                    auto hrp = character.find_first_child("HumanoidRootPart");
                    if (!hrp.address) continue;

                    Vector3 world_pos = hrp.position();
                    Vector2 screen_pos = WorldToScreen(world_pos, view_matrix, screen_width, screen_height);

                    if (screen_pos.x != -1 && screen_pos.y != -1)
                    {
                        if (globals::esp::esp_boxes)
                            drawing.add_square(screen_pos.x - 10, screen_pos.y - 10, screen_pos.x + 10, screen_pos.y + 10, ImColor(globals::esp::esp_box_color));

                        if (globals::esp::esp_names)
                            drawing.add_text(screen_pos.x, screen_pos.y - 20, ImColor(globals::esp::esp_name_color), player.name().c_str());

                        if (globals::esp::esp_tracers)
                            drawing.add_line(screen_width / 2, screen_height, screen_pos.x, screen_pos.y, ImColor(globals::esp::esp_tracer_color));

                        if (globals::esp::esp_health)
                        {
                            auto humanoid = character.find_first_child_of_class("Humanoid");
                            if (humanoid.address)
                            {
                                float health = memory::read<float>(humanoid.address + offsets::Health);
                                float max_health = memory::read<float>(humanoid.address + offsets::MaxHealth);
                                float health_percent = health / max_health;

                                drawing.add_square(screen_pos.x - 15, screen_pos.y - 10, screen_pos.x - 12, screen_pos.y + 10, ImColor(0, 0, 0));
                                drawing.add_square(screen_pos.x - 15, screen_pos.y - 10 + (20 * (1 - health_percent)), screen_pos.x - 12, screen_pos.y + 10, ImColor(0, 255, 0));
                            }
                        }

                        float distance = std::sqrt(std::pow(screen_pos.x - screen_width / 2, 2) + std::pow(screen_pos.y - screen_height / 2, 2));
                        if (distance < closest_distance)
                        {
                            closest_distance = distance;
                            closest_player = { player.name(), player.display_name(), world_pos };
                        }
                    }
                }

                if (globals::esp::aimbot_enabled && closest_distance != FLT_MAX)
                {
                    globals::esp::target = closest_player;
                    Vector3 target_pos = globals::esp::target.position;

                    Vector2 screen_pos = WorldToScreen(target_pos, view_matrix, screen_width, screen_height);

                    if (screen_pos.x != -1 && screen_pos.y != -1)
                    {
                        float dx = screen_pos.x - screen_width / 2;
                        float dy = screen_pos.y - screen_height / 2;

                        mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
                    }
                }
            }
        }

        if (globals::esp::esp_enabled && globals::esp::show_fov)
        {
            ImVec4 fov_color = ImColor(globals::esp::fov_circle_color);
            fov_color.w = 1 - globals::esp::fov_transparency;

            if (globals::esp::fov_strobe)
            {
                globals::esp::fov_hue += globals::esp::fov_strobe_speed / 1000;
                if (globals::esp::fov_hue > 1.0f) globals::esp::fov_hue = 0.0f;

                fov_color = ImColor::HSV(globals::esp::fov_hue, 1.0f, 1.0f);
                fov_color.w = 1 - globals::esp::fov_transparency;
            }

            drawing.add_circle(mouse_pos.x, mouse_pos.y, globals::esp::fov_size, ImColor(fov_color), globals::esp::fov_segments, globals::esp::fov_thickness);
        }

        if (show_ui)
            SetWindowLongA(overlay_window, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
        else
            SetWindowLongA(overlay_window, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

        ImGui::EndFrame();
        ImGui::Render();

        float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        d3d_context->OMSetRenderTargets(1, &render_target_view, nullptr);
        d3d_context->ClearRenderTargetView(render_target_view, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        swap_chain->Present(1, 0);

        static DWORD last_time = timeGetTime();
        DWORD current_time = timeGetTime();
        float delta = (current_time - last_time) / 165.0f;
        const float target_frame_time = 1.0f / 165.0f;

        if (delta < target_frame_time)
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((target_frame_time - delta) * 1000)));

        last_time = current_time;
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    cleanup_d3d_device();
    DestroyWindow(overlay_window);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);

    return true;
}