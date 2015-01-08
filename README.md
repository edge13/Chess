# Chess
A 3D chess game in C++, for an AI project at KU.

Team Girella's Chess
By Joel Angelone, Justin Hendricks, Justin Donnel, and Adam Riha

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

type ./chess to run the program.

### Controls
- Use the arrow keys to move the cursor (the blue target)
- Once the cursor is positioned at the piece you want to move, press <ENTER> to select the piece
- Now move the cursor to the destination you want to move the piece to and press <ENTER>
- If at any time you want to choose a different piece, press <ESC> and it will cancel your selection
- To move the camera, hold down the left mouse button and drag the mouse
- Right clicking will access a menu with several options:
  - New game - starts a new game
  - White - select who controls the white pieces, either a human player or one of several bots.
  - Black - select who controls the black pieces (human or bot)
  - Disco Mode - enables the special disco mode feature
  - Exit - exits the program

Bots will play automatically, and bot settings will persist between games.
