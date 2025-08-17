#pragma once
#include <Windows.h>

namespace offsets
{
	const uintptr_t fake_datamodel = 0x6e72138;
	const uintptr_t real_datamodel = 0x1c0;

	const uintptr_t primitive = 0x178;
	const uintptr_t position = 0x14c;

	const uintptr_t name = 0x88;
	const uintptr_t parent = 0x58;
	const uintptr_t children = 0x68;
	const uintptr_t local_player = 0x128;
	const uintptr_t display_name = 0x118;
	const uintptr_t class_descriptor = 0x18;
	const uintptr_t primitive_gravity = 0x120;
	const uintptr_t ts_max_fps = 0x6636710;
	
	const uintptr_t walk_speed = 0x1dc;
	const uintptr_t walk_speed_check = 0x3b8;
	const uintptr_t jump_power = 0x1b8;
	const uintptr_t sitting = 0x1e3;
}