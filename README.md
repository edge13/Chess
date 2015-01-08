# Chess
A 3D chess game in C++, for an AI project at KU.

- Minimax search with alpha-beta pruning
- OpenGL using GLUT, FMOD for sound
- Configurable "bots" for AI opponents
- Disco mode :)

![Alt text](/Screenshot.png?raw=true "Screenshot")

### Compiling the project

Navigate to the Chess directory and invoke the makefile
```
bash-3.1$ make
g++ -g -c  main.cpp
g++ -g -c  BMPLoader.cpp
g++ -g -c  bot.cpp
g++ -g -c  chess.cpp
g++ -g -c  geometry.cpp
g++ -g -c  mesh.cpp
g++ -g -c  sound.cpp
g++ -g -o chess main.o BMPLoader.o bot.o chess.o geometry.o mesh.o sound.o  -lglut -lGLU -lGL  -lXmu -lXext -lX11 -lXi -lm  lib/libfmodex.so -pthread
```

### Controls
- Use the arrow keys to move the cursor (the blue target)
- Once the cursor is positioned at the piece you want to move, press ENTER to select the piece
- Now move the cursor to the destination and press ENTER
- Press ESC to cancel your selection
- Click and drag the left mouse button to move the camera
- Right click to access menu options
  - New game - starts a new game
  - White - select who controls the white pieces, either a human player or one of several bots
  - Black - select who controls the black pieces (human or bot)
  - Disco Mode - enables the special disco mode feature
  - Exit - exits the program

Bots will play automatically, and bot settings will persist between games.
