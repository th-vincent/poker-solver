import argparse
import json
from pathlib import Path

# Déduit de tes résultats : à la racine, player=1 agit en premier (hors position)
POSITION = {0: "IP", 1: "OOP"}


def flatten(node, path, out):
    if node.get("node_type") != "action_node":
        return
    strat = node["strategy"]
    hands = {
        hand: [round(p, 3) if p >= 0.001 else 0.0 for p in probs]
        for hand, probs in strat["strategy"].items()
    }
    out[path or "root"] = {
        "player": node["player"],
        "position": POSITION.get(node["player"]),
        "actions": strat["actions"],
        "hands": hands,
    }
    for action, child in node.get("childrens", {}).items():
        flatten(child, f"{path}/{action}" if path else action, out)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", help="JSON brut du solveur")
    parser.add_argument("output", help="JSON allégé pour le web")
    parser.add_argument("--board", required=True, help="ex: QsJh2h")
    parser.add_argument("--pot", type=float, required=True)
    parser.add_argument("--stack", type=float, required=True)
    args = parser.parse_args()

    with Path(args.input).open() as f:
        root = json.load(f)

    nodes = {}
    flatten(root, "", nodes)

    data = {"board": args.board, "pot": args.pot, "stack": args.stack, "nodes": nodes}
    with Path(args.output).open("w") as f:
        json.dump(data, f, separators=(",", ":"))

    print(f"{len(nodes)} nœuds exportés vers {args.output}")


if __name__ == "__main__":
    main()