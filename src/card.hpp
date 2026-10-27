#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

// Une carte = entier de 0 à 51 : rang = c / 4 (0 = deux ... 12 = As), couleur = c % 4
using Card = std::uint8_t;

constexpr int kNumCards = 52;

constexpr int rank_of(Card c) { return c / 4; }
constexpr int suit_of(Card c) { return c % 4; }
constexpr Card make_card(int rank, int suit) { return static_cast<Card>(rank * 4 + suit); }

std::optional<Card> parse_card(std::string_view s);  // "As", "2c", "Td"...
std::string to_string(Card c);