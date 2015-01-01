lightship
=========
A cross-platform multi-player 3D arcade game implemented entirely in plain C 
and OpenGL.

Features
--------
+ Plugin API allowing high modability of the game.
+ OpenGL 3.3
+ Events and services based communications system supporting advanced features 
  like event interception and service discovery, making it possible for plugins 
  to re-define game mechanics on the fly.

How to Build
------------
The tools you will need to build this project are:
+ [CMake](http://www.cmake.org/).
+ A C89 compliant C compiler.
+ [Git](http://git-scm.com/) (if you want to make to make updating the code easy).

**Windows MSVC Developers** should start the "Visual Studio Command Prompt" from the
start menu instead of CMD. It sets the required environment variables. If you are
using MSYS then you can use regular CMD (or bash).

The general procedure is as follows:
+ Make a new directory called *build*.
+ cd into that directory with your command prompt/terminal/whatever.
+ Type ```cmake ..``` (Two dots are important)

Depending on what platform you're on, there will now be Visual Studio project
files, Makefiles, or whatever your default compiler needs inside the *build*
directory. So go in there and open them up. *nix users now type *make* to build.

Set the target "lightship" as the executable target and build. You're good to go.

Concept
-------
You are given a 2D grid of tiles. You drive around on top of these tiles, 
thereby claiming them to be your own. If an enemy drives over the tiles you 
claimed, then they no longer belong to you, so you'll have to re-claim them. 
The goal of the game is to have claimed more tiles than the enemy when the game 
timer runs out.

To make things more interesting, we throw in power ups, teleporters, special 
tiles, weapons, you name it.

The whole game is geared towards being played on-line and some maps can be 
designed to support 20+ players simultaneously.

Game Modes
----------
Different game modes exist, to name a few:
+ **FFA**: Every man for himself.
+ **Team FFA**: Have X number of teams with Y number of players per team.
+ **Plague**: Two teams, starts off as one person (the plague team) versus 
  everyone else. If you get surrounded by too many tiles belonging to the 
  plague team, you start playing for the plague. The plague wins if every 
  player is assimilated within a certain amount of time.
+ **Map Control**: Same as team FFA, except the team who has claimed most of 
  the map at a given point in time is the only team who will incrementally gain 
  points.
+ **Sudden Death**: If you fall off the map, you will not re-spawn.

Power Ups
---------
+ **Black Hole**: Deploy a bomb which when detonated creates a black hole and 
  sucks all surrounding players into it. The players are released when the 
  black hole expires.
+ **Jump**: Have the ability to jump up onto places.
+ **Speed**: Increase your speed by 150%.
+ **Slowdown**: Decrease everyone elses speed to 50%.
+ **Force Field**: Deploy a force field only you can pass through.
+ **Size**: Increase your size by 3, making you claim 9 tiles at a time.
+ **Dash**: Dash a short distance at high speed. Knocks players far away.

Prototype
---------
A playable prototype exists and can be 
downloaded [here](http://forum.thegamecreators.com/?m=forum_view&t=195801&b=8).

The prototype was written in DarkBASIC Pro and demonstrates the main game 
mechanics. The DBPro source code can be downloaded [here](https://bitbucket.org/TheComet/lightship/downloads).

![slow down power up](http://i.imgur.com/euHzZuR.png)

![force field power up](http://i.imgur.com/VI9zm9F.png)

![level editor](http://i.imgur.com/SnlExXN.png)
