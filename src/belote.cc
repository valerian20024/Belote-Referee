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
typedef array<array<bool, 2>, 4> BeloteLookupTable;

// Returns the RHS part of the Card
string suit(const string card);

// Returns the LHS of the Card
string value(const string card);

// Takes into account current trump
bool is_stronger(const string given, const string compared_to, const string trump);

// Computes the points associated to a card.
int points(string card, bool is_trump);




// Returns the partner of a given player
int partner(int player);

// Returns the team number of a given player
int team(int player);


// Returns the current player giving the card
int current_player(int leader, int offset);


void process_trick(
    pair<int, int>& scores,
    CardsCollection& cards_played,
    BeloteLookupTable& belote_table,
    int& previous_trick_winner,
    array<bool, 8>& tricks_won,
    istream& in,
    ostream& out,
    ostream& err,
    const string trump,
    const int trick_number
);


void update_state(
    CardsCollection&        cards_played, 
    BeloteLookupTable&      belote_table,
    int&                    master,
    string&                 highest_trump_card,
    string&                 highest_led_card,
    int&                    trick_points,
    const int               player, 
    const string            card,
    const string            trump
);

void update_cards_played(
    CardsCollection& cards_played, 
    const int player, 
    const string card
);

void update_belote_table(
    BeloteLookupTable& belote_table, 
    const int player, 
    const string card, 
    const string trump
);

void update_highest_cards(
    string& highest_trump_card, 
    string& highest_led_card, 
    const string card, 
    const string trump
);

// Returns true if rank of given card is stronger. False otherwise.
bool is_stronger_trump(const string given, const string compared_to);

// Returns true if rank of given card is stronger. False otherwise.
bool is_stronger_raw(const string given, const string compared_to);

void update_master(
    int&            master,
    const int       player,
    const string    card,
    const string    highest_trump_card,
    const string    highest_led_card,
    const string    trump
);

void update_trick_points();




void add_points(int team, int points);

void check_and_award_belotte();  // must be added directly

// Sets score to 252 or 272 if belote happened
void check_and_award_capot();  // don't overwrite belote scores!

void check_and_award_dix_de_der();

// If is_inside, must zero out the points.
void check_is_inside(int team);  // return team != winning_team

// Check whether the two team score sum up to 162 / 252 (+ 20 if belote)
bool complete_sum_of_points();

int current_leader(const int trick_number, const int previous_trick_winner);


bool is_legal_play(string card, int p);



/* Helpers and debug */
void print_cards_played(const CardsCollection& cards_played, ostream& out);

void print_scores(const pair<int, int>& scores, const int trick_winner, ostream& out);

void print_final_scores(const pair<int, int>& scores, ostream& out);

void print_belote_assets(const BeloteLookupTable& belote_assets, ostream& out);

void print_tricks_won(const array<bool, 8>& tricks_won, ostream& out);





bool game(istream& in, ostream& out, ostream& err) {
    string              trump = {};
    int                 contract_team = {};
    pair<int, int>      scores = {};
    BeloteLookupTable   belote_table = {};
    CardsCollection     cards_played = {};          // By each player
    int                 previous_trick_winner = {}; // Is going to start the next trick
    array<bool, 8>      tricks_won = {};            // Tricks which team won which trick (for capot)
    
    in >> trump >> contract_team;

    for (int trick_counter = 0; trick_counter < 3; trick_counter++) {
        process_trick(
            scores,
            cards_played,
            belote_table,
            previous_trick_winner,
            tricks_won,
            in,
            out,
            err,
            trump,
            trick_counter
        );

        print_scores(scores, previous_trick_winner, out);
    }

    print_cards_played(cards_played, out);
    print_belote_assets(belote_table, out);
    print_tricks_won(tricks_won, out);
    print_final_scores(scores, out);

    return true;
}

void process_trick(
    pair<int, int>&         scores,
    CardsCollection&        cards_played,
    BeloteLookupTable&      belote_table,
    int&                    previous_trick_winner,
    array<bool, 8>&         tricks_won,
    istream&                in,
    ostream&                out,
    ostream&                err,
    const string            trump,
    const int               trick_number
) {
    int master = -1;            // The player currently winning the trick. Sentinel value.
    int leader = current_leader(trick_number, previous_trick_winner);
    string led_suit;            // The trick's suit
    string highest_trump_card;
    string highest_led_card;
    int trick_points = 0;

    //todo change to idiomatic cin
    for (int i = 0; i < 4; i++) {
        string card;
        in >> card;

        if (i == 0) led_suit = suit(card);

        int player = current_player(leader, i);

        update_state(
            cards_played, 
            belote_table,
            master,
            highest_trump_card,
            highest_led_card,
            trick_points,
            player,
            card,
            trump
        );
    }
}

// Orchestrator for updating each variable
void update_state(
    CardsCollection&      cards_played,
    BeloteLookupTable&    belote_table,
    int&                  master,
    string&               highest_trump_card,
    string&               highest_led_card,
    int&                  trick_points,
    const int             player,
    const string          card,
    const string          trump
) {
    update_cards_played(cards_played, player, card);

    update_belote_table(belote_table, player, card, trump);

    update_master(master, player, card, highest_trump_card, highest_led_card, trump);

    update_highest_cards(
        highest_trump_card, 
        highest_led_card, 
        card, 
        trump
    );

    
}


void update_cards_played(CardsCollection& cards_played, const int player, const string card) {
    cards_played[static_cast<size_t>(player)].insert(card);
}

void update_belote_table(
    BeloteLookupTable& belote_table,
    const int          player,
    const string       card,
    const string       trump
) {
    if (suit(card) != trump)
        return;

    const string val = value(card);

    if (val == "K") {
        belote_table[static_cast<size_t>(player)][0] = true;
    } else if (val == "Q") {
        belote_table[static_cast<size_t>(player)][1] = true;
    }
}

void update_highest_cards(
    string& highest_trump_card, 
    string& highest_led_card, 
    const string card, 
    const string trump
) {
    if (suit(card) == trump) {
        if (highest_trump_card.empty() || 
            is_stronger(card, highest_trump_card, trump)
        ) {
            highest_trump_card = card;
        }
        return;
    }

    //* First checking for empty strings
    if (highest_led_card.empty() || suit(card) == suit(highest_led_card)) {
        if (highest_led_card.empty() || 
            is_stronger(card, highest_led_card, trump)
        ) {
            highest_led_card = card;
        }
    }
}

// Assumes that the compared_to card is the led suit in case of a tie.
bool is_stronger(const string given, const string compared_to, const string trump) {
    const string suit_given = suit(given);
    const string suit_comp  = suit(compared_to);

    const bool given_is_trump = (suit_given == trump);
    const bool comp_is_trump  = (suit_comp == trump);

    // Any trump card beats any non-trump card
    if (given_is_trump && !comp_is_trump)
        return true;
    if (!given_is_trump && comp_is_trump)
        return false;

    // Cards now have the same suit
    // Comparing trump suits
    if (given_is_trump)
        return is_stronger_trump(value(given), value(compared_to));

    // Comparing non-trump suits
    if (suit_given == suit_comp)
        return is_stronger_raw(value(given), value(compared_to));

    // Cards have different suits
    return false;
}

// Returns true if the first value is stronger when both are trumps
bool is_stronger_trump(const string given, const string compared_to) {
    const string trump_order = "78QKTA9J";
    size_t pos_given = trump_order.find(given);
    size_t pos_compared_to = trump_order.find(compared_to);
    return pos_given > pos_compared_to;
}

// Returns true is the first value is stronger in a raw (non-trump) suit
bool is_stronger_raw(const string given, const string compared_to) {
    const string plain_order = "789JQKTA";
    size_t pos_given = plain_order.find(given);
    size_t pos_compared_to = plain_order.find(compared_to);
    return pos_given > pos_compared_to;
}


void update_trick_points();

void update_master(
    int&            master,                // current trick winner (will be updated)
    const int       player,                // player who just played
    const string    card,                  // card just played
    const string    highest_trump_card,    // best trump seen so far this trick
    const string    highest_led_card,      // best card of led suit seen so far
    const string    trump
) {
    
    cout << "Master is: " 
         << master
         << "\n"
         << "Player is: "
         << player
         << "\n"
         << "Card is: "
         << card
         << "\n"
         << "Highest cards : \n"
         << "  trump: " << highest_trump_card
         << "\n"
         << "  led: " << highest_led_card
         << "\n"
         << "Trump: "
         << trump
         << "\n"
         << endl;
    

    
    if (master == -1) {
        master = player;
        return;
    }

    const string current_best = (!highest_trump_card.empty()) ?
        highest_trump_card : highest_led_card;
    
    if (is_stronger(card, current_best, trump))
        master = player;
}


//todo  Check for empty strings. Should be more robust and incorporate
//todo  error management
string suit(const string card) {
    return string(1, card.back());
}

string value(const string card) {
    return string(1, card.front());
}

int current_player(int leader, int offset) {
    return (leader + offset) % 4;
}

// On the first trick, the leader is player 0
int current_leader(const int trick_number, const int previous_winner) {
    return trick_number == 0 ? 0 : previous_winner;
}






// Helper function to print the current cards played by each player
void print_cards_played(const CardsCollection& cards_played, ostream& out) {
    out << "=== Cards played by each player so far ===\n";
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
    out << "====================================================\n" << endl;
}

// Helper to print current belote assets state
void print_belote_assets(const BeloteLookupTable& assets, ostream& out)
{
    out << "=== Belote assets (King/Queen of trump per player) ===\n";
    for (int p = 0; p < 4; ++p) {  //todo dynamically with array size
        out << "Player " << (p + 1) << ": "
            << (assets[static_cast<size_t>(p)][0] ? "K " : "- ")
            << (assets[static_cast<size_t>(p)][1] ? "Q" : "-")
            << "\n";
    }
    out << "====================================================\n" << endl;
}

// Helper to print which team won each trick
void print_tricks_won(const array<bool, 8>& tricks_won, ostream& out)
{
    out << "=== Tricks won by team ===\n";
    for (int t = 0; t < 8; ++t) {  //todo dynamically with array size
        out << "Trick " << (t + 1) << ": Team " 
            << (tricks_won[static_cast<size_t>(t)] ? "1" : "2") 
            << "\n";
    }
    out << "=======================================================\n" << endl;
}

//todo remove the pretty printing
void print_scores(const pair<int, int>& scores , const int trick_winner, ostream& out) {
    out << "=== Scores ===\n"
        << scores.first << " " 
        << scores.second << " " 
        << trick_winner << "\n"
        << "=================\n" << endl;
}

void print_final_scores(const pair<int, int>& scores, ostream& out) {
    out << "=== Finals ===\n"
        << scores.first << " " 
        << scores.second << "\n" 
        << "=================\n" << endl;
}