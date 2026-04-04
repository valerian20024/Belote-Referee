#include <iostream>
#include <sstream>

#include "belote.hh"



using namespace std;

int main() {
    std::string input_data = R"(
    d 2
    Ac Qc Kc 7c
    8c Jc 9c 7d
    8h Ah Qh 8d
    Ad Td Qd 9d
    Th Kd Kh 7h
    As Ts Qs 8s
    Js 9s Ks 7s
    Jh Tc Jd 9h
    )";

    std::istringstream in(input_data);

    return game(in, std::cout, std::cerr);
} 
