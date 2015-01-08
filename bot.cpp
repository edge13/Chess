// Below is a fix for windows
#if defined(WIN32)
#define GLUT_DISABLE_ATEXIT_HACK    //Used to stop a error when compiling on Windows machines
#include <windows.h>
#else
#endif

#include <time.h>

#include "bot.h"
#include "chess.h"

// Sets the defaults for each piece
bot::piece::piece()
{
	m_weight = 0;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			m_bValue[i][j] = 0;
		}
	}

	m_last = 0;
}

/****************************************************************************
 * Name:        bot
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This is the default constructor for the bot which merely
 *		loads the default values for the bot.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
bot::bot()
{
	loadDefault();
}

/****************************************************************************
 * Name:        ~bot
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This is the default destructor for the bot which currently
 *		does nothing!  Implemented for completeness..
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
bot::~bot()
{
	// Nothing now...
}

/****************************************************************************
 * Name:        loadDefault
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This function is simplistic in that all it does is load the
 *		most likely defaults that a bot would use.
 * Invokes:     None
 * Note:        This will override any bot values you currently have.
 *		Use at your own risk.
 ***************************************************************************/
void bot::loadDefault()
{
	// Set all the default weights of the pieces
	m_king.m_weight = 100;
	m_queen.m_weight = 9;
	m_bishop.m_weight = 3;
	m_knight.m_weight = 3;
	m_rook.m_weight = 5;
	m_pawn.m_weight = 1;
	
	//Set m_last to zero (Used to indicate last row of piece-board value read in)
	m_king.m_last = 0;
	m_queen.m_last = 0;
	m_bishop.m_last = 0;
	m_knight.m_last = 0;
	m_rook.m_last = 0;
	m_pawn.m_last = 0;

	// A default bot should move its pieces forward (when nothing else is available), 
	// thus moving toward the opponent is given a slight boost
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			m_pawn.m_bValue[x][y] = 7-y;
			m_rook.m_bValue[x][y] = 7-y;
			m_knight.m_bValue[x][y] = 7-y;
			m_bishop.m_bValue[x][y] = 7-y;
			m_queen.m_bValue[x][y] = 7-y;
			m_king.m_bValue[x][y] = 7-y;
		}
	}

	// Setup the threshold
	for(int i = 0; i < 32; i++)
	{
		m_threshold[i] = 1;
	}

	// Default for a bot is no randomness
	m_random = 5;

	// Default is the bot wants less pieces
	m_morePieces = false;

	// Start the time at 0
	m_totalTime = 0;
}

/****************************************************************************
 * Name:        loadAI
 * Input:       s - string
 * Output:      None
 * Returns:     None
 * Description: This function loads the data from the .bot script given by s.
 *              It reads the file being careful to omit comments / spaces
 *              and to keep track of what state it is currently in.
 *              Most of the actual data parsing is done by loadData.
 * Invokes:     loadData, loadDefault
 * Note:        The .bot extension is just a convention.  Any extension may
 *		be used provided it is in text format and has the appropriate
 *		syntax.  For more information see one of the other bot files.
 ***************************************************************************/
void bot::loadAI(std::string s)
{
	std::string currState = "START";  // the current state of the reading

	loadDefault();

	// Loads the AI in from a file
	ifstream in;
	in.open(s.c_str(), std::ios::in);

	// If the file is open
	if(in.is_open())
	{
		std::string temp_str;

		// Keep reading the file while there is data
		while(std::getline(in, temp_str))
		{
			// Put each line in a string stream
			istringstream temp_ss(temp_str);

			// Check the first value and see if it is a #
			// if so that is a comment don't read values
			char c = temp_ss.peek();

			if(c == '#' || temp_str.size() < 1)
			{
				// Do nothing
				//cout << "Comment or space\n";
			}
			else
			{
				// Create a copy of our string stream and let this copy have the
				// string of data (from that line) in the new stream
				// then read in the first string to check if it is a state switch
				istringstream temp_ss2(temp_ss.str());
				std::string s;
				temp_ss2 >> s;

				// Read in data and see if we are at one of many possible states
				if(s == "BEGIN_NUM_PIECES_SWITCH")
					currState = "BEGIN_NUM_PIECES_SWITCH";

				else if(s == "BEGIN_THRESHOLD")
					currState = "BEGIN_THRESHOLD";

				else if(s == "BEGIN_RANDOM")
					currState = "BEGIN_RANDOM";

				else if(s == "BEGIN_BOARD_EMPHASIS")
					currState = "BEGIN_BOARD_EMPHASIS";

				else if(s == "BEGIN_KING")
					currState = "BEGIN_KING";

				else if(s == "BEGIN_QUEEN")
					currState = "BEGIN_QUEEN";

				else if(s == "BEGIN_ROOK")
					currState = "BEGIN_ROOK";

				else if(s == "BEGIN_BISHOP")
					currState = "BEGIN_BISHOP";

				else if(s == "BEGIN_KNIGHT")
					currState = "BEGIN_KNIGHT";

				else if(s == "BEGIN_PAWN")
					currState = "BEGIN_PAWN";

				// Otherwise our information must actually be data
				else
				{
					// Load in the data
					loadData(temp_ss, currState);
				}
			}
		}

		in.close();
	}

	cout << "Loaded: " << s << endl;
}

/****************************************************************************
 * Name:        loadData
 * Input:       ss - istringstream, state - string
 * Output:      None
 * Returns:     None
 * Description: This function loads the appropriate data given the state.
 *		The stringstream ss contains the data and is given in that
 *		format for convient data parsing.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void bot::loadData(std::istringstream &ss, std::string &state)
{
	if(state == "BEGIN_NUM_PIECES_SWITCH")
	{
		int numPieces;

		// Read in a single value
		ss >> numPieces;

		if(numPieces == 0)
			m_morePieces = false;
		else
			m_morePieces = true;
	}

	else if(state == "BEGIN_THRESHOLD")
	{

		// Read in a pair of values
		int numTotalPieces, numPly;

		ss >> numTotalPieces;
		ss >> numPly;

		if(numPly < 0)
			numPly = (rand() % (-numPly)) + 1;

		m_threshold[numTotalPieces] = numPly;
	}

	else if(state == "BEGIN_RANDOM")
	{
		int random;

		// Read in a single value

		ss >> random;

		m_random = random;
	}

	else if(state == "BEGIN_BOARD_EMPHASIS")
	{
		// Read in a single value
	}

	else if(state == "BEGIN_KING")
	{
			if(m_king.m_weight == 100)
			{
				// Extract weight
				int w;

				std::string s = ss.str();

				ss >> w;

				m_king.m_weight = w;
			}
			else
			{
				int col;

				// Read in 8 digits
				for(int i = 0; i < 8; i++)
				{
					ss >> col;
					m_king.m_bValue[m_king.m_last][i] = col;
				}

				// update last place
				m_king.m_last++;
			}

	}

	else if(state == "BEGIN_QUEEN")
	{
		if(m_queen.m_weight == 9)
		{
			// Extract weight

			int w;

			ss >> w;

			m_queen.m_weight = w;
		}
		else
		{
			int col;

			// Read in 8 digits
			for(int i = 0; i < 8; i++)
			{
				ss >> col;
				m_queen.m_bValue[m_queen.m_last][i] = col;
			}

			// update last place
			m_queen.m_last++;
		}
	}

	else if(state == "BEGIN_ROOK")
	{
		if(m_rook.m_weight == 5)
		{
			// Extract weight

			int w;

			ss >> w;

			m_rook.m_weight = w;
		}
		else
		{
			int col;

			// Read in 8 digits
			for(int i = 0; i < 8; i++)
			{
				ss >> col;
				m_rook.m_bValue[m_rook.m_last][i] = col;
			}

			// update last place
			m_rook.m_last++;
		}
	}

	else if(state == "BEGIN_BISHOP")
	{
		if(m_bishop.m_weight == 3)
		{
			// Extract weight

			int w;

			ss >> w;

			m_bishop.m_weight = w;
		}
		else
		{
			int col;

			// Read in 8 digits
			for(int i = 0; i < 8; i++)
			{
				ss >> col;
				m_bishop.m_bValue[m_bishop.m_last][i] = col;
			}

			// update last place
			m_bishop.m_last++;
		}
	}

	else if(state == "BEGIN_KNIGHT")
	{
		if(m_knight.m_weight == 3)
		{
			// Extract weight

			int w;

			ss >> w;

			m_knight.m_weight = w;
		}
		else
		{
			int col;

			// Read in 8 digits
			for(int i = 0; i < 8; i++)
			{
				ss >> col;
				m_knight.m_bValue[m_knight.m_last][i] = col;
			}

			// update last place
			m_knight.m_last++;
		}
	}

	else if(state == "BEGIN_PAWN")
	{
		if(m_pawn.m_weight == 1)
		{
			// Extract weight

			int w;

			ss >> w;

			m_pawn.m_weight = w;
		}
		else
		{
			int col;

			// Read in 8 digits
			for(int i = 0; i < 8; i++)
			{
				ss >> col;
				m_pawn.m_bValue[m_pawn.m_last][i] = col;
			}

			// update last place
			m_pawn.m_last++;
		}
	}
}

/****************************************************************************
 * Name:        evaluate
 * Input:       pChess - pointer to an instance of the chess class, 
 *				player - whose perspective from which to evaluate
 * Output:      None
 * Returns:     an integer representing the desirability of that state of the board
 * Description: Given the state of the board, this function sums up the values of
 *				all pieces on the board using its heuristic values.  This number
 *				indicates how "good" that particular configuration is for the given
 *				player.
 * Invokes:     GetNumPieces() from the chess class
 *				GetBoard() from the chess class
 * Note:        Rather than summing differently for each player, we just sum up from
 *				white's perspective, then if player == black, just take the opposite.
 *				This takes advantage of the zero-sum property of the chess board.
 ***************************************************************************/
int bot::evaluate(Chess *pChess, int player)
{
	// Start with a value of 0
	int value = 0;
	int piece_value = 0;

	// Check every location on the board
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			// Find out what type of piece is at this location
			int type = pChess->GetBoard(x, y);

			// If it's empty, move on to the next spot
			if (type == EMPTY)
				continue;

			// Add the appropriate heuristic value given the piece type
			if (type == PAWN_WHITE)
			{
				piece_value = m_pawn.m_weight;
				piece_value += m_pawn.m_bValue[x][y];
			}

			else if (type == PAWN_BLACK)
			{
				piece_value = m_pawn.m_weight;
				piece_value += m_pawn.m_bValue[7-x][7-y];
			}

			else if (type == ROOK_WHITE)
			{
				piece_value = m_rook.m_weight;
				piece_value += m_rook.m_bValue[x][y];
			}

			else if (type == ROOK_BLACK)
			{
				piece_value = m_rook.m_weight;
				piece_value += m_rook.m_bValue[7-x][7-y];
			}

			else if (type == KNIGHT_WHITE)
			{
				piece_value = m_knight.m_weight;
				piece_value += m_knight.m_bValue[x][y];
			}

			else if (type == KNIGHT_BLACK)
			{
				piece_value = m_knight.m_weight;
				piece_value += m_knight.m_bValue[7-x][7-y];
			}

			else if (type == BISHOP_WHITE)
			{
				piece_value = m_bishop.m_weight;
				piece_value += m_bishop.m_bValue[x][y];
			}

			else if (type == BISHOP_BLACK)
			{
				piece_value = m_bishop.m_weight;
				piece_value += m_bishop.m_bValue[7-x][7-y];
			}

			else if (type == QUEEN_WHITE)
			{
				piece_value = m_queen.m_weight;
				piece_value += m_queen.m_bValue[x][y];
			}

			else if (type == QUEEN_BLACK)
			{
				piece_value = m_queen.m_weight;
				piece_value += m_queen.m_bValue[7-x][7-y];
			}

			else if (type == KING_WHITE)
			{
				piece_value = m_king.m_weight;
				piece_value += m_king.m_bValue[x][y];
			}

			else if (type == KING_BLACK)
			{
				piece_value = m_king.m_weight;
				piece_value += m_king.m_bValue[7-x][7-y];
			}

			// Now add the piece value for white pieces, subtract it for black
			if (pChess->WhitePiece(type))
				value += piece_value;
			else
				value -= piece_value;
		}
	}

	// Once the total value is determined, account for the randomness of the bot
	if (m_random > 0)
		value += rand() % m_random;

	// If the bot doesn't like to have more pieces on the board, subtract a small value
	// depending on the total number
	// Normalize to pawn value so that AI's with low piece values aren't too drastically affected
	if (!m_morePieces)
		value -= pChess->GetNumPieces() / m_pawn.m_weight;

	// Now return the value or it's inverse depending on the evaluating player
	if (player == PLAYER_WHITE)
		return value;
	else
		return -value;
}

/****************************************************************************
 * Name:        negaMax
 * Input:       pChess - pointer to an instance of the chess class,
 *				depth - how much deeper to search (in plies)
 *				player - the current player in the search (ie min or max)
 *				alpha - the lower bound for our alpha beta pruning
 *				beta - the upper bound for our alpha beta pruning
 * Output:      None
 * Returns:     A Move structure that contains the best move for that player to make
 *				For most levels of iteration we only care about the value of this move,
 *				but at the top level (ie when depth equals the search depth of our bot)
 *				this move represents the move that our bot will make.
 * Description: This search is what drives all of our bots.  Each bot script will have different
 *				heuristic values, but this search will control all of them.  "Nega Max" is a variant
 *				of min max, but it saves time and complication by encapsulating both min's turn and
 *				max's turn into one routine.  Essentially, the value of the opponents best move (reply.value)
 *				tells us the value of that move from the player's perspective, but reversed.  So, the value
 *				of a given move is equal to the opposite of the opponents best move in that scenario.  Say, for
 *				example, we generate all the moves for white.  Now we look at the first possible moves, and calculate
 *				black's best reply (recursively).  The value of this first move is set to the value of black's best
 *				reply, but negative.
 * Invokes:     negaMax() (recursively)
 *				evaluate()
 *				GenerateMoves() - from the chess class
 *				orderMoves()
 *				GetState() - from the chess class
 *				InCheck() - from the chess class
 *				GetRecentMove() - from the chess class
 *				SimulateMove() - from the chess class
 *				UnSimulateMove() - from the chess class
 * Note:        alpha should be set to a very low value to start the search (in theory, negative infinity),
 *				and beta should be set likewise to a very high value.  Since we can't use infinity we use
 *				-10*CHECKMATE and +10*CHECKMATE as bounds.
 ***************************************************************************/
Move bot::negaMax(Chess *pChess, int depth, int player, int alpha, int beta)
{
	// Stores our best move
	Move best;

	// At depth 0, we simply evaluate the board and return
	if (depth == 0)
	{
		best.value = evaluate(pChess, player);
		//Used for Testing purposes - prints out evaluated value of board
		//cout << "Board Value = " << best.value << endl;
		return best;
	}

	// Holds the list of possible moves
	Move move_list[75];
	// Holds the opponents best reply
	Move reply;

	// Generate the list of moves
	int num_moves = pChess->GenerateMoves(move_list, player);

	// Order the moves with captured moves first
	// This makes it more likely that we can prune off large portions of the search
	orderMoves(move_list, num_moves);

	// Start with the best move equal to the first move
	best = move_list[0];
	best.value = -depth * CHECKMATE;

	// Loop through all the possible moves
	for (int i = 0; i < num_moves; i++)
	{
		// Can't allow a castle out of check (illegal)
		if (move_list[i].castle && pChess->GetState() == STATE_CHECK && depth == m_searchDepth) 
			continue;

		// Make the move
		pChess->SimulateMove(&move_list[i]);

		// If we are capturing a king, there's no need to search further
		if (move_list[i].captured == KING_WHITE || move_list[i].captured == KING_BLACK)
			move_list[i].value = depth * m_king.m_weight;
	
		// A normal case, do a recursive call for the opponent
		else
		{
			// We have to swap and take the opposite of alpha and beta because we are now evaluating
			// from the other player's perspective
			// Also decrement the depth for the recursive call
			reply = negaMax(pChess, depth - 1, other(player), -beta, -alpha);
			// Our new move's value is the opposite of the opponent's reply
			move_list[i].value = -reply.value;
		}

		// Reset alpha if necessary
		if (move_list[i].value > alpha)
			alpha = move_list[i].value;

		// If it looks like we've found a new best move, do a few additional checks
		if (move_list[i].value > best.value)
		{
			// At the top level of the search we want to control a couple of factors
			if (depth == m_searchDepth)
			{
				// Definitely don't want to move into check
				if (pChess->InCheck(player))
					move_list[i].value = -depth * CHECKMATE;
				
				// A stalemate is undesirable so we check the last few moves and
				// discourage repetition
				Move x = pChess->GetRecentMove(3);
				Move y = pChess->GetRecentMove(7);

				if (move_list[i] == x && move_list[i] == y)
					move_list[i].value -= m_rook.m_weight;

				else if (move_list[i] == x || move_list[i] == y)
					move_list[i].value -= m_pawn.m_weight;
			}
		}

		//Used for Printing out the Board for Testing purposes
		//Used in printing out all legal moves when using the onePly.bot
		//printBoard(pChess);

		// Undo the move
		pChess->UnSimulateMove(&move_list[i]);

		// If we have a new best move, make sure we update accordingly
		if (move_list[i].value > best.value)
			best = move_list[i];

		// If our best value is bigger than beta we can break early
		if (best.value >= beta)
			return best;
	}
	
	//Used for Testing Purposes - prints out the best value chosen
	//cout << "Best Value Chosen: " << -best.value << endl;

	// Return the best move
	return best;
}

/****************************************************************************
 * Name:        printBoard
 * Input:       pChess - pointer to an instance of the chess class
 * Output:      prints the state of the board to the console
 * Returns:     None
 * Description: This function is used for testing.  It prints the board
 *				at it's current state for examination.
 * Invokes:     GetBoard() - from chess class
 * Note:        None
 ***************************************************************************/
void bot::printBoard(Chess* pChess)
{		
	int temp = 0;

	// Loop through all locations
	for(int m = 0; m < 8; m++)
	{
		for(int n = 0; n < 8; n++)
		{
			// Get the piece at that spot
			temp = pChess->GetBoard(n,m);

			// Print accordingly
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
				cout << "-- ";
		}
		cout << endl;
	}
	cout << endl;
}

/****************************************************************************
 * Name:        orderMoves
 * Input:       move_list - a list of moves
 *				num_moves - the number of moves in the list
 * Output:      None
 * Returns:     None
 * Description: This function re-orders a list of moves so that all moves
 *				that capture a piece are at the front of the list.
 * Invokes:     None
 * Note:        Although this algorithm does a sorting-like operation, it is
 *				actually done in linear time, since we don't care exactly what
 *				order the elements are, only that captured moves appear first.
 *				Essentially it is more analogous to a partition routine.
 *				This saves a great deal of time since it is run at every iteration
 *				of our recursive search.
 ***************************************************************************/
void bot::orderMoves(Move *move_list, int num_moves)
{
	// A temporary array of moves
	Move temp[75];

	// Start at the beginning of both lists
	int index = 0;
	int i = 0;

	// Copy our list into the temporary list
	for (i = 0; i < num_moves; i++)
		temp[i] = move_list[i];

	// Loop through the temporary list and copy over any captures first
	for (i = 0; i < num_moves; i++)
	{
		if (!temp[i].captured == EMPTY)
		{
			move_list[index] = temp[i];
			index++;
		}
	}

	// Now fill the remainder of the list with non-capture moves
	for (i = 0; i < num_moves; i++)
	{
		if (temp[i].captured == EMPTY)
		{
			move_list[index] = temp[i];
			index++;
		}
	}
}

/****************************************************************************
 * Name:        run
 * Input:       pChess - instance of the chess class
 * Output:      None
 * Returns:     a Move
 * Description: This function actually runs the logic behind the AI.  This basically
 *		establishes the proper parameters and calls the search function.
 * Invokes:     negaMax()
 * Note:        None
 ***************************************************************************/
Move bot::run(Chess *pChess)
{
	int num_pieces = pChess->GetNumPieces();

	m_searchDepth = m_threshold[num_pieces - 1];

	// Safety check (don't want to search negative depths)
	if (m_searchDepth < 1)
		m_searchDepth = 1;

	//Used for Testing Purposes - prints the current players turn
	//if(pChess->GetTurn() == PLAYER_WHITE)
	//	cout << "*********************WHITE PLAYER POSSIBLE MOVES*********************\n";
	//else
	//	cout << "*********************BLACK PLAYER POSSIBLE MOVES*********************\n";	

	// Used for timing tests
	//unsigned long time = GetTime();

	// Run the actual search
	Move move = negaMax(pChess, m_searchDepth, pChess->GetTurn(), -10*CHECKMATE, 10*CHECKMATE);

	// Divide by 1000 to get milliseconds
	//unsigned long moveTime = (GetTime() - time)/1000;

	//m_totalTime += moveTime;

	//cout << "Move took: " << moveTime << " milliseconds" << endl;	

	return move;
}

/****************************************************************************
 * Name:        other
 * Input:       player - integer ID of a given player
 * Output:      None
 * Returns:     integer ID of the other player
 * Description: Returns the opponent player ID
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
int bot::other(int player)
{
	if (player == PLAYER_WHITE)
		return PLAYER_BLACK;
	else
		return PLAYER_WHITE;
}

/****************************************************************************
 * Name:        printBotValues
 * Input:       None
 * Output:      Bot values
 * Returns:     None
 * Description: This function outputs any bot values that the bot is using.
 *		this is primarily useful for debug mode, or if you wish to
 *		have a better feeling of what the bot is doing.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
void bot::printBotValues()
{
	int i;

	cout << "King Weight: " << m_king.m_weight << endl;
	cout << "King Board Values: \n";
	for(i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		     cout << m_king.m_bValue[i][j] << " ";
		cout << endl;
	}
	cout << endl;

	cout << "Queen Weight: " << m_queen.m_weight << endl;
	cout << "Queen Board Values: \n";
	for(i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		     cout << m_queen.m_bValue[i][j] << " ";
		cout << endl;
	}
	cout << endl;

	cout << "Bishop Weight: " << m_bishop.m_weight << endl;
	cout << "Bishop Board Values: \n";
	for(i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		     cout << m_bishop.m_bValue[i][j] << " ";
		cout << endl;
	}
	cout << endl;

	cout << "Knight Weight: " << m_knight.m_weight << endl;
	cout << "Knight Board Values: \n";
	for(i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		     cout << m_knight.m_bValue[i][j] << " ";
		cout << endl;
	}
	cout << endl;

	cout << "Rook Weight: " << m_rook.m_weight << endl;
	cout << "Rook Board Values: \n";
	for(i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		     cout << m_rook.m_bValue[i][j] << " ";
		cout << endl;
	}
	cout << endl;

	cout << "Pawn Weight: " << m_pawn.m_weight << endl;
	cout << "Pawn Board Values: \n";
	for(i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		     cout << m_pawn.m_bValue[i][j] << " ";
		cout << endl;
	}

	cout << "Threshold Values: (NumPieces-Threshold): \n";
	for(i = 0; i < 32; i++)
	     cout << i << "-" << m_threshold[i] << "\t";
	cout << endl;

	cout << "MorePieces: ";
	if(m_morePieces)
	     cout << "True\n\n";
	else
	     cout << "False\n\n";

	cout << "Randomness: " << m_random << "\n";
}

/****************************************************************************
 * Name:        getTotalTime
 * Input:       None
 * Output:      None
 * Returns:     unsigned long - m_totalTime
 * Description: Returns the total time for all searches so far
 * Invokes:     None
 * Note:        m_totalTime doesn't get reset for a new game, it only gets reset
 *		when a new AI is loaded.  Keep in mind when doing timing tests.
 ***************************************************************************/
unsigned long bot::getTotalTime()
{
	return m_totalTime;
}

/****************************************************************************
 * Name:        GetTime
 * Input:       None
 * Output:      None
 * Returns:     unsigned long - the current time
 * Description: This function returns the current time in microseconds.
 * Invokes:     None
 * Note:        None
 ***************************************************************************/
unsigned long bot::GetTime()
{
    //timeval t;
	//gettimeofday(&t, NULL);
	//return t.tv_sec * 1000000ul + t.tv_usec;
	return 0;
}
