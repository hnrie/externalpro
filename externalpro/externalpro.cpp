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

struct dm
{
    instance datamodel;
    instance players;
    instance workspace;
};

void find_roblox()
{
    globals::rbx = FindWindowA(0, "Roblox");
    GetWindowThreadProcessId(globals::rbx, &globals::pid);
    globals::handle = OpenProcess(PROCESS_ALL_ACCESS, false, globals::pid);

    HMODULE modules[1024];
    DWORD real;

    if (EnumProcessModules(globals::handle, modules, sizeof(modules), &real))
        globals::base = (uintptr_t)(modules[0]);
}

dm get_datamodel()
{
    instance datamodel(memory::read<uintptr_t>(memory::read_module<uintptr_t>(offsets::fake_datamodel) + offsets::real_datamodel));
    instance players = datamodel.find_first_child("Players");
    instance workspace = datamodel.find_first_child("Workspace");

    globals::workspace = workspace.address;
    globals::players = players.address;
    globals::datamodel = datamodel.address;

    return dm{ datamodel, players, workspace };
}

int main()
{
    find_roblox();

    std::thread(&overlay_manager::initialize_overlay, overlay_instance.get()).detach();

    start:

    while (true)
    {
        auto shit = get_datamodel();
        auto datamodel = shit.datamodel;
        auto players = shit.players;
        auto workspace = shit.workspace;

        if (datamodel.name() == "LuaApp") goto start;

        globals::player_list.clear();

        instance player_ptr = instance(memory::read<uintptr_t>(players.address + offsets::local_player));
        player player_info = random_shit::get_player_info(player_ptr);

        std::vector<instance> characters = random_shit::get_characters(workspace);
        for (const auto& character : characters)
        {
            instance other_player_instance = players.find_first_child(character.name());
            player other_player_info = random_shit::get_player_info(other_player_instance);

            std::string display_name = "";

            if (other_player_info.displayname != other_player_info.name || !other_player_info.displayname.empty())
                display_name = other_player_info.displayname;

            globals::player_list.push_back({ character.name(), display_name, instance(other_player_info.hrp).position() });
        }

        if (globals::set_position) {
            instance(player_info.hrp).set_position(globals::position); 
            globals::set_position = false;
        }
        player_info.set_walkspeed(globals::walk_speed);
        player_info.set_jumppower(globals::jump_power);
        player_info.set_sitting(globals::sitting);
    }

    CloseHandle(globals::handle);
}