# Battlerite Aimbot and Scripts
This is a very basic external way to add aimbot and scripting to Battlerite.

Discord server for more support: https://discord.gg/qF5Hggk

[![Preview of Cheatrite on Youtube](https://img.youtube.com/vi/RPr5pDSZ7gE/0.jpg)](https://www.youtube.com/watch?v=RPr5pDSZ7gE)
https://www.youtube.com/watch?v=RPr5pDSZ7gE

# Build (Windows)
To build, clone this repository, open the solution (.sln) file and build the solution in Visual Studio under Debug x86.
The [static library](https://github.com/ih1115/CheatRite/tree/master/Core) will automatically build in your solution directory and copy headers to their appropriate folders.
[Cheatrite](https://github.com/ih1115/CheatRite/tree/master/Hack) will build in your solution directory under the folder "binaries".

# Notes and changes
I'll try to update this as soon as I can.

#### Todo (General):
- [x] Edit controls
- [x] Automatically get screen size
- [x] Add compensation for locked and unlocked screen ([EXPERIMENTAL, READ THIS](https://github.com/ih1115/Cheatrite/blob/master/README.md#experimental))
- [ ] Add more smooth aim to look less suspicious
- [ ] Add aim prediction
- [ ] Draw a map overlay to the window to show location of people in FoW
- [ ] Get orb entities and draw timers on screen

#### Todo (Scripts):
- [ ] Bakko Scripts
- [ ] Thorn Scripts
- [ ] Improve Jade Scripts
- [ ] Improve Raigon Scripts

#### Mouse Offset
Originally, you had to change the "offset", or as some of you might know, the "69" until you locked on to the enemy.
In this version, you can simply press the + and - key on your numpad to increase and decrease the "offset". Go into a practice game versus a bot, 1v1, and press + and - until your cursor is on the character at all times.
Right now, the offset does not save anywhere, so you will have to manually edit it everytime you reopen the game and Cheatrite.

#### Controls
Left Control Key: Enable / Disable Aimbot (enabled by default)
Caps Lock Key: Enable / Disable Scripts (enabled by default)
Numpad + Key: Increase aimbot offset
Numpad - Key: Decrease aimbot offset
Middle Mouse Button: Change offset from locked and unlocked ([EXPERIMENTAL, READ THIS](https://github.com/ih1115/Cheatrite/blob/master/README.md#experimental))

#### Using different scripts
Upon running, you can choose whatever script you wish to use.

# Run
Pretty simple to run, open Battlerite and then open Aimrite.
Either build the solution yourself, or go to [releases](https://github.com/ih1115/CheatRite/releases) and download the latest release.

#### Game Configuration
Please set your ingame settings to minimum and disable ```accelerate input``` under ```Gameplay``` then restart the game ([Issue])(https://github.com/ih1115/CheatRite/issues/7)

# Issues
Hack not working? [Issue #2](https://github.com/ih1115/CheatRite/issues/2) and [Issue #7](https://github.com/ih1115/CheatRite/issues/7)

# Experimental
Added compensation for switching from locked to unlocked camera view, however, it's a hardcoded number, not calculated in the slightest. It may be way off, or it may be on point.
Make sure your camera is default LOCKED. When in game, press the middle mouse button to tell the aimbot to switch to unlocked camera and vice versa. Also change your controls in Battlerite from making camera locked and unlocked to the middle mouse button. They should work perfectly together.

