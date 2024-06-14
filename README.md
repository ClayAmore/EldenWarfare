# WARNING
This is still in alpha so bugs are expected.

# EldenWarfare
A mod for playing Team Fights and Brawls in Elden Rings vast open world.

## Requirments
This mod is designed to work with Seamless 2.0. Make sure to get it from Seamless NexusMod page.

## Installation
1. Download the latest mod [Release](https://github.com/ClayAmore/EldenWarfare/releases).
2. Extract the files anywhere.
3. Run Launcher.exe.

## Features
* Gamemodes. Currently Team Fights and Brawls.
* Keeps score.
* Announces Greatest combatant.
* Respawns without loading screen.

## Thank You
[NordGaren](https://github.com/Nordgaren/) - For being very helpful 🙌 <br/>
[Techiew](https://github.com/Techiew/) - For their Modloader and DirectXHook. <br/>
[Dasaav](https://github.com/dasaav-dsv/) - For their RTTI Scanner and VFTHook. <br/>
Pav - Author of one the most useful EldenRing Cheat Tables. <br/>
[tremwil](https://github.com/tremwil/) - Helped me a bunch on my previous mod. That knowledge carried over. <br/>
[LukeYui](https://github.com/LukeYui/) - Author of Seamless coop. <br/>

## Compilation Notes
In `Project Properties -> Build Events -> Post-Build Event -> Command Line.`. There's a post build event that copies the DLL into the `Elden Ring/Game/mods` folder. This can cause compilation errors if your game is installed elsewhere. Remove it.
