#pragma once
#include <string>
#include <vector>
#include "../memory/memory.hpp"
#include "../offsets.hpp"
#include "../structures/structures.hpp"

class instance {
public:
    uintptr_t address;

    instance() : address(0) {}
    explicit instance(uintptr_t address) : address(address) {}

    std::string name() const;
    std::string display_name() const;
    instance parent() const;
    std::vector<instance> children() const;
    instance find_first_child(const std::string& childname) const;
    instance find_first_descendant(const std::string& target_name) const;
    std::vector<instance> get_descendants() const;
    instance find_first_child_of_class(const std::string& class_name) const;
    instance find_first_child_which_isa(const std::string& class_name) const;
    instance find_first_descendant_of_class(const std::string& class_name) const;
    instance find_first_descendant_which_isa(const std::string& class_name) const;
    std::vector<instance> get_ancestors() const;
    instance find_first_ancestor(const std::string& target_name) const;
    instance find_first_ancestor_of_class(const std::string& class_name) const;
    instance find_first_ancestor_which_isa(const std::string& class_name) const;
    std::string class_name() const;
    bool isa(const std::string& class_name) const;
    uintptr_t primitive() const;
    Vector3 position() const;
    void set_position(Vector3 position) const;
};

class player {
public:
    uintptr_t address;

    player(uintptr_t addy) : address(addy) {}

    uintptr_t torso;
    uintptr_t head;
    uintptr_t hrp;
    uintptr_t uppertorso;
    uintptr_t lowertorso;
    uintptr_t rightupperarm;
    uintptr_t leftupperarm;
    uintptr_t rightlowerarm;
    uintptr_t leftlowerarm;
    uintptr_t lefthand;
    uintptr_t righthand;
    uintptr_t leftupperleg;
    uintptr_t rightupperleg;
    uintptr_t rightlowerleg;
    uintptr_t leftlowerleg;
    uintptr_t rightfoot;
    uintptr_t leftfoot;
    uintptr_t humanoid;
    uintptr_t main;

    void set_walkspeed(float value) const;
    void set_jumppower(float value) const;
    void set_sitting(bool value) const;

    Vector3 position;
    int maxhealth;
    int health;
    std::string name;
    std::string displayname;
    instance userid;
    int status;
};