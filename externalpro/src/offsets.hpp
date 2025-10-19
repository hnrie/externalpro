#pragma once
#include <Windows.h>

namespace offsets
{
	// DataModel
	const uintptr_t fake_datamodel = 0x7268A88; // FakeDataModelPointer
	const uintptr_t real_datamodel = 0x1C0;     // RealDataModel

	// Instance properties
	const uintptr_t primitive = 0x170;        // Primitive
	const uintptr_t position = 0x11C;         // Position
	const uintptr_t name = 0x80;              // Name
	const uintptr_t parent = 0x50;            // Parent
	const uintptr_t children = 0x60;          // Children
	const uintptr_t local_player = 0x120;     // LocalPlayer
	const uintptr_t display_name = 0x110;     // DisplayName
	const uintptr_t class_descriptor = 0x18;  // ClassDescriptor
	const uintptr_t primitive_gravity = 0x1AC;// Gravity

	// TaskScheduler
	const uintptr_t ts_ptr = 0x75BE1B8;		// TaskSchedulerPointer
	const uintptr_t ts_max_fps = 0x1B0;    // MaxFPS

	// Player properties
	const uintptr_t walk_speed = 0x1D4;       // Walkspeed
	const uintptr_t walk_speed_check = 0x3A0; // WalkspeedCheck
	const uintptr_t jump_power = 0x1B0;       // JumpPower
	const uintptr_t sitting = 0x1e3;          // No new offset for sitting, keeping the old one.

	// Camera properties
	const uintptr_t cframe = 0x1C0;           // CFrame
	const uintptr_t view_matrix = 0x4B0;      // ViewMatrix
}
