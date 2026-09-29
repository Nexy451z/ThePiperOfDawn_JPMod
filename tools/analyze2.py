import json
import os
import re

BASE = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(BASE, "dist", "JapaneseMod")
REP = os.path.join(BASE, "reports")
os.makedirs(REP, exist_ok=True)


def load(p):
    with open(p, "r", encoding="utf-8") as f:
        return json.load(f)


def dump_list(fh, title, items, fmt):
    fh.write(f"\n### {title}: {len(items)}\n")
    for e in items:
        fh.write("  " + fmt(e) + "\n")


standalone_wa = re.compile(r"(?<![我々])我(?![々がの])")

lang = load(os.path.join(DIST, "Language.json"))
talk = load(os.path.join(DIST, "LanguageTalk.json"))
old_lang = load(os.path.join(BASE, "extracted_v2", "Language.json"))
old_talk = load(os.path.join(BASE, "extracted_v2", "LanguageTalk.json"))
old_map_l = {e["id"]: e.get("jp") for e in old_lang}
old_map_t = {e["id"]: e.get("jp") for e in old_talk}

with open(os.path.join(REP, "analyze2.txt"), "w", encoding="utf-8") as fh:
    for name, data, omap in [("Language.json", lang, old_map_l), ("LanguageTalk.json", talk, old_map_t)]:
        fh.write("=" * 70 + f"\n{name}: {len(data)}\n")
        wa = [e for e in data if standalone_wa.search(e.get("jp") or "")]
        dump_list(fh, "standalone 我", wa,
                  lambda e: f"id={e['id']} f={e.get('flag','')} | {e.get('jp','')[:110]}")
        cncomma = [e for e in data if re.search(r"[，；]", e.get("jp") or "")]
        dump_list(fh, "CH comma/semicolon", cncomma,
                  lambda e: f"id={e['id']} | {e.get('jp','')[:110]}")
        asciiparen = [e for e in data if re.search(r"[\u3040-\u30ff\u4e00-\u9fff][(]", e.get("jp") or "")]
        fh.write(f"\n### ascii paren after CJK: {len(asciiparen)}\n")
        # unchanged since v2
        unchanged = [e for e in data if e.get("jp") == omap.get(e["id"])]
        fh.write(f"\n### unchanged since extracted_v2: {len(unchanged)}\n")
        changed = [e for e in data if e["id"] in omap and e.get("jp") != omap.get(e["id"])]
        fh.write(f"### changed since extracted_v2: {len(changed)}\n")
        newids = [e for e in data if e["id"] not in omap]
        fh.write(f"### new since extracted_v2: {len(newids)}\n")
    print("done")
