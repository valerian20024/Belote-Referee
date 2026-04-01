#include "belote.hh"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <set>
#include <utility>   // std::pair
#include <cstdint>

using namespace std;

// Tracks the cards played by each player.
typedef array<set<string>, 4> CardsCollection;


// Returns the RHS part of the Card
string suit(const string& card);

// Returns the LHS of the Card
string value(const string& card);

// Takes into account current trump
bool is_stronger(string given, string compared_to, string trump);

// Computes the points associated to a card.
int points(string card, bool is_trump);



// Returns the partner of a given player
int partner(int player);

// Returns the team number of a given player
int team(int player);

// Whether a player is trick master
bool is_master(int player);

// Returns the current player giving the card
int current_player(int leader, int offset);

// Add a new card 
void play(CardsCollection& collection, int player, const string& card);




void add_points(int team, int points);

void check_and_award_belotte();  // must be added directly

// Sets score to 252 or 272 if belote happened
void check_and_award_capot();  // don't overwrite belote scores!

void check_and_award_dix_de_der();

// If is_inside, must zero out the points.
void check_is_inside(int team);  // return team != winning_team

// Check whether the two team score sum up to 162 / 252 (+ 20 if belote)
bool complete_sum_of_points();

int current_leader(int trick_number);


bool is_legal_play(string card, int p);



/* Helpers and debug */
void print_cards_played(const CardsCollection& cards_played, ostream& out);

void print_scores(const int& score1, const int& score2, const int& trick_winner, ostream& out);

void print_final_scores(const int& score1, const int& score2, ostream& out);




bool game(istream& in, ostream& out, ostream& err) {

    (void) err;
    
    // State variables required through the whole processing
    string trump;
    int contract_team;

    pair<int, int> scores = {};
    pair<bool, bool> belote_scored = {};
    
    CardsCollection cards_played;           // By each player
    
    int previous_trick_winner;              // Is going to start the next trick
    array<bool, 8> tricks_won;              // Tricks which team won which trick (for capot)

    (void) scores;
    (void) belote_scored;
    
    (void) previous_trick_winner;
    (void) tricks_won;

    in >> trump >> contract_team;

    for (int trick_counter = 0; trick_counter < 3; trick_counter++) {
        int master;                 // The player currently winning the trick
        int leader = 0;             // Who started first the trick
        string led_suit;            // The trick's suit
        string highest_trump_card;
        string highest_led_card;

        (void) master;
        (void) highest_trump_card;
        (void) highest_led_card;

        // Reads each card of the trick
        for (int i = 0; i < 4; i++) {
            string card;
            in >> card;

            if (i == 0) led_suit = suit(card);

            int player = current_player(leader, i);

            // call the play function
            play(cards_played, player, card);
        }
    }

    print_cards_played(cards_played, out);

    return true;
}

string suit(const string& card) {
    return string(1, card.back());
}

string value(const string& card) {
    return string(1, card.front());
}

int current_player(int leader, int offset) {
    return (leader + offset) % 4;
}

// Helper function to print the current cards played by each player
void print_cards_played(const CardsCollection& cards_played, ostream& out) {   
    for (int player = 0; player < 4; ++player) {
        out << "Player " << (player + 1) << ": ";
        
        if (cards_played[static_cast<size_t>(player)].empty()) {
            out << "(none)";
        } else {
            bool first = true;
            for (const auto& card : cards_played[static_cast<size_t>(player)]) {
                if (!first) out << " ";
                out << card;
                first = false;
            }
        }
        out << "\n";
    }
}


// Insert a card in the right player's set
void play(CardsCollection& collection, int player, const string& card) {
    if (player < 0 || player > 3) {
        cerr << "error in play function" << endl;
        return;
    }

    collection[static_cast<size_t>(player)].insert(card);
}

// On the first trick, the leader is player 0
int current_leader(int trick_number, int previous_winner) {
    return trick_number == 0 ? 0 : previous_winner;
}