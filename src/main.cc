#include <iostream>
#include <sstream>

#include "belote.hh"



using namespace std;

int main() {
    std::string input_data = R"(
    s 2
    Qh 8h Ah 9h
    Kh Th 7h Qs
    Jh 9s 8s Js
    )";

    std::istringstream in(input_data);

    return game(in, std::cout, std::cerr);
} 
