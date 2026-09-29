import json
import os
import re
from collections import Counter

BASE_DIR = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(BASE_DIR, "dist", "JapaneseMod")
REP = os.path.join(BASE_DIR, "reports")
os.makedirs(REP, exist_ok=True)


def load(name):
    with open(os.path.join(DIST, name), "r", encoding="utf-8") as f:
        return json.load(f)


def dump(fh, title, items, fmt=lambda e: ""):
    fh.write(f"\n### {title}: {len(items)}\n")
    for e in items[:400]:
        fh.write(f"  {e.get('id')} f={e.get('flag','')} | CN={e.get('cn','')[:50]!r} | JP={(e.get('jp') or '')[:90]!r} {fmt(e)}\n")


def scan(name, data, fh):
    fh.write("=" * 80 + f"\n{name}: {len(data)}\n")
    ws = [e for e in data if (e.get("jp") or "") != (e.get("jp") or "").strip()]
    dblspace = [e for e in data if "  " in (e.get("jp") or "")]
    fwspace = [e for e in data if "\u3000" in (e.get("jp") or "")]
    ascii_paren = [e for e in data if re.search(r"[A-Za-z0-9\u3040-\u30ff\u4e00-\u9fff]\(", (e.get("jp") or ""))]
    lv = [e for e in data if re.search(r"Lv\.?|LV\.?|レベル", (e.get("jp") or ""))]
    level_mix = Counter()
    for e in data:
        jp = e.get("jp") or ""
        for m in re.findall(r"Lv\.?\d|LV\.?\d|レベル\d|レベル ?\d|Lv |レベル ", jp):
            level_mix[m[:6]] += 1
    cn_punct = [e for e in data if re.search(r"[，。；：！？]", e.get("jp") or "")]
    quote_cn = [e for e in data if re.search(r"[\u201c\u201d\u2018\u2019]", e.get("jp") or "")]
    tilde = [e for e in data if re.search(r"[~〜]", e.get("jp") or "")]
    dots = [e for e in data if re.search(r"\.\.\.", e.get("jp") or "")]
    dash = [e for e in data if "--" in (e.get("jp") or "")]
    dump(fh, "leading/trailing whitespace", ws)
    dump(fh, "double space", dblspace)
    dump(fh, "full-width space", fwspace)
    dump(fh, "ascii paren after jp", ascii_paren)
    dump(fh, "CN punctuation in jp", cn_punct)
    dump(fh, "CN curly quotes", quote_cn)
    dump(fh, "tilde", tilde)
    dump(fh, "ascii ellipsis ...", dots)
    dump(fh, "double hyphen --", dash)
    fh.write("\n  LEVEL token counts: " + str(level_mix.most_common(20)) + "\n")


if __name__ == "__main__":
    with open(os.path.join(REP, "issues.txt"), "w", encoding="utf-8") as fh:
        scan("Language.json", load("Language.json"), fh)
        scan("LanguageTalk.json", load("LanguageTalk.json"), fh)
    print("done")
