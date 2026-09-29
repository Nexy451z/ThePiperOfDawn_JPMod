import glob
import json
import os
import shutil

BASE_DIR = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
EXT_DIR = os.path.join(BASE_DIR, "extracted")
BATCH_DIR = os.path.join(BASE_DIR, "batches")
DIST_MOD_DIR = os.path.join(BASE_DIR, "dist", "JapaneseMod")
GAME_MOD_DIR = r"C:\Program Files (x86)\Steam\steamapps\common\The Piper Of Dawn\JapaneseMod"

os.makedirs(DIST_MOD_DIR, exist_ok=True)
os.makedirs(GAME_MOD_DIR, exist_ok=True)

with open(os.path.join(BASE_DIR, "glossary.json"), "r", encoding="utf-8") as f:
    glossary = json.load(f)

char_names = glossary.get("character_names", {})

def load_out_files(pattern):
    merged = {}
    files = sorted(glob.glob(os.path.join(BATCH_DIR, pattern)))
    for path in files:
        try:
            with open(path, "r", encoding="utf-8") as f:
                data = json.load(f)
            if isinstance(data, dict):
                for k, v in data.items():
                    if isinstance(v, str) and v.strip():
                        merged[int(k)] = v
            elif isinstance(data, list):
                for item in data:
                    if isinstance(item, dict) and "id" in item and "jp" in item:
                        if isinstance(item["jp"], str) and item["jp"].strip():
                            merged[int(item["id"])] = item["jp"]
        except Exception as e:
            print(f"[WARN] Failed to load {path}: {e}")
    return merged, len(files)

lang_trans, lang_files = load_out_files("out_lang_*.json")
talk_trans, talk_files = load_out_files("out_talk_*.json")

with open(os.path.join(EXT_DIR, "Language.json"), "r", encoding="utf-8") as f:
    lang_data = json.load(f)

lang_updated = 0
for e in lang_data:
    eid = e["id"]
    cn = (e.get("cn") or "").strip()
    if eid in lang_trans:
        e["jp"] = lang_trans[eid]
        lang_updated += 1
    elif cn in char_names:
        e["jp"] = char_names[cn]

with open(os.path.join(DIST_MOD_DIR, "Language.json"), "w", encoding="utf-8") as f:
    json.dump(lang_data, f, ensure_ascii=False, indent=2)
shutil.copy2(os.path.join(DIST_MOD_DIR, "Language.json"), os.path.join(GAME_MOD_DIR, "Language.json"))

with open(os.path.join(EXT_DIR, "LanguageTalk.json"), "r", encoding="utf-8") as f:
    talk_data = json.load(f)

talk_updated = 0
for e in talk_data:
    eid = e["id"]
    if eid in talk_trans:
        e["jp"] = talk_trans[eid]
        talk_updated += 1

with open(os.path.join(DIST_MOD_DIR, "LanguageTalk.json"), "w", encoding="utf-8") as f:
    json.dump(talk_data, f, ensure_ascii=False, indent=2)
shutil.copy2(os.path.join(DIST_MOD_DIR, "LanguageTalk.json"), os.path.join(GAME_MOD_DIR, "LanguageTalk.json"))

print(f"Merged Language: {lang_updated}/{len(lang_data)} entries from {lang_files} batch files.")
print(f"Merged LanguageTalk: {talk_updated}/{len(talk_data)} entries from {talk_files} batch files.")
print(f"Synced to {GAME_MOD_DIR}")
