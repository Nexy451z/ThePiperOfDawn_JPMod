import json
import os
import re
from collections import Counter, defaultdict

BASE_DIR = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
TALKP = os.path.join(BASE_DIR, "dist", "JapaneseMod", "LanguageTalk.json")
OUT = os.path.join(BASE_DIR, "reports")
os.makedirs(OUT, exist_ok=True)

PRONOUNS = ["俺", "僕", "私", "わたし", "わたくし", "あたし", "オラ", "おじさん", "自分", "我", "我々"]
SECOND = ["お前", "あなた", "あんた", "君", "キミ", "貴様", "てめえ", "先輩", "お兄ちゃん", "旦那様", "錬金術師様", "錬金術師殿", "マスター", "閣下"]
ENDINGS = ["ですわ", "ますわ", "ですの", "ますの", "かしら", "だぜ", "だぞ", "だな", "だよ", "だわ", "だね", "だポン", "にゃ", "だもん", "ございます", "くださいませ", "だろ", "じゃない", "だぜぇ"]

with open(TALKP, "r", encoding="utf-8") as f:
    data = json.load(f)

by_flag = defaultdict(list)
for e in data:
    by_flag[e.get("flag")].append(e)

# exclude narration flag 0 and non-character labels
lines = []
for flag, entries in by_flag.items():
    pc = Counter()
    sc = Counter()
    ec = Counter()
    for e in entries:
        jp = e.get("jp") or ""
        for w in PRONOUNS:
            pc[w] += jp.count(w)
        for w in SECOND:
            sc[w] += jp.count(w)
        for w in ENDINGS:
            ec[w] += jp.count(w)
    lines.append((flag, len(entries), pc, sc, ec))

lines.sort(key=lambda x: -x[1])
with open(os.path.join(OUT, "register_usage.txt"), "w", encoding="utf-8") as f:
    for flag, n, pc, sc, ec in lines:
        f.write(f"=== flag {flag}  lines={n}\n")
        f.write("  1st: " + ", ".join(f"{k}:{v}" for k, v in pc.most_common(8) if v) + "\n")
        f.write("  2nd: " + ", ".join(f"{k}:{v}" for k, v in sc.most_common(8) if v) + "\n")
        f.write("  end: " + ", ".join(f"{k}:{v}" for k, v in ec.most_common(10) if v) + "\n")
print("done", len(lines))
