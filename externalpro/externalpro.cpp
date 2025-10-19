#include <iostream>
#include <Windows.h>
#include <Psapi.h>
#include <thread>

#include "src/instance/instance.hpp"
#include "src/instance/randomshit.hpp"
#include "src/memory/memory.hpp"
#include "src/structures/structures.hpp"

#include "src/globals.hpp"
#include "src/offsets.hpp"

#include "src/overlay/overlay.hpp"

struct game_data
{
    instance datamodel;
    instance players;
    instance workspace;
};

bool find_roblox()
{
    globals::rbx = FindWindowA(0, "Roblox");
    if (!globals::rbx) return false;

    GetWindowThreadProcessId(globals::rbx, &globals::pid);
    if (globals::pid == 0) return false;

    globals::handle = OpenProcess(PROCESS_ALL_ACCESS, false, globals::pid);
    if (!globals::handle) return false;

    HMODULE modules[1024];
    DWORD real;

    if (EnumProcessModules(globals::handle, modules, sizeof(modules), &real))
    {
        globals::base = (uintptr_t)(modules[0]);
        return true;
    }

    CloseHandle(globals::handle);
    return false;
}

game_data get_datamodel()
{
    instance datamodel(memory::read<uintptr_t>(memory::read_module<uintptr_t>(offsets::fake_datamodel) + offsets::real_datamodel));
    instance players = datamodel.find_first_child("Players");
    instance workspace = datamodel.find_first_child("Workspace");

    globals::workspace = workspace.address;
    globals::players = players.address;
    globals::datamodel = datamodel.address;

    return game_data{ datamodel, players, workspace };
}

int main()
{
    if (!find_roblox())
    {
        MessageBoxA(0, "Failed to find Roblox!", "externalpro", MB_OK | MB_ICONERROR);
        return 1;
    }

    std::thread overlay_thread(&overlay_manager::initialize_overlay, overlay_instance.get());
    overlay_thread.detach();

    auto roblox = get_datamodel();
    auto datamodel = roblox.datamodel;
    auto players = roblox.players;
    auto workspace = roblox.workspace;

    while (IsWindow(globals::rbx))
    {
        if (datamodel.name() == "LuaApp")
        {
            Sleep(100);
            continue;
        }

        globals::player_list.clear();

        // Get the local player and their info
        instance player_ptr = instance(memory::read<uintptr_t>(players.address + offsets::local_player));
        player player_info = random_shit::get_player_info(player_ptr);

        // Update player properties based on global settings
        if (globals::set_position) {
            instance(player_info.hrp).set_position(globals::position);
            globals::set_position = false;
        }
        instance(player_info.hrp).set_gravity(globals::gravity);
        player_info.set_walkspeed(globals::walk_speed);
        player_info.set_jumppower(globals::jump_power);
        player_info.set_sitting(globals::sitting);

        // Update the maximum FPS
        memory::write<int32_t>(globals::base + offsets::ts_max_fps, globals::fps);

        // Sleep for a short duration to reduce CPU usage
        Sleep(1);
    }

    CloseHandle(globals::handle);
    return 0;
}