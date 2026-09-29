import json
import os
import re
from collections import Counter

BASE = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(BASE, "dist", "JapaneseMod")
REP = os.path.join(BASE, "reports")

TERMS = {
    "ダイヤモンドの城": 0, "繫花林地": 0, "ヒトシバ": 0, "ゴールド": 0,
    "軍空": 0, "空軍": 0, "レベル": 0, "Lv.": 0, "LV.": 0,
    "ダイヤモンド・シティ": 0, "ダイヤモンドの海": 0, "銀のカギ": 0,
    "母の会": 0, "母親": 0, "人柴": 0, "天の種": 0, "隕星協会": 0,
}

with open(os.path.join(REP, "style.txt"), "w", encoding="utf-8") as fh:
    for name in ["Language.json", "LanguageTalk.json"]:
        data = json.load(open(os.path.join(DIST, name), encoding="utf-8"))
        fh.write("=" * 70 + f"\n{name}\n")
        for t in TERMS:
            n = sum((e.get("jp") or "").count(t) for e in data)
            fh.write(f"  {t}: {n}\n")
        patterns = {
            "ascii ?": r"\?",
            "ascii !": r"!",
            "ascii ...": r"\.\.\.",
            "ascii ( )": r"[()]",
            "ascii ,": r",",
            "todo marker": r"待翻译|待翻訳|TODO|FIXME|XX+",
            "space before JP punct": r"[ 　]+[、。！？「」]",
            "space after open bracket": r"[「（【][ 　]+",
            "double JP punct": r"([。！？])\1",
            "long spaces": r"[ 　]{2,}",
        }
        for label, pat in patterns.items():
            rx = re.compile(pat)
            hits = [e for e in data if rx.search(e.get("jp") or "")]
            fh.write(f"  [{label}]: {len(hits)}\n")
            for e in hits[:8]:
                fh.write(f"      {e['id']} f={e.get('flag','')} | {(e.get('jp') or '')[:80]!r}\n")
print("done")
