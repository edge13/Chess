//===========================================================================
//
//  File name ......: geometry.cpp
//  Author(s) ......: Joel Angelone (base) Justin Donnell (texturing, base, disco!)
//  Language .......: C++
//  Started ........: April 15, 2007
//  Last modified ..: May 3, 2007
//  Version ........: 1.0
//  Source Path ....: .
//  Operating System: Linux, Windows
//  Purpose ........: Implementation of Geometry routines.
//  Note ...........: 
//
//===========================================================================

#include <GLUT/glut.h>
#include <stdlib.h>
#include "BMPLoader.h"
#include "geometry.h"
#include "mesh.h"
#include "chess.h"

/****************************************************************************
 * Name:        Geometry
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This is the default constructor for the Geometry.  All it does
 *		is set the initial location of the cursor.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
Geometry::Geometry()
{
	// Set the cursor to the middle of the board
	cursorx = 3;
	cursory = 3;
}

/****************************************************************************
 * Name:        ~Geometry
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This deletes all the mesh files and cleans up the class.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
Geometry::~Geometry()
{
	// Delete the pawn
	if (pawn)
		delete pawn;

	// Delete the rook
	if (rook)
		delete rook;

	// Delete the knight
	if (knight)
		delete knight;

	// Delete the bishop
	if (bishop)
		delete bishop;

	// Delete the queen
	if (queen)
		delete queen;

	// Delete the king
	if (king)
		delete king;
}

/****************************************************************************
 * Name:        Init
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This function initializes all the loading of the texture
 *		and the meshes.
 * Invokes:     None
 * Note:        None.
 ***************************************************************************/
void Geometry::Init()
{

	// Load the light tile bitmap
	if (loadOpenGL2DTextureBMP("Textures/tile_light.bmp", &tile_light, GL_RGB) != LOAD_TEXTUREBMP_SUCCESS)
	{
		cout << "Error loading texture: tile_light.bmp" << endl;
	}

	// Bind the texture to the ID
	glBindTexture(GL_TEXTURE_2D, tile_light);

	// Set some parameters
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	// Load the dark tile bitmap
	if (loadOpenGL2DTextureBMP("Textures/tile_dark.bmp", &tile_dark, GL_RGB) != LOAD_TEXTUREBMP_SUCCESS)
	{
		cout << "Error loading texture: tile_dark.bmp" << endl;
	}

	// Bind the texture to the ID
	glBindTexture(GL_TEXTURE_2D, tile_dark);

	// Set some parameters
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	// Load the background bitmap
	if (loadOpenGL2DTextureBMP("Textures/background.bmp", &background, GL_RGB) != LOAD_TEXTUREBMP_SUCCESS)
	{
		cout << "Error loading texture: background.bmp" << endl;
	}

	// Bind the texture to the ID
	glBindTexture(GL_TEXTURE_2D, background);

	// Set some parameters
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	// Load the pawn mesh
	pawn = new mesh("Models/pawn.obj");

	// Load the rook mesh
	rook = new mesh("Models/rook.obj");

	// Load the knight mesh
	knight = new mesh("Models/knight.obj");

	// Load the bishop mesh
	bishop = new mesh("Models/bishop.obj");

	// Load the queen mesh
	queen = new mesh("Models/queen.obj");

	// Load the king mesh
	king = new mesh("Models/king.obj");

	// Load the base mesh
	base = new mesh("Models/boardVer3Tex.obj");

	// Load the wood bitmap (used for base)
	if (loadOpenGL2DTextureBMP("Textures/wood.bmp", &baseTex, GL_RGB) != LOAD_TEXTUREBMP_SUCCESS)
	{
		cout << "Error loading texture: wood.bmp" << endl;
	}

	// Bind the texture to the ID
	glBindTexture(GL_TEXTURE_2D, baseTex);

	// Set some parameters
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	// Load white's promotion menu bitmap
	if(loadOpenGL2DBMP("Textures/promotion_white.bmp", &promotion_white, GL_RGB) != LOAD_TEXTUREBMP_SUCCESS)
	{
		cout << "Error loading image file: promotion_white.bmp" << endl;
	}

	// Load black's promotion menu bitmap
	if(loadOpenGL2DBMP("Textures/promotion_black.bmp", &promotion_black, GL_RGB) != LOAD_TEXTUREBMP_SUCCESS)
	{
		cout << "Error loading image file: promotion_black.bmp" << endl;
	}

	cout << endl;
}

/****************************************************************************
 * Name:        SetMaterial
 * Input:       int - type
 * Output:      None
 * Returns:     None
 * Description: This function takes a type which coorelates to either a tile,
 *		a black piece / white piece, the background, the cursor, or
 *		the text and sets the material property of that object.
 * Invokes:     None
 * Note:        Must be called before object is drawn.
 ***************************************************************************/
void Geometry::SetMaterial(int type)
{

	// Set the material for the TILE
	if (type == TILE)
	{
		GLfloat amb[] = {0.1, 0.1, 0.1, 1.0 };
		GLfloat diff[] = {1.0, 1.0, 1.0, 1.0};
		GLfloat spec[] = { 1.0, 1.0, 1.0, 1.0};
		GLfloat emiss[] = {0.0, 0.0, 0.0, 1.0};
		GLfloat shine = 20.0;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emiss);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
	}

	// Set the material for the black piece
	else if (type == PIECE_BLACK)
	{
		GLfloat amb[] = {0.1, 0.1, 0.1, 1.0};
		GLfloat diff[] = {0.13, 0.13, 0.13, 1.0};
		GLfloat spec[] = { 0.2, 0.2, 0.2, 1.0};
		GLfloat emiss[] = {0.0, 0.0, 0.0, 1.0};
		GLfloat shine = 1.0;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emiss);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
	}

	// Set the material for the white piece
	else if (type == PIECE_WHITE)
	{
		GLfloat amb[] = {0.6, 0.6, 0.6, 1.0};
		GLfloat diff[] = {0.8, 0.8, 0.8, 1.0};
		GLfloat spec[] = { 0.8, 0.8, 0.8, 1.0};
		GLfloat emiss[] = {0.0, 0.0, 0.0, 1.0};
		GLfloat shine = 1.0;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emiss);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
	}

	// Set the material of the background
	else if (type == BACKGROUND)
	{
		GLfloat amb[] = {1.0, 1.0, 1.0, 1.0 };
		GLfloat diff[] = {1.0, 1.0, 1.0, 1.0};
		GLfloat spec[] = { 1.0, 1.0, 1.0, 1.0};
		GLfloat emiss[] = {0.0, 0.0, 0.0, 1.0};
		GLfloat shine = 10.0;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emiss);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
	}

	// Set the material of the cursor
	else if (type == CURSOR)
	{
		GLfloat amb[] = {0.1, 0.1, 0.3, 1.0 };
		GLfloat diff[] = {0.0, 0.0, 0.9, 1.0};
		GLfloat spec[] = { 0.0, 0.0, 0.9, 1.0};
		GLfloat emiss[] = {0.0, 0.0, 0.9, 1.0};
		GLfloat shine = 20.0;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emiss);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
	}

	// Set material of the text
	else if (type == TEXT_WHITE)
	{
		GLfloat amb[] = {1.0, 1.0, 1.0, 1.0};
		GLfloat diff[] = {1.0, 1.0, 1.0, 1.0};
		GLfloat spec[] = { 1.0, 1.0, 1.0, 1.0};
		GLfloat emiss[] = {1.0, 1.0, 1.0, 1.0};
		GLfloat shine = 1.0;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emiss);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
	}
}

/****************************************************************************
 * Name:        Lights
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This sets the default lighting for the scene.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void Geometry::Lights()
{
	// Set the attributes for the default light (LIGHT 0)
	GLfloat position[] = {10.0, 100.0, 10.0, 0.0};
	GLfloat diffuse[] = {0.85, 0.85, 0.85, 1.0};
	GLfloat specular[] = {0.85, 0.85, 0.85, 1.0};
	GLfloat ambient[] = {0.1, 0.1, 0.1, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Set the attributes for light one
	GLfloat lightPos1[]={ 0.0, 0.05, 0.0, 0.0};
	GLfloat ambientLight1[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat diffuseLight1[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat specularLight1[] = {1.0, 1.0, 1.0, 1.0};

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

	// Set the attributes for light two
	GLfloat lightPos2[]={ 0.0, 0.05, 0.0, 0.0};
	GLfloat ambientLight2[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat diffuseLight2[] = {0.4, 0.4, 0.4, 1.0};
	GLfloat specularLight2[] = {0.4, 0.4, 0.4, 1.0};

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specularLight2);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPos2);

	// Disable lights 3 and 4 (disco lights)
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);

}

void Geometry::UpdateDiscoLights()
{
	// Have a static variable for the rotation
	static int rotate = 0;
	rotate += 10;

	// Enable the disco lights
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);

	glPushMatrix();

	// Rotate the lights
	glRotatef(rotate, 0.0, 1.0, 0.0);

	// Set the lights attributes
	GLfloat lightPos3[] = {0.0, 300.0, 0.0, 0.0};
	GLfloat ambientLight3[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat diffuseLight3[] = { (rand()%256)/256.0, 
		(rand()%256)/256.0, (rand()%256)/256.0, 1.0};
	GLfloat specularLight3[] = {(rand()%256)/256.0, 
		(rand()%256)/256.0, (rand()%256)/256.0, 1.0};

	glLightfv(GL_LIGHT3, GL_AMBIENT, ambientLight3);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuseLight3);
	glLightfv(GL_LIGHT3, GL_SPECULAR, specularLight3);

	glLightfv(GL_LIGHT3, GL_POSITION, lightPos3);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-rotate, 0.0, 0.0, 1.0);
	GLfloat lightPos4[] = {0.0, 300.0, 0.0, 0.0};
	GLfloat ambientLight4[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat diffuseLight4[] = { (rand()%256)/256.0, 
		(rand()%256)/256.0, (rand()%256)/256.0, 1.0};
	GLfloat specularLight4[] = {(rand()%256)/256.0, 
		(rand()%256)/256.0, (rand()%256)/256.0, 1.0};

	glLightfv(GL_LIGHT4, GL_AMBIENT, ambientLight4);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, diffuseLight4);
	glLightfv(GL_LIGHT4, GL_SPECULAR, specularLight4);

	glLightfv(GL_LIGHT4, GL_POSITION, lightPos4);
	glPopMatrix();
}

/****************************************************************************
 * Name:        DrawTile
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This draws a tile with material property type.
 * Invokes:     None
 * Note:        None.
 ***************************************************************************/
void Geometry::DrawTile(int type)
{
	// Enable texturing for the tile
	glEnable(GL_TEXTURE_2D);

	// Load the dark tile's properities
	if (type == DARK)
	{
		SetMaterial(TILE);
		glBindTexture(GL_TEXTURE_2D, tile_dark);
	}

	// Load the light tile's properities
	else
	{
		SetMaterial(TILE);
		glBindTexture(GL_TEXTURE_2D, tile_light);
	}

	// Light should effect the tiles
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Set the normal value
	glNormal3f(0.0, 1.0, 0.0);

	// Draw our textured quad
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(4.0, 0.0, 0.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(4.0, 0.0, 4.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(0.0, 0.0, 4.0);
	glEnd();

	// Disable texturing
	glDisable(GL_TEXTURE_2D);
}

/****************************************************************************
 * Name:        DrawBoard
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This draws the individual squares of the board.
 * Invokes:     DrawTile
 * Note:        None.
 ***************************************************************************/
void Geometry::DrawBoard()
{
	// Start with a light tile
	int type = LIGHT;

	glPushMatrix();

	// Translate the board
	glTranslated(-16.0, 0.0, -16.0);

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{

			// We want LIGHT1 to be over the current active piece
			if(cursorx == j && cursory == i)
				glEnable(GL_LIGHT1);

			// Otherwise we want LIGHT2 to be enabled
			else
				glEnable(GL_LIGHT2);

			// Draw the appropriate tile
			DrawTile(type);

			// Disable LIGHT 1 and 2
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);

			// Move the next tile over 4
			glTranslated(4.0, 0.0, 0.0);

			// Switch to the other type of tile
			if (type == LIGHT)
				type = DARK;
			else
				type = LIGHT;
		}

		glTranslated(-32.0, 0.0, 4.0);

		if (type == LIGHT)
				type = DARK;
		else
			type = LIGHT;
	}

	glPopMatrix();
}

/****************************************************************************
 * Name:        DrawPieces
 * Input:       chess - Chess*
 * Output:      None
 * Returns:     None
 * Description: This function draws all of the chess pieces.
 * Invokes:     DrawPiece
 * Note:        None.
 ***************************************************************************/
void Geometry::DrawPieces(Chess *chess)
{
	glPushMatrix();

	// Move the piece
	glTranslated(-16.0, 0.0, -16.0);

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{

			// Move each individual piece
			glTranslated(2, 0.0, 2);

			// Light up the current piece
			if(cursorx == i && cursory == j)
				glEnable(GL_LIGHT1);

			// Draw the current piece
			DrawPiece(chess->GetBoard(i, j));

			glDisable(GL_LIGHT1);

			glTranslated(-2, 0.0, 2);
		}

		glTranslated(4.0, 0.0, -32.0);
	}

	glPopMatrix();
}

/****************************************************************************
 * Name:        DrawPiece
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This draws a piece with material property type.
 * Invokes:     None
 * Note:        None.
 ***************************************************************************/
void Geometry::DrawPiece(int type)
{
	// Don't draw anything for an empty square
	if (type == EMPTY)
		return;

	mesh *obj;

	// Set the material of the pieces
	if (type < 10)
		SetMaterial(PIECE_WHITE);
	else
		SetMaterial(PIECE_BLACK);

	glPushMatrix();

	// Set which object to use
	if (type == PAWN_WHITE || type == PAWN_BLACK)
		obj = pawn;

	else if (type == ROOK_WHITE || type == ROOK_BLACK)
		obj = rook;

	else if (type == KNIGHT_WHITE || type == KNIGHT_BLACK)
		obj = knight;

	else if (type == BISHOP_WHITE || type == BISHOP_BLACK)
		obj = bishop;

	else if (type == QUEEN_WHITE || type == QUEEN_BLACK)
		obj = queen;

	else if (type == KING_WHITE || type == KING_BLACK)
		obj = king;

	else
		return;

	if (!obj)
		return;

	// Scale the pieces
	glScaled(0.5, 0.5, 0.5);

	// Move the pieces
	glTranslated(0.0, 0.01, 0.0);

	// Rotate the pieces
	glRotated(-90.0, 1.0, 0.0, 0.0);

	// Draw the mesh
	for (int i = 0; i < obj->fTotal; i++)
	{
		glBegin(GL_POLYGON);

		for (int j = 0; j < 3; j++)
		{
			// Set the normal
			glNormal3fv(obj->nList[obj->faceList[i][j].n].ptr);
			glVertex3fv(obj->vList[obj->faceList[i][j].v].ptr);
		}

		glEnd();
	}

	glPopMatrix();
}

/****************************************************************************
 * Name:        DrawString
 * Input:       text - char*
 * Output:      None
 * Returns:     None
 * Description: Draws the text at the top center of the screen.
 * Invokes:     None
 * Note:        None.
 ***************************************************************************/
void Geometry::DrawString(char* text)
{
	// Get the size of the string
	int size = strlen(text);
	double x = 0.0 - (double) size / 4.4;

	glMatrixMode(GL_MODELVIEW_MATRIX);

	glPushMatrix();

	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);

	gluLookAt(0.0, 0.0, 30.0,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);

	// Set the material of the string
	SetMaterial(TEXT_WHITE);

	// Set the normal
	glNormal3d(0.0, 0.0, 1.0);

	// Set the color
	glColor3d(1.0, 1.0, 1.0);


	glRasterPos2d(x, 12);

	// Draw all the characters of the string
	for(int i = 0; i < size; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);
	}

	glEnable(GL_DEPTH_TEST);
    glPopMatrix();
}

/****************************************************************************
 * Name:        DrawPromotionMenu
 * Input:       player - int
 * Output:      None
 * Returns:     None
 * Description: This draws the promotional menu and it takes a player so
 *		that the promotional menu either cooresponds to white pieces
 *		or black pieces.
 * Invokes:     None
 * Note:        None.
 ***************************************************************************/
void Geometry::DrawPromotionMenu(int player)
{
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);

	glLoadIdentity();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	gluLookAt(0.0, 0.0, 5.0,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);

	glDisable(GL_DEPTH_TEST);
	glRasterPos2i(-1, 0);

	// Draw the promotional menu
	if (player == PLAYER_WHITE)
		glDrawPixels(285, 118, GL_RGB, GL_UNSIGNED_BYTE, promotion_white);
	else
		glDrawPixels(285, 118, GL_RGB, GL_UNSIGNED_BYTE, promotion_black);

	glEnable(GL_DEPTH_TEST);

	glPopAttrib();
	glPopMatrix();
}

/****************************************************************************
 * Name:        DrawBackground
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This function draws the background for the scene.
 * Invokes:     None.
 * Note:        None.
 ***************************************************************************/
void Geometry::DrawBackground()
{
	// Enable texturing
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Have the quad face the user
	gluLookAt(0.0, 0.0, 30.0,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);

	// Load the texture from memory
	glBindTexture(GL_TEXTURE_2D, background);

	// Set the background material
	SetMaterial(BACKGROUND);

	// Use the replace mode so we don't see lighting effects
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Set the normal
	glNormal3d(0.0, 0.0, 1.0);

	// Draw the quad for the background
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3d(-40.0, -40.0, -30.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(40.0, -40.0, -30.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(40.0, 40.0, -30.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(-40.0, 40.0, -30.0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

/****************************************************************************
 * Name:        DrawCursor
 * Input:       x - int
 *		y - int
 * Output:      None
 * Returns:     None
 * Description: Draws the semi-square cursor at the position x, y
 * Invokes:     None
 * Note:        None.
 ***************************************************************************/
void Geometry::DrawCursor(int x, int y)
{
	glPushMatrix();

	// Set the normal
	glNormal3d(0.0, 1.0, 0.0);

	// Set the material of the cursor
	SetMaterial(CURSOR);

	// Update the cursor's position for lighting purposes
	cursorx = x;
	cursory = y;

	// Move the cursor
	glTranslated(-16.0, 0.0, -16.0);
	glTranslated(x*4.0, 0.0, y*4.0);	

	// Draw the eight "spokes"
	glBegin(GL_QUADS);
	glVertex3d(0.5, 0.2, 0.5);
	glVertex3d(1.5, 0.2, 0.5);
	glVertex3d(1.5, 0.2, 0.7);
	glVertex3d(0.5, 0.2, 0.7);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3d(0.5, 0.2, 0.5);
	glVertex3d(0.7, 0.2, 0.5);
	glVertex3d(0.7, 0.2, 1.5);
	glVertex3d(0.5, 0.2, 1.5);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3d(3.5, 0.2, 0.5);
	glVertex3d(2.5, 0.2, 0.5);
	glVertex3d(2.5, 0.2, 0.7);
	glVertex3d(3.5, 0.2, 0.7);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3d(3.5, 0.2, 0.5);
	glVertex3d(3.3, 0.2, 0.5);
	glVertex3d(3.3, 0.2, 1.5);
	glVertex3d(3.5, 0.2, 1.5);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3d(0.5, 0.2, 3.5);
	glVertex3d(1.5, 0.2, 3.5);
	glVertex3d(1.5, 0.2, 3.3);
	glVertex3d(0.5, 0.2, 3.3);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3d(0.5, 0.2, 3.5);
	glVertex3d(0.7, 0.2, 3.5);
	glVertex3d(0.7, 0.2, 2.5);
	glVertex3d(0.5, 0.2, 2.5);
	glEnd();

	
	glBegin(GL_QUADS);
	glVertex3d(3.5, 0.2, 3.5);
	glVertex3d(2.5, 0.2, 3.5);
	glVertex3d(2.5, 0.2, 3.3);
	glVertex3d(3.5, 0.2, 3.3);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3d(3.5, 0.2, 3.5);
	glVertex3d(3.3, 0.2, 3.5);
	glVertex3d(3.3, 0.2, 2.5);
	glVertex3d(3.5, 0.2, 2.5);
	glEnd();

	glPopMatrix();
}

/****************************************************************************
 * Name:        DrawBase
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This draws the base of the board.  Which is what the tiles
 *		and pieces are on top of.
 * Invokes:     None
 * Note:        None.
 ***************************************************************************/
void Geometry::DrawBase()
{
	mesh *obj;

	obj = base;
	
	glPushMatrix();

	// Enable texture mapping
	glEnable(GL_TEXTURE_2D);

	// Load the material
	SetMaterial(TILE);

	// Load the texture
	glBindTexture(GL_TEXTURE_2D, baseTex);

	// Set the lighting mode
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Scale and translate the base
	glScaled(8.7, 8.7, 8.7);
	glTranslatef(-0.0, 0.21, 0.05);

	// Draw the base
	for (int i = 0; i < obj->fTotal; i++)
	{
		glBegin(GL_POLYGON);

		for (int j = 0; j < 3; j++)
		{
			glNormal3fv(obj->nList[obj->faceList[i][j].n].ptr);
			glTexCoord2fv(obj->tList[obj->faceList[i][j].t].ptr);
			glVertex3fv(obj->vList[obj->faceList[i][j].v].ptr);
		}

		glEnd();
	}

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}
