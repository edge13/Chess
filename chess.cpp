//===========================================================================
//
//  File name ......: chess.cpp
//  Author(s) ......: Joel Angelone (base)
//  Language .......: C++
//  Started ........: April 15, 2007
//  Last modified ..: May 3, 2007
//  Version ........: 1.0
//  Source Path ....: .
//  Operating System: Linux, Windows
//  Purpose ........: Implementation of the chess class routines.
//
//===========================================================================

// Below is a fix for windows
#if defined(WIN32)
#define GLUT_DISABLE_ATEXIT_HACK    //Used to stop a error when compiling on Windows machines
#include <windows.h>
#else
#endif

#include "chess.h"

/****************************************************************************
 * Name:        Chess
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: Default constructor, just used as a placeholder.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
Chess::Chess()
{

}

/****************************************************************************
 * Name:        ~Chess
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: Default destructor, used as a placeholder.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
Chess::~Chess()
{

}

/****************************************************************************
 * Name:        Init
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This function initializes all the values in the chess class.
 * Invokes:     None
 * Note:        This can always be called to start a new match.
 ***************************************************************************/
void Chess::Init()
{
	// Initial state values
	game_state = STATE_NORMAL;
	turn = PLAYER_WHITE;
	num_players = 1;

	// Initial piece positions
	for (int x = 0; x < 8; x++)
		for (int y = 2; y < 6; y++)
			board[x][y] = EMPTY;

	board[0][0] = ROOK_BLACK;
	board[1][0] = KNIGHT_BLACK;
	board[2][0] = BISHOP_BLACK;
	board[3][0] = QUEEN_BLACK;
	board[4][0] = KING_BLACK;
	board[5][0] = BISHOP_BLACK;
	board[6][0] = KNIGHT_BLACK;
	board[7][0] = ROOK_BLACK;

	board[0][7] = ROOK_WHITE;
	board[1][7] = KNIGHT_WHITE;
	board[2][7] = BISHOP_WHITE;
	board[3][7] = QUEEN_WHITE;
	board[4][7] = KING_WHITE;
	board[5][7] = BISHOP_WHITE;
	board[6][7] = KNIGHT_WHITE;
	board[7][7] = ROOK_WHITE;

	for (int i = 0; i < 8; i++)
		board[i][1] = PAWN_BLACK;

	for (int j = 0; j < 8; j++)
		board[j][6] = PAWN_WHITE;

	// Castling is set to available
	WhiteCastleLeft = true;
	WhiteCastleRight = true;
	BlackCastleLeft = true;
	BlackCastleRight = true;

	num_pieces = 32;
}

/****************************************************************************
 * Name:        GenerateMoves
 * Input:       move_list - array of moves
 *				player - int
 * Output:      None
 * Returns:     an integer indicating the number of available moves
 * Description: This function generates all of the moves possible for the
 *				given player and puts them into move_list
 * Invokes:     LegalMove()
 *				WhitePiece()
 *				BlackPiece()
 * Note:        All moves generated are legal moves except the special cases
 *				of moving into check or castling out of check.  These are
 *				handled elsewhere
 ***************************************************************************/
int Chess::GenerateMoves(Move *move_list, int player)
{
	Move m;
	int num = 0;

	// Loop through every tile
	for (int x1 = 0; x1 < 8; x1++)
	{
		for (int y1 = 0; y1 < 8; y1++)
		{
			// Can't be a move starting at an empty piece
			if (board[x1][y1] == EMPTY)
				continue;

			// If the piece belongs to the wrong player, it's an illegal move
			if (player == PLAYER_WHITE && !WhitePiece(board[x1][y1]))
				continue;

			if (player == PLAYER_BLACK && !BlackPiece(board[x1][y1]))
				continue;

			// Legal piece, try to move to every possible tile
			for (int x2 = 0; x2 < 8; x2++)
			{
				for (int y2 = 0; y2 < 8; y2++)
				{
					// Not moving isn't a legal move
					if (x1 == x2 && y1 == y2)
						continue;

					// Fill out a move structure and pass it to LegalMove()
					m.oldx = x1;
					m.oldy = y1;
					m.newx = x2;
					m.newy = y2;

					m.piece = board[x1][y1];
					m.captured = board[x2][y2];

					if (LegalMove(&m))
					{
						// The move is legal, add it to the list
						move_list[num] = m;
						num++;
					}
				}
			}
		}
	}

	return num;
}

/****************************************************************************
 * Name:        InCheck
 * Input:       player - int
 * Output:      None
 * Returns:     bool
 * Description: Returns true if the player is in check.
 * Invokes:     GenerateMoves()
 * Note:        None
 ***************************************************************************/
// Returns true if the given player is in check
bool Chess::InCheck(int player)
{
	Move move_list[75];

	int num_moves = GenerateMoves(move_list, player == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE);

	// Examine all moves, any moves resulting in the king being captured denotes "check"
	for (int i = 0; i < num_moves; i++)
	{
		if (player == PLAYER_WHITE && move_list[i].captured == KING_WHITE)
			return true;

		if (player == PLAYER_BLACK && move_list[i].captured == KING_BLACK)
			return true;
	}

	return false;
}

/****************************************************************************
 * Name:        InCheckmate
 * Input:       player - int
 * Output:      None
 * Returns:     true or false indicating if the player is in checkmate
 * Description: Checks if a player is in checkmate and returns an appropriate
 *				boolean.
 * Invokes:     GenerateMove()
 *				SimulateMove()
 *				UnSimulateMove()
 *				InCheck()
 * Note:        None
 ***************************************************************************/
bool Chess::InCheckmate(int player)
{
	Move move_list[75];

	int num_moves = GenerateMoves(move_list, player);

	bool check_mate = true;

	// For every move, make the move, then find out
	// if we are still in check
	for (int i = 0; i < num_moves; i++)
	{
		SimulateMove(&move_list[i]);

		check_mate = InCheck(player);

		UnSimulateMove(&move_list[i]);

		if (!check_mate)
			return false;
	}

	return true;
}

/****************************************************************************
 * Name:        Stalemate
 * Input:       None
 * Output:      None
 * Returns:     true or false indicating if the game is a stalemate
 * Description: Given the current board configuration, returns an appropriate
 *				boolean if the game has reached stalemate status.
 * Invokes:     None
 * Note:        Stalemate can happen in the following ways:
 *				1. Each player makes the same sequence of moves for 3 straight
 *				   turns.
 *				2. The pieces remaining on the board are such that neither play
 *				   can win.
 ***************************************************************************/
bool Chess::Stalemate()
{
	int bbishop = 0;
	int wbishop = 0;
	int bknight = 0;
	int wknight = 0;

	// Case 1: check the last few moves
	if (last_12_moves[0] == last_12_moves[4] && last_12_moves[0] == last_12_moves[8] &&
		last_12_moves[1] == last_12_moves[5] && last_12_moves[1] == last_12_moves[9] &&
		last_12_moves[2] == last_12_moves[6] && last_12_moves[2] == last_12_moves[10] &&
		last_12_moves[3] == last_12_moves[7] && last_12_moves[3] == last_12_moves[11])
		return true;

	// Case 2: count the number of pieces
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (board[x][y] == PAWN_WHITE || board[x][y] == PAWN_BLACK) return false;
			if (board[x][y] == ROOK_WHITE || board[x][y] == ROOK_BLACK) return false;
			if (board[x][y] == QUEEN_WHITE || board[x][y] == QUEEN_BLACK) return false;

			if (board[x][y] == KNIGHT_WHITE) wknight++;
			if (board[x][y] == KNIGHT_BLACK) bknight++;
			if (board[x][y] == BISHOP_WHITE) wbishop++;
			if (board[x][y] == BISHOP_WHITE) bbishop++;

			if (bknight + bbishop > 1) return false;
			if (wknight + wbishop > 1) return false;
		}
	}

	return true;
}

/****************************************************************************
 * Name:        Update
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This function updates the state of the game.  It checks for
 *		stalemate, check, and checkmate.  It updates the game_state variable
 *		accordingly.  It also flips the turn to the opponent.
 * Invokes:     InCheckmate()
 *				InCheck()
 *				Stalemate()
 *				ToggleTurn()
 * Note:        None
 ***************************************************************************/
void Chess::Update()
{
	ToggleTurn();

	if (InCheckmate(turn))
		game_state = STATE_CHECKMATE;

	else if (InCheck(turn))
		game_state = STATE_CHECK;

	else if (Stalemate())
		game_state = STATE_STALEMATE;

	else
		game_state = STATE_NORMAL;
}

/****************************************************************************
 * Name:        ReplaceBoard
 * Input:       piece - int
 *				x - int
 *				y - int
 * Output:      None
 * Returns:     None
 * Description: Replaces the board at the given x/y location with the piece
 *				passed as a parameter.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void Chess::ReplaceBoard(int piece, int x, int y)
{
	if (x >= 0 && x < 8 && y >= 0 && y < 8)
		board[x][y] = piece;
}

/****************************************************************************
 * Name:        ToggleTurn
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This switches the turn to the opponent's turn
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void Chess::ToggleTurn()
{
	turn == PLAYER_WHITE ? turn = PLAYER_BLACK : turn = PLAYER_WHITE;
}

/****************************************************************************
 * Name:        SimulateMove
 * Input:       m - pointer to a move
 * Output:      None
 * Returns:     None
 * Description: This is the function that actually carries out a given move.
 *				It's called "simulate" move because it is also used in a 
 *				backtracking sense for our AI search.  We need the flexibility
 *				to make a move, then undo it.  That's exactly what this function
 *				allows us to do.
 * Invokes:     WhitePiece()
 *				BlackPiece()
 * Note:        None
 ***************************************************************************/
void Chess::SimulateMove(Move *m)
{
	// Normally we just update the new and old locations on the board
	board[m->oldx][m->oldy] = EMPTY;
	board[m->newx][m->newy] = m->piece;

	// enpassant, castle, and promotion have special considerations
	if (m->enpassant)
	{
		board[m->newx][m->oldy] = EMPTY;
	}

	if (m->castle)
	{
		if (m->newx == 2)
		{
			board[3][m->newy] = board[0][m->newy];
			board[0][m->newy] = EMPTY;
		}

		else if (m->newx == 6)
		{
			board[5][m->newy] = board[7][m->newy];
			board[7][m->newy] = EMPTY;
		}
	}

	if (m->promotion)
	{
		if (WhitePiece(m->piece))
			board[m->newx][m->newy] = QUEEN_WHITE;
		else
			board[m->newx][m->newy] = QUEEN_BLACK;
	}

	// if we capture a piece, the total number of pieces drops
	if (m->captured != EMPTY)
		num_pieces--;
}

/****************************************************************************
 * Name:        UnSimulateMove
 * Input:       m - pointer to a move
 * Output:      None
 * Returns:     None
 * Description: This is the function that undoes a given move.  All values are
 *				returned to their status before the move was made.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void Chess::UnSimulateMove(Move *m)
{
	// If the move was enpassant, the captured piece doesn't go back
	// to the destination
	if (!m->enpassant)
		board[m->newx][m->newy] = m->captured;

	board[m->oldx][m->oldy] = m->piece;

	// special cases (promotion takes care of itself here because the
	// m->piece variable will still be a pawn)
	if (m->enpassant)
	{
		board[m->newx][m->newy] = EMPTY;
		board[m->newx][m->oldy] = m->captured;
	}

	if (m->castle)
	{
		if (m->newx == 2)
		{
			board[0][m->newy] = board[3][m->newy];
			board[3][m->newy] = EMPTY;
		}

		if (m->newx == 6)
		{
			board[7][m->newy] = board[5][m->newy];
			board[5][m->newy] = EMPTY;
		}
	}

	if (m->captured != EMPTY)
		num_pieces++;
}

/****************************************************************************
 * Name:        FinalizeMove
 * Input:       m - pointer to a move
 * Output:      None
 * Returns:     None
 * Description: This updates odds and ends that are not necessary to include
 *				in the search but still must be done after every move is made.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void Chess::FinalizeMove(Move *m)
{
	// Update the availability of castling
	if (m->piece == KING_WHITE)
	{
		WhiteCastleLeft = false;
		WhiteCastleRight = false;
	}

	else if (m->piece == KING_BLACK)
	{
		BlackCastleLeft = false;
		BlackCastleRight = false;
	}

	if (m->oldx == 0 && m->oldy == 0)
		BlackCastleLeft = false;

	else if (m->oldx == 0 && m->oldy == 7)
		WhiteCastleLeft = false;

	else if (m->oldx == 7 && m->oldy == 0)
		BlackCastleRight = false;

	else if (m->oldx == 7 && m->oldy == 7)
		WhiteCastleRight = false;

	// Add the move the list of the last 12 moves
	for (int i = 11; i > 0; i--)
		last_12_moves[i] = last_12_moves[i-1];

	last_12_moves[0] = *m;
}

/****************************************************************************
 * Name:        GetBoard
 * Input:       x - int
 *				y - int
 * Output:      None
 * Returns:     an integer representing a piece
 * Description: This gets the piece currently at position x,y on the board.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
int Chess::GetBoard(int x, int y)
{
	return board[x][y];
}

/****************************************************************************
 * Name:        LegalMove
 * Input:       m - pointer to a move
 * Output:      None
 * Returns:     true or false indicating if the move is legal
 * Description: This function is the heart of the game logic.  It contains the
 *				definition for what exactly makes a legal move in chess.  As such
 *				it has a long list of numeric values for various situations on the
 *				board.  There is really no clean way to get around coding all these
 *				possibilities.
 * Invokes:     WhitePiece()
 *				BlackPiece()
 * Note:        Will return true in some anomaly situations (moving into check,
 *				castling out of check), but these are better handled elsewhere.
 ***************************************************************************/
bool Chess::LegalMove(Move *m)
{
	int delta_x;
	int delta_y;

	// Can't capture a piece of the same type
	if (board[m->newx][m->newy] != EMPTY)
	{
		if (WhitePiece(m->piece) && WhitePiece(board[m->newx][m->newy]))
			return false;
		if (BlackPiece(m->piece) && BlackPiece(board[m->newx][m->newy]))
			return false;
	}

	m->castle = false;
	m->promotion = false;
	m->enpassant = false;

	switch (m->piece)
	{
		case PAWN_BLACK:
			// Special case - enpassant
			if (m->newy == 5 && m->oldy == 4 && (m->newx == m->oldx - 1 || m->newx == m->oldx + 1) && board[m->newx][m->newy] == EMPTY)
			{
				if (last_12_moves[0].piece == PAWN_WHITE && last_12_moves[0].oldy == 6 && last_12_moves[0].newy == 4 && last_12_moves[0].newx == m->newx)
				{
					m->enpassant = true;
					m->captured = board[m->newx][m->oldy];
					return true;
				}
			}

			// Moving forward 2 spaces - only valid from the initial position
			if (m->oldy != m->newy - 1)
			{
				if (m->oldy == 1 && m->newy == 3 && m->oldx == m->newx)
				{
					// Only valid if there is nothing in the way
					if (board[m->newx][m->newy-1] != EMPTY) return false;
				}

				else return false;
			}

			// Can't move forward to non empty spots
			if (m->oldx == m->newx && board[m->newx][m->newy] != EMPTY)
				return false;

			// Can only move diagonal by 1
			if (m->oldx > m->newx + 1 || m->oldx < m->newx - 1)
				return false;

			// Can only move diagonal if there is a piece to capture
			if (m->oldx == m->newx + 1 || m->oldx == m->newx - 1)
			{
				if (board[m->newx][m->newy] == EMPTY)
					return false;
			}

			if (m->newy == 7)
				m->promotion = true;
		break;

		case PAWN_WHITE:
			// Special case - enpassant
			if (m->newy == 2 && m->oldy == 3 && (m->newx == m->oldx - 1 || m->newx == m->oldx + 1) && board[m->newx][m->newy] == EMPTY)
			{
				if (last_12_moves[0].piece == PAWN_BLACK && last_12_moves[0].oldy == 1 && last_12_moves[0].newy == 3 && last_12_moves[0].newx == m->newx)
				{
					m->enpassant = true;
					m->captured = board[m->newx][m->oldy];
					return true;
				}
			}

			// Moving forward 2 spaces - only valid from the initial position
			if (m->oldy != m->newy + 1)
			{
				if (m->oldy == 6 && m->newy == 4 && m->oldx == m->newx)
				{
					if (board[m->newx][m->newy+1] != EMPTY) return false;
				}

				else return false;
			}

			// Can't move forward to non empty spots
			if (m->oldx == m->newx && board[m->newx][m->newy] != EMPTY)
				return false;

			// Can only move diagonal by 1
			if (m->oldx > m->newx + 1 || m->oldx < m->newx - 1)
				return false;

			// Can only move diagonal if there is a piece to capture
			if (m->oldx == m->newx + 1 || m->oldx == m->newx - 1)
			{
				if (board[m->newx][m->newy] == EMPTY)
					return false;
			}

			if (m->newy == 0)
				m->promotion = true;
		break;


		case ROOK_BLACK:
		case ROOK_WHITE:
			// Have to move along a straight line
			if (m->oldx != m->newx && m->oldy != m->newy)
				return false;

			// 4 directions
			if (m->oldx == m->newx)
			{
				if (m->oldy > m->newy + 1)
				{
					for (int y = m->oldy - 1; y > m->newy; y--)
						if (board[m->newx][y] != EMPTY)
							return false;
				}

				if (m->oldy < m->newy - 1)
				{
					for (int y = m->oldy + 1; y < m->newy; y++)
						if (board[m->newx][y] != EMPTY)
							return false;
				}
			}

			if (m->oldy == m->newy)
			{
				if (m->oldx > m->newx + 1)
				{
					for (int x = m->oldx - 1; x > m->newx; x--)
						if (board[x][m->newy] != EMPTY)
							return false;
				}

				if (m->oldx < m->newx - 1)
				{
					for (int x = m->oldx + 1; x < m->newx; x++)
						if (board[x][m->newy] != EMPTY)
							return false;
				}
			}
		break;

		case KNIGHT_BLACK:
		case KNIGHT_WHITE:
			// 8 possible moves for knights
			if (m->oldx == m->newx + 1 && m->oldy == m->newy + 2)
				return true;
			if (m->oldx == m->newx + 1 && m->oldy == m->newy - 2)
				return true;
			if (m->oldx == m->newx - 1 && m->oldy == m->newy + 2)
				return true;
			if (m->oldx == m->newx - 1 && m->oldy == m->newy - 2)
				return true;
			if (m->oldx == m->newx + 2 && m->oldy == m->newy + 1)
				return true;
			if (m->oldx == m->newx + 2 && m->oldy == m->newy - 1)
				return true;
			if (m->oldx == m->newx - 2 && m->oldy == m->newy + 1)
				return true;
			if (m->oldx == m->newx - 2 && m->oldy == m->newy - 1)
				return true;
			else
				return false;
		break;

		case BISHOP_BLACK:
		case BISHOP_WHITE:
			// Must move along a diagonal line
			delta_x = m->oldx - m->newx;
			delta_y = m->oldy - m->newy;

			AbsValue(delta_x);
			AbsValue(delta_y);
	
			if (delta_x != delta_y) return false;
			
			// 4 directions
			if (m->oldx > m->newx + 1 && m->oldy > m->newy + 1)
			{
				int x = m->oldx - 1;
				int y = m->oldy - 1;

				while (x > m->newx)
				{
					if (board[x][y] != EMPTY)
						return false;
					x--;
					y--;
				}
			}

			if (m->oldx > m->newx + 1 && m->oldy < m->newy - 1)
			{
				int x = m->oldx - 1;
				int y = m->oldy + 1;

				while (x > m->newx)
				{
					if (board[x][y] != EMPTY)
						return false;
					x--;
					y++;
				}
			}

			if (m->oldx < m->newx - 1 && m->oldy > m->newy + 1)
			{
				int x = m->oldx + 1;
				int y = m->oldy - 1;

				while (x < m->newx)
				{
					if (board[x][y] != EMPTY)
						return false;
					
					x++;
					y--;
				}
			}

			if (m->oldx < m->newx - 1 && m->oldy < m->newy - 1)
			{
				int x = m->oldx + 1;
				int y = m->oldy + 1;

				while(x < m->newx)
				{
					if (board[x][y] != EMPTY)
						return false;

					x++;
					y++;
				}
			}
		break;

		case QUEEN_BLACK:
		case QUEEN_WHITE:
			// Combination of rook and bishop conditions
			if (m->oldx != m->newx && m->oldy != m->newy)
			{
				delta_x = m->oldx - m->newx;
				delta_y = m->oldy - m->newy;

				AbsValue(delta_x);
				AbsValue(delta_y);
		
				if (delta_x != delta_y) return false;
				
				if (m->oldx > m->newx + 1 && m->oldy > m->newy + 1)
				{
					int x = m->oldx - 1;
					int y = m->oldy - 1;

					while (x > m->newx)
					{
						if (board[x][y] != EMPTY)
							return false;
						x--;
						y--;
					}
				}

				if (m->oldx > m->newx + 1 && m->oldy < m->newy - 1)
				{
					int x = m->oldx - 1;
					int y = m->oldy + 1;

					while (x > m->newx)
					{
						if (board[x][y] != EMPTY)
							return false;
						x--;
						y++;
					}
				}

				if (m->oldx < m->newx - 1 && m->oldy > m->newy + 1)
				{
					int x = m->oldx + 1;
					int y = m->oldy - 1;

					while (x < m->newx)
					{
						if (board[x][y] != EMPTY)
							return false;
						
						x++;
						y--;
					}
				}

				if (m->oldx < m->newx - 1 && m->oldy < m->newy - 1)
				{
					int x = m->oldx + 1;
					int y = m->oldy + 1;

					while(x < m->newx)
					{
						if (board[x][y] != EMPTY)
							return false;

						x++;
						y++;
					}
				}
			}

			else
			{
				if (m->oldx == m->newx)
				{
					if (m->oldy > m->newy + 1)
					{
						for (int y = m->oldy - 1; y > m->newy; y--)
							if (board[m->newx][y] != EMPTY)
								return false;
					}

					if (m->oldy < m->newy - 1)
					{
						for (int y = m->oldy + 1; y < m->newy; y++)
							if (board[m->newx][y] != EMPTY)
								return false;
					}
				}

				if (m->oldy == m->newy)
				{
					if (m->oldx > m->newx + 1)
					{
						for (int x = m->oldx - 1; x > m->newx; x--)
							if (board[x][m->newy] != EMPTY)
								return false;
					}

					if (m->oldx < m->newx - 1)
					{
						for (int x = m->oldx + 1; x < m->newx; x++)
							if (board[x][m->newy] != EMPTY)
								return false;
					}
				}
			}
		break;

		case KING_BLACK:
			// Castling
			if (m->oldx == 4 && m->oldy == 0 && m->newx == 2 && m->newy == 0)
			{
				if (board[1][0] == EMPTY && board[2][0] == EMPTY && board[3][0] == EMPTY)
				{
					if (BlackCastleLeft && board[0][0] == ROOK_BLACK)
					{
						m->castle = true;
						return true;
					}
				}
			}

			else if (m->oldx == 4 && m->oldy == 0 && m->newx == 6 && m->newy == 0)
			{
				if (board[5][0] == EMPTY && board[6][0] == EMPTY)
				{
					if (BlackCastleRight && board[7][0] == ROOK_BLACK)
					{
						m->castle = true;
						return true;
					}
				}
			}

			// 8 possibilities for kings
			if (m->oldx == m->newx + 1 && m->oldy == m->newy)
				return true;
			if (m->oldx == m->newx + 1 && m->oldy == m->newy + 1)
				return true;
			if (m->oldx == m->newx + 1 && m->oldy == m->newy - 1)
				return true;
			if (m->oldx == m->newx - 1 && m->oldy == m->newy)
				return true;
			if (m->oldx == m->newx - 1 && m->oldy == m->newy + 1)
				return true;
			if (m->oldx == m->newx - 1 && m->oldy == m->newy - 1)
				return true;
			if (m->oldx == m->newx && m->oldy == m->newy + 1)
				return true;
			if (m->oldx == m->newx && m->oldy == m->newy - 1)
				return true;
			else
				return false;
		break;

		case KING_WHITE:
			if (m->oldx == 4 && m->oldy == 7 && m->newx == 2 && m->newy == 7)
			{
				if (board[1][7] == EMPTY && board[2][7] == EMPTY && board[3][7] == EMPTY)
				{
					if (WhiteCastleLeft && board[0][7] == ROOK_WHITE)
					{
						m->castle = true;
						return true;
					}
				}
			}

			else if (m->oldx == 4 && m->oldy == 7 && m->newx == 6 && m->newy == 7)
			{
				if (board[5][7] == EMPTY && board[6][7] == EMPTY)
				{
					if (WhiteCastleRight && board[7][7] == ROOK_WHITE)
					{
						m->castle = true;
						return true;
					}
				}
			}

			if (m->oldx == m->newx + 1 && m->oldy == m->newy)
				return true;
			if (m->oldx == m->newx + 1 && m->oldy == m->newy + 1)
				return true;
			if (m->oldx == m->newx + 1 && m->oldy == m->newy - 1)
				return true;
			if (m->oldx == m->newx - 1 && m->oldy == m->newy)
				return true;
			if (m->oldx == m->newx - 1 && m->oldy == m->newy + 1)
				return true;
			if (m->oldx == m->newx - 1 && m->oldy == m->newy - 1)
				return true;
			if (m->oldx == m->newx && m->oldy == m->newy + 1)
				return true;
			if (m->oldx == m->newx && m->oldy == m->newy - 1)
				return true;
			else
				return false;
		break;
	}

	// Made it through all the checks, it's a legal move
	return true;
}

/****************************************************************************
 * Name:        GetRecentMove
 * Input:       index - int
 * Output:      None
 * Returns:     Move structure
 * Description: Returns a move from the list of the last 12, which one is
 *				determined by the index parameter.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
Move Chess::GetRecentMove(int index)
{
	if (index < 0 || index > 11)
		return last_12_moves[0];

	return last_12_moves[index];
}

/****************************************************************************
 * Name:        WhitePiece
 * Input:       piece - integer representing the piece to check
 * Output:      None
 * Returns:     true or false
 * Description: A small wrapper function to check if a piece is white
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
bool Chess::WhitePiece(int piece)
{
	return (piece > 0 && piece < 7);
}

/****************************************************************************
 * Name:        WhitePiece
 * Input:       piece - integer representing the piece to check
 * Output:      None
 * Returns:     true or false
 * Description: A small wrapper function to check if a piece is black
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
bool Chess::BlackPiece(int piece)
{
	return (piece > 9);
}

/****************************************************************************
 * Name:        AbsValue
 * Input:       x - integer to be changed (passed by reference)
 * Output:      None
 * Returns:     None
 * Description: This function changes x to the absolute value of x
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void Chess::AbsValue(int &x)
{
	x < 0 ? x = -x : x = x;
}

/****************************************************************************
 * Name:        GetTurn
 * Input:       None
 * Output:      None
 * Returns:     int
 * Description: This gets whose turn it currently is (white or black)
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
int Chess::GetTurn()
{
	return turn;
}

/****************************************************************************
 * Name:        GetState
 * Input:       None
 * Output:      None
 * Returns:     int
 * Description: This returns the current state the game is in.
 *				NORMAL, CHECK, CHECKMATE, or STALEMATE
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
int Chess::GetState()
{
	return game_state;
}

/****************************************************************************
 * Name:        GetNumPieces
 * Input:       None
 * Output:      None
 * Returns:     int
 * Description: This gets the total number of pieces on the board.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
int Chess::GetNumPieces()
{
	return num_pieces;
}
