import json
import os
import re
import sys
from collections import Counter

BASE_DIR = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(BASE_DIR, "dist", "JapaneseMod")

TAG_RE = re.compile(r"\{[^}]*\}")
PLACEHOLDER_RE = re.compile(r"\{\d+\}")
# rich text tags like <color=...>, </color>, <size=..>, <sprite=..>
RICHTEXT_RE = re.compile(r"</?[a-zA-Z][^>]*>")
ANGLE_TAG_RE = re.compile(r"<[^>]*>")

# Chinese function words / simplified-only characters that never appear in JP prose
CN_ONLY = set("们你吗呢呗嘛这说让级东门无头应还时实现发处关开农员运轻术钟齐丽亚严丝为义乐习乡书买举么岛众优递迟择标记认识语题类检务动劳业价贵贱贫富积蓄压强调查验试读写爱亲银飞龙鱼鸟车马书")


def load(name):
    with open(os.path.join(DIST, name), "r", encoding="utf-8") as f:
        return json.load(f)


def report(name, data):
    print("=" * 70)
    print(name, "entries:", len(data))
    ph_mismatch = []
    tag_mismatch = []
    cn_residue = []
    empty = []
    lead_trail = []
    double_punct = []
    half_katakana = []
    full_ascii_mix = []
    for e in data:
        cn = e.get("cn") or ""
        jp = e.get("jp") or ""
        if not cn.strip():
            continue
        if not jp.strip():
            empty.append(e)
            continue
        if jp != jp.strip():
            lead_trail.append(e)
        cph = sorted(PLACEHOLDER_RE.findall(cn))
        jph = sorted(PLACEHOLDER_RE.findall(jp))
        if cph != jph:
            ph_mismatch.append(e)
        ct = sorted(ANGLE_TAG_RE.findall(cn))
        jt = sorted(ANGLE_TAG_RE.findall(jp))
        if ct != jt:
            tag_mismatch.append(e)
        bad = set(ch for ch in jp if ch in CN_ONLY)
        if bad:
            cn_residue.append((e, "".join(bad)))
        if re.search(r"(。。|、、|！！|？？|！。|。！|？。|。？|！？|？！)", jp):
            double_punct.append(e)
        if re.search(r"[\uff66-\uff9f]", jp):
            half_katakana.append(e)
    print("  empty jp:", len(empty))
    print("  leading/trailing whitespace:", len(lead_trail))
    print("  placeholder mismatch:", len(ph_mismatch))
    for e in ph_mismatch[:25]:
        print("    PH", e["id"], "cn=", e["cn"][:60], "| jp=", (e.get("jp") or "")[:60])
    print("  angle-tag mismatch:", len(tag_mismatch))
    for e in tag_mismatch[:25]:
        print("    TAG", e["id"], "cn=", e["cn"][:60], "| jp=", (e.get("jp") or "")[:60])
    print("  CN-only residue:", len(cn_residue))
    for e, b in cn_residue[:25]:
        print("    CN", e["id"], b, "|", (e.get("jp") or "")[:60])
    print("  doubled punctuation:", len(double_punct))
    for e in double_punct[:10]:
        print("    PP", e["id"], (e.get("jp") or "")[:60])
    print("  half-width katakana:", len(half_katakana))
    for e in half_katakana[:10]:
        print("    HK", e["id"], (e.get("jp") or "")[:60])


if __name__ == "__main__":
    report("Language.json", load("Language.json"))
    report("LanguageTalk.json", load("LanguageTalk.json"))
