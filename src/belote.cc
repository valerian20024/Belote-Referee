#include "belote.hh"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <utility>

using namespace std;

//todo change back to 8 tricks

// Business logic of the belote game.
constexpr int NUM_TRICKS    = 3;
constexpr int NUM_CARDS     = 4;
constexpr int NUM_PLAYERS   = 4;
constexpr int NUM_TEAMS     = 2;

// Whether to compile with debug printing.
constexpr bool DEBUG_MODE = false;

// Tracks the cards played by each player.
typedef array<vector<string>, NUM_CARDS> CardsCollection;
typedef array<array<bool, NUM_TEAMS>, NUM_CARDS> BeloteLookupTable;

// Returns the RHS part of the Card.
string suit(const string card);

// Returns the LHS of the Card.
string value(const string card);

// Takes into account current trump.
bool is_stronger(
    const string given, 
    const string compared_to, 
    const string trump
);

// Computes the points associated to a card.
int points(const string card, const bool is_trump);




// Returns the partner of a given player.
int partner(const int player);

// Returns the team number of a given player.
int team(const int player);

// Returns the current player giving the card.
int current_player(const int leader, const int offset);


void process_trick(
    pair<int, int>& scores,
    CardsCollection& cards_played,
    BeloteLookupTable& belote_table,
    int& previous_trick_winner,
    array<bool, NUM_TRICKS>& tricks_won,
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

// Updates the current trick points.
void update_trick_points(
    int&            trick_points, 
    const string    card, 
    const string    trump
);

void update_tricks_won(
    array<bool, NUM_TRICKS>&    tricks_won, 
    const int                   master, 
    const int                   trick_number
);

void update_scores(
    pair<int, int>& scores, 
    const int       trick_points, 
    const int       winner
);


void add_points(int team, int points);

void check_and_award_belotte();  // Must be added directly.

// Sets score to 252 or 272 if belote happened.
void check_and_award_capot();  // Don't overwrite belote scores!

void check_and_award_dix_de_der();

// If is_inside, must zero out the points.
void check_is_inside(int team);  // Returns team != winning_team

// Checks whether the two team score sum up to 162 / 252 (+ 20 if belote).
bool complete_sum_of_points();

int current_leader(const int trick_number, const int previous_trick_winner);


bool is_legal_play(string card, int p);



// Function to print the intermediary scores of both teams and the trick winner.
void print_scores(
    const pair<int, int>&   scores, 
    const int               trick_winner, 
    ostream&                out
);

// Helper function to print the current cards played by each player.
void print_cards_played(const CardsCollection& cards_played, ostream& out);

// Function to print the final scores of both teams.
void print_final_scores(const pair<int, int>& scores, ostream& out);

// Helper to print current belote assets state.
void print_belote_assets(const BeloteLookupTable& belote_assets, ostream& out);

// Helper to print which team won which trick.
void print_tricks_won(const array<bool, NUM_TRICKS>& tricks_won, ostream& out);


bool game(istream& in, ostream& out, ostream& err) {
    string                      trump = {};
    int                         contract_team = {};
    pair<int, int>              scores = {};
    BeloteLookupTable           belote_table = {};
    CardsCollection             cards_played = {};
    int                         previous_trick_winner = {};
    array<bool, NUM_TRICKS>     tricks_won = {};
    
    in >> trump >> contract_team;

    for (int trick_counter = 0; trick_counter < NUM_TRICKS; trick_counter++) {
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

    if (DEBUG_MODE) {
        print_cards_played(cards_played, out);
        print_belote_assets(belote_table, out);
        print_tricks_won(tricks_won, out);
        print_final_scores(scores, out);
    }
    
    return true;
}

void process_trick(
    pair<int, int>&             scores,
    CardsCollection&            cards_played,
    BeloteLookupTable&          belote_table,
    int&                        previous_trick_winner,
    array<bool, NUM_TRICKS>&    tricks_won,
    istream&                    in,
    ostream&                    out,
    ostream&                    err,
    const string                trump,
    const int                   trick_number
) {
    int master = -1;
    int leader = current_leader(trick_number, previous_trick_winner);
    string led_suit = {};
    string highest_trump_card = {};
    string highest_led_card = {};
    int trick_points = 0;

    //todo change to idiomatic cin
    for (int i = 0; i < NUM_CARDS; i++) {
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

    update_scores(scores, trick_points, master);

    update_tricks_won(tricks_won, master, trick_number);

    previous_trick_winner = master;
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

    // Updated based on the previous highest cards
    update_master(master, player, card, highest_trump_card, highest_led_card, trump);

    update_highest_cards(highest_trump_card, highest_led_card, card, trump);

    update_trick_points(trick_points, card, trump);

    if (DEBUG_MODE) {
        cout << "----- New card -----\n"
            << "Master is [" << master << "]\n"
            << "Player [" << player << "] played card [" << card << "]\n"
            << "Highest cards : \n"
            << "  trump: [" << highest_trump_card << "]\n"
            << "  led:   [" << highest_led_card << "]\n"
            << "---------------------\n"
            << endl;
    }
}


void update_cards_played(
    CardsCollection&    cards_played, 
    const int           player, 
    const string        card
) {
    cards_played[static_cast<size_t>(player)].push_back(card);
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

    if (val == "K")
        belote_table[static_cast<size_t>(player)][0] = true;
    else if (val == "Q")
        belote_table[static_cast<size_t>(player)][1] = true;
}

void update_highest_cards(
    string&         highest_trump_card, 
    string&         highest_led_card, 
    const string    card, 
    const string    trump
) {
    if (suit(card) == trump) {
        if (highest_trump_card.empty() 
            || is_stronger(card, highest_trump_card, trump)
        ) {
            highest_trump_card = card;
        }
        return;
    }

    // First checking for empty strings
    if (highest_led_card.empty() || suit(card) == suit(highest_led_card)) {
        if (highest_led_card.empty() 
            || is_stronger(card, highest_led_card, trump)
        ) {
            highest_led_card = card;
        }
    }
}

// Assumes that the compared_to card is the led suit in case of a tie.
bool is_stronger(
    const string given, 
    const string compared_to, 
    const string trump
) {
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

bool is_stronger_trump(
    const string given, 
    const string compared_to
) {
    const string trump_order = "78QKTA9J";
    size_t pos_given = trump_order.find(given);
    size_t pos_compared_to = trump_order.find(compared_to);
    return pos_given > pos_compared_to;
}

bool is_stronger_raw(
    const string given, 
    const string compared_to
) {
    const string plain_order = "789JQKTA";
    size_t pos_given = plain_order.find(given);
    size_t pos_compared_to = plain_order.find(compared_to);
    return pos_given > pos_compared_to;
}

void update_master(
    int&            master,
    const int       player,
    const string    card,
    const string    highest_trump_card,
    const string    highest_led_card,
    const string    trump
) {    
    if (master == -1) {
        master = player;
        return;
    }

    const string current_best = (!highest_trump_card.empty()) ?
        highest_trump_card : highest_led_card;
    
    if (is_stronger(card, current_best, trump))
        master = player;
}

void update_trick_points(
    int&            trickpoints,
    const string    card,
    const string    trump
) {
    const bool is_trump = suit(card) == trump;
    int score = points(card, is_trump);
    trickpoints += score;
}

// Computes the points associated to a card, depending on whether it is trump or not.
// Returns 0 for any card that has no point value.
int points(const string card, const bool is_trump) {
    if (card.empty())
        return 0;

    const char val = card.front();

    if (is_trump) {
        switch (val) {
            case 'J': return 20;
            case '9': return 14;
            case 'A': return 11;
            case 'T': return 10;
            case 'K': return  4;
            case 'Q': return  3;
            
            default:  return  0;
        }
    } else {
        switch (val) {
            case 'A': return 11;
            case 'T': return 10;
            case 'K': return  4;
            case 'Q': return  3;
            case 'J': return  2;
            
            default:  return  0;
        }
    }
}

void update_scores(
    pair<int, int>& scores, 
    const int       trick_points, 
    const int       winner
) {
    const int winning_team = team(winner);

    if (winning_team == 0) 
        scores.first += trick_points;
    else
        scores.second += trick_points;
}

void update_tricks_won(
    array<bool, NUM_TRICKS>& tricks_won, 
    const int                master, 
    const int                trick_number
) {
    const int winning_team = team(master);

    if (winning_team == 0)
        tricks_won[static_cast<size_t>(trick_number)] = true;
    else
        tricks_won[static_cast<size_t>(trick_number)] = false;
}

// Returns 0 if player belongs to the first team. Returns 1 otherwise.
int team(const int player) {
    switch (player) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 0;
        case 3: return 1;

        default: return -1;
    }
}

string suit(const string card) {
    if (card.empty()) {
        cerr << "ERROR: suit() called on empty string." << endl;
        return "";
    }
    return string(1, card.back());
}

string value(const string card) {
    if (card.empty()) {
        cerr << "ERROR: value() called on empty string." << endl;
        return "";
    }
    return string(1, card.front());
}

int current_player(const int leader, const int offset) {
    return (leader + offset) % NUM_PLAYERS;
}

// On the first trick, the leader is player 0
int current_leader(const int trick_number, const int previous_winner) {
    return trick_number == 0 ? 0 : previous_winner;
}






void print_cards_played(const CardsCollection& cards_played, ostream& out) {
    out << "=== Cards played by each player so far ===\n";
    for (int player = 0; player < NUM_PLAYERS; ++player) {
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

void print_belote_assets(const BeloteLookupTable& assets, ostream& out) {
    out << "=== Belote assets (King/Queen of trump per player) ===\n";
    for (int p = 0; p < NUM_PLAYERS; ++p) {
        out << "Player " << (p + 1) << ": "
            << (assets[static_cast<size_t>(p)][0] ? "K " : "- ")
            << (assets[static_cast<size_t>(p)][1] ? "Q" : "-")
            << "\n";
    }
    out << "====================================================\n" << endl;
}

void print_tricks_won(
    const array<bool, NUM_TRICKS>&  tricks_won, 
    ostream&                        out
) {
    out << "=== Tricks won by team ===\n";
    for (int t = 0; t < NUM_TRICKS; ++t) {
        out << "Trick " << (t + 1) << ": Team " 
            << (tricks_won[static_cast<size_t>(t)] ? "1" : "2") 
            << "\n";
    }
    out << "=======================================================\n" << endl;
}

//todo remove the pretty printing
void print_scores(
    const pair<int, int>&   scores, 
    const int               trick_winner, 
    ostream&                out
) {
    if (DEBUG_MODE) {
        // 0-indexed player and pretty-printing.
        out << "=== Scores ===\n"
            << scores.first << " " 
            << scores.second << " " 
            << trick_winner << "\n"
            << "=================\n" << endl;
    } else {
        // Release version.
        out << scores.first << " " 
            << scores.second << " " 
            << trick_winner + 1 << endl;
    }
}

void print_final_scores(const pair<int, int>& scores, ostream& out) {
    if (DEBUG_MODE) {
        out << "===== Finals =====\n"
            << scores.first << " " 
            << scores.second << "\n" 
            << "==================\n" << endl;
    } else {
        out << scores.first << " "
            << scores.second << endl;
    }
}