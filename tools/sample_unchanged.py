import json
import os
import random

BASE = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(BASE, "dist", "JapaneseMod")
REP = os.path.join(BASE, "reports")


def load(p):
    with open(p, "r", encoding="utf-8") as f:
        return json.load(f)


random.seed(3)
for name in ["Language.json", "LanguageTalk.json"]:
    data = load(os.path.join(DIST, name))
    old = load(os.path.join(BASE, "extracted_v2", name))
    omap = {e["id"]: e.get("jp") for e in old}
    unch = [e for e in data if e.get("jp") == omap.get(e["id"])]
    pick = random.sample(unch, min(120, len(unch)))
    with open(os.path.join(REP, f"unchanged_{name}.txt"), "w", encoding="utf-8") as fh:
        for e in pick:
            fh.write(f"{e['id']} f={e.get('flag','')}\n  CN: {e.get('cn','')}\n  JP: {e.get('jp','')}\n")
print("done")
