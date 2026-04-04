#include <iostream>
#include <sstream>

#include "belote.hh"



using namespace std;

int main() {
    std::string test1 = R"(
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

    std::string test8 = R"(
    c 1
    Jd Td 9d Ad
    Ts Qs Js As
    Kh Th Ah 9h
    Kc Jc Tc Ac
    Jh Qh 9c 7h
    8s Qc 7s 9s
    8h 8c 7d Ks
    8d Qd 7c Kd
    )";

    std::istringstream in(test1);

    return game(in, std::cout, std::cerr);
} 
