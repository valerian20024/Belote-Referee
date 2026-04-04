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

    std::string test9 = R"(
    s 1
    Ah Qh Jh 7h
    Qd Jd 9d Td
    Kc 8s Tc Ac
    9h Ts 7s 8h
    Qc Jc 7c As
    Th Qs 8c Kd
    Ks 7d Ad Js
    Kh 9s 9c 8d
    )";

    std::istringstream in(test9);

    return game(in, std::cout, std::cerr);
} 
