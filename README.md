# externalpro

**externalpro** is a powerful external tool for the game Roblox, designed to enhance the user experience with a wide range of features. It operates by reading and writing to the game's memory, providing real-time modifications and information overlays. The tool is built in C++ and uses ImGui for its in-game overlay.

## Features

**externalpro** comes packed with a variety of features, all accessible through a clean and intuitive in-game menu.

### ESP (Extrasensory Perception)

The ESP allows you to see other players and gain crucial information about them.
- **Enabled:** Toggles all ESP features on or off.
- **Boxes:** Draws a box around each player.
- **Names:** Displays the username of each player.
- **Health:** Shows a health bar for each player.
- **Tracers:** Draws a line from the bottom of your screen to each player.
- **Distance:** (Currently not implemented) Will show the distance to each player.
- **Customization:** You can customize the colors of boxes, names, and tracers.

### Aimbot

The aimbot helps you aim at other players with precision.
- **Enabled:** Toggles the aimbot on or off. When enabled, it will automatically aim at the player closest to your crosshair.

### Player Modifications

These features modify your own character's properties.
- **WalkSpeed:** Adjust your movement speed.
- **JumpPower:** Control the height of your jumps.
- **No-clip:** Allows you to fly and move through solid objects.
- **HipHeight:** Change your character's height.
- **Gravity:** Modify the gravitational pull on your character.
- **Set Position:** Teleport to specific coordinates.

### World Modifications

Alter the game world to your advantage.
- **Wallhack:** Makes all solid objects in the world transparent, allowing you to see through them. You can adjust the transparency level.

### Environment Controls

Fine-tune the visual environment of the game.
- **Time of Day:** Change the time to anywhere between midnight and midnight.
- **Brightness:** Adjust the overall brightness of the scene.
- **Ambient Light:** Change the color of the ambient lighting.
- **Fog:** Control the start and end distance of the fog, as well as its color.

### Client Settings

- **FPS Unlocker:** Unlocks the framerate, allowing you to play at a higher FPS than the default cap.

## How to Build and Run

To build and run **externalpro**, you will need:
- Visual Studio with the C++ toolset.
- The project should be configured to build for the x64 architecture.

1.  Clone the repository to your local machine.
2.  Open the `externalpro.sln` file in Visual Studio.
3.  Set the build configuration to **Release | x64**.
4.  Build the solution (F7 or Build > Build Solution).
5.  Once built, you can run the resulting `.exe` file while Roblox is running.

## How to Use

Once the tool is running and has successfully attached to the Roblox process, the in-game overlay can be opened and closed by pressing the **INSERT** key. From there, you can navigate through the different tabs to access and control all the features.

## A Note on Offsets

This tool relies on a set of hardcoded memory offsets to find the necessary values in the game's memory. These offsets can change with every new update to Roblox. If the tool stops working, it is very likely that the offsets need to be updated in the `externalpro/src/offsets.hpp` file.
