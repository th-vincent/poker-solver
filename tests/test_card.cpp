#include <cassert>
#include <iostream>

#include "card.hpp"

int main() {
    assert(parse_card("2c") == 0);
    assert(parse_card("As") == 51);
    assert(parse_card("ah") == make_card(12, 2));  // insensible à la casse
    assert(!parse_card("1x"));
    assert(!parse_card("A"));
    assert(!parse_card("Asx"));

    // aller-retour sur les 52 cartes
    for (int c = 0; c < kNumCards; ++c) {
        auto parsed = parse_card(to_string(static_cast<Card>(c)));
        assert(parsed && *parsed == c);
    }

    std::cout << "card tests OK\n";
}