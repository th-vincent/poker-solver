#include <array>
#include <cassert>
#include <iostream>
#include <string_view>
#include <vector>

#include "card.hpp"
#include "evaluator.hpp"

// "AsKsQsJsTs2c3d" -> score
HandScore eval(std::string_view s) {
    std::vector<Card> cards;
    for (std::size_t i = 0; i + 1 < s.size(); i += 2) {
        auto c = parse_card(s.substr(i, 2));
        assert(c);
        cards.push_back(*c);
    }
    return evaluate(cards.data(), static_cast<int>(cards.size()));
}

HandCategory cat(std::string_view s) { return category_of(eval(s)); }

int main() {
    // Catégories
    assert(cat("AsKsQsJsTs2c3d") == HandCategory::StraightFlush);
    assert(cat("9s9h9d9c2c3d4h") == HandCategory::FourOfAKind);
    assert(cat("KsKhKd2c2d7h9s") == HandCategory::FullHouse);
    assert(cat("As9s7s4s2sKcQd") == HandCategory::Flush);
    assert(cat("5s6h7d8c9sKdKh") == HandCategory::Straight);
    assert(cat("7s7h7d2cKd9h4s") == HandCategory::ThreeOfAKind);
    assert(cat("AsAhKdKc2s3d4h") == HandCategory::TwoPair);
    assert(cat("AsAh2c5d9hJcKs") == HandCategory::Pair);
    assert(cat("AsKd9h7c4s3d2h") == HandCategory::HighCard);

    // La roue est une quinte, mais la plus faible
    assert(cat("As2c3d4h5sKdQh") == HandCategory::Straight);
    assert(eval("As2c3d4h5sKdQh") < eval("2c3d4h5s6hKdQd"));

    // Kickers
    assert(eval("AsKcAd7c5d3h2s") > eval("AhQcAd7c5d3h2s"));

    // Trois paires : le kicker peut être la troisième paire
    assert(eval("AsAhKdKcQsQd2h") > eval("AsAhKdKcJs2d3h"));

    // Full : le brelan prime sur la paire
    assert(eval("KsKhKd2c2d7h9s") > eval("QsQhQdAcAd7h9s"));

    // Égalité : les deux jouent le board
    assert(eval("AsKsQsJsTs2c3d") == eval("AsKsQsJsTs4h5h"));

    // Test exhaustif : toutes les mains de 5 cartes
    std::array<long, 9> counts{};
    for (int a = 0; a < 52; ++a)
        for (int b = a + 1; b < 52; ++b)
            for (int c = b + 1; c < 52; ++c)
                for (int d = c + 1; d < 52; ++d)
                    for (int e = d + 1; e < 52; ++e) {
                        std::array<Card, 5> h = {
                            static_cast<Card>(a), static_cast<Card>(b), static_cast<Card>(c),
                            static_cast<Card>(d), static_cast<Card>(e)};
                        ++counts[static_cast<int>(category_of(evaluate(h.data(), 5)))];
                    }

    const std::array<long, 9> expected = {1302540, 1098240, 123552, 54912, 10200,
                                          5108,    3744,    624,    40};
    for (int i = 0; i < 9; ++i) {
        if (counts[i] != expected[i])
            std::cout << "categorie " << i << " : " << counts[i] << " au lieu de " << expected[i] << "\n";
        assert(counts[i] == expected[i]);
    }

    std::cout << "evaluator tests OK\n";
}