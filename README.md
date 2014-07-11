# LoL Ability Timers

## Motivation
This project was initially undertaken as a way to learn how to reverse engineer software. I thought that learning on something that 
I cared about would make it a more interesting and engaging process, so I decided to reverse engineer the game "League of Legends."

Originally I just wanted to poke around in the League of Legends program and find out how things worked, however having a goal to 
show my progress and make for more focused dissection of the league client was important. Thus I came up with the idea to make a simple
"flash timer" application because timing enemy flash cooldowns would often be neglected in my games. This was kind of inspired by the 
Curse Voice jungle and ultimate timers, and my desire to see how this worked. Eventually this idea evolved into a fully fledged ability timer.

Note that I never intended to use this program to gain an in-game advantage but was creating this purely out of self-interest in technology
and reverse engineering.

## What is it
The project contains a DLL and a DLL injector. The DLL is injected into "League of Legends.exe" and provides a way for the player to view
the current cooldowns of all abilities (summoner and regular) for every player in the game (both teams). Note that the player must have
seen the enemy cast the spell (i.e. the enemy cannot be in fog of war if you want to know that they cast a spell).

Once the DLL is injected, it hooks into some DirectX calls in order to draw text to the screen. Showing the scoreboard in game will also
show each champions current cooldowns (although the interface isn't very good looking).

## Usage instructions
Build the solution (two projects) and run LoLHook.exe after League of Legends has entered the loading screen. Once in game you should then
be able to hold tab (or whatever other keybind is used for your scoreboard) and see the ability timers for every champion in the game.
To remove this functionality you must restart the program, there are no inbuilt unhook / exit methods.

## Goals
+ Learn to reverse engineer
    + Learn x86 ASM
    + Learn common ASM structures
    + Learn how to use debuggers and disassembly tools
    + Extract useful information from a target program
+ Learn about hooking and injection
    + How to hook a function (related to x86 ASM)
    + How to use DLL injection to run code in a remote process
+ Learn / refresh Windows API knowledge
+ Refresh / expand C++ knowledge

## Extras (not in repository)
When creating this program, IDA was used for disassembly. An IDA database was created and commented as the program was made, but as it is ~1GB it is not hosted here. 
A document full of the findings made while creating the program has also not been listed here, although if anybody is interested in that information then feel free to email me.

The DirectX9 files are also required ([download the DirectX SDK] (http://www.microsoft.com/en-au/download/details.aspx?id=6812)) and must be linked to the project file.

## Legal Disclaimer
Use of this program is done entirely at the user's discretion. I take no responsibility for anyone using this program who gets punished 
in any way by Riot Games, or any other problems resulting from the use of this software.
