# Project Aurora

Aurora is a game prototype with an isometric view that I developed for learning purposes.

The entire game logic is in 3D.
The terrain is a 3D mesh, where we can apply individual textures to each tile.
The characters and scene objects are 2D sprites.

Graphics are rendered using OpenGL.

# Dependencies

To compile, you need a C++23 capable compiler and the following libraries:

- SDL
- OpenGL
- My-lib (https://github.com/ehmcruz/my-lib).
- My-game-library (https://github.com/ehmcruz/my-game-lib)

The Makefile is configured to search **my-lib** and **my-game-lib** in the same parent folder as this repository. If you put somewhere else, just modify the Makefile.

# Compiling

I only tested in Linux, but should compile on any system that has the required dependencies.

**make MYGLIB_TARGET_LINUX=1**

# Downloading assets

I don't like to store assets in GitHub.
So download them from my drive:

**https://drive.google.com/drive/folders/1cIVI_gKErJk4YB9KU44Fzt0wOR8VQL85?usp=sharing**

Just put the assets folder in project's root directory.

# Running

**./aurora**