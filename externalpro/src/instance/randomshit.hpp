#pragma once
#include "instance.hpp"
#include <vector>

namespace random_shit
{
    inline std::vector<instance> get_characters(instance workspace)
    {
        std::vector<instance> characters = {};
        std::vector<instance> children = workspace.children();

        for (const instance& player : children)
        {
            if (player.find_first_child("HumanoidRootPart").address != 0)
            {
                characters.push_back(player);
            }
        }

        return characters;
    }

    inline player get_player_info(instance plr)
    {
        player player_info(plr.address);

        player_info.name = plr.name();
        player_info.displayname = plr.display_name();
        player_info.position = plr.position();

        instance character = instance(globals::workspace).find_first_child(plr.name());

        auto get_part = [&](const std::string& part_name) -> uintptr_t
            {
                return character.find_first_child(part_name).address;
            };

        player_info.torso = get_part("Torso");
        player_info.head = get_part("Head");
        player_info.hrp = get_part("HumanoidRootPart");
        player_info.uppertorso = get_part("UpperTorso");
        player_info.lowertorso = get_part("LowerTorso");
        player_info.rightupperarm = get_part("RightUpperArm");
        player_info.leftupperarm = get_part("LeftUpperArm");
        player_info.rightlowerarm = get_part("RightLowerArm");
        player_info.leftlowerarm = get_part("LeftLowerArm");
        player_info.lefthand = get_part("LeftHand");
        player_info.righthand = get_part("RightHand");
        player_info.leftupperleg = get_part("LeftUpperLeg");
        player_info.rightupperleg = get_part("RightUpperLeg");
        player_info.rightlowerleg = get_part("RightLowerLeg");
        player_info.leftlowerleg = get_part("LeftLowerLeg");
        player_info.rightfoot = get_part("RightFoot");
        player_info.leftfoot = get_part("LeftFoot");
        player_info.humanoid = get_part("Humanoid");

        return player_info;
    }
}