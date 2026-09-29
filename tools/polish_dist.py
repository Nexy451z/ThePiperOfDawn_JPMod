"""
polish_dist.py - Audit & polish pass for dist/JapaneseMod/{Language,LanguageTalk}.json

Design goals
------------
* Never touch the CN/EN/TW columns, only `jp`.
* Strictly preserve {0}, {1} placeholders and rich-text tags (<color=...>, {/color}, ...).
  Transformations are applied only to the *text* segments between such tokens.
* Be conservative: every rule is a deterministic, verifiable rewrite.
* Produce a detailed change report.

Usage:
    python tools/polish_dist.py            # dry run (report only)
    python tools/polish_dist.py --apply    # write polished JSON + report
"""

import json
import os
import re
import sys
from collections import Counter, defaultdict

BASE = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(BASE, "dist", "JapaneseMod")
REP = os.path.join(BASE, "reports")
os.makedirs(REP, exist_ok=True)

# Tokens that must survive untouched.
TOKEN_RE = re.compile(r"(\{\d+\}|</?[a-zA-Z][^>]*>|\{[^}]*\})")

# ---------------------------------------------------------------- explicit fixes
# id -> (old, new). Applied to LanguageTalk.json and Language.json.
TALK_FIXES = {
    # untranslated CN TODO markers (cn is empty in source)
    30009: ("第五次回溯待翻译", "第五次遡行（翻訳待ち）"),
    70015208: ("第五次回溯待翻译", "第五次遡行（翻訳待ち）"),
    # non-Japanese / simplified / garbled fragments
    11052067: ("まるで「狐につままれる（鬼打墻）」みたいな話だな。",
               "まるで「狐に鼻をつままれる」ような話だな。"),
    11301064: ("生身のمد人間", "生身の人間"),
    12171021: ("T0\u200b", "T0"),
    14181012: ("濃雾", "濃霧"),
    60201301: ("虚탈状態", "虚脱状態"),
    30205115: ("深渊の大口", "深淵の大口"),
}

LANG_FIXES = {
    1001258: ("呼び込めます。\n\n", "呼び込めます。\n"),
    1001681: ("工房閉店中...", "工房閉店中…"),
    104011000: ("パートナー♪\u3000", "パートナー♪ "),
    104031000: ("パートナー♪\u3000", "パートナー♪ "),
    105211000: ("よぉ～♪\u3000", "よぉ～♪ "),
    108141: ("技能の源種：XX熟練", "技能の源種：XXの熟練"),
    108191: ("技能の源種：XX熟練", "技能の源種：XXの熟練"),
}

# Currency consistency: 金币 -> 金貨 (glossary). id -> replacement pairs.
CURRENCY_FIXES = {
    12063005: [("400ゴールド", "金貨400枚")],
    12063008: [("800ゴールド", "金貨800枚")],
    12063012: [("2000ゴールド", "金貨2000枚")],
    12063041: [("2000ゴールド", "金貨2000枚")],
    13151130: [("100000ゴールド", "金貨100000枚")],
    13177099: [("賞金10000ゴールドを与える", "金貨10000枚の懸賞金を与える")],
    13177110: [("10000ゴールドの値", "金貨10000枚の値")],
    14104003: [("二千ゴールド", "金貨二千枚")],
    14104005: [("二千ゴールド", "金貨二千枚")],
    15083018: [("人間が5ゴールド", "人間が金貨5枚")],
    15083022: [("そっちは5ゴールド", "そっちは金貨5枚")],
    15308130: [("百ゴールドだ", "金貨百枚だ")],
    30103011: [("10ゴールド金貨を一枚置いた", "額面10の金貨を一枚置いた")],
    30103097: [("一個5ゴールド", "一個につき金貨5枚")],
    30103098: [("5ゴールドですか", "金貨5枚ですか")],
    30103105: [("一個5ゴールドの", "一個につき金貨5枚の")],
    60302127: [("治療費の100ゴールド", "治療費の金貨100枚")],
    60302166: [("ハンカチが1ゴールドもする", "ハンカチが金貨1枚もする")],
    60302172: [("100ゴールドで買う", "金貨100枚で買う")],
    60302213: [("9999999999ゴールド", "金貨9999999999枚")],
    60502025: [("一個五ゴールドだよ", "金貨五枚で一個だよ")],
    60502053: [("五ゴールドの", "金貨五枚の")],
}

# ---------------------------------------------------------------- global text rules
# Applied to text segments only (never inside placeholders / tags).
GLOBAL_RULES = [
    # natural Japanese; cut wordy "することができる" -> "できる".
    # Require >=2 kanji before する so single-kanji サ変 verbs (発する, 有する, ...),
    # whose potential form cannot be "〜できる", are left untouched.
    (re.compile(r"([一-龥]{2})することができ"), r"\1でき"),
    # unify level notation
    (re.compile(r"LV\."), "Lv."),
    # full-width tilde -> wave dash (visual/native consistency)
    (re.compile("\uFF5E"), "\u301C"),
    # remove zero-width space / convert non-breaking space
    (re.compile("\u200B"), ""),
    (re.compile("\u00A0"), " "),
]

# Language.json only (UI labels / item descriptions / tutorials)
LANG_ONLY_RULES = [
    # "(1)" -> "（1）", "(日本語)" -> "（日本語）" but leave kaomoji untouched
    (re.compile(r"\((\d+)\)"), r"（\1）"),
    (re.compile(r"\(([ぁ-んァ-ヶ一-龥][^()\d]*)\)"), r"（\1）"),
    # tighten number/spacing around the multiplication sign
    (re.compile(r"(\d)\s*✕\s*(\d)"), r"\1✕\2"),
    (re.compile(r"([一-龥])-([一-龥])"), r"\1・\2"),
]


def transform(text, rules):
    """Apply regex rules only to non-token text segments."""
    if not text:
        return text, 0
    parts = TOKEN_RE.split(text)
    changed = 0
    for i, seg in enumerate(parts):
        if not seg:
            continue
        # placeholders / tags are odd indices; also skip any that look like a token
        if i % 2 == 1:
            continue
        new = seg
        for rx, repl in rules:
            new2 = rx.sub(repl, new)
            if new2 != new:
                new = new2
        if new != seg:
            changed += 1
            parts[i] = new
    return "".join(parts), changed


def process(name, data, id_fixes, extra_rules, is_lang):
    counts = Counter()
    examples = defaultdict(list)
    before_after = []
    for e in data:
        cn = e.get("cn") or ""
        jp = e.get("jp") or ""
        if not jp:
            continue
        orig = jp
        # 1) explicit id-targeted fixes (also for entries with empty cn)
        for old, new in id_fixes.get(e["id"], []):
            if old and old in jp:
                jp = jp.replace(old, new)
                counts["explicit"] += 1
        # 2) per-id currency fixes
        for old, new in CURRENCY_FIXES.get(e["id"], []):
            if old in jp:
                jp = jp.replace(old, new)
                counts["currency"] += 1
        # entries without source text get id-fixes only
        if not cn.strip() or not jp:
            e["jp"] = jp
            if jp != orig and len(before_after) < 4000:
                before_after.append((e["id"], e.get("flag"), orig, jp))
            continue
        # 3) global + language-only text rules
        rules = list(GLOBAL_RULES) + (list(extra_rules) if is_lang else [])
        jp, n = transform(jp, rules)
        if n:
            counts["text_rules"] += n
        # 4) whitespace: strip trailing spaces but keep newlines; no double blank lines
        jp = re.sub(r"[ \t]+\n", "\n", jp)
        jp = re.sub(r"\n{3,}", "\n\n", jp)
        jp = jp.rstrip(" \t")
        if jp != orig:
            if len(before_after) < 4000:
                before_after.append((e["id"], e.get("flag"), orig, jp))
        e["jp"] = jp
    return counts, before_after


def main():
    apply = "--apply" in sys.argv

    lang = json.load(open(os.path.join(DIST, "Language.json"), encoding="utf-8"))
    talk = json.load(open(os.path.join(DIST, "LanguageTalk.json"), encoding="utf-8"))

    # build id_fixes maps as list of pairs
    def pairs(d):
        return {k: [v] for k, v in d.items()}

    lc, lb = process("Language.json", lang, pairs(LANG_FIXES), LANG_ONLY_RULES, True)
    tc, tb = process("LanguageTalk.json", talk, pairs(TALK_FIXES), [], False)

    report = os.path.join(REP, "polish_report.txt")
    with open(report, "w", encoding="utf-8") as fh:
        fh.write("=" * 78 + "\n")
        fh.write("POLISH REPORT\n")
        fh.write("=" * 78 + "\n")
        fh.write(f"Language.json    : {len(lb)} entries changed\n")
        fh.write(f"LanguageTalk.json: {len(tb)} entries changed\n")
        fh.write(f"Language counters    : {dict(lc)}\n")
        fh.write(f"LanguageTalk counters: {dict(tc)}\n")
        fh.write("\n--- Language.json examples ---\n")
        for eid, flag, old, new in lb[:120]:
            fh.write(f"id={eid}\n  - {old[:160]!r}\n  + {new[:160]!r}\n")
        fh.write("\n--- LanguageTalk.json examples ---\n")
        for eid, flag, old, new in tb[:200]:
            fh.write(f"id={eid} f={flag}\n  - {old[:160]!r}\n  + {new[:160]!r}\n")

    print(f"Language.json changed: {len(lb)}")
    print(f"LanguageTalk.json changed: {len(tb)}")
    print("report:", report)

    if apply:
        with open(os.path.join(DIST, "Language.json"), "w", encoding="utf-8") as f:
            json.dump(lang, f, ensure_ascii=False, indent=2)
        with open(os.path.join(DIST, "LanguageTalk.json"), "w", encoding="utf-8") as f:
            json.dump(talk, f, ensure_ascii=False, indent=2)
        print("APPLIED to dist/JapaneseMod/*.json")


if __name__ == "__main__":
    main()
