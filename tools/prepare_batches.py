import json
import os

BASE_DIR = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
EXT_DIR = os.path.join(BASE_DIR, "extracted")
BATCH_DIR = os.path.join(BASE_DIR, "batches")

os.makedirs(BATCH_DIR, exist_ok=True)

with open(os.path.join(EXT_DIR, "Language.json"), "r", encoding="utf-8") as f:
    lang_data = json.load(f)

# Filter out empty cn
lang_items = []
for e in lang_data:
    cn = (e.get("cn") or "").strip()
    if not cn:
        continue
    lang_items.append({
        "id": e["id"],
        "cn": e["cn"]
    })

BATCH_SIZE = 500
lang_batch_count = 0
for i in range(0, len(lang_items), BATCH_SIZE):
    lang_batch_count += 1
    chunk = lang_items[i:i + BATCH_SIZE]
    out_path = os.path.join(BATCH_DIR, f"in_lang_{lang_batch_count:02d}.json")
    with open(out_path, "w", encoding="utf-8") as f:
        json.dump(chunk, f, ensure_ascii=False, indent=2)

print(f"Created {lang_batch_count} Language input batches ({len(lang_items)} total entries).")

with open(os.path.join(EXT_DIR, "LanguageTalk.json"), "r", encoding="utf-8") as f:
    talk_data = json.load(f)

talk_items = []
for e in talk_data:
    cn = (e.get("cn") or "").strip()
    if not cn or cn.startswith("#") or cn.startswith("＃"):
        continue
    talk_items.append({
        "id": e["id"],
        "speaker": e.get("speaker_cn") or "旁白",
        "cn": e["cn"]
    })

TALK_BATCH_SIZE = 500
talk_batch_count = 0
for i in range(0, len(talk_items), TALK_BATCH_SIZE):
    talk_batch_count += 1
    chunk = talk_items[i:i + TALK_BATCH_SIZE]
    out_path = os.path.join(BATCH_DIR, f"in_talk_{talk_batch_count:03d}.json")
    with open(out_path, "w", encoding="utf-8") as f:
        json.dump(chunk, f, ensure_ascii=False, indent=2)

print(f"Created {talk_batch_count} LanguageTalk input batches ({len(talk_items)} active entries).")
