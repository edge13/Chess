//===========================================================================
//
//  File name ......: main.cpp
//  Author(s) ......: Joel Angelone, Justin Hendricks
//  Language .......: C++
//  Started ........: April 15, 2007
//  Last modified ..: May 3, 2007
//  Version ........: 1.0
//  Source Path ....: .
//  Operating System: Linux, Windows
//  Purpose ........: This file handles the colaboration between all components of
//			the game: the graphical and audio representation, the game
//			logic and rules, and the artificial intelligence of our bot
//			objects.
//
//===========================================================================

// Below is a fix for windows
#if defined(WIN32)
#define GLUT_DISABLE_ATEXIT_HACK    //Used to stop a error when compiling on Windows machines
#include <windows.h>
#else
#endif

#include <GL/glut.h>
#include <iostream>
#include "sound.h"
#include "geometry.h"
#include "chess.h"
#include "bot.h"

// Various modes the User Interface can be in
#define UI_NORMAL	0
#define UI_CHOOSE	1
#define UI_PROMOTE	2

// The rotation matrix for the view (initial values are set
// to give a nice, "isometric" looking angle)
double	rot[16] = { 0.82, -0.33, 0.45, 0.0 ,
					0.06, 0.86, 0.51, 0.0 ,
					-0.55, -0.39, 0.72, 0.0 ,
					0.0, 0.0, 0.0, 1.0 };

// Keep track of these values for screen manipulation
bool left_button = false;
int screen_x;
int screen_y;

// Instances of our main classes
Sounds sound;
Geometry geom;
Chess chess;

// Keep track of where the cursor is on the board, and where
// the picked piece is
int curs_x, curs_y;
int pick_x, pick_y;

// A move that is actually made on screen (not just simulated)
Move animated_move;

// Keep track of the mode that the UI is in
int UImode = UI_NORMAL;

// Our latest output message
char output[50];
// Whether we want to currently be displaying a message
bool display_output = false;

// Who is controlling each player
// White human vs black bot by default
bool WhiteHuman = true;
bool BlackHuman = false;

// The two bots (not always used)
bot bot_white;
bot bot_black;

// For testing we keep track of total moves made
int numWhiteMoves = 0;
int numBlackMoves = 0;
void printResults();

// Outputs the status of the board (via message)
void OutputStatus();

// Keep track of if we are in disco mode
bool discoMode = false;

/****************************************************************************
 * Name:        printBoard
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: Sets up the view in open gl using the rotation matrix
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void EstablishModelviewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslated(0.0, 0.0, -38.0);
	glMultMatrixd(rot);
	glTranslated(0.0, 0.0, 38.0);

	gluLookAt(0.0, 0.0, 38.0,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);
}

/****************************************************************************
 * Name:        EstablishProjectionMatrix
 * Input:       double - the viewport aspect ratio
 * Output:      None
 * Returns:     None
 * Description: Defines the perspective projection we want
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void EstablishProjectionMatrix(double viewportAR)
{
	gluPerspective(50.0, 1/viewportAR, 2, 100);
}

/****************************************************************************
 * Name:        ReshapeCB
 * Input:       w - integer width of the viewport
 *				h - integer height of the viewport
 * Output:      None
 * Returns:     None
 * Description: Resets our perspective projection based on the new window parameters
 * Invokes:     EstablishProjectionMatrix()
 * Note:        This runs every time the window is re-sized or re-drawn.
 ***************************************************************************/
void ReshapeCB(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	EstablishProjectionMatrix(double(h) / double(w));
}

/****************************************************************************
 * Name:        ReshapeCB
 * Input:       value - integer that functions as the "id" of the callback
 * Output:      None
 * Returns:     None
 * Description: Handles timer callback events.  This includes stopping the display
 *				of a given text message, and more imporantly it handles making
 *				the next AI move.
 * Invokes:     SimulateMove() - from the chess class
 *				FinalizeMove() - from the chess class
 *				Update() - from the chess class
 *				OutputStatus()
 *				GetState() - from the chess class
 *				GetTurn() - from the chess class
 *				run() - from the bot class
 *				playSound() - from the sound class
 * Note:        None
 ***************************************************************************/
void TimerCB(int value)
{
	// Do nothing if the game is over
	if (chess.GetState() == STATE_CHECKMATE || chess.GetState() == STATE_STALEMATE)
		return;

	// 0 value indicates we just want to stop displaying some message
	if (value == 0)
		display_output = false;

	// 1 value means we want the white bot to make a move
	else if (value == 1 && chess.GetTurn() == PLAYER_WHITE && !WhiteHuman)
	{
		// Run the bot search
		animated_move = bot_white.run(&chess);
		
		// Make the move
		chess.SimulateMove(&animated_move);

		// Finalize the move
		chess.FinalizeMove(&animated_move);

		// Increment white moves (for testing)
		numWhiteMoves++;

		// play the appropriate sound
		if (animated_move.captured == EMPTY)
			sound.playSound("Audio/Move.wav", false);
		else
			sound.playSound("Audio/Capture.wav", false);

		// Update the game status
		chess.Update();
		// Output any necessary status messages
		OutputStatus();

		// Redraw the scene
		glutPostRedisplay();

		// If black is an AI, register another timer callback
		if (!BlackHuman)
			glutTimerFunc(1000, TimerCB, 2);

	}

	// 2 value means we want the black bot to make a move
	else if (value == 2 && chess.GetTurn() == PLAYER_BLACK && !BlackHuman)
	{
		// Run the bot search
		animated_move = bot_black.run(&chess);

		// Make the move
		chess.SimulateMove(&animated_move);

		// Finalize the move
		chess.FinalizeMove(&animated_move);

		// Increment black moves (for testing)
		numBlackMoves++;

		// play the appropriate sound
		if (animated_move.captured == EMPTY)
			sound.playSound("Audio/Move.wav", false);
		else
			sound.playSound("Audio/Capture.wav", false);

		// Update the game status
		chess.Update();
		// Output any necessary status messages
		OutputStatus();

		// Redraw the scene
		glutPostRedisplay();

		// If white is an AI, register another timer callback
		if (!WhiteHuman)
			glutTimerFunc(1000, TimerCB, 1);;
	}
}

/****************************************************************************
 * Name:        DiscoTimer
 * Input:       value - integer ID of the timer callback
 * Output:      None
 * Returns:     None
 * Description: Updates the disco lighting
 * Invokes:     UpdateDiscoLights() - geometry class
 *				Lights() - geometry class
 * Note:        None
 ***************************************************************************/
void DiscoTimer(int value)
{
	if (discoMode)
	{
		geom.UpdateDiscoLights();
		glutTimerFunc(500, DiscoTimer, 0);	
	}

	else
		geom.Lights();
	
	glutPostRedisplay();
}

/****************************************************************************
 * Name:        ReshapeCB
 * Input:       text - array of characters to display
 * Output:      None
 * Returns:     None
 * Description: Sets the current text string and the display flag so that next
 *				time the display callback is invoked, it shows text
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void Message(char *text)
{
	strcpy(output, text);
	display_output = true;

	glutTimerFunc(3000, TimerCB, 0);
}

/****************************************************************************
 * Name:        printResults
 * Input:       verbose - boolean that determines whether to output the total
 *				piece information
 * Output:      None
 * Returns:     None
 * Description: Prints the board to the screen (used for testing only)
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void printResults(bool verbose)
{
	cout << "Final Board Positions: \n";
	int numWhitePieces = 0;
	int numBlackPieces = 0;

	int temp = 0;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			temp = chess.GetBoard(j,i);
				if(temp == 1)
					cout << "WP ";
				else if(temp == 2)
					cout << "WR ";
				else if(temp == 3)
					cout << "WN ";
				else if(temp == 4)
					cout << "WB ";
				else if(temp == 5)
					cout << "WQ ";
				else if(temp == 6)
					cout << "WK ";
				else if(temp == 11)
					cout << "BP ";
				else if(temp == 12)
					cout << "BR ";
				else if(temp == 13)
					cout << "BN ";
				else if(temp == 14)
					cout << "BB ";
				else if(temp == 15)
					cout << "BQ ";
				else if(temp == 16)
					cout << "BK ";
				else
					cout << "## ";

			if(chess.WhitePiece(temp))
				numWhitePieces++;
			else if(chess.BlackPiece(temp))
				numBlackPieces++;
		}
		cout << endl;
	}

	if(verbose)
	{
		cout << "Number of Total White Moves: " << numWhiteMoves << endl;
		cout << "Number of Total Black Moves: " << numBlackMoves << endl;
		cout << "White Pieces Left: " << numWhitePieces << endl;
		cout << "Black Pieces Left: " << numBlackPieces << endl;
	}
	else
	{
		cout << numWhiteMoves;
		cout << numBlackMoves;
	}	
	cout << endl;
}

/****************************************************************************
 * Name:        OutputStatus
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: Displays a message if the current state necessitates it.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void OutputStatus()
{
	// Get the current state
	int state = chess.GetState();

	if (state == STATE_CHECKMATE)
	{
		if (chess.GetTurn() == PLAYER_WHITE)
		{
			Message("Checkmate! Black wins!");
			
			// If testing:
			//cout << "Checkmate! Black wins!\n";
			//printResults(true);
		}
		else
		{
			Message("Checkmate! White wins!");
			
			// If testing:
			//cout << "Checkmate! White wins!\n";
			//printResults(true);
		}
		
		// Used for timing tests:
		//if (numBlackMoves)
		//{
		//	cout << "Average time for Black to make a move: ";
		//	cout << bot_black.getTotalTime() / numBlackMoves;
		//	cout << " milliseconds" << endl;
		//}
	
		//if (numWhiteMoves)
		//{
		//	cout << "Average time for White to make a move: ";
		//	cout << bot_white.getTotalTime() / numWhiteMoves;
		//	cout << " milliseconds" << endl;
		//}			

		sound.playSound("Audio/Checkmate.wav", false);
		sound.stop(1);
	}
	
	if (state == STATE_CHECK)
	{
		if (chess.GetTurn() == PLAYER_WHITE)
			Message("White in check");
		else
			Message("Black in check");

		sound.playSound("Audio/Check.wav", false);
	}

	else if (state == STATE_STALEMATE)
	{
		Message("Stalemate...");
		sound.playSound("Audio/Stalemate.wav", false);
		sound.stop(1);
		
		// If testing:
		//cout << "Stalemate!\n";
		//printResults(true);
	}
}

/****************************************************************************
 * Name:        MouseFuncCB
 * Input:       button - int representing the mouse button pressed
 *				state - int representing whether the button is up or down
 *				x, y - int coordinate of the mouse
 * Output:      None
 * Returns:     None
 * Description: Handles mouse clicks.
 * Invokes:     ReplaceBoard() - from the chess class
 *				OutputStatus()
 *				GetTurn() - from the chess class
 *				Update() - from the chess class
 * Note:        None
 ***************************************************************************/
void MouseFuncCB(int button, int state, int x, int y)
{
	// Left button clicked
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		// Handle promotion menu
		if (UImode == UI_PROMOTE)
		{
			// Click outside the promotion box
			if (y < 230 || y > 295)
				return;

			// White promotion
			if (chess.GetTurn() == PLAYER_WHITE)
			{
				if (x > 280 && x < 340)
					chess.ReplaceBoard(ROOK_WHITE, animated_move.newx, animated_move.newy);
				else if (x > 350 && x < 410)
					chess.ReplaceBoard(KNIGHT_WHITE, animated_move.newx, animated_move.newy);
				else if (x > 415 && x < 480)
					chess.ReplaceBoard(BISHOP_WHITE, animated_move.newx, animated_move.newy);
				else if (x > 485 && x < 550)
					chess.ReplaceBoard(QUEEN_WHITE, animated_move.newx, animated_move.newy);
				else
					return;
			}
			
			// Black promotion
			else
			{
				if (x > 280 && x < 340)
					chess.ReplaceBoard(ROOK_BLACK, animated_move.newx, animated_move.newy);
				else if (x > 350 && x < 410)
					chess.ReplaceBoard(KNIGHT_BLACK, animated_move.newx, animated_move.newy);
				else if (x > 415 && x < 480)
					chess.ReplaceBoard(BISHOP_BLACK, animated_move.newx, animated_move.newy);
				else if (x > 485 && x < 550)
					chess.ReplaceBoard(QUEEN_BLACK, animated_move.newx, animated_move.newy);
				else
					return;
			}

			// Reset the UI mode back to normal
			UImode = UI_NORMAL;
			// Update the state
			chess.Update();

			// Start bot timers if necessary
			if (chess.GetTurn() == PLAYER_WHITE)
			{
				if (!WhiteHuman)
					glutTimerFunc(1000, TimerCB, 1);
			}

			else
			{
				if (!BlackHuman)
					glutTimerFunc(1000, TimerCB, 2);
			}

			// Output the status of the game
			OutputStatus();
			// Update the display
			glutPostRedisplay();
		}

		// Keep track of the mouse coordinates
		else
		{
			screen_x = x;
			screen_y = y;
			left_button = true;
		}
	}

	else
		left_button = false;
}

/****************************************************************************
 * Name:        MouseMotionCB
 * Input:       new_x - integer location of the mouse x coordinate
 *				new_y - integer location of the mouse y coordinate
 * Output:      None
 * Returns:     None
 * Description: Updates the rotation matrix based on the amount the mouse moves.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void MouseMotionCB(int new_x, int new_y)
{
	if (!left_button)
		return;

	// Compute the change in x and change in y
	int dx = (new_x - screen_x);
	int dy = (new_y - screen_y);

	screen_x = new_x;
	screen_y = new_y;

	double scale = 0.1;

	// Adjust the rotation matrix accordingly
	double rot_y = scale * dx;
	double rot_x = scale * dy;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotated(rot_x, 1.0, 0.0, 0.0);
	glRotated(rot_y, 0.0, 1.0, 0.0);
	glMultMatrixd(rot);
	glGetDoublev(GL_MODELVIEW_MATRIX, rot);

	// Redraw the scene
	glutPostRedisplay();
}

/****************************************************************************
 * Name:        DisplayCB
 * Input:       None
 * Output:      The graphical representation of our openGL scene.
 * Returns:     None
 * Description: Renders the entire scene in the viewport
 * Invokes:     DrawBackground(), Lights(), DrawBoard(), DrawPieces(), DrawBase(),
 *				DrawCursor(), DrawPromotionMenu(), DrawString() - all from
 *				the geometry class
 * Note:        None
 ***************************************************************************/
void DisplayCB()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the background before establishing the modelview matrix because
	// We don't want the background to move with the camera
	geom.DrawBackground();

	// Now setup the modelview matrix
	EstablishModelviewMatrix();

	// Update the lighting effects
	if (!discoMode)
		geom.Lights();

	// Draw the selection cursor
	geom.DrawCursor(curs_x, curs_y);
	// Draw the board
	geom.DrawBoard();
	// Draw the pieces
	geom.DrawPieces(&chess);
	// Draw the base of the board
	geom.DrawBase();

	// If we are choosing a destination, draw another cursor
	// at that location
	if (UImode == UI_CHOOSE)
		geom.DrawCursor(pick_x, pick_y);
	// If we are promoting a piece, draw the menu
	if (UImode == UI_PROMOTE)
		geom.DrawPromotionMenu(chess.GetTurn());

	// If there is a message to display, show it
	if (display_output)
		geom.DrawString(output);

	// Now flip the backbuffer (we are using double buffering)
	glFlush();
	glutSwapBuffers();
}

/****************************************************************************
 * Name:        SpecialCB
 * Input:       key - integer representing the key pressed
 *				x - integer location of the mouse x coordinate
 *				y - integer location of the mouse y coordinate
 * Output:      None
 * Returns:     None
 * Description: Handles arrow key events.
 * Invokes:     None
 * Note:        Does nothing when the game is finished.
 ***************************************************************************/
void SpecialCB(int key, int x, int y)
{
	// Just return if the game is over
	if (chess.GetState() == STATE_CHECKMATE || chess.GetState() == STATE_STALEMATE)
		return;

	// Move the cursor according to the arrow direction pressed
	if (key == GLUT_KEY_UP)
	{
		if (curs_y > 0)
			curs_y--;

		glutPostRedisplay();
	}

	else if (key == GLUT_KEY_DOWN)
	{
		if (curs_y < 7)
			curs_y++;

		glutPostRedisplay();
	}

	else if (key == GLUT_KEY_LEFT)
	{
		if (curs_x > 0)
			curs_x--;

		glutPostRedisplay();

	}

	else if (key == GLUT_KEY_RIGHT)
	{
		if (curs_x < 7)
			curs_x++;

		glutPostRedisplay();
	}	
}

/****************************************************************************
 * Name:        KeyboardCB
 * Input:       key - unsigned char representing the key pressed
 *				x,y - integer coordinates of the mouse
 * Output:      None
 * Returns:     None
 * Description: Handles normal keyboard events.
 * Invokes:     GetState() - from chess class
 *				GetTurn() - from chess class
 *				playSound() - from sound class
 *				LegalMove() - from chess class
 *				Message()
 *				SimulateMove() - from chess class
 *				UnSimulateMove() - from chess class
 *				FinalizeMove() - from chess class
 * Note:        Normally, pressing enter will select the piece at the cursor's
 *				location.  The UI then goes into "select" mode where the next
 *				time enter is pressed, it tries to move the piece to that location.
 *				Pressing escape will allow the user to unselect the current piece.
 ***************************************************************************/
void KeyboardCB(unsigned char key, int x, int y)
{
	// Do nothing if the game is already over
	if (chess.GetState() == STATE_CHECKMATE || chess.GetState() == STATE_STALEMATE)
		return;

	// Do nothing if bots are playing
	if (chess.GetTurn() == PLAYER_WHITE)
	{
		if (!WhiteHuman)
			return;
	}

	else
	{
		if (!BlackHuman)
			return;
	}

	// Escape key
	if (key == 27)
	{
		// Unselect the currently selected piece
		if (UImode == 1)
		{
			UImode = 0;
			curs_x = pick_x;
			curs_y = pick_y;
			glutPostRedisplay();
		}
	}

	// Enter key
	if (key == 13)
	{
		// UI mode is normal, try to select a piece
		if (UImode == UI_NORMAL)
		{
			if (chess.GetTurn() == PLAYER_WHITE && chess.WhitePiece(chess.GetBoard(curs_x, curs_y)))
			{
				pick_x = curs_x;
				pick_y = curs_y;

				// Picked up a piece, UI mode is now choose
				UImode = UI_CHOOSE;

				sound.playSound("Audio/Cursor.wav", false);
			}

			else if (chess.GetTurn() == PLAYER_BLACK && chess.BlackPiece(chess.GetBoard(curs_x, curs_y)))
			{
				pick_x = curs_x;
				pick_y = curs_y;

				// Picked up a piece, UI mode is now choose
				UImode = UI_CHOOSE;

				sound.playSound("Audio/Cursor.wav", false);
			}
		}

		// UI mode was already choose, we're now picking the destination
		else if (UImode == UI_CHOOSE)
		{
			// Setup a move structure to check if it's legal
			animated_move.oldx = pick_x;
			animated_move.oldy = pick_y;
			animated_move.newx = curs_x;
			animated_move.newy = curs_y;

			animated_move.piece = chess.GetBoard(pick_x, pick_y);
			animated_move.captured = chess.GetBoard(curs_x, curs_y);

			// If it's a legal move, try to make it
			if (chess.LegalMove(&animated_move))
			{
				// Special case, can't castle out of check
				if (animated_move.castle)
				{
					if (chess.GetState() == STATE_CHECK)
					{
						Message("You can't castle out of check");
						sound.playSound("Audio/IllegalMove.wav", false);
						glutPostRedisplay();
						return;
					}
				}

				// Try making the move
				chess.SimulateMove(&animated_move);

				// Don't allow moving into check
				if (chess.InCheck(chess.GetTurn()))
				{
					Message("You can't move into check");
					chess.UnSimulateMove(&animated_move);
					sound.playSound("Audio/IllegalMove.wav", false);
					glutPostRedisplay();
					return;
				}

				// Legal move, finalize it
				chess.FinalizeMove(&animated_move);

				// Play the appropriate sound
				if (animated_move.captured == EMPTY)
					sound.playSound("Audio/Move.wav", false);
				else
					sound.playSound("Audio/Capture.wav", false);

				// New UI state if we need to promote the piece
				if (animated_move.promotion)
					UImode = UI_PROMOTE;

				// Otherwise proceed normally
				else
				{
					// UI mode goes back to normal
					UImode = UI_NORMAL;
					// Update the game status
					chess.Update();

					// Output the game status
					OutputStatus();

					// Run a bot if the opponent is a bot
					if (chess.GetTurn() == PLAYER_WHITE)
					{
						if (!WhiteHuman)
							glutTimerFunc(1000, TimerCB, 1);
					}

					else
					{
						if (!BlackHuman)
							glutTimerFunc(1000, TimerCB, 2);
					}
				}

				// Redraw the scene
				glutPostRedisplay();
			}

			// Move wasn't legal, give feedback
			else
			{
				Message("Illegal Move");
				sound.playSound("Audio/IllegalMove.wav", false);
				glutPostRedisplay();
			}
		}


	}
}

/****************************************************************************
 * Name:        MenuCB
 * Input:       value - integer indicating which menu button was clicked
 * Output:      None
 * Returns:     None
 * Description: Handles all menu events.
 * Invokes:     Init() - chess class
 *				Message()
 *				LoadAI() - bot class
 * Note:        This menu should be attached to the right mouse button.
 ***************************************************************************/
void MenuCB(int value)
{
	switch(value)
	{
		// "New Game"
		case 100:
			// Reset the total number of (bot) moves made (for testing)
			numWhiteMoves = 0;
			numBlackMoves = 0;

			// Reset the UI mode
			UImode = UI_NORMAL;

			// Re-initialize the chess board
			chess.Init();
			Message("White plays first");
			
			// If white is a bot, start it
			if (!WhiteHuman)
				glutTimerFunc(3000, TimerCB, 1);
			
			glutPostRedisplay();
		break;

		// "Exit"
		case 101:
			exit(0);
		break;

		// "White --> Player"
		case 0:
			// White is now controlled by the user
			WhiteHuman = true;
		break;

		// "White --> joel.bot"
		case 1:
			// Load up Joel's bot
			cout << "Loading White Bot..." << endl;
			bot_white.loadAI("AI/joel.bot");
			
			// If testing
			//bot_white.printBotValues();
			WhiteHuman = false;
			glutTimerFunc(1000, TimerCB, 1);
		break;

		// "White --> riha.bot"
		case 2:
			// Load up Adam's bot
			cout << "Loading White Bot..." << endl;
			bot_white.loadAI("AI/riha.bot");
			
			// If testing
			//bot_white.printBotValues();
			WhiteHuman = false;
			glutTimerFunc(1000, TimerCB, 1);
		break;

		// "White --> justin.bot"
		case 3:
			// Load up Justin's bot
			cout << "Loading White Bot..." << endl;
			bot_white.loadAI("AI/justin.bot");
			
			// If testing
			//bot_white.printBotValues();
			WhiteHuman = false;
			glutTimerFunc(1000, TimerCB, 1);
		break;

		// "White --> jc.bot"
		case 4:
			// Load up Justin's bot
			cout << "Loading White Bot..." << endl;
			bot_white.loadAI("AI/jc.bot");
			
			// If testing
			//bot_white.printBotValues();
			WhiteHuman = false;
			glutTimerFunc(1000, TimerCB, 1);
		break;

		case 6:
			cout << "Loading White Bot..." << endl;
			bot_white.loadAI("AI/fivePly.bot");
	
			// If testing			
			//bot_white.printBotValues();
			WhiteHuman = false;
			glutTimerFunc(1000, TimerCB, 1);
		break;

		case 7:
			cout << "Loading White Bot..." << endl;
			bot_white.loadAI("AI/threePly.bot");

			// If testing
			//bot_white.printBotValues();
			WhiteHuman = false;
			glutTimerFunc(1000, TimerCB, 1);
		break;

		case 8:
			cout << "Loading White Bot..." << endl;
			bot_white.loadAI("AI/onePly.bot");
			
			// If testing
			//bot_white.printBotValues();
			WhiteHuman = false;
			glutTimerFunc(1000, TimerCB, 1);
		break;

		// "Black --> Player"
		case 10:
			BlackHuman = true;
		break;

		// "Black --> joel.bot"
		case 11:
			cout << "Loading Black Bot..." << endl;
			bot_black.loadAI("AI/joel.bot");
			
			// If testing
			//bot_black.printBotValues();
			BlackHuman = false;
			glutTimerFunc(1000, TimerCB, 2);
		break;

		// "Black --> riha.bot"
		case 12:
			cout << "Loading Black Bot..." << endl;
			bot_black.loadAI("AI/riha.bot");
			
			// If testing
			//bot_black.printBotValues();
			BlackHuman = false;
			glutTimerFunc(1000, TimerCB, 2);
		break;

		// "Black --> justin.bot"
		case 13:
			cout << "Loading Black Bot..." << endl;
			bot_black.loadAI("AI/justin.bot");
			
			// If testing
			//bot_black.printBotValues();
			BlackHuman = false;
			glutTimerFunc(1000, TimerCB, 2);
		break;
		
		// "Black --> jc.bot"
		case 14: 
			cout << "Loading Black Bot..." << endl;
			bot_black.loadAI("AI/jc.bot");
			
			// If testing
			//bot_black.printBotValues();
			BlackHuman = false;
			glutTimerFunc(1000, TimerCB, 2);
		break;

		case 16:
			cout << "Loading Black Bot..." << endl;
			bot_black.loadAI("AI/fivePly.bot");
			
			// If testing
			//bot_black.printBotValues();
			BlackHuman = false;
			glutTimerFunc(1000, TimerCB, 2);
		break;

		case 17:
			cout << "Loading Black Bot..." << endl;
			bot_black.loadAI("AI/threePly.bot");
			
			// If testing
			//bot_black.printBotValues();
			BlackHuman = false;
			glutTimerFunc(1000, TimerCB, 2);
		break;

		case 18:
			cout << "Loading Black Bot..." << endl;
			bot_black.loadAI("AI/onePly.bot");
			
			// If testing
			//bot_black.printBotValues();
			BlackHuman = false;
			glutTimerFunc(1000, TimerCB, 2);
		break;

		// "Music --> On"
		case 200:
			sound.stop(1);
			sound.playMusic("Audio/Background.mp3", true);
		break;

		// "Music --> Off"
		case 201:
			sound.stop(1);
		break;

		// "Disco Mode --> Enable"
		case 300:
			discoMode = true;
			glutTimerFunc(15, DiscoTimer, 0);
			sound.stop(1);
			sound.playMusic("Audio/Disco.mp3", true);
		break;

		// "Disco Mode --> Disable"
		case 301:
			discoMode = false;
			glutPostRedisplay();
			sound.stop(1);
			sound.playMusic("Audio/Background.mp3", true);
		break;
	}
}

/****************************************************************************
 * Name:        BuildMenu
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: Creates the in-game menu.
 * Invokes:     None
 * Note:        Attaches the menu to the right mouse button.
 ***************************************************************************/
void BuildMenu()
{	
	int WhiteMenu;
	int BlackMenu;
	int MusicMenu;
	int DiscoMenu;

	WhiteMenu = glutCreateMenu(MenuCB);
	glutAddMenuEntry("Player", 0);
	glutAddMenuEntry("joel.bot", 1);
	glutAddMenuEntry("riha.bot", 2);
	glutAddMenuEntry("justin.bot", 3);
	glutAddMenuEntry("jc.bot", 4);
	glutAddMenuEntry("--------", 5);
	glutAddMenuEntry("Five-Ply.bot", 6);
	glutAddMenuEntry("Three-Ply.bot", 7);
	glutAddMenuEntry("One-Ply.bot", 8);

	BlackMenu = glutCreateMenu(MenuCB);
	glutAddMenuEntry("Player", 10);
	glutAddMenuEntry("joel.bot", 11);
	glutAddMenuEntry("riha.bot", 12);
	glutAddMenuEntry("justin.bot", 13);
	glutAddMenuEntry("jc.bot", 14);
	glutAddMenuEntry("--------", 15);
	glutAddMenuEntry("Five-Ply.bot", 16);
	glutAddMenuEntry("Three-Ply.bot", 17);
	glutAddMenuEntry("One-Ply.bot", 18);

	MusicMenu = glutCreateMenu(MenuCB);
	glutAddMenuEntry("On", 200);
	glutAddMenuEntry("Off", 201);

	DiscoMenu = glutCreateMenu(MenuCB);
	glutAddMenuEntry("Enable", 300);
	glutAddMenuEntry("Disable", 301);

	glutCreateMenu(MenuCB);
	glutAddMenuEntry("New Game", 100);
	glutAddSubMenu("White", WhiteMenu);
	glutAddSubMenu("Black", BlackMenu);
	glutAddSubMenu("Music", MusicMenu);
	glutAddSubMenu("Disco Mode", DiscoMenu);
	glutAddMenuEntry("Exit", 101);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/****************************************************************************
 * Name:        InitOpenGL
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: Initializes all OpenGL properties for the scene.
 * Invokes:     BuildMenu()
 * Note:        None
 ***************************************************************************/
void InitOpenGL()
{
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(250, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("Team Girella's Chess");

	// Register our callbacks
	glutDisplayFunc(DisplayCB);
	glutKeyboardFunc(KeyboardCB);
	glutSpecialFunc(SpecialCB);
	glutReshapeFunc(ReshapeCB);
	glutMouseFunc(MouseFuncCB);
	glutMotionFunc(MouseMotionCB);

	// Build our menu
	BuildMenu();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/****************************************************************************
 * Name:        main
 * Input:       argc - integer number of arguments to the program
 *				argv - list of arguments to the program
 * Output:      None
 * Returns:     None
 * Description: Runs the game!
 * Invokes:     InitOpenGL()
 *				Init() - geometry calss
 *				Init() - chess class
 *				Message()
 *				playMusic() - sound class
 * Note:        None
 ***************************************************************************/
int main(int argc, char* argv[])
{
	// Let the glut look at the arguments first
	glutInit(&argc, argv);
	
	// Initialize opengl
	InitOpenGL();

	// Initialize our geometry
	geom.Init();

	// Initialize the chess class
	chess.Init();

	// Seed the random number generator
	srand(timeGetTime());

	Message("White plays first");
	sound.playMusic("Audio/Background.mp3", true);

	// Starting location for the cursor
	curs_x = curs_y = 3;

	// Give event control to the glut
	glutMainLoop();

	return 0;
}

