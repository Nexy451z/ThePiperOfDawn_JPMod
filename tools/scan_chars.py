import json
import os
from collections import Counter

BASE = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(BASE, "dist", "JapaneseMod")
REP = os.path.join(BASE, "reports")


def load(p):
    with open(p, "r", encoding="utf-8") as f:
        return json.load(f)


c = Counter()
for name in ["Language.json", "LanguageTalk.json"]:
    for e in load(os.path.join(DIST, name)):
        for ch in e.get("jp") or "":
            c[ch] += 1

with open(os.path.join(REP, "char_freq.txt"), "w", encoding="utf-8") as fh:
    rare = [(ch, n) for ch, n in c.items() if n <= 2]
    fh.write("rare chars (<=2 occurrences):\n")
    for ch, n in sorted(rare, key=lambda x: ord(x[0])):
        fh.write(f"  U+{ord(ch):04X} {ch!r} x{n}  {__import__('unicodedata').name(ch, '?')}\n")
    fh.write(f"\ntotal distinct chars: {len(c)}\n")
print("done")
