# The Piper of Dawn 日本語化Mod（非公式）

『The Piper of Dawn（破晓的吹笛人 / 夜明けの笛吹き者）』の非公式日本語化MODです。
ゲーム本体を一切書き換えず、**プロキシDLL（version.dll）方式**で日本語テキストを読み込ませます。

- UI・アイテム・システム翻訳: **Language.json（6,459件）**
- ストーリー・会話翻訳: **LanguageTalk.json（41,159件）**
- 翻訳方針は中国語原文（cn）を基準に、用語集とキャラ別口調定義（`TRANSLATION_BIBLE.md`）で統一

## 特徴

- ゲーム本体のファイルを一切改変しない（`version.dll` と `JapaneseMod` フォルダを置くだけ）
- アンインストールはファイル削除のみ
- JSONのホットリロード・翻訳ON/OFF・表示中テキストIDのダンプ機能

## インストール方法

ゲームのインストールフォルダ（`The Piper Of Dawn.exe` があるフォルダ）に以下をコピーしてください。

```
version.dll
JapaneseMod\Language.json
JapaneseMod\LanguageTalk.json
```

または `dist/ThePiperOfDawn_JPMod_v1.0.zip` を解答してゲームフォルダへコピーします。

## ゲーム内ショートカット

| キー | 動作 |
|---|---|
| F5 | `JapaneseMod` フォルダ内の JSON を再読み込み（ホットリロード） |
| F6 | 日本語化Modの ON / OFF 切り替え（原文との比較用） |
| F7 | 直近に表示されたテキストID一覧を `JapaneseMod\recent_ids.txt` に出力 |

## カスタム上書き

`JapaneseMod` フォルダ内に以下のファイルを作成すると、特定のIDだけ優先的に上書きできます。

- `JapaneseMod\override_Language.json`
- `JapaneseMod\override_LanguageTalk.json`

形式例:

```json
{ "1000001": "セーブデータをロード" }
```

## フォルダ構成

```
dist/
  version.dll                   ビルド済みプロキシDLL
  JapaneseMod/
    Language.json               UI・アイテム・システム翻訳
    LanguageTalk.json           ストーリー・会話翻訳
  ThePiperOfDawn_JPMod_v1.0.zip 配布用パッケージ
  README.txt                    同梱インストール説明
mod_src/
  version.cpp                   Proxy DLL 本体（C++ / Win32）
  version.def                   エクスポート定義
  build.bat                     ビルド用バッチ（MSVC）
tools/
  *.py                          翻訳抽出・マージ・QA・用語統一スクリプト
glossary.json                   固有名詞・用語集
TRANSLATION_BIBLE.md            翻訳バイブル（世界観・口調定義）
```

## ビルド方法（DLLの再ビルド）

`mod_src/build.bat` をMSVC（x86 Native Tools プロンプト）で実行してください。
出力された `version.dll` をゲームフォルダへ配置します。

## 翻訳について

- 原文は中国語（cn）のみを参照し、既存の機械翻訳や英語訳の誤りは引き継いでいません
- 地名・陣営・キャラクター名などの固有名詞は `glossary.json` で統一
- 話者ID（flag）ごとに一人称・二人称・文末表現を `TRANSLATION_BIBLE.md` で固定

## 免責

- 本MODは非公式のファン翻訳であり、ゲームの開発元・販売元とは一切関係ありません
- ゲーム本体のファイルおよび著作権は各権利者に帰属します。本リポジトリにはゲーム本体のファイルは含まれません
- 本MODの利用は自己責任でお願いします
