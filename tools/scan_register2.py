import json
import os
import re

BASE = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
REP = os.path.join(BASE, "reports")
TALK = os.path.join(BASE, "dist", "JapaneseMod", "LanguageTalk.json")

# canon first person per flag (from TRANSLATION_BIBLE)
CANON = {
    2001: ["俺", "私"], 2009: ["私"], 2005: ["私", "あたし"], 2010: ["私"],
    2003: ["私"], 2004: ["私"], 2012: ["私"], 2135: ["私"], 2140: ["ぼく", "私"],
    2146: ["私"], 2014: ["私", "我"], 2108: ["私"], 2103: ["俺"], 2119: ["俺"],
    2117: ["私"], 2120: ["私"], 2123: ["私"], 2131: ["私"], 2127: ["ウサ", "あたし"],
    2128: ["ウサ", "私"], 2110: ["私", "余"], 2114: ["私", "手前"], 2104: ["私"],
    2132: ["私", "僕"], 2133: ["私"], 2142: ["私"], 2145: ["私"], 2112: ["僕", "私"],
    2118: ["俺"], 2152: ["俺", "おじさん"], 2106: ["オラ", "タヌ"], 2109: ["私", "我"],
    2139: ["僕", "私"], 2143: ["ぼく", "私"],
}
ALLPRON = ["俺", "僕", "私", "わたし", "わたくし", "あたし", "オラ", "我", "余", "手前"]

data = json.load(open(TALK, encoding="utf-8"))
with open(os.path.join(REP, "register2.txt"), "w", encoding="utf-8") as fh:
    for flag, canon in CANON.items():
        entries = [e for e in data if e.get("flag") == flag]
        offenders = []
        for e in entries:
            jp = e.get("jp") or ""
            used = [p for p in ALLPRON if p in jp]
            bad = [p for p in used if p not in canon]
            if bad:
                offenders.append((e, bad))
        fh.write(f"flag {flag} canon={canon} lines={len(entries)} offenders={len(offenders)}\n")
        for e, bad in offenders[:12]:
            fh.write(f"   id={e['id']} bad={bad} | CN={e.get('cn','')[:40]}\n     JP={e.get('jp','')[:120]}\n")
print("done")
