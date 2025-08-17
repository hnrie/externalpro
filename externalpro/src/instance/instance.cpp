#include "instance.hpp"

std::string instance::name() const {
    const auto ptr = memory::read<uintptr_t>(address + offsets::name);
    return memory::get_string(ptr);
}

std::string instance::display_name() const {
    const auto ptr = memory::read<uintptr_t>(address + offsets::display_name);
    return memory::get_string(ptr);
}

instance instance::parent() const {
    return instance(memory::read<uintptr_t>(address + offsets::parent));
}

std::vector<instance> instance::children() const {
    std::vector<instance> container;

    auto start = memory::read<uint64_t>(address + offsets::children);
    auto end = memory::read<uint64_t>(start + 0x8);

    for (auto child = memory::read<uint64_t>(start); child != end; child += 0x10)
        container.emplace_back(memory::read<uint64_t>(child));

    return container;
}

instance instance::find_first_child(const std::string& childname) const {
    for (const instance& child : children()) {
        if (child.name() == childname) {
            return child;
        }
    }
    return instance(0);
}

instance instance::find_first_descendant(const std::string& target_name) const {
    for (const auto& child : children()) {
        if (child.name() == target_name)
            return child;

        const instance nested = child.find_first_descendant(target_name);
        if (nested.address)
            return nested;
    }
    return instance(0);
}

std::string instance::class_name() const {
    return memory::get_string(memory::read<uintptr_t>(memory::read<uintptr_t>(address + offsets::class_descriptor) + 0x8));
}

bool instance::isa(const std::string& class_name) const {
    return this->class_name() == class_name;
}

std::vector<instance> instance::get_descendants() const {
    std::vector<instance> container;
    for (const auto& child : children()) {
        container.push_back(child);
        auto nested = child.get_descendants();
        container.insert(container.end(), nested.begin(), nested.end());
    }
    return container;
}

instance instance::find_first_child_of_class(const std::string& class_name) const {
    for (const auto& child : children()) {
        if (child.class_name() == class_name)
            return child;
    }
    return instance(0);
}

instance instance::find_first_child_which_isa(const std::string& class_name) const {
    for (const auto& child : children()) {
        if (child.isa(class_name))
            return child;
    }
    return instance(0);
}

instance instance::find_first_descendant_of_class(const std::string& class_name) const {
    for (const auto& child : get_descendants()) {
        if (child.class_name() == class_name)
            return child;
    }
    return instance(0);
}

instance instance::find_first_descendant_which_isa(const std::string& class_name) const {
    for (const auto& child : get_descendants()) {
        if (child.isa(class_name))
            return child;
    }
    return instance(0);
}

std::vector<instance> instance::get_ancestors() const {
    std::vector<instance> container;
    instance current = parent();
    while (current.address) {
        container.push_back(current);
        current = current.parent();
    }
    return container;
}

instance instance::find_first_ancestor(const std::string& target_name) const {
    instance current = parent();
    while (current.address) {
        if (current.name() == target_name)
            return current;
        current = current.parent();
    }
    return instance(0);
}

instance instance::find_first_ancestor_of_class(const std::string& class_name) const {
    instance current = parent();
    while (current.address) {
        if (current.class_name() == class_name)
            return current;
        current = current.parent();
    }
    return instance(0);
}

instance instance::find_first_ancestor_which_isa(const std::string& class_name) const {
    instance current = parent();
    while (current.address) {
        if (current.isa(class_name))
            return current;
        current = current.parent();
    }
    return instance(0);
}

uintptr_t instance::primitive() const {
    return memory::read<uintptr_t>(address + offsets::primitive);
}

Vector3 instance::position() const {
    return memory::read<Vector3>(primitive() + offsets::position);
}

void instance::set_position(Vector3 position) const {
    memory::write<Vector3>(primitive() + offsets::position, position);
}

void instance::set_gravity(float gravity) const {
    memory::write<float>(primitive() + offsets::primitive_gravity, gravity);
}

// player shit

uintptr_t player::primitive() const {
    return memory::read<uintptr_t>(address + offsets::primitive);
}

void player::set_walkspeed(float value) const {
    memory::write<float>(instance(humanoid).address + offsets::walk_speed, value);
    memory::write<float>(instance(humanoid).address + offsets::walk_speed_check, value);
}

void player::set_jumppower(float value) const {
    memory::write<float>(instance(humanoid).address + offsets::jump_power, value);
}

void player::set_sitting(bool value) const {
    memory::write<int>(instance(humanoid).address + offsets::sitting, value ? 0 : 257);
}