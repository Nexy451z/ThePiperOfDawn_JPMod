import json, os, re, collections, random

B = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
OUT = os.path.join(B, "reports", "quality_review")
os.makedirs(OUT, exist_ok=True)

def load(p):
    with open(p, encoding="utf-8") as f:
        return json.load(f)

lang = load(os.path.join(B, "dist", "JapaneseMod", "Language.json"))
talk = load(os.path.join(B, "dist", "JapaneseMod", "LanguageTalk.json"))
gloss = load(os.path.join(B, "glossary.json"))

def w(name, lines):
    with open(os.path.join(OUT, name), "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print("wrote", name, len(lines))

CJK = re.compile(r"[\u3400-\u9fff]")
SIMPLIFIED = set("们为个时说过还对发后专业题习爱击术师样办见车马鸟风长门问间关无张汉语译读书电话买卖贵钱银铁钢战记让请谁谢识认论谈讲词试验检单边变头脑脸经济团园尔从众优传伦伟冲减凤刘则刚创别卫厂历压县类华协归处备够岁币帮广庆应库废开录恶总忧怀态惊领览观视觉访军农矿场儿亲严运远达进连迟适选递逻遗邮郑释气义务员议营围执职级极织弃页兴统强墙联乡丰乐标准报纸练续约给结亿")
TRAD = set("們說對發專擊樣辦關譯讀賣錢鐵戰讓驗檢單邊變腦臉濟團從眾傳廠歷壓縣歸處夠歲幫廣應廢錄惡總懷麼嗎妳裡沒來學國體樂藥點將與灣")

def zh_offenders(s):
    return [ch for ch in set(s) if ch in SIMPLIFIED or ch in TRAD]

# ---------- 1. untranslated / empty ----------
unt = []
for e in lang + talk:
    cn = (e.get("cn") or "").strip()
    jp = (e.get("jp") or "").strip()
    if cn and not jp:
        unt.append(f'EMPTY\t{e["id"]}\t{cn[:120]}')
    elif cn == jp and CJK.search(cn) and len(cn) > 1:
        unt.append(f'SAME\t{e["id"]}\t{cn[:120]}')
w("issues_untranslated.txt", unt)

# ---------- 2. chinese chars left in jp ----------
off_lines = []
char_counter = collections.Counter()
for e in lang + talk:
    jp = e.get("jp") or ""
    off = zh_offenders(jp)
    if off:
        char_counter.update(off)
        off_lines.append(f'{e["id"]}\t[{"".join(sorted(set(off)))}]\t{jp[:150]}')
w("issues_zh_chars.txt", off_lines)
w("issues_zh_chars_count.txt", [f"{k}\t{v}" for k, v in char_counter.most_common(60)])

# ---------- 3. placeholders ----------
PH = re.compile(r"\{\d+\}")
BR = re.compile(r"\[[^\]\n]{0,30}\]")
NL = re.compile(r"\\n")
ph_bad = []
for e in lang + talk:
    cn = e.get("cn") or ""
    jp = e.get("jp") or ""
    if set(PH.findall(cn)) != set(PH.findall(jp)):
        ph_bad.append(f'PH\t{e["id"]}\tcn={set(PH.findall(cn))} jp={set(PH.findall(jp))}\t{cn[:80]} | {jp[:80]}')
    if len(NL.findall(cn)) != len(NL.findall(jp)):
        ph_bad.append(f'NL\t{e["id"]}\tcn={len(NL.findall(cn))} jp={len(NL.findall(jp))}\t{cn[:60]} | {jp[:60]}')
    bcn = set(BR.findall(cn))
    bjp = set(BR.findall(jp))
    if bcn != bjp and (bcn or bjp):
        ph_bad.append(f'BR\t{e["id"]}\tcn={sorted(bcn)[:4]} jp={sorted(bjp)[:4]}\t{cn[:60]} | {jp[:60]}')
w("issues_placeholder.txt", ph_bad)

# ---------- 4. forbidden terms ----------
FORBID = [
    "ダイヤモンドの城", "空軍", "繫花林地", "シルバーキー", "銀鑰",
    "ペドラ", "デビル", "オーロラ姫", "ナナコ",
    "アカネコ", "ヘルマン", "ブラック・ジャック",
    "ヒプノス", "ニュックス", "ヒュプノス",
]
forb_hits = collections.Counter()
forb_lines = []
for e in lang + talk:
    jp = e.get("jp") or ""
    for bad in FORBID:
        if bad in jp:
            forb_hits[bad] += 1
            if forb_hits[bad] <= 12:
                forb_lines.append(f'{bad}\t{e["id"]}\t{jp[:150]}')
w("issues_forbidden.txt", [f"{k}\t{v}" for k, v in forb_hits.most_common()] + ["---"] + forb_lines)

# ---------- 5. glossary miss ----------
VARIANT = {
    "钻石之城": ["ダイヤモンド・シティ", "ダイヤの街", "ダイヤモンドシティ"],
    "五月花广场": ["五月花広場", "メイフラワー広場", "五月花"],
    "矿洞": ["鉱山", "坑道", "鉱洞"],
    "精灵": ["精霊", "亜人"],
    "母亲": ["母", "母親", "母の会"],
    "吹笛人": ["笛吹き者", "笛吹き", "笛吹男"],
    "持笛者": ["笛奏者", "笛を持つ者"],
    "我": None,
}
gloss_miss = collections.Counter()
gloss_lines = []
pairs = []
for cat in ("locations", "factions_and_lore", "system_terms", "character_names"):
    for k, v in gloss.get(cat, {}).items():
        pairs.append((cat, k, v))
for cat, k, v in pairs:
    if len(k) < 2:
        continue
    variants = VARIANT.get(k, [v])
    for e in lang + talk:
        cn = e.get("cn") or ""
        jp = e.get("jp") or ""
        if k in cn and jp:
            if not any(x in jp for x in variants):
                gloss_miss[k] += 1
                if gloss_miss[k] <= 8:
                    gloss_lines.append(f'{cat}\t{k}→{v}\t{e["id"]}\tCN:{cn[:70]} | JP:{jp[:70]}')
w("issues_glossary_miss.txt", [f"{k}\t{v}" for k, v in gloss_miss.most_common(50)] + ["---"] + gloss_lines)

# ---------- 6. same cn -> different jp ----------
def norm(s):
    return re.sub(r"\s+", "", s)

g1 = collections.defaultdict(set)
for e in lang + talk:
    cn = norm(e.get("cn") or "")
    jp = norm(e.get("jp") or "")
    if len(cn) >= 5 and jp:
        g1[cn].add(jp)
inc = [(k, v) for k, v in g1.items() if len(v) > 1]
inc.sort(key=lambda x: -len(x[1]))
lines = [f"groups={len(inc)}"]
for k, v in inc[:120]:
    lines.append(f'CN:{k[:60]}')
    for jp in list(v)[:6]:
        lines.append(f'   JP:{jp[:90]}')
w("issues_same_cn_diff_jp.txt", lines)

# ---------- 7. same jp <- different cn ----------
g2 = collections.defaultdict(set)
rev = collections.Counter()
for e in lang + talk:
    cn = norm(e.get("cn") or "")
    jp = norm(e.get("jp") or "")
    if len(jp) >= 5 and cn and cn != jp:
        g2[jp].add(cn)
        rev[jp] += 1
dup = [(k, v, rev[k]) for k, v in g2.items() if len(v) > 1]
dup.sort(key=lambda x: -len(x[1]))
lines = [f"groups={len(dup)}"]
for jp, v, c in dup[:80]:
    lines.append(f'JP:{jp[:60]}  (x{c})')
    for cn in list(v)[:5]:
        lines.append(f'   CN:{cn[:70]}')
w("issues_same_jp_diff_cn.txt", lines)

# ---------- 8. length ratio ----------
lr = []
for e in lang + talk:
    cn = e.get("cn") or ""
    jp = e.get("jp") or ""
    if len(cn) >= 8 and jp:
        r = len(jp) / len(cn)
        if r < 0.4 or r > 3.2:
            lr.append(f'{r:.2f}\t{e["id"]}\tCN({len(cn)}):{cn[:60]} | JP({len(jp)}):{jp[:60]}')
w("issues_length_ratio.txt", lr)

# ---------- 9. per-flag register ----------
plates = {}
for e in lang:
    i = e["id"]
    if 20000 <= i < 30000 and i % 10 == 1:
        plates[i // 10] = e.get("jp")
flag_lines = collections.defaultdict(list)
for e in talk:
    flag_lines[e.get("flag")].append(e)
PRON = ["俺", "僕", "私", "あたし", "わたくし", "我", "小生", "オラ", "ウサ", "わし", "わたし", "拙者", "余", "手前"]
ADDR = ["お前", "あんた", "あなた", "君", "キミ", "貴様", "てめえ", "先輩", "旦那様", "相棒", "閣下", "我が友", "坊や", "嬢ちゃん", "若いの"]
lines = []
flag_counts = sorted(flag_lines.items(), key=lambda x: -len(x[1]))
for flag, entries in flag_counts[:60]:
    txt = "\n".join(e.get("jp") or "" for e in entries)
    pron = {p: txt.count(p) for p in PRON}
    pron = {k: v for k, v in pron.items() if v}
    addr = {p: txt.count(p) for p in ADDR}
    addr = {k: v for k, v in addr.items() if v}
    name = plates.get(flag, "?")
    lines.append(f'flag={flag} ({name}) n={len(entries)}')
    lines.append(f'   1st: {pron}')
    lines.append(f'   2nd: {addr}')
w("register_by_flag.txt", lines)

# ---------- 10. name plate check ----------
name_lines = []
char_names = gloss["character_names"]
plate_map = {v: k for k, v in char_names.items()}
bad_name = []
for e in lang:
    i = e["id"]
    if 20000 <= i < 30000 and i % 10 == 1:
        cn = e.get("cn")
        jp = e.get("jp")
        expect = char_names.get(cn)
        rec = f'id={i} flag={i//10} cn={cn} jp={jp} expect={expect}'
        if expect and jp != expect:
            bad_name.append(rec)
        name_lines.append(rec)
w("names_check.txt", ["--- MISMATCH ---"] + bad_name + ["--- ALL ---"] + name_lines)

# ---------- 11. samples for manual review ----------
rng = random.Random(20260929)

def fmt(e, k=180):
    cn = (e.get("cn") or "").replace("\n", "\\n")
    jp = (e.get("jp") or "").replace("\n", "\\n")
    return f'{e["id"]}\tCN:{cn[:k]}\n\tJP:{jp[:k]}'

lang_sorted = sorted(lang, key=lambda e: e["id"])
lines = []
BUCKET = 300
buckets = collections.defaultdict(list)
for e in lang_sorted:
    buckets[e["id"] // BUCKET].append(e)
for b in sorted(buckets):
    sample = rng.sample(buckets[b], min(14, len(buckets[b])))
    lines.append(f'==== bucket {b * BUCKET}-{b * BUCKET + BUCKET - 1} ====')
    for e in sorted(sample, key=lambda x: x["id"]):
        lines.append(fmt(e, 200))
w("samples_lang_stratified.txt", lines)

main_flags = [2001, 2009, 2002, 2005, 2010, 2003, 2004, 2012, 2014, 2135, 2140, 2146, 2106, 2109, 2139, 2199,
              2108, 2103, 2119, 2117, 2120, 2123, 2131, 2127, 2128, 2110, 2114, 2104, 2132, 2133, 2142, 2145, 2112, 2118, 2152, 10081]
lines = []
for flag in main_flags:
    entries = flag_lines.get(flag, [])
    if not entries:
        continue
    lines.append(f'==== flag={flag} ({plates.get(flag, "?")}) n={len(entries)} ====')
    step = max(1, len(entries) // 12)
    for e in entries[::step][:12]:
        lines.append(fmt(e, 170))
w("samples_talk_speakers.txt", lines)

lines = []
for e in rng.sample(talk, 220):
    lines.append(fmt(e, 170))
w("samples_talk_random.txt", lines)

# ---------- 12. markup stats ----------
pat_counts = collections.Counter()
for e in lang + talk:
    for s in ((e.get("cn") or ""), (e.get("jp") or "")):
        for m in re.findall(r"\{[^}\n]{0,40}\}|<[^>\n]{0,40}>|\[[^\]\n]{0,40}\]|\\n|%[sd]", s):
            pat_counts[m] += 1
w("markup_counts.txt", [f"{k}\t{v}" for k, v in pat_counts.most_common(80)])

# ---------- 13. misc oddities ----------
misc = []
for e in lang + talk:
    jp = e.get("jp") or ""
    if "\ufffd" in jp:
        misc.append(f'REPL_CHAR\t{e["id"]}\t{jp[:120]}')
    if re.search(r"[?？]{3,}", jp) and re.search(r"[?？]{3,}", e.get("cn") or "") is None:
        misc.append(f'QQQ\t{e["id"]}\t{jp[:120]}')
    if "??" in jp and "??" not in (e.get("cn") or ""):
        misc.append(f'QQ\t{e["id"]}\t{jp[:120]}')
    if "＿" in jp or "＿" in (e.get("cn") or ""):
        misc.append(f'UNDERSCORE\t{e["id"]}\t{jp[:120]}')
    if re.search(r"\b[a-zA-Z]{4,}\b", jp) and not re.search(r"[a-zA-Z]{3,}", e.get("cn") or ""):
        misc.append(f'LATIN\t{e["id"]}\t{jp[:120]}')
w("issues_misc.txt", misc)

print("done")
