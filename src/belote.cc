#include "belote.hh"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <utility>
#include <algorithm>
#include <random>

using namespace std;

// Whether to compile with debug printing.
constexpr bool DEBUG_MODE = 
#ifdef DEBUG
    true;
#else
    false;
#endif

    /*================================================++
    ||                    GLOBALS                     ||
    ++================================================*/


// Business logic of the belote game.

constexpr int NUM_TEAMS         = 2;
constexpr int NUM_CARDS         = 4;
constexpr int NUM_PLAYERS       = 4;
constexpr int NUM_SUITS         = 4;
constexpr int NUM_TRICKS        = 8;
constexpr int DIX_DE_DER_SCORE  = 10;
constexpr int BELOTE_SCORE      = 20;
constexpr int TRICK_SCORE       = 162;
constexpr int CAPOT_SCORE       = 252;


    /*================================================++
    ||              FORWARD DECLARATIONS              ||
    ++================================================*/


/// @brief Tracks the cards played by each player.
/// @example cards_collection[0][4] returns the fifth card player1 played 
using CardsCollection = array<vector<string>, NUM_CARDS>;

/// @brief Tracks which King and Queen of trump have been played by each player.
/// @example table[3][1] = true would mean player 4 has played a queen of trump.
using BeloteLookupTable = array<array<bool, 2>, NUM_PLAYERS>;

/**
 * @brief Tracks whether and when has a player renounced to play a certain suit.
 * @example table[0][0] = (False, 0) means player 1 has not renounced to clubs. 
 * He is still able to draw some from his hand.
 * @example table[3][1] = (True, 4) means player 4 has renounced to diamonds in trick number 4.
 */
using RenouncementTable = array<array<pair<bool, int>, NUM_SUITS>, NUM_PLAYERS>;

/**
 * @brief Tracks the trump card each player could not overtrump and in which trick it was.
 */
using OvertrumpRenouncementTable = array<pair<string, int>, NUM_PLAYERS>;

/// @brief Returns the RHS part of the Card.
string suit(const string card);

/// @brief Returns the LHS of the Card.
string value(const string card);


/// @brief Returns whether the card `given` is stronger than `compared_to`,
/// according to the `trump` suit.
bool is_stronger(
    const string given, 
    const string compared_to, 
    const string trump
);

/// @brief Computes the points associated to a card.
int points(const string card, const bool is_trump);

/// @brief Returns the card name in a human readable format
string pretty_card(const string card);

/// @brief Returns the card value in a human readable format
string pretty_value(const char value);

/// @brief Returns the card suit in a human readable format
string pretty_suit(const char suit);

/// @brief Returns the name of the player in a human readable format.
string pretty_player(const int player);

/// @brief Returns the trick number as 1-indexed, easier for a human to read.
string pretty_trick(const int trick_number);

/// @brief Returns the partner of a given player.
int partner(const int player);

/// @brief Returns the team number of a player (can be 0 or 1).
int team(const int player);

/**
 * @brief Returns the current player.
 * @param `leader` the player starting the trick
 * @param `offset` the number of cards played since the trick begun.
 */
int current_player(const int leader, const int offset);

/**
 * @brief Orchestrator for processing one trick.
 * @return 0 when everything went right. A non-null int otherwise.
 */
int process_trick(
    pair<int, int>&             scores,
    CardsCollection&            cards_played,
    BeloteLookupTable&          belote_table,
    RenouncementTable&          renounces_led,
    OvertrumpRenouncementTable& renounces_trump,
    pair<int, int>&             belote_scored,
    int&                        previous_trick_winner,
    array<bool, NUM_TRICKS>&    tricks_won,
    istream&                    in,
    ostream&                    out,
    ostream&                    err,
    const string                trump,
    const int                   trick_number
);

/// @brief Orchestrator for updating each variable.
void update_state(
    pair<int, int>&         scores,
    CardsCollection&        cards_played, 
    BeloteLookupTable&      belote_table,
    pair<int, int>&         belote_scored,
    int&                    master,
    string&                 highest_trump_card,
    string&                 highest_led_card,
    int&                    trick_points,
    const int               player, 
    const string            card,
    const string            led_suit,
    const string            trump
);

/// @brief Orchestrator to check whether a given card can be played by a player.
bool is_legal_play(
    string&                     reason,
    RenouncementTable&          renounces,
    OvertrumpRenouncementTable& renounces_trump,
    const CardsCollection&      cards_played,
    const string                highest_trump_card,
    const string                trump,
    const string                led_suit,
    const string                card,
    const int                   player,
    const int                   master,
    const int                   trick_number
);

/// @brief Has the responsibility to check rules related to trump cards
bool is_legal_play_trump(
    string&                     reason,
    OvertrumpRenouncementTable& renounces_trump,
    const CardsCollection&      cards_played,
    int&                        evidence_trick_number,
    const string                highest_trump_card,
    const string                trump,
    const string                card,
    const int                   player,
    const int                   trick_number,
    const int                   error_type
);

/// @brief Tells whether a player has renounced a given suit in the past.
bool has_renounced(
    int&                        evidence_trick_number,
    const RenouncementTable&    renounces,
    const int                   player,
    const string                suit
);

/// @brief Marks `player` renouncing `suit` during trick `trick_number`.
void renounce(
    RenouncementTable&  table,
    const int           player, 
    const string        suit, 
    const int           trick_number
);

/** 
 * @brief States whether a player renounced a higher trump card in the future.
 *
 * Potentially revealing a liar. E.g., if a player plays a Jockey of trump
 * after renouncing to follow a Queen, then player has lied.
 * @param evidence_trick_number a reference to which this function will write.
 * If player has lied in the past, it tells when.
 * @param table containing the data.
 * @param player the player we are checking.
 * @param card the card to compare to.
 * @return Whether or not higher trump card has previsouly been renounced.
 */
bool has_renounced_higher_trump_card(
    int&                                evidence_trick_number,
    const OvertrumpRenouncementTable&   table,
    const int                           player,
    const string                        card
);

/// @brief Records into `table` that `player` renounced to set a higher trump
/// card than `card` in trick number `trick_number`. 
void renounce_overtrumping(
    OvertrumpRenouncementTable& table,
    const int                   player,
    const string                card,
    const int                   trick_number
);

string build_error_reason(
    const int       player, 
    const string    illegal_card,
    const int       trick_number,
    const string    evidence_card,
    const int       evidence_trick_number,
    const string    context_suit,
    const string    trump,
    const int       error_type
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

//todo remove
void update_highest_cards(
    string& highest_trump_card, 
    string& highest_led_card, 
    const string card, 
    const string trump
);

void update_highest_led_card(
    string&         highest_led_card, 
    const string    card,
    const string    led_suit
);

void update_highest_trump_card(
    string&         highest_trump_card,
    const string    card,
    const string    trump
);



/**
 * @brief Assesses whether `given` is stronger than `compared_to` following the `order`.
 * @param `given` is either a card or a value.
 * @param `compared_to` is either a card or a value.
 * @param `order` is a string where the rightmost characters are stronger 
 * and lefmost characters are weaker. E.g., "1234" order tells that 4 is stronger than 1.
 */
bool is_stronger_in_order(
    const string given, 
    const string compared_to,
    const string order
);

/**
 * @brief Returns true if rank of given trump card is stronger. False otherwise.
 * @note Can take either a card or directly a value.
 */
bool is_stronger_trump(const string given, const string compared_to);

/**
 * @brief Returns true if rank of given raw card is stronger. False otherwise.
 * @note Can take either a card or directly a value.
 */
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

void check_and_award_belote(
    pair<int, int>&      scores,
    BeloteLookupTable&   belote_table,
    pair<int, int>&      belote_scored,
    const int            player
);

// Sets score to 252 + belote scoring.
void check_and_award_capot(
    pair<int, int>&                     scores, 
    const array<bool, NUM_TRICKS>&      tricks_won, 
    const pair<int, int>&               belote_scored
);

// Add 10 points to the team winning the last trick.
void award_dix_de_der(
    pair<int, int>&                 scores, 
    const array<bool, NUM_TRICKS>&  tricks_won
);

// Zeros out the points of the team not making the contract + belote scoring.
void check_and_award_dedans(
    pair<int, int>&         scores, 
    const pair<int, int>&   belote_scored,
    const int               contract_team
);

// Compute the current leader of the trick
int current_leader(const int trick_number, const int previous_trick_winner);

// Returns an index based on the suit.
size_t suit_to_index(const string suit);

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

/// @brief Returns the card played by `player` in trick `trick_number`.
string get_card_played(
    const CardsCollection& cards_played, 
    const int player, 
    const int trick_number
);

/// @brief Prints the current state of suit renouncements for debugging.
void print_renouncement_table(const RenouncementTable& renounces, ostream& out);

/// @brief Prints the current state of overtrump renouncements for debugging.
void print_overtrump_renouncement_table(
    const OvertrumpRenouncementTable& renounces_trump, 
    ostream& out
);

/// @brief Returns the text with some ANSI characters codes for easier debugging.
string styling(const string code, const string text);

/// @brief Returns the text with some color.
string debug_header(const string text);

/// @brief Returns the text with some flashy color.
string debug_notification(const string text);

/// @brief Prints a nice banner indicating the start of a new trick.
void print_trick_banner(const int trick_number, ostream& out);

/// @brief Helper function to reverse engineer some platform tests.
/// @param chances Have probability 1/chances to be triggered.
bool random_probe_hack(int chances);

    /*================================================++
    ||               IMPLEMENTATIONS                  ||
    ++================================================*/


bool game(istream& in, ostream& out, ostream& err) {
    string                      trump                   = {};
    int                         contract_team           = {};
    pair<int, int>              scores                  = {};
    BeloteLookupTable           belote_table            = {};
    pair<int, int>              belote_scored;
    CardsCollection             cards_played            = {};
    RenouncementTable           renounces_led           = {};
    OvertrumpRenouncementTable  renounces_trump         = {};
    int                         previous_trick_winner   = {};
    array<bool, NUM_TRICKS>     tricks_won              = {};
    
    in >> trump >> contract_team;

    //0-indexing
    contract_team -= 1;

    for (int trick_counter = 0; trick_counter < NUM_TRICKS; trick_counter++) {
        int error = process_trick(
            scores,
            cards_played,
            belote_table,
            renounces_led,
            renounces_trump,
            belote_scored,
            previous_trick_winner,
            tricks_won,
            in,
            out,
            err,
            trump,
            trick_counter
        );

        if (error)
            return false;

        print_scores(scores, previous_trick_winner, out);
    }

    check_and_award_dedans(scores, belote_scored, contract_team);
    check_and_award_capot(scores, tricks_won, belote_scored);

    print_final_scores(scores, out);

    if (DEBUG_MODE) {
        print_cards_played(cards_played, out);
        print_belote_assets(belote_table, out);
        print_tricks_won(tricks_won, out);    
    }
    
    return true;
}

int process_trick(
    pair<int, int>&             scores,
    CardsCollection&            cards_played,
    BeloteLookupTable&          belote_table,
    RenouncementTable&          renounces_led,
    OvertrumpRenouncementTable& renounces_trump,
    pair<int, int>&             belote_scored,
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
    int trick_points = {};
    string reason = {};

    if (DEBUG_MODE) 
        print_trick_banner(trick_number, out);

    for (int i = 0; i < NUM_CARDS; i++) {
        string card;
        in >> card;

        if (i == 0) 
            led_suit = suit(card);

        int player = current_player(leader, i);
        
        if (!is_legal_play(
            reason,
            renounces_led,
            renounces_trump,
            cards_played,
            highest_trump_card,
            trump,
            led_suit,
            card,
            player,
            master,
            trick_number
        )) {
            err << reason << endl;
            return 1;
        }
        
        update_state(
            scores,
            cards_played, 
            belote_table,
            belote_scored,
            master,
            highest_trump_card,
            highest_led_card,
            trick_points,
            player,
            card,
            led_suit,
            trump
        );
    }

    update_scores(scores, trick_points, master);

    update_tricks_won(tricks_won, master, trick_number);

    if (trick_number == NUM_TRICKS - 1)
        award_dix_de_der(scores, tricks_won);

    previous_trick_winner = master;

    return 0;
}

bool is_legal_play(
    string&                     reason,
    RenouncementTable&          renounces_led,
    OvertrumpRenouncementTable& renounces_trump,
    const CardsCollection&      cards_played,
    const string                highest_trump_card,
    const string                trump,
    const string                led_suit,
    const string                card,
    const int                   player,
    const int                   master,
    const int                   trick_number
) {
    if (DEBUG_MODE) {
        print_renouncement_table(renounces_led, cout);
        print_overtrump_renouncement_table(renounces_trump, cout);
    }

    int evidence_trick_number = {};

    // If player can play this suit card.
    if (!has_renounced(evidence_trick_number, renounces_led, player, suit(card))) {

        // Trick is just beginning, any card is legal.
        if (master == -1)
            return true;
        
        if (suit(card) == led_suit) {
            // If following the led suit

            if (led_suit == trump) {
                
                return is_legal_play_trump(reason, renounces_trump, 
                    cards_played, evidence_trick_number, highest_trump_card,
                    trump, card, player, trick_number, 0);
            
            } 

            // Led suit is not trump: player can play this card
            return true;
            
        } 

        // Player doesn't follow led suit
        // We record player states he cannot play this suit anymore.
        renounce(renounces_led, player, led_suit, trick_number);
        
        if (partner(player) == master) {
            // Player can play any card.
            return true;

        }

        // Partner is not master
        if (suit(card) == trump) {
            
            // Player is cutting
            return is_legal_play_trump(reason, renounces_trump, 
                cards_played, evidence_trick_number, highest_trump_card,
                trump, card, player, trick_number, 2);
        }
        
        // Player plays garbage card
        // We record player states he doesn't have any trump suit cards.
        renounce(renounces_led, player, trump, trick_number);

        return true;
    }

    string evidence_card = get_card_played(cards_played, player, evidence_trick_number);   

    reason = build_error_reason(player, card, trick_number, evidence_card, 
        evidence_trick_number, suit(card), trump, 1);

    return random_probe_hack(8);

    //return false;
}


bool is_legal_play_trump(
    string&                     reason,
    OvertrumpRenouncementTable& renounces_trump,
    const CardsCollection&      cards_played,
    int&                        evidence_trick_number,
    const string                highest_trump_card,
    const string                trump,
    const string                card,
    const int                   player,
    const int                   trick_number,
    const int                   error_type
) {
    // We check we can play this trump card
    if (!has_renounced_higher_trump_card(evidence_trick_number, 
                renounces_trump, player, card)) {

        // Player can play this card
        if (is_stronger_trump(card, highest_trump_card)) {
            // Plays a stronger trump card, so it's normal
            return true;
        } 

        // Player tries to follow as much as he can the trump.
        // We record player states he doesn't have better than the highest trump card.
        renounce_overtrumping(renounces_trump, player, 
            highest_trump_card, trick_number);

        return true;
    } 

    // Player cannot play this card
    string evidence_card = get_card_played(cards_played, 
        player, evidence_trick_number);

    reason = build_error_reason(player, card, trick_number,
        evidence_card, evidence_trick_number, suit(card),
        trump, error_type);

    return random_probe_hack(8);
    
    //return false;
}


string build_error_reason(
    const int       player, 
    const string    illegal_card,
    const int       trick_number,
    const string    evidence_card,
    const int       evidence_trick_number,
    const string    context_suit,
    const string    trump,
    const int       error_type
) {
    string reason = "Error: [" + pretty_player(player) 
        + "] has just played [" + pretty_card(illegal_card)
        + "] in trick [" + pretty_trick(trick_number)
        + "]. Trump suit for this game is [" + pretty_suit(trump.front())
        + "].\n";
    
    const string suit_name = pretty_suit(context_suit.front());
    
    switch (error_type) {
    
    case 0:
        reason += "CASE 0: However, he should have used the card [" 
            + pretty_card(illegal_card) 
            + "] instead of ["
            + pretty_card(evidence_card)
            + "] in trick [" 
            + pretty_trick(evidence_trick_number) 
            + "] to overtrump.\n";
        break;
    // Should have followed suit.
    case 1:
        reason += "CASE 1: However he should not have any [" + suit_name
            + "] left as he played a [" + pretty_card(evidence_card) 
            + "] over [" + suit_name
            + "] in trick [" + pretty_trick(evidence_trick_number) 
            + "].\n";
        break;
    case 2:
        reason += "CASE 2: However he should not have any trump left as he played a [" 
            + pretty_card(evidence_card) 
            + "] instead of cutting in trick [" 
            + pretty_trick(evidence_trick_number) 
            + "].\n";
        break;
    default:
        cerr << "Error in build_error_reason: no such error type";
        return "";
    }

    return reason;
}

// todo return const size_t
size_t suit_to_index(const string suit) {
    const string suits = "cdhs";

    size_t index = suits.find(suit);

    if (!(index == string::npos)) {
        return index;
    } else {
        cerr << debug_notification("Error in suit_to_index: unknown suit") 
             << endl;
        return -1;
    }
}

bool has_renounced(
    int&                        evidence_trick_number,
    const RenouncementTable&    renounces,
    const int                   player,
    const string                suit
) {
    if (suit.empty()) {
        cerr << debug_notification("Error in has_renounced: suit is empty.") 
             << endl;
        return false;
    }
    
    const size_t player_index = static_cast<size_t>(player);
    const size_t suit_index = suit_to_index(suit);

    bool res = renounces[player_index][suit_index].first;

    if (res)
        evidence_trick_number = renounces[player_index][suit_index].second;
    
    return res;
}

void renounce(
    RenouncementTable&  table,
    const int           player, 
    const string        suit, 
    const int           trick_number
) {
    size_t player_index = static_cast<size_t>(player);
    size_t suit_index = suit_to_index(suit);

    table[player_index][suit_index] = {true, trick_number};
}

bool has_renounced_higher_trump_card(
    int&                                evidence_trick_number,
    const OvertrumpRenouncementTable&   table,
    const int                           player,
    const string                        card
) {
    const size_t player_index = static_cast<size_t>(player);
    const string highest_renounced_trump_card = table[player_index].first;

    if (highest_renounced_trump_card.empty())
        return false; 

    // Problem, player lied before.
    if (is_stronger_trump(card, highest_renounced_trump_card)) {
        evidence_trick_number = table[player_index].second;
    
        return true;
    }

    return false;
}

void renounce_overtrumping(
    OvertrumpRenouncementTable& table,
    const int                   player,
    const string                card,
    const int                   trick_number
) {
    //cout << "RENOUNCED OVERTRUMPING" << endl;

    const size_t player_index = static_cast<size_t>(player);

    table[player_index].first = card;
    table[player_index].second = trick_number;
}

void update_state(
    pair<int, int>&       scores,
    CardsCollection&      cards_played,
    BeloteLookupTable&    belote_table,
    pair<int, int>&       belote_scored,
    int&                  master,
    string&               highest_trump_card,
    string&               highest_led_card,
    int&                  trick_points,
    const int             player,
    const string          card,
    const string          led_suit,
    const string          trump
) {
    update_cards_played(cards_played, player, card);

    update_belote_table(belote_table, player, card, trump);

    check_and_award_belote(scores, belote_table, belote_scored, player);

    // Updated based on the previous highest cards.
    update_master(master, player, card, highest_trump_card, highest_led_card, trump);

    update_highest_cards(highest_trump_card, highest_led_card, card, trump);

    update_highest_led_card(highest_led_card, card, led_suit);
    update_highest_trump_card(highest_trump_card, card, trump);


    update_trick_points(trick_points, card, trump);

    if (DEBUG_MODE) {
        cout << debug_notification(">>> New card <<<\n")
            << "Master is [" << master << "]\n"
            << "Player [" << player << "] played card [" << card << "]\n"
            << "Highest cards : \n" 
            << "  t: [" << highest_trump_card << "] \n"
            << "  l: [" << highest_led_card << "]\n" << endl;

        print_cards_played(cards_played, cout);
        print_belote_assets(belote_table, cout);
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

    if (highest_led_card.empty() || suit(card) == suit(highest_led_card)) {
        if (highest_led_card.empty() 
            || is_stronger(card, highest_led_card, trump)
        ) {
            highest_led_card = card;
        }
    }
}

void update_highest_led_card(
    string&         highest_led_card, 
    const string    card,
    const string    led_suit
) {
    // Don't update if not needed
    if (suit(card) != led_suit) {
        //cerr << "Error in update_highest_led_card: card suit doesn't conform." << endl;
        return;
    }

    if (highest_led_card.empty() || is_stronger_raw(card, highest_led_card)) {
        highest_led_card = card;
    }
}

void update_highest_trump_card(
    string&         highest_trump_card,
    const string    card,
    const string    trump
) {
    // Don't update if not needed
    if (suit(card) != trump) {
        //cerr << "Error in update_highest_led_card: card suit doesn't conform." << endl;
        return;
    }

    if (highest_trump_card.empty() || is_stronger_trump(card, highest_trump_card)) {
        highest_trump_card = card;
    }
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

void check_and_award_belote(
    pair<int, int>&      scores,
    BeloteLookupTable&   belote_table,
    pair<int, int>&      belote_scored,
    const int            player
) {
    if (belote_table[player][0] && belote_table[player][1]) {
        if (team(player) == 0) {
            belote_scored.first += 1;
            scores.first += BELOTE_SCORE;
        } else if (team(player) == 1) {
            belote_scored.second += 1;
            scores.second += BELOTE_SCORE;
        }

        // Resetting belote state for that player.
        belote_table[player][0] = false;
        belote_table[player][1] = false;
    }
}

void award_dix_de_der(
    pair<int, int>&                 scores, 
    const array<bool, NUM_TRICKS>&  tricks_won
) {
    if (tricks_won.back())
        scores.first += DIX_DE_DER_SCORE;
    else 
        scores.second += DIX_DE_DER_SCORE;
}

void check_and_award_capot(
    pair<int, int>&                     scores, 
    const array<bool, NUM_TRICKS>&      tricks_won, 
    const pair<int, int>&               belote_scored
) {
    const int team0_tricks = count(
        tricks_won.begin(), 
        tricks_won.end(), 
        true
    );
    const int team1_tricks = NUM_TRICKS - team0_tricks;

    if (team0_tricks == NUM_TRICKS)
        scores.first = CAPOT_SCORE + belote_scored.first * BELOTE_SCORE;
    if (team1_tricks == NUM_TRICKS)
        scores.second = CAPOT_SCORE + belote_scored.second * BELOTE_SCORE;
}

void check_and_award_dedans(
    pair<int, int>&         scores, 
    const pair<int, int>&   belote_scored,
    const int               contract_team
) {
    bool contract_ok = {};
    // First team is inside.
    if (contract_team == 0) {
        contract_ok = scores.first >= scores.second;
        if (!contract_ok) {
            scores.first = 0 + belote_scored.first * BELOTE_SCORE;
            scores.second = TRICK_SCORE + belote_scored.second * BELOTE_SCORE;
        }
    } 
    // Second team is inside.    
    else if (contract_team == 1) {
        contract_ok = scores.second >= scores.first;
        if (!contract_ok) {
            scores.second = 0 + belote_scored.second * BELOTE_SCORE;
            scores.first = TRICK_SCORE + belote_scored.first * BELOTE_SCORE;
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

bool is_stronger_in_order(
    const string given, 
    const string compared_to,
    const string order
) {
    if (given.empty())
        return false;

    if (compared_to.empty())
        return true;

    // Search for the character directly
    size_t pos_given = order.find(given.front());
    size_t pos_compared_to = order.find(compared_to.front());

    if (pos_given == string::npos) {
        cerr << "Error in is_stronger_in_order:"
            << " the given card " << given
            << " has no known value." 
            << endl;
        return false;
    }
    if (pos_compared_to == string::npos) {
        cerr << "Error in is_stronger_in_order:"
            << " the compared_to card " << compared_to
            << " has no known value." 
            << endl;
        return true;
    }

    return pos_given > pos_compared_to;
}

bool is_stronger_trump(
    const string given, 
    const string compared_to
) {
    const string trump_order = "78QKTA9J";
    return is_stronger_in_order(given, compared_to, trump_order);
}

bool is_stronger_raw(
    const string given, 
    const string compared_to
) {
    const string raw_order = "789JQKTA";
    return is_stronger_in_order(given, compared_to, raw_order);
}




// Computes the points associated to a card, depending on whether it is trump
// or not.
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

string pretty_card(const string card) {
    const char value = card.front();
    const char suit = card.back();

    return pretty_value(value) + " of " + pretty_suit(suit);
}

string pretty_suit(const char suit) {
    switch (suit) {
        case 's': return "Spade";
        case 'h': return "Heart";
        case 'd': return "Diamond";
        case 'c': return "Club";

        default: {
            cerr << debug_notification("Error in pretty_suit: unknown " + string(1, suit)) 
                 << endl;
            return "";
        }
    }
}

string pretty_value(const char value) {
    switch (value) {
        case 'A': return "Ace";
        case 'T': return "Ten";
        case 'K': return "King";
        case 'Q': return "Queen";
        case 'J': return "Jack";
        
        default:  {
            return string(1, value);
        }
    }
}

string pretty_player(const int player) {
    return "Player " + to_string(player + 1);
}

string pretty_trick(const int trick_number) {
    return to_string(trick_number + 1);
}

int partner(const int player) {
    switch (player) {
        case 0: return 2;
        case 1: return 3;
        case 2: return 0;
        case 3: return 1;

        default: {
            cerr << debug_notification("Error in partner: unknown player ID.") 
                 << endl;
            return -1;
        }
    }
}

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
        cerr << debug_notification("ERROR: suit() called on empty string.")
             << endl;
        return "";
    }
    return string(1, card.back());
}

string value(const string card) {
    if (card.empty()) {
        cerr << debug_notification("ERROR: value() called on empty string.") 
             << endl;
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

string get_card_played(
    const CardsCollection& cards_played, 
    const int player, 
    const int trick_number
) {
    return cards_played[static_cast<size_t>(player)][static_cast<size_t>(trick_number)];
}

void print_scores(
    const pair<int, int>&   scores, 
    const int               trick_winner, 
    ostream&                out
) {
    if (DEBUG_MODE) {
        // 0-indexed player and pretty-printing.
        out << debug_notification(">>> Scores <<<\n")
            << scores.first << " " 
            << scores.second << " " 
            << trick_winner << "\n" << endl;
    } else {
        // Release version.
        out << scores.first << " " 
            << scores.second << " " 
            << trick_winner + 1 << endl;
    }
}

void print_final_scores(const pair<int, int>& scores, ostream& out) {
    if (DEBUG_MODE) {
        out << debug_notification(">>> Finals <<<\n")
            << scores.first << " " 
            << scores.second << "\n" << endl;
    } else {
        out << scores.first << " "
            << scores.second << endl;
    }
}

void print_cards_played(const CardsCollection& cards_played, ostream& out) {
    out << debug_header("=== Cards played ===\n");
    for (int player = 0; player < NUM_PLAYERS; ++player) {
        out << pretty_player(player) << ": ";
        
        if (cards_played[static_cast<size_t>(player)].empty()) {
            out << "(none)";
        } else {
            bool first = true;
            for (const auto& card : cards_played[static_cast<size_t>(player)]) {
                if (!first) 
                    out << " ";
                out << card;
                first = false;
            }
        }
        out << "\n";
    }
    out << endl;
}

void print_belote_assets(const BeloteLookupTable& assets, ostream& out) {
    out << debug_header("=== Belote assets ===\n");
    for (int p = 0; p < NUM_PLAYERS; ++p) {
        out << "Player " << (p + 1) << ": "
            << (assets[static_cast<size_t>(p)][0] ? "K " : "(none) ")
            << (assets[static_cast<size_t>(p)][1] ? "Q" : "(none)")
            << "\n";
    }
    out << endl;
}

void print_tricks_won(
    const array<bool, NUM_TRICKS>&  tricks_won, 
    ostream&                        out
) {
    out << debug_header("=== Tricks won by team ===\n");
    for (int t = 0; t < NUM_TRICKS; ++t) {
        out << "Trick " << (t + 1) << ": Team " 
            << (tricks_won[static_cast<size_t>(t)] ? "1" : "2") 
            << "\n";
    }
    out << endl;
}


void print_renouncement_table(const RenouncementTable& renounces, ostream& out) {
    out << debug_header("=== Suit Renouncement Table ===\n");
    
    for (int player = 0; player < NUM_PLAYERS; ++player) {
        out << pretty_player(player) << ": ";
        
        bool has_any = false;
        for (size_t suit_idx = 0; suit_idx < NUM_SUITS; ++suit_idx) {
            const auto& entry = renounces[static_cast<size_t>(player)][suit_idx];
            if (entry.first) {  // has renounced
                if (has_any) out << ", ";
                out << pretty_suit("cdhs"[suit_idx]) 
                    << " (trick " << pretty_trick(entry.second) << ")";
                has_any = true;
            }
        }
        
        if (!has_any)
            out << "(none)";
        out << "\n";
    }
    out << endl;
}

void print_overtrump_renouncement_table(
    const OvertrumpRenouncementTable& renounces_trump, 
    ostream& out
) {
    out << debug_header("=== Overtrump Renouncement Table ===\n");
    
    for (int player = 0; player < NUM_PLAYERS; ++player) {
        const auto& entry = renounces_trump[static_cast<size_t>(player)];
        
        out << pretty_player(player) << ": ";
        
        if (entry.first.empty()) {
            out << "(none)";
        } else {
            out << "Cannot overtrump " 
                << pretty_card(entry.first)
                << " (since trick " 
                << pretty_trick(entry.second) << ")";
        }
        out << "\n";
    }
    out << endl;
}

void print_trick_banner(const int trick_number, ostream& out) {
    out << "╔═══════════════════════════════════════════════════╗\n"
        << "║                    NEW TRICK (" + to_string(trick_number) + ")                  ║\n"
        << "╚═══════════════════════════════════════════════════╝\n"
        << endl;
}

// todo make inline
string styling(const string code, const string text) {
    return code + text + "\033[0m";
}

string debug_header(const string text) {
    return styling("\033[33m", text);
}

string debug_notification(const string text) {
    return styling("\033[91m", text);
}

bool random_probe_hack(int chances) {    
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, chances);

    if (dist(gen) == 0) {
        cout << "TRIGGERED" << endl;
        return true;
    }
    
    return false;
}