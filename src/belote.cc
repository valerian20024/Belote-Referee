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

void print_scores(const pair<int, int> scores, const int& trick_winner, ostream& out);

void print_final_scores(const int& score1, const int& score2, ostream& out);

void print_belote_assets(const std::array<std::array<bool, 2>, 4>& belote_assets, std::ostream& out);

void print_tricks_won(const std::array<bool, 8>& tricks_won, std::ostream& out);



bool game(istream& in, ostream& out, ostream& err) {

    (void) err;
    
    // State variables required through the whole processing
    string trump;
    int contract_team;

    pair<int, int> scores = {};
    array<array<bool, 2>, 4> belote_assets = {};
    
    CardsCollection cards_played;           // By each player
    
    int previous_trick_winner;              // Is going to start the next trick
    array<bool, 8> tricks_won = {};              // Tricks which team won which trick (for capot)

    (void) scores;
    //(void) belote_assets;
    
    print_belote_assets(belote_assets, out);
    print_tricks_won(tricks_won, out);
    print_scores(scores, 0, out);

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

// On the first trick, the leader is player 0
int current_leader(int trick_number, int previous_winner) {
    return trick_number == 0 ? 0 : previous_winner;
}

// Insert a card in the right player's set
void play(CardsCollection& collection, int player, const string& card) {
    collection[static_cast<size_t>(player)].insert(card);
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

// Helper to print current belote assets state
void print_belote_assets(const array<array<bool, 2>, 4>& assets, ostream& out)
{
    out << "=== Belote assets (King/Queen of trump per player) ===" << endl;
    for (int p = 0; p < 4; ++p) {  //todo dynamically with array size
        out << "Player " << (p + 1) << ": "
            << (assets[static_cast<size_t>(p)][0] ? "K " : "- ")
            << (assets[static_cast<size_t>(p)][1] ? "Q" : "-")
            << endl;
    }
    out << "====================================================\n" << endl;
}

// Helper to print which team won each trick (very useful for debugging capot)
void print_tricks_won(const array<bool, 8>& tricks_won, ostream& out)
{
    out << "=== Tricks won by team ===" << endl;
    for (int t = 0; t < 8; ++t) {  //todo dynamically with array size
        out << "Trick " << (t + 1) << ": Team " 
            << (tricks_won[static_cast<size_t>(t)] ? "1" : "2") 
            << "\n";
    }
    out << "=======================================================\n" << endl;
}

//todo remove the pretty printing
void print_scores(const pair<int, int> scores , const int& trick_winner, ostream& out) {
    out << "=== Scores ===" << endl
        << scores.first << " " 
        << scores.second << " " 
        << trick_winner << endl
        << "=================" << endl;
}

