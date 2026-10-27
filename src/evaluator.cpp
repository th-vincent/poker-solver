#include "evaluator.hpp"


namespace {

// Construit le score : on pousse les rangs par ordre d'importance
class ScoreBuilder {
public:
    void push(int rank) {
        kickers_ = (kickers_ << 4) | static_cast<HandScore>(rank);
        ++count_;
    }
    // Ajoute les k plus hauts rangs présents dans mask
    void push_top(unsigned mask, int k) {
        for (int r = 12; r >= 0 && k > 0; --r) {
            if (mask & (1u << r)) {
                push(r);
                --k;
            }
        }
    }
    HandScore build(HandCategory cat) const {
        return (static_cast<HandScore>(cat) << 20) | (kickers_ << (4 * (5 - count_)));
    }

private:
    HandScore kickers_ = 0;
    int count_ = 0;
};

// Plus haut rang d'un masque non vide
// Plus haut rang d'un masque non vide
int top_rank(unsigned mask) { return 31 - __builtin_clz(mask); }

int popcount(unsigned mask) { return __builtin_popcount(mask); }

// mask : bit r à 1 si le rang r est présent.
// Renvoie le rang de la carte haute de la meilleure quinte, ou -1.
int straight_high(unsigned mask) {
    for (int high = 12; high >= 4; --high) {
        unsigned window = 0b11111u << (high - 4);
        if ((mask & window) == window) return high;
    }
    // La roue A-2-3-4-5 : l'as (bit 12) + les rangs 0 à 3, la carte haute est le 5 (rang 3)
    constexpr unsigned wheel = (1u << 12) | 0b1111u;
    if ((mask & wheel) == wheel) return 3;
    return -1;
}

}  // namespace

HandScore evaluate(const Card* cards, int n) {
    int rank_count[13] = {};
    int suit_count[4] = {};
    unsigned suit_ranks[4] = {};
    unsigned all_ranks = 0;

    for (int i = 0; i < n; ++i) {
        int r = rank_of(cards[i]), s = suit_of(cards[i]);
        ++rank_count[r];
        ++suit_count[s];
        suit_ranks[s] |= 1u << r;
        all_ranks |= 1u << r;
    }

    int flush_suit = -1;
    for (int s = 0; s < 4; ++s) {
        if (suit_count[s] >= 5) flush_suit = s;
    }

    ScoreBuilder b;

    // Quinte flush
    if (flush_suit >= 0) {
        int high = straight_high(suit_ranks[flush_suit]);
        if (high >= 0) {
            b.push(high);
            return b.build(HandCategory::StraightFlush);
        }
    }

    // Masques des rangs présents 4, 3 et 2 fois
    unsigned four = 0, three = 0, two = 0;
    for (int r = 0; r < 13; ++r) {
        switch (rank_count[r]) {
            case 4: four |= 1u << r; break;
            case 3: three |= 1u << r; break;
            case 2: two |= 1u << r; break;
            default: break;
        }
    }

    // Carré + meilleur kicker
    if (four) {
        b.push_top(four, 1);
        b.push_top(all_ranks & ~four, 1);
        return b.build(HandCategory::FourOfAKind);
    }

    // Full : un brelan + (un autre brelan ou une paire)
    if (three && (popcount(three) >= 2 || two)) {
        int t = top_rank(three);
        b.push(t);
        b.push_top((three | two) & ~(1u << t), 1);
        return b.build(HandCategory::FullHouse);
    }

    // Couleur
    if (flush_suit >= 0) {
        b.push_top(suit_ranks[flush_suit], 5);
        return b.build(HandCategory::Flush);
    }

    // Quinte
    if (int high = straight_high(all_ranks); high >= 0) {
        b.push(high);
        return b.build(HandCategory::Straight);
    }

    // Brelan
    if (three) {
        b.push_top(three, 1);
        b.push_top(all_ranks & ~three, 2);
        return b.build(HandCategory::ThreeOfAKind);
    }

    // Double paire (avec 3 paires, on garde les 2 plus hautes ; le kicker peut être la 3e paire)
    if (popcount(two) >= 2) {
        int p1 = top_rank(two);
        int p2 = top_rank(two & ~(1u << p1));
        b.push(p1);
        b.push(p2);
        b.push_top(all_ranks & ~(1u << p1) & ~(1u << p2), 1);
        return b.build(HandCategory::TwoPair);
    }

    // Paire
    if (two) {
        b.push_top(two, 1);
        b.push_top(all_ranks & ~two, 3);
        return b.build(HandCategory::Pair);
    }

    // Hauteur
    b.push_top(all_ranks, 5);
    return b.build(HandCategory::HighCard);
}