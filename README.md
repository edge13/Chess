# Chess
A 3D chess game in C++, for an AI project at KU.

Team Girella's Chess
By Joel Angelone, Justin Hendricks, Justin Donnel, and Adam Riha

Instructions for unpacking the project:
	
Navigate to the directory containing the TAR file in the terminal and type the following:
	
tar xvf Chess.tar.gz

This will unpack the project.

Example:

>bash-3.1$ tar xvf Chess.tar.gz
Chess/Audio/Stalemate.wav
Chess/Audio/Move.wav
Chess/Audio/Cursor.wav
Chess/Audio/IllegalMove.wav
Chess/Audio/Capture.wav
Chess/Audio/Checkmate.wav
Chess/Audio/Background.mp3
Chess/Audio/Check.wav
Chess/Audio/Disco.mp3
Chess/AI/jc.bot~
Chess/AI/fivePly.bot
Chess/AI/jc.bot
Chess/AI/joel.bot
Chess/AI/justin.bot
Chess/AI/onePly.bot
Chess/AI/riha.bot
Chess/AI/threePly.bot
Chess/inc/fmod.hpp
Chess/inc/fmod.h
Chess/inc/fmod_codec.h
Chess/inc/fmod_errors.h
Chess/inc/fmod_dsp.h
Chess/inc/fmod_output.h
Chess/lib/libfmodexp.so.4.06.16
Chess/lib/libfmodex.so.4.06.16
Chess/lib/libfmodex.so
Chess/lib/libfmodexp.so
Chess/Graphics/wood.bmp
Chess/Graphics/boardVer3Tex.obj
Chess/Graphics/tile_light.bmp
Chess/Graphics/tile_dark.bmp
Chess/Graphics/rook.obj
Chess/Graphics/riha_promotion.bmp
Chess/Graphics/queen.obj
Chess/Graphics/promotion_white.bmp
Chess/Graphics/promotion_black.bmp
Chess/Graphics/pawn.obj
Chess/Graphics/knight.obj
Chess/Graphics/king.obj
Chess/Graphics/bishop.obj
Chess/Graphics/background.bmp
Chess/makefile
Chess/sound.h
Chess/sound.cpp
Chess/mesh.h
Chess/mesh.cpp
Chess/main.cpp
Chess/geometry.h
Chess/geometry.cpp
Chess/chess.h
Chess/chess.cpp
Chess/bot.h
Chess/bot.cpp
Chess/BMPLoader.h
Chess/BMPLoader.cpp

Instructions for compiling the project:

Now navigate to the Chess directory and type "make"

Example:
bash-3.1$ cd Chess
bash-3.1$ make
g++ -g -c  main.cpp
g++ -g -c  BMPLoader.cpp
g++ -g -c  bot.cpp
g++ -g -c  chess.cpp
g++ -g -c  geometry.cpp
g++ -g -c  mesh.cpp
g++ -g -c  sound.cpp
g++ -g -o chess main.o BMPLoader.o bot.o chess.o geometry.o mesh.o sound.o  -lglut -lGLU -lGL  -lXmu -lXext -lX11 -lXi -lm  lib/libfmodex.so -pthread

Instructions for running the project:

type ./chess to run the program.

Program controls:
-Use the arrow keys (UP, DOWN, LEFT, RIGHT) on the keyboard to move the cursor (The blue target)
-Once the cursor is positioned at the piece you want to move, press <ENTER> to select the piece
-Now move the cursor to the destination you want to move the piece to and press <ENTER>
-If at any time you want to choose a different piece, press <ESC> and it will cancel your selection
-To move the camera, hold down the left mouse button and drag the mouse
-Right clicking will access a menu with several options:
  - New game - starts a new game
  - White - select who controls the white pieces, either a human player or one of several bots.
  - Black - select who controls the black pieces (human or bot)
  - Disco Mode - enables the special disco mode feature
  - Exit - exits the program

When bots are selected for either color, they will play automatically when it is their turn.
Bot settings will be saved even when the new game option is chosen.
Note that, depending on the complexity of the bot, it may take several seconds to move, during
which time interactivity with the program will be limited.
