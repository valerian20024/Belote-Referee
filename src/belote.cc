#include "belote.hh"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <set>
#include <utility>   // std::pair

using namespace std;

typedef pair<string, string> Card;      // A Card(V, S) is made of a value and a suit.
typedef int Player;                     // Can be [0; 3]
typedef pair<Player, Player> Team;      // Team1 : players 1 and 3; Team2 players 2 and 4
typedef set<Card> CardSet;              // A set of different cards

bool game(istream& in, ostream& out, ostream& err)
{

    // State variables required through the whole processing

    pair<int, int> team_scores;
    int trick_counter;
    vector<CardSet> cards_played_by_players;    // array ?
    Player leader;                              // who started first the trick
    string trump_suit;            // initialized once only const?
    int team_taking_contract;     // initialized once only const?

    
    /* Per trick state : */

    Player master;  // The player currently winning the trick




    (void) in;
    (void) out;
    (void) err;
    return true;
}

// Parse and update state variables. e.g.,
// s 2
void parse_game_header(string header);

// Parse and update state variables. e.g.,
// Qh 8h Ah 9h
void parse_trick(string trick);

// Creates a card based on raw string input, e.g.,
// 9h
Card make_card(string card);

// Computes the points associated to a card.
unsigned int points(Card card, bool is_trump);

/* Tiny helpers */
Team players_in_team(int team_number);

bool same_suit(string suit1, string suit2);

bool is_trump(string suit, string trump_suit);

Player partner(Player p);

void add_points(int team, int points);




