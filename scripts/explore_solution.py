import json
import sys
from pathlib import Path

path = Path(sys.argv[1])
max_depth = int(sys.argv[2]) if len(sys.argv) > 2 else 2

with path.open() as f:
    data = json.load(f)


def show(node, name="root", depth=0):
    indent = "  " * depth
    print(f"{indent}[{name}] keys={list(node.keys())} type={node.get('node_type')}")

    strat = node.get("strategy")
    if isinstance(strat, dict):
        other = {k: v for k, v in strat.items() if k != "strategy"}
        hands = strat.get("strategy", {})
        print(f"{indent}  strategy: autres clés={other}, {len(hands)} mains")
        if hands:
            h, probs = next(iter(hands.items()))
            print(f"{indent}  exemple: {h} -> {probs}")

    if depth >= max_depth:
        return
    for child_name, child in node.get("childrens", {}).items():
        show(child, child_name, depth + 1)


show(data)