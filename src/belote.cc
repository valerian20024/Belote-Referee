#include "belote.hh"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <set>
#include <utility>   // std::pair

typedef std::pair<char, char> Card;

/*
namespace {  // anonymous namespace → completely hidden from outside

    // Dummy / placeholder helpers – replace with real logic
    bool parse_trump_and_team(std::istream& in, char& trump, int& contract_team);
    bool read_trick(std::istream& in, std::vector<Card>& trick_cards);
    int  determine_leader(int previous_winner);
    bool is_legal_play(const Card& played, const std::vector<Card>& trick_so_far,
                       int player, const std::array<std::set<char>,5>& renounced,
                       char trump, int contract_team);
    int  compute_trick_winner(const std::vector<Card>& trick, char trump);
    int  trick_points(const std::vector<Card>& trick, char trump);
    void update_renouncements(int player, const Card& played,
                              const std::vector<Card>& trick, char trump,
                              std::array<std::set<char>,5>& renounced);
    bool check_and_add_belote(int player, const Card& played, char trump,
                              std::array<bool,5>& has_k, std::array<bool,5>& has_q,
                              int& scores);
    void print_scores(std::ostream& out, const std::array<int,2>& scores, int winner);

}
*/



bool game(std::istream& in, std::ostream& out, std::ostream& err)
{
    (void) in;
    (void) out;
    (void) err;
    return true;
}


