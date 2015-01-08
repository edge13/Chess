//===========================================================================
//
//  File name ......: bot.h
//  Author(s) ......: Joel Angelone (error checking, ai) Justin Donnell (body, ai),
//			Justin Hendrix (error checking, ai), Adam Riha (error checking, ai)
//  Language .......: C++
//  Started ........: April 15, 2007
//  Last modified ..: May 3, 2007
//  Version ........: 1.0
//  Source Path ....: .
//  Operating System: Linux, Windows
//  Purpose ........: This program handles the ai's behavior and wraps it in
//                    the bot class for easy handling. 
//  Note ...........: None
//
//===========================================================================

#ifndef BOT_H
#define BOT_H

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
using namespace std;

#define CHECKMATE	65535

struct Move;
class Chess;


/////////////////////////////////////////////////////////////////////////////
// Name:        bot
// Description: This class is the heart of the AI.  It loads the appropriate
//              ai script at initialization and makes the ai pick the best
//              course of action.  The bot is written in a very general manner
//              so multiple bots is not a problem.
/////////////////////////////////////////////////////////////////////////////
class bot
{
public:

	// Default constructor for the bot class
	bot();

	// Destructor for the bot class
	~bot();

	// Interface to run the AI search
	Move run(Chess *pChess);

	// Loads AI heuristic values from a file
	void loadAI(std::string s);

	// Prints the bot heuristics to the console
	void printBotValues();

	// Prints the state of the board to the console
	void printBoard(Chess* pChess);

	// Loads a default bot (simple heuristics)
	void loadDefault();

	// Gets the total time so far
	unsigned long getTotalTime();

private:

	// Holds heuristic values for a particular type of piece
	struct piece
	{
		piece();
		int m_weight;  // each piece has a base weight
		int m_bValue[8][8];  // value of piece in regards to the board
		int m_last;  // the last value the piece has gotten input for
	};

	// Loads data from a file
	void loadData(std::istringstream &ss, std::string &state);

	// The actual search - uses the heuristic values to determine the best move	
	Move negaMax(Chess *pChess, int depth, int player, int alpha, int beta);

	// Evaluates the board at a given state
	int evaluate(Chess *pChess, int player);

	// takes a generated list of moves and orders them - "capture" moves first
	void orderMoves(Move* move_list, int num_moves);

	// Returns the opponent of a given player
	int other(int player);

	// Returns the time in microseconds
	unsigned long GetTime();

	// The number of ply to look ahead
	int m_searchDepth;

	// The piece values
	piece m_king;
	piece m_queen;
	piece m_knight;
	piece m_bishop;
	piece m_rook;
	piece m_pawn;

	// a variable that let's the AI know it wants more pieces on the board
	// as opposed to less pieces on the board
	bool m_morePieces;

	// as more pieces are on or off the board, the AI can act more or
	// less agressive
	int m_threshold[32];

	// how random the bot should act
	int m_random;

	// total time so far
	unsigned long m_totalTime;
};

#endif
