#include "belote.hh"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <set>
#include <utility>   // std::pair

using namespace std;

typedef string Card;                    // A Card(V, S) is made of a value and a suit.
typedef int Player;                     // Can be [0; 3]
typedef pair<Player, Player> Team;      // Team1 : players 1 and 3; Team2 players 2 and 4
typedef set<Card> CardSet;              // A set of different cards

bool game(istream& in, ostream& out, ostream& err)
{

    // State variables required through the whole processing

    pair<int, int> team_scores = pair(0, 0);
    int trick_counter = 0;
    array<CardSet, 4> cards_played;         // By each player
    Player leader;                          // Who started first the trick
    string trump_suit;                      // Initialized once only const?
    int team_taking_contract;               // Initialized once only const?
    Player previous_trick_winner;           // Is going to start the next trick
    array<int, 8> winning_tricks;           // Tracks which team won which trick (for capot)

    /* Per trick state : */

    Player master;  // The player currently winning the trick
    string suit;    // The trick's suit



    (void) in;
    (void) out;
    (void) err;
    return true;
}

/* Parsing */
//* Surely useless, we will use >> for each element

// Returns a string representing one element.
// Can be a card, or a part of the header.
// string make_one_element(istream& input);

// Parse and update state variables. e.g., "s 2"
void parse_game_header(string header);

// Parse and update state variables. e.g. "Qh 8h Ah 9h"
void parse_trick(string trick);

// Creates a card based on raw string input, e.g. "9h"
Card make_card(string card);

// Computes the points associated to a card.
unsigned int points(Card card, bool is_trump);

/* Cards, suits and values */

// Whether two suits are the same or not
bool same_suit(string suit1, string suit2);

// Whether a suit is trump or not
bool is_trump(string suit, string trump_suit);

// Returns the RHS part of the Card
string suit(Card card);

// Returns the LHS of the Card
string value(Card card);

// Takes into account current trump
bool is_stronger(Card given, Card compared_to);


/* Team and players */

// Returns the partner of a given player
Player partner(Player p);

// Returns the team number of a given player
int team(Player p);

// Returns the two players in a given team
Team players_in_team(int team_number);

// Whether a player is trick master
bool is_master(Player p);

// Returns the team that has more score 
int winning_team();

/* Scoring */

void add_points(int team, int points);

void check_and_award_belotte();  // must be added directly

// Sets score to 252 or 272 if belote happened
void check_and_award_capot();  // don't overwrite belote scores!

void check_and_award_dix_de_der();

// If is_inside, must zero out the points.
bool is_inside(int team);  // team != winning_team

// Check whether the two team score sum up to 162 / 252 (+ 20 if belote)
bool complete_sum_of_points();

/* Rule checking */

bool is_legal_play(Card card, Player p);

//bool must_play_trump_card();

