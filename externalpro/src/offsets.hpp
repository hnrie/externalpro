#pragma once
#include <Windows.h>

namespace offsets
{
	// DataModel
	const uintptr_t fake_datamodel = 0x6e72138; // Pointer to the fake DataModel
	const uintptr_t real_datamodel = 0x1c0;     // Offset to the real DataModel

	// Instance properties
	const uintptr_t primitive = 0x178;        // Offset to the primitive property
	const uintptr_t position = 0x14c;         // Offset to the position property
	const uintptr_t name = 0x88;              // Offset to the name property
	const uintptr_t parent = 0x58;            // Offset to the parent property
	const uintptr_t children = 0x68;          // Offset to the children property
	const uintptr_t local_player = 0x128;     // Offset to the local player
	const uintptr_t display_name = 0x118;     // Offset to the display name
	const uintptr_t class_descriptor = 0x18;  // Offset to the class descriptor
	const uintptr_t primitive_gravity = 0x120;// Offset to the primitive gravity

	// TaskScheduler
	const uintptr_t ts_max_fps = 0x6636710;    // Offset to the maximum FPS setting

	// Player properties
	const uintptr_t walk_speed = 0x1dc;       // Offset to the walk speed property
	const uintptr_t walk_speed_check = 0x3b8; // Offset to the walk speed check
	const uintptr_t jump_power = 0x1b8;       // Offset to the jump power property
	const uintptr_t sitting = 0x1e3;          // Offset to the sitting property
}