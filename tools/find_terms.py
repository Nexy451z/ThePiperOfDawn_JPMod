import json
import os

BASE = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(BASE, "dist", "JapaneseMod")
REP = os.path.join(BASE, "reports")

TERMS = ["ゴールド", "ヒトシバ", "空軍", "軍空", "ダイヤモンドの城"]
with open(os.path.join(REP, "terms.txt"), "w", encoding="utf-8") as fh:
    for name in ["Language.json", "LanguageTalk.json"]:
        data = json.load(open(os.path.join(DIST, name), encoding="utf-8"))
        for e in data:
            jp = e.get("jp") or ""
            for t in TERMS:
                if t in jp:
                    fh.write(f"{name} id={e['id']} f={e.get('flag','')} [{t}]\n  CN={e.get('cn','')[:60]}\n  JP={jp[:130]}\n")
print("done")
