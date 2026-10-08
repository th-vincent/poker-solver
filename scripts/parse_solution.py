import argparse
import json
from collections import defaultdict
from pathlib import Path

RANKS = "23456789TJQKA"


def load_solution(path):
    with Path(path).open() as f:
        return json.load(f)


def get_node(root, action_path):
    """Descend dans l'arbre en suivant une liste d'actions."""
    node = root
    for action in action_path:
        children = node.get("childrens", {})
        if action not in children:
            raise KeyError(f"Action '{action}' introuvable. Disponibles : {list(children)}")
        node = children[action]
    return node


def hand_class(hand):
    """'AhKh' -> 'AKs', 'AhKs' -> 'AKo', 'QdQc' -> 'QQ'."""
    r1, s1, r2, s2 = hand[0], hand[1], hand[2], hand[3]
    if RANKS.index(r1) < RANKS.index(r2):
        r1, s1, r2, s2 = r2, s2, r1, s1
    if r1 == r2:
        return r1 + r2
    return r1 + r2 + ("s" if s1 == s2 else "o")


def hand_strategy(node, hand):
    """Retourne {action: probabilité} pour un combo précis."""
    hands = node["strategy"]["strategy"]
    if hand not in hands:
        swapped = hand[2:4] + hand[0:2]  # le solveur peut inverser l'ordre des cartes
        if swapped not in hands:
            raise KeyError(f"Main '{hand}' absente de ce nœud")
        hand = swapped
    return dict(zip(node["strategy"]["actions"], hands[hand]))


def class_strategy(node):
    """Moyenne des combos par classe de main (22, AKs, AKo...). Moyenne simple, non pondérée."""
    actions = node["strategy"]["actions"]
    sums = defaultdict(lambda: [0.0] * len(actions))
    counts = defaultdict(int)
    for hand, probs in node["strategy"]["strategy"].items():
        c = hand_class(hand)
        for i, p in enumerate(probs):
            sums[c][i] += p
        counts[c] += 1
    return {c: dict(zip(actions, [v / counts[c] for v in vals])) for c, vals in sums.items()}


def fmt(strategy):
    return "  ".join(f"{a}: {p * 100:5.1f}%" for a, p in strategy.items())


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("file")
    parser.add_argument("--path", default="", help="actions séparées par des virgules, ex: CHECK,BET 25.000000")
    parser.add_argument("--hand", help="combo précis, ex: AhKh")
    parser.add_argument("--summary", action="store_true", help="stratégie par classe de main")
    args = parser.parse_args()

    root = load_solution(args.file)
    path = [a for a in args.path.split(",") if a]
    node = get_node(root, path)

    print(f"Nœud : {path or 'racine'} | type={node.get('node_type')} | player={node.get('player')}")
    if node.get("node_type") != "action_node":
        print("Ce nœud n'est pas un nœud d'action (rue suivante non exportée).")
        return
    print(f"Actions : {node['strategy']['actions']}")

    if args.hand:
        print(f"{args.hand} : {fmt(hand_strategy(node, args.hand))}")

    if args.summary:
        order = sorted(class_strategy(node).items(), key=lambda kv: (-RANKS.index(kv[0][0]), -RANKS.index(kv[0][1]), kv[0]))
        for cls, strat in order:
            print(f"{cls:4s} {fmt(strat)}")


if __name__ == "__main__":
    main()