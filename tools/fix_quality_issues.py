import json, os, shutil, datetime

B = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(B, "dist", "JapaneseMod")
BK = r"C:\Users\haruk\AppData\Local\Temp\opencode\backup_pre_fix"
os.makedirs(BK, exist_ok=True)

# per-id replacement rules: id -> list of (from, to)
RULES = {
    60306104: [("人薪（じんしん）", "人柴")],
    60306107: [("人薪", "人柴")],
    70021140: [("ヒト薪", "人柴"), ("ダイヤモンドシティ", "ダイヤモンド・シティ")],
    14002031: [("オールド・ジン", "キング")],
    60207008: [("ヒーマン衛兵隊", "ヘーマン衛隊"), ("ヒーマン", "ヘーマン")],
    60306024: [("メテオハンター", "隕石ハンター")],
    60306025: [("メテオハンター", "隕石ハンター")],
    60303073: [("蒲鹿鈴", "浦越鈴")],
    60506074: [("ブリジット", "ブリギット")],
    15312043: [("身体の不自由な年寄り", "身体の不自由な人")],
    60502076: [("居住区", "住宅地")],
    60302192: [("大公邸", "大公府")],
    60506157: [("大公邸", "大公府")],
    60303007: [("太陽の酒場", "太陽酒場")],
    60304064: [("太陽の酒場", "太陽酒場")],
    60206126: [("笛吹き", "笛奏者")],
    14002100: [("彩りの衣の笛吹き", "彩りの衣の笛吹き者")],
    70031054: [("彩りの衣の笛吹き", "彩りの衣の笛吹き者")],
    30401232: [("エルフ", "精霊")],
    30402161: [("エルフ", "精霊")],
    30402169: [("エルフ", "精霊")],
    30403025: [("エルフ", "精霊")],
    30403027: [("エルフ", "精霊")],
    30403029: [("エルフ", "精霊")],
    60501150: [("アビゲイル", "アビゲール")],
    60501170: [("アビゲイル", "アビゲール")],
    60506077: [("アビゲイル", "アビゲール")],
    70015080: [("ダイヤモンドシティ", "ダイヤモンド・シティ")],
    70015214: [("ダイヤモンドシティ", "ダイヤモンド・シティ")],
    70015217: [("ダイヤモンドシティ", "ダイヤモンド・シティ")],
    70021052: [("ダイヤモンドシティ", "ダイヤモンド・シティ")],
    70021053: [("ダイヤモンドシティ", "ダイヤモンド・シティ")],
    70021062: [("ダイヤモンドシティ", "ダイヤモンド・シティ")],
    70021081: [("ダイヤモンドシティ", "ダイヤモンド・シティ")],
    70021142: [("ダイヤモンドシティ", "ダイヤモンド・シティ")],
    70022127: [("ダイヤモンドシティ", "ダイヤモンド・シティ")],
}

stamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
report = []

for name in ("Language.json", "LanguageTalk.json"):
    path = os.path.join(DIST, name)
    shutil.copy2(path, os.path.join(BK, name))
    data = json.load(open(path, encoding="utf-8"))
    idx = {e["id"]: e for e in data}
    hit = 0
    for i, rules in RULES.items():
        e = idx.get(i)
        if not e:
            continue
        jp = e.get("jp") or ""
        orig = jp
        for fr, to in rules:
            jp = jp.replace(fr, to)
        if jp != orig:
            e["jp"] = jp
            hit += 1
            report.append(f"{name}\t{i}\t{orig}\n\t-> {jp}")
    json.dump(data, open(path, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    print(f"{name}: fixed {hit} entries")

open(os.path.join(BK, "fix_report.txt"), "w", encoding="utf-8").write("\n".join(report))
print("backup:", BK)
