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

// Computes the points associated to a card.
int points(Card card, bool is_trump);


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

// Returns the current player giving the card
Player current_player(Player leader, int offset);

/* Scoring */

void add_points(int team, int points);

void check_and_award_belotte();  // must be added directly

// Sets score to 252 or 272 if belote happened
void check_and_award_capot();  // don't overwrite belote scores!

void check_and_award_dix_de_der();

// If is_inside, must zero out the points.
bool is_inside(int team);  // return team != winning_team

// Check whether the two team score sum up to 162 / 252 (+ 20 if belote)
bool complete_sum_of_points();

/* Rule checking */
//* How to implement the checking rules mecanisms?

bool is_legal_play(Card card, Player p);



//bool must_play_trump_card();





bool game(istream& in, ostream& out, ostream& err)
{

    // State variables required through the whole processing
    string trump;                       // Initialized once only const?
    int contract_team;                  // Initialized once only const?

    
    array<int, 2> team_scores = {};
    array<bool, 2> belote_scored = {};  
    size_t trick_counter = 0;
    array<CardSet, 4> cards_played;         // By each player
    Player leader = 1;                      // Who started first the trick
    Player previous_trick_winner;           // Is going to start the next trick
    array<int, 8> trick_won;           // Tracks which team won which trick (for capot)

    in >> trump >> contract_team;

    while (trick_counter < 8) {

        /* Per-trick state : */

        Player master;              // The player currently winning the trick
        string led_suit;            // The trick's suit
        string highest_trump_card;
        string highest_led_card;

        // Reads each card of the trick
        for (size_t i = 0; i < 4; i++) {
            Card card;
            in >> card;

            //if (i == 0) led_suit = suit(card);

            //Player player = current_player(leader, i);

            // IF it's not a legal play, print an error and return 0

            //cards_played[player].insert(card);
        }

        trick_counter++;
    }

    out << "trump suit is : " << trump << endl
        << "team taking contract: " << contract_team << endl;

    return true;
}
