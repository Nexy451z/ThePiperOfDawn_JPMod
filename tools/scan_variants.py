import json
import os
from collections import Counter
import zhconv

BASE = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(BASE, "dist", "JapaneseMod")
REP = os.path.join(BASE, "reports")

c = Counter()
examples = {}
for name in ["Language.json", "LanguageTalk.json"]:
    data = json.load(open(os.path.join(DIST, name), encoding="utf-8"))
    for e in data:
        for ch in e.get("jp") or "":
            if "\u4e00" <= ch <= "\u9fff":
                c[ch] += 1
                if ch not in examples:
                    examples[ch] = (name, e["id"], (e.get("jp") or "")[:80])

rows = []
for ch, n in c.items():
    t = zhconv.convert(ch, "zh-hant")
    if t != ch and n <= 40:
        rows.append((n, ch, t))
rows.sort()
with open(os.path.join(REP, "variants.txt"), "w", encoding="utf-8") as fh:
    for n, ch, t in rows:
        nm, eid, ex = examples.get(ch, ("", "", ""))
        fh.write(f"x{n} {ch}->{t}  [{nm} {eid}] {ex}\n")
print("done", len(rows))
