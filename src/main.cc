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

    string test6 = R"(
    s 1
    9h 7h Kh Th
    Qc 8c 7c Ac
    Ad Td 7d Jd
    8d Kd Ks 9s
    7s As Js Ah
    Kc Tc Qs 8s
    8h Qh 9c Jh
    Qd 9d Jc Ts
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

    string test71 = R"(
    h 2
    8s 7s Qs 9s
    Js Ts Jh As
    9d Ad Td 7d
    Qc Tc 7c Jc
    Ah 9h Kh Qh
    Qd 8d Th 7h
    Kc Ac 8h 9c
    Jd Kd 8c Ks
    )";

    string test135 = R"(
    c 2
    Jh Ah Qh 8h
    Th Qc 7c Kc
    Td Kd Jd Qd
    Ts Jc Js As
    Kh 9c 8c Ks
    7s 9s Qs Tc
    9h Ac Ad 9d
    8s 7d 8d 7h
    )";


    string test251 = R"(
    d 2
    Ac Jc Js Ts
    8c Qc 8d 7d
    Kc 9s Jh Th
    As Td 7h 7s
    Kh Qd Tc 9c
    Qs 8s Jd 7c
    Ad 8h Ah Qh
    Kd 9h Ks 9d
    )";

    string test252 = R"(
    c 1
    Ac 7d Ad 8c
    Qc 7c Tc Js
    8d 9s 9h Ks
    9c 9d Qh 8h
    Ts Jd Kh Kd
    Kc Qs 7h 8s
    Qd As Ah Jc
    Th Jh Td 7s    
    )";

    string test253 = R"(
    h 1
    Ks Js Kc Kd
    Ad 7h Kh 9d
    Qc 9s Jd 8h
    9h Ac Qd Qh
    Tc Qs Ah Jc
    As 9c 8d Ts
    Th Td 7c 7d
    8s 7s Jh 8c
    )";

    string test257 = R"(
    d 1
    9h 9c Td Qs
    8h As 9d Ks
    8d 8c Kd Qh
    Jh 7c Ad 7d
    8s Jc Ac Qc
    Qd Th Jd 9s
    Ts Tc Js Kc
    7s Ah Kh 7h
    )";

    string test334 = R"(
    h 2
    8s 9h Jh 8h
    As Tc Ks Kh
    Qs 8c 7s Qd
    9d Jc 9s Kd
    Ts Qc Kc Js
    Ah Ac Jd 7c
    Qh 7d 7h Ad
    9c Th 8d Td
    )";

    istringstream in(test257);

    return game(in, cout, cerr);
} 
