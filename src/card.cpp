#include "card.hpp"

#include <cctype>

namespace {
constexpr std::string_view kRanks = "23456789TJQKA";
constexpr std::string_view kSuits = "cdhs";
}  // namespace

std::optional<Card> parse_card(std::string_view s) {
    if (s.size() != 2) return std::nullopt;
    auto r = kRanks.find(static_cast<char>(std::toupper(static_cast<unsigned char>(s[0]))));
    auto su = kSuits.find(static_cast<char>(std::tolower(static_cast<unsigned char>(s[1]))));
    if (r == std::string_view::npos || su == std::string_view::npos) return std::nullopt;
    return make_card(static_cast<int>(r), static_cast<int>(su));
}

std::string to_string(Card c) {
    return std::string{kRanks[rank_of(c)], kSuits[suit_of(c)]};
}