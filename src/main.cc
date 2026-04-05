#include <iostream>
#include <sstream>

#include "belote.hh"

using namespace std;

int main() {
    string test1 = R"(
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

    string test9 = R"(
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

    string test26 = R"(
    s 2
    7d Qd Ad 8d
    7s Ts 9s Qs
    Ah Qh Kh 8h
    Js Jd As 8s
    7c 8c Ac Qc
    7h 9h Ks Jh
    9d 9c Kd Tc
    Td Th Jc Kc
    )";

    istringstream in(test26);

    return game(in, cout, cerr);
} 
