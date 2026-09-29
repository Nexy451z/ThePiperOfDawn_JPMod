import json
import os

BASE = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod"
DIST = os.path.join(BASE, "dist", "JapaneseMod")
REP = os.path.join(BASE, "reports")

CNWORDS = ["我的", "你的", "他的", "她的", "我们", "你们", "他们", "她们", "这个", "那个",
           "什么", "怎么", "因为", "所以", "但是", "如果", "可以", "已经", "还是", "没有",
           "不是", "就是", "一个", "一样", "这样", "那样", "的话", "而已", "罢了", "为了",
           "虽然", "然后", "而且", "并且", "或者", "应该", "必须", "需要", "知道", "觉得",
           "喜欢", "东西", "时候", "地方", "问题", "事情", "一直", "一定", "非常", "真的",
           "不要", "不会", "不能", "没有", "还有", "只是", "现在", "以前", "以后", "之后",
           "可以", "可能", "然后", "于是", "终于", "突然", "果然", "当然", "其实", "到底",
           "着", "了", "们", "吗", "呢", "这", "那", "为", "让", "给", "对", "从", "把"]

with open(os.path.join(REP, "cnword_issues.txt"), "w", encoding="utf-8") as fh:
    for name in ["Language.json", "LanguageTalk.json"]:
        data = json.load(open(os.path.join(DIST, name), encoding="utf-8"))
        for e in data:
            jp = e.get("jp") or ""
            found = [w for w in CNWORDS if w in jp]
            if found:
                fh.write(f"{name} id={e['id']} f={e.get('flag','')} found={found}\n")
                fh.write(f"  JP: {jp[:150]}\n")
print("done")
