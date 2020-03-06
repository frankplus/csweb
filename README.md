# CSWEB
CSPSP was a homebrew 2D video game based on counter strike originally developed for PSP by 
@kevinbchen : 
- [link to the repository](https://github.com/kevinbchen/cspsp) 
- [link to the game website](https://cspsp.appspot.com/)

This is a porting to run the game on the web using webassembly and emscripten to compile the code. 
I have rewritten the game engine to support webGL/opengles 3.

Live demo: https://frankinfotech.it/csweb/

## Features
- Working rendering using opengles3 drawing polygons, polylines, circles...
- Sprite renderer using shaders
- keyboard support using WASD to move QE to rotate the player, space to conferm/shoot, H to reload, K to change weapon.
- Working sound engine using SDL_mixer

##TODO
- implement mouse support to rotate the player
- implement mouse support for Gui control
- implement online multiplayer support
