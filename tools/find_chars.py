import json
import os
from collections import defaultdict

BASE = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(BASE, "dist", "JapaneseMod")
REP = os.path.join(BASE, "reports")
TARGETS = "译雾탈ㅅدم\u200b｡崗墻"

with open(os.path.join(REP, "char_issues.txt"), "w", encoding="utf-8") as fh:
    for name in ["Language.json", "LanguageTalk.json"]:
        data = json.load(open(os.path.join(DIST, name), encoding="utf-8"))
        for e in data:
            jp = e.get("jp") or ""
            found = [c for c in TARGETS if c in jp]
            if found:
                fh.write(f"{name} id={e['id']} f={e.get('flag','')} found={[hex(ord(c)) for c in found]}\n")
                fh.write(f"  CN: {e.get('cn','')}\n  JP: {jp}\n")
print("done")
