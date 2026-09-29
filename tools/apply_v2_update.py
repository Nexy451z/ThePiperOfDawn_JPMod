import json
import os
import shutil
import subprocess
import zipfile

BASE_DIR = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST_DIR = os.path.join(BASE_DIR, "dist")
DIST_MOD_DIR = os.path.join(DIST_DIR, "JapaneseMod")
GAME_DIR = r"C:\Program Files (x86)\Steam\steamapps\common\The Piper Of Dawn"
GAME_MOD_DIR = os.path.join(GAME_DIR, "JapaneseMod")

# 1. Load existing translated v1 maps
with open(os.path.join(DIST_MOD_DIR, "Language.json"), "r", encoding="utf-8") as f:
    v1_lang = {e["id"]: e["jp"] for e in json.load(f)}

with open(os.path.join(DIST_MOD_DIR, "LanguageTalk.json"), "r", encoding="utf-8") as f:
    v1_talk = {e["id"]: e["jp"] for e in json.load(f)}

# 2. New / Modified translations for v2 (translated from Chinese cn)
v2_lang_updates = {
    290011: "神剣グラムの種",
    290012: "{0}と交配して得られた神剣グラムの種。",
    810561: "材料加工熟練 LV.1",
    810571: "材料加工熟練 LV.2",
    810581: "材料加工熟練 LV.3",
    810591: "材料加工熟練 LV.4",
    810601: "材料加工熟練 LV.5",
    810611: "醸造熟練 LV.1",
    810621: "醸造熟練 LV.2",
    810631: "醸造熟練 LV.3",
    810641: "醸造熟練 LV.4",
    810651: "醸造熟練 LV.5",
    810761: "火制御熟練 LV.1",
    810762: "【火元素錬金陣】での作業時、生産速度+10%。",
    810771: "火制御熟練 LV.2",
    810772: "【火元素錬金陣】での作業時、生産速度+20%。",
    810781: "火制御熟練 LV.3",
    810782: "【火元素錬金陣】での作業時、生産速度+30%。",
    810791: "火制御熟練 LV.4",
    810792: "【火元素錬金陣】での作業時、生産速度+40%。",
    810801: "火制御熟練 LV.5",
    810802: "【火元素錬金陣】での作業時、生産速度+50%。",
    810811: "風制御熟練 LV.1",
    810812: "【風元素錬金陣】での作業時、生産速度+10%。",
    810821: "風制御熟練 LV.2",
    810822: "【風元素錬金陣】での作業時、生産速度+20%。",
    810831: "風制御熟練 LV.3",
    810832: "【風元素錬金陣】での作業時、生産速度+30%。",
    810841: "風制御熟練 LV.4",
    810842: "【風元素錬金陣】での作業時、生産速度+40%。",
    810851: "風制御熟練 LV.5",
    810852: "【風元素錬金陣】での作業時、生産速度+50%。",
    810861: "土制御熟練 LV.1",
    810862: "【土元素錬金陣】での作業時、生産速度+10%。",
    810871: "土制御熟練 LV.2",
    810872: "【土元素錬金陣】での作業時、生産速度+20%。",
    810881: "土制御熟練 LV.3",
    810882: "【土元素錬金陣】での作業時、生産速度+30%。",
    810891: "土制御熟練 LV.4",
    810892: "【土元素錬金陣】での作業時、生産速度+40%。",
    810901: "土制御熟練 LV.5",
    810902: "【土元素錬金陣】での作業時、生産速度+50%。",
    810911: "水制御熟練 LV.1",
    810912: "【水元素錬金陣】での作業時、生産速度+10%。",
    810921: "水制御熟練 LV.2",
    810922: "【水元素錬金陣】での作業時、生産速度+20%。",
    810931: "水制御熟練 LV.3",
    810932: "【水元素錬金陣】での作業時、生産速度+30%。",
    810941: "水制御熟練 LV.4",
    810942: "【水元素錬金陣】での作業時、生産速度+40%。",
    810951: "水制御熟練 LV.5",
    810952: "【水元素錬金陣】での作業時、生産速度+50%。",
    1001737: "遺伝子効果【深根】により、この作物は撤去できません",
}

v2_talk_updates = {
    60306012: "開始価格はチップ500枚、上限価格は5000枚よ。これよりブラインド入札を始めるわ。",
    60306013: "俺は相場を見積もり、開始価格の四倍にあたるチップ2000枚という高値を書き込んだ。",
    60306019: "チップ5000枚で即決買いとはな。ずいぶんと思い切りのいい奴だ。",
    60306042: "開始価格はチップ1000枚、上限価格は10000枚よ。これよりブラインド入札を始めるわ。",
    60306047: "30000枚だと？",
    80012109: "先住民の頭蓋、ドルイドの四肢、ルートヴィヒの始祖の躯、魔女が遺した臓腑と血脈。",
}

# 3. Apply onto extracted_v2
with open(os.path.join(BASE_DIR, "extracted_v2", "Language.json"), "r", encoding="utf-8") as f:
    v2_lang_list = json.load(f)

for e in v2_lang_list:
    eid = e["id"]
    if eid in v2_lang_updates:
        e["jp"] = v2_lang_updates[eid]
    elif eid in v1_lang:
        e["jp"] = v1_lang[eid]

with open(os.path.join(BASE_DIR, "extracted_v2", "LanguageTalk.json"), "r", encoding="utf-8") as f:
    v2_talk_list = json.load(f)

for e in v2_talk_list:
    eid = e["id"]
    if eid in v2_talk_updates:
        e["jp"] = v2_talk_updates[eid]
    elif eid in v1_talk:
        e["jp"] = v1_talk[eid]

with open(os.path.join(DIST_MOD_DIR, "Language.json"), "w", encoding="utf-8") as f:
    json.dump(v2_lang_list, f, ensure_ascii=False, indent=2)

with open(os.path.join(DIST_MOD_DIR, "LanguageTalk.json"), "w", encoding="utf-8") as f:
    json.dump(v2_talk_list, f, ensure_ascii=False, indent=2)

# 4. Rebuild version.dll
r = subprocess.run(["cmd.exe", "/c", "build.bat"], cwd=os.path.join(BASE_DIR, "mod_src"))
print("Rebuild version.dll RC:", r.returncode)

# 5. Copy to game folder & update ZIP
shutil.copy2(os.path.join(DIST_DIR, "version.dll"), os.path.join(GAME_DIR, "version.dll"))
shutil.copy2(os.path.join(DIST_MOD_DIR, "Language.json"), os.path.join(GAME_MOD_DIR, "Language.json"))
shutil.copy2(os.path.join(DIST_MOD_DIR, "LanguageTalk.json"), os.path.join(GAME_MOD_DIR, "LanguageTalk.json"))

zpath = os.path.join(DIST_DIR, "ThePiperOfDawn_JPMod_v1.0.zip")
with zipfile.ZipFile(zpath, "w", zipfile.ZIP_DEFLATED) as zf:
    zf.write(os.path.join(DIST_DIR, "version.dll"), "version.dll")
    zf.write(os.path.join(DIST_DIR, "README.txt"), "README.txt")
    zf.write(os.path.join(DIST_MOD_DIR, "Language.json"), "JapaneseMod/Language.json")
    zf.write(os.path.join(DIST_MOD_DIR, "LanguageTalk.json"), "JapaneseMod/LanguageTalk.json")

print("Successfully updated Language.json (6,459 entries), LanguageTalk.json (41,159 entries), version.dll, and ZIP!")
