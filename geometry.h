//===========================================================================
//
//  File name ......: geometry.h
//  Author(s) ......: Joel Angelone (base) Justin Donnell (texturing, base, disco!)
//  Language .......: C++
//  Started ........: April 15, 2007
//  Last modified ..: May 3, 2007
//  Version ........: 1.0
//  Source Path ....: .
//  Operating System: Linux, Windows
//  Purpose ........: This file handles all of the geometry for the chess program.
//  Note ...........: There are disco lights, despite Joel's anger, they will go in.
//
//===========================================================================

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
using namespace std;

#define TILE		0
#define PIECE_BLACK	1
#define PIECE_WHITE	2
#define BACKGROUND	3
#define CURSOR		4
#define TEXT_WHITE	5

#define DARK	0
#define LIGHT	1

class Chess;
class mesh;


/////////////////////////////////////////////////////////////////////////////
// Name:        geometry
// Description: This class takes care of drawing the pieces, the background,
//		the board, the base, the cursor, and any other graphic
//		related tasks.
/////////////////////////////////////////////////////////////////////////////
class Geometry
{
	public:

		// Default constructor
		Geometry();
		
		// Destructor - performs memory cleanup
		~Geometry();

		// Allows easy changing between material properties
		void SetMaterial(int type);
	
		// Manages the lighting effects in the scene
		void Lights();

		// Disco effects (sigh)
		void UpdateDiscoLights();

		// Draws the board on the screen
		void DrawBoard();

		// Draws the background
		void DrawBackground();

		// Draws all pieces
		void DrawPieces(Chess *chess);

		// Draws a pawn promotion menu
		void DrawPromotionMenu(int player);

		// Draws text
		void DrawString(char *text);

		// Draws the selection cursor for pieces
		void DrawCursor(int x, int y);
	
		// Draws the base of the board
		void DrawBase();

		// Initializes all the textures for the scene
		void Init();

	protected:

		// Draws a single tile
		void DrawTile(int type);

		// Draws a single piece
		void DrawPiece(int type);

		// mesh objects hold the piece models
		mesh *pawn;
		mesh *rook;
		mesh *knight;
		mesh *bishop;
		mesh *queen;
		mesh *king;
		mesh *base;

		// these hold our textures
		GLuint tile_dark;
		GLuint tile_light;
		GLuint background;
		GLuint baseTex;

		// keep track of the cursor position internally
		int cursorx;
		int cursory;

		// these hold the bitmaps for the promotion menu
		unsigned char* promotion_white;
		unsigned char* promotion_black;
};


#endif
