import json
import os
import random

BASE_DIR = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
TALKP = os.path.join(BASE_DIR, "dist", "JapaneseMod", "LanguageTalk.json")
OUT = os.path.join(BASE_DIR, "reports", "talk_sample.txt")

with open(TALKP, "r", encoding="utf-8") as f:
    data = json.load(f)

random.seed(7)
pick = random.sample(data, 260)
with open(OUT, "w", encoding="utf-8") as fh:
    for e in pick:
        fh.write(f"{e['id']} flag={e.get('flag')}\n")
        fh.write(f"  CN: {e.get('cn','')}\n")
        fh.write(f"  JP: {e.get('jp','')}\n")
print("done")
