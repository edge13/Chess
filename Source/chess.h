//===========================================================================
//
//  File name ......: chess.h
//  Author(s) ......: Joel Angelone (base)
//  Language .......: C++
//  Started ........: April 15, 2007
//  Last modified ..: May 3, 2007
//  Version ........: 1.0
//  Source Path ....: .
//  Operating System: Linux, Windows
//  Purpose ........: This file handles all of the gameplay mechanics for the
//			chess game.
//
//===========================================================================

#ifndef CHESS_H
#define CHESS_H

#define STATE_NORMAL	0
#define STATE_CHECK		1
#define STATE_CHECKMATE	2
#define STATE_STALEMATE	3

#define PLAYER_WHITE	0
#define PLAYER_BLACK	1

#define EMPTY			0

#define PAWN_WHITE		1
#define ROOK_WHITE		2
#define KNIGHT_WHITE	3
#define BISHOP_WHITE	4
#define QUEEN_WHITE		5
#define KING_WHITE		6

#define PAWN_BLACK		11
#define ROOK_BLACK		12
#define KNIGHT_BLACK	13
#define BISHOP_BLACK	14
#define QUEEN_BLACK		15
#define KING_BLACK		16


/////////////////////////////////////////////////////////////////////////////
// Name:        Move
// Description: This structure holds all the data required for a move
//		It also allows for
/////////////////////////////////////////////////////////////////////////////
struct Move
{
	int oldx, oldy;
	int newx, newy;

	int piece;
	int captured;

	bool castle;
	bool promotion;
	bool enpassant;

	int value;

	Move()
	{
		oldx = oldy = newx = newy = 0;
	}

	bool operator == (Move &m)
	{
		if (oldx == m.oldx && oldy == m.oldy && newx == m.newx && newy == m.newy && piece == m.piece)
		{
			return true;
		}

		return false;
	}
};


/////////////////////////////////////////////////////////////////////////////
// Name:        Chess
// Description: This class handles all the gameplay and mechanics of the
//		chess game.  It also is the primary class that all other
//		classes reference.
/////////////////////////////////////////////////////////////////////////////
class Chess
{
	public:

		// Default constructor - does nothing
		Chess();

		// Destructor - does nothing
		~Chess();

		// Sets up a new game of chess
		void Init();

		// Checks if a given move is legal or not
		bool LegalMove(Move *m);

		// Generates a list of moves for the given player
		int GenerateMoves(Move *move_list, int player);

		// Returns true if player is in check
		bool InCheck(int player);

		// Returns true if player is in checkmate
		bool InCheckmate(int player);

		// Checks if the game is in a stalemate configuration
		bool Stalemate();

		// Makes a move
		void SimulateMove(Move *m);

		// Unmakes a move
		void UnSimulateMove(Move *m);

		// Sets flags for castling and recent moves after a move is made
		void FinalizeMove(Move *m);

		// Updates the state of the game
		// Also toggles the turn
		void Update();

		// Returns true if a piece belongs to white
		bool WhitePiece(int piece);

		// Returns true if a piece belongs to black
		bool BlackPiece(int piece);

		// Puts a piece on the board at a given location "by hand"
		void ReplaceBoard(int piece, int x, int y);

		// Gets a recent move at the specified index
		Move GetRecentMove(int index);

		// Gets the piece at position x,y of the board
		int GetBoard(int x, int y);

		// Returns whose turn it is currently
		int GetTurn();

		// Returns the current state of the game
		int GetState();

		// Returns the total number of pieces on the board
		int GetNumPieces();

	protected:

		// Changes a variable to its absolute value
		void AbsValue(int &x);

		// Changes the turn to the opponent's turn
		void ToggleTurn();
	
		// Keeps track of the board
		int board[8][8];

		int num_players;	// Number of players playing
		int num_pieces;		// Number of pieces on the board
		int turn;			// Whose turn it is
		int game_state;		// The state of the game

		// Keeps track of the last 12 moves made
		Move last_12_moves[12];

		// Keep track of the availability of castling
		bool WhiteCastleLeft;
		bool WhiteCastleRight;
		bool BlackCastleLeft;
		bool BlackCastleRight;

};

#endif
