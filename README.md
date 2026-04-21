# TS2 Extender
 
A mod that greatly expands The Sims 2 Legacy Collection by fixing several issues, implementing new features, enhancing modding support and tweaks.

Tested to work with the Steam version.

## Features

* Skip intro videos.
* Adds completely new Lua functionality for modders, and a text Lua script loader for easier modding.
* Allows you to use separate top/bottom clothes in outfit categories other than just Everyday, if you have the CC for it. Can be enabled via the .ini.

## Fixes

* Fixes a Maxis oversight that caused pink flashing on 64 bit computers. Thanks to [@spockthewok](https://github.com/spockthewok) for discovering this.
* Fixes the game's random number generator, solving Firstborn Syndrome and other random number quirks.
* Fixes Open For Business uniforms messing up employee's faces.
* Fixes broken date stood up timer.
* Fixes aging and other timed events randomly breaking permanently in saves.
* Fixes broken wall cutaway textures.

## Lua Features

Check out the [Documentation](https://lazyduchess.github.io/TS2-Extender-Docs) if you're interested in the new features for Lua script modding. You can do a lot more now!

## Installation

* Firstly, download the latest version from the [releases](https://github.com/LazyDuchess/TS2-Extender/releases/latest) tab.
* Extract the zip. Inside you will find "TSBin", "Downloads", "TSData" and "Lua" folders.
* The "TSBin" and "TSData" folders go into your game's installation directory -> EP9, overriding the folders that are already there. Example: "E:\Steam\steamapps\common\The Sims 2 Legacy Collection\EP9"
* Note: If you had a version of TS2 Extender already installed older than 0.8.0, you might have a Lua folder in TSBin. Remove it, as this folder now lives in the Documents folder below.
* The "Downloads" and "Lua" folders go into your "Documents/EA Games/The Sims 2 Legacy" folder. Make sure custom content is enabled in your in-game settings.
* If everything went well, you should have the new features in your game, and you will also have a new cheat command: "ts2extender", which will tell you the current TS2 Extender version (Cheat console can be opened with Shift+Control+C) :
  <img width="1912" height="232" alt="image" src="https://github.com/user-attachments/assets/20dbb242-33c3-40e6-8e1f-65a0cacab284" />


The mod can be configured via the .ini file.
