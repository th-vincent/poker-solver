#pragma once
#include <cstdint>

#include "card.hpp"

enum class HandCategory : std::uint8_t {
    HighCard, Pair, TwoPair, ThreeOfAKind, Straight,
    Flush, FullHouse, FourOfAKind, StraightFlush
};

using HandScore = std::uint32_t;

// Évalue entre 5 et 7 cartes. Plus le score est grand, meilleure est la main.
HandScore evaluate(const Card* cards, int n);

constexpr HandCategory category_of(HandScore s) {
    return static_cast<HandCategory>(s >> 20);
}