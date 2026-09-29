import json
import os
import re
import shutil

BASE_DIR = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST_MOD_DIR = os.path.join(BASE_DIR, "dist", "JapaneseMod")
GAME_MOD_DIR = r"C:\Program Files (x86)\Steam\steamapps\common\The Piper Of Dawn\JapaneseMod"

with open(os.path.join(BASE_DIR, "glossary.json"), "r", encoding="utf-8") as f:
    glossary = json.load(f)

char_names = glossary.get("character_names", {})

# Forbidden / unmapped terms to auto-fix if any slipped into jp
REPLACEMENTS = [
    ("ダイヤモンドの城", "ダイヤモンド・シティ"),
    ("繫花林地", "繁花林地"),
    ("{color/}", "{/color}"),
    ("{size/}", "{/size}"),
    ("茜茜", "アカネ"),
    ("奈奈", "ナナ"),
    ("即墨离", "ジモリー"),
    ("即墨 離", "ジモリー"),
    ("ジモ・リー", "ジモリー"),
    ("格蕾特", "グレーテル"),
    ("奥罗拉", "オーロラ"),
    ("丽卡妲", "リカダ"),
    ("リカルダ", "リカダ"),
    ("乌鸫", "クロウタドリ"),
    ("ウードン", "クロウタドリ"),
    ("依黛拉", "イデラ"),
    ("蒲鹿铃", "浦越鈴"),
    ("蒲 鹿鈴", "浦越鈴"),
    ("プー・ルーリン", "浦越鈴"),
    ("粉豆", "ピンクマメ"),
    ("ピンクビーン", "ピンクマメ"),
    ("豆粉", "マメコ"),
    ("ビーンパウダー", "マメコ"),
    ("修普洛斯", "ヒュプロス"),
    ("ヒュプノス", "ヒュプロス"),
    ("塔纳托斯", "タナトス"),
    ("霓克斯", "ニュクス"),
    ("芙华茨基夫人", "フヴァツキー夫人"),
    ("ブラヴァツキー夫人", "フヴァツキー夫人"),
    ("芙华茨基", "フヴァツキー"),
    ("ブラヴァツキー", "フヴァツキー"),
    ("巴娜卡", "バーナーカ"),
    ("バナーカ", "バーナーカ"),
    ("韩赛尔", "ハンゼル"),
    ("毛奇", "モーチ"),
    ("モルトケ", "モーチ"),
    ("古登", "コトン"),
    ("グッデン", "コトン"),
    ("埃李希", "エルリッヒ"),
    ("エーリヒ", "エルリッヒ"),
    ("凯达露西", "ケダルシー"),
    ("布丽基特", "ブリギット"),
    ("温斯顿", "ウィンストン"),
    ("格伦德尔", "グレンデル"),
    ("维斯塔妮", "ウェスタニー"),
    ("夏绿蒂", "シャルロッテ"),
    ("海伦娜·沃夫根", "ヘレナ・ヴォルフゲン"),
    ("老沃夫根", "ヴォルフゲンさん"),
    ("华特男爵", "ウォルター男爵"),
    ("贝安男爵", "ベアン男爵"),
]

placeholder_re = re.compile(r"\{\d+\}")

def check_and_fix(file_path, is_lang=False):
    with open(file_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    fixed_terms = 0
    fixed_placeholders = 0
    exact_char_fixed = 0

    for e in data:
        cn = e.get("cn") or ""
        jp = e.get("jp") or ""
        if not cn.strip():
            continue

        orig_jp = jp

        # If exact character name in Language.json
        if is_lang and cn.strip() in char_names:
            # Special case: ID 2001 is "我" -> "私" in UI speaker plate
            jp = char_names[cn.strip()]
            if jp != orig_jp:
                exact_char_fixed += 1
        else:
            for bad, good in REPLACEMENTS:
                if bad in jp:
                    jp = jp.replace(bad, good)
            if jp != orig_jp:
                fixed_terms += 1

        # Verify {0}, {1} placeholders
        cn_ph = set(placeholder_re.findall(cn))
        jp_ph = set(placeholder_re.findall(jp))
        if cn_ph != jp_ph:
            fixed_placeholders += 1
            print(f"[PH WARN] ID {e['id']}: cn_ph={cn_ph} vs jp_ph={jp_ph} | cn={cn[:40]} | jp={jp[:40]}")

        e["jp"] = jp

    with open(file_path, "w", encoding="utf-8") as f:
        json.dump(data, f, ensure_ascii=False, indent=2)

    return len(data), fixed_terms, exact_char_fixed, fixed_placeholders

lang_path = os.path.join(DIST_MOD_DIR, "Language.json")
talk_path = os.path.join(DIST_MOD_DIR, "LanguageTalk.json")

print("Validating Language.json...")
l_total, l_fixed, l_char, l_ph = check_and_fix(lang_path, is_lang=True)
print(f"  Language.json: total={l_total}, term_fixes={l_fixed}, char_name_fixes={l_char}, ph_warnings={l_ph}")

print("Validating LanguageTalk.json...")
t_total, t_fixed, t_char, t_ph = check_and_fix(talk_path, is_lang=False)
print(f"  LanguageTalk.json: total={t_total}, term_fixes={t_fixed}, ph_warnings={t_ph}")

# Remove temporary override_Language.json if present so Language.json is the single source of truth
for d in (DIST_MOD_DIR, GAME_MOD_DIR):
    ov = os.path.join(d, "override_Language.json")
    if os.path.exists(ov):
        os.remove(ov)

shutil.copy2(lang_path, os.path.join(GAME_MOD_DIR, "Language.json"))
shutil.copy2(talk_path, os.path.join(GAME_MOD_DIR, "LanguageTalk.json"))
shutil.copy2(os.path.join(BASE_DIR, "dist", "README.txt"), os.path.join(GAME_MOD_DIR, "README.txt"))
print("Synced validated Language.json and LanguageTalk.json to game folder!")
