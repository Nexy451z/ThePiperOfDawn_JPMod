import json
import os
import unicodedata
from collections import Counter

BASE = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(BASE, "dist", "JapaneseMod")
REP = os.path.join(BASE, "reports")


def load(p):
    with open(p, "r", encoding="utf-8") as f:
        return json.load(f)


ranges = [
    ("Hangul", 0xAC00, 0xD7A3),
    ("HangulJamo", 0x1100, 0x11FF),
    ("Cyrillic", 0x0400, 0x04FF),
    ("Greek", 0x0370, 0x03FF),
    ("Bopomofo", 0x3100, 0x312F),
    ("Replacement", 0xFFFD, 0xFFFD),
    ("Control", 0x0000, 0x001F),
    ("private", 0xE000, 0xF8FF),
]

with open(os.path.join(REP, "unicode_issues.txt"), "w", encoding="utf-8") as fh:
    for name in ["Language.json", "LanguageTalk.json"]:
        data = load(os.path.join(DIST, name))
        fh.write("=" * 70 + f"\n{name}\n")
        for rname, lo, hi in ranges:
            hits = []
            for e in data:
                jp = e.get("jp") or ""
                bad = [c for c in jp if lo <= ord(c) <= hi]
                if bad:
                    hits.append((e, bad))
            fh.write(f"\n### {rname}: {len(hits)}\n")
            for e, bad in hits[:60]:
                fh.write(f"  id={e['id']} f={e.get('flag','')} chars={[hex(ord(c)) for c in bad]} | {e.get('jp','')[:90]}\n")
print("done")
