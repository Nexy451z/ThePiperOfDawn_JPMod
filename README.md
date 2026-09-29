# The Piper of Dawn 日本語化Mod（非公式）

『The Piper of Dawn（破晓的吹笛人 / 夜明けの笛吹き者）』の非公式日本語化MODです。
ゲーム本体を一切書き換えず、**プロキシDLL（version.dll）方式**で日本語テキストを読み込ませます。

- UI・アイテム・システム翻訳: **Language.json（6,459件）**
- ストーリー・会話翻訳: **LanguageTalk.json（41,159件）**
- 公開ファイルは**翻訳文のみ**です（ゲームの原文テキストは含みません）

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
    Language.json                UI・アイテム・システム翻訳（id と jp のみ）
    LanguageTalk.json            ストーリー・会話翻訳（id と jp のみ）
  ThePiperOfDawn_JPMod_v1.0.zip 配布用パッケージ
  README.txt                    同梱インストール説明
mod_src/
  version.cpp                   Proxy DLL 本体（C++ / Win32）
  version.def                   エクスポート定義
  build.bat                     ビルド用バッチ（MSVC）
```

## ビルド方法（DLLの再ビルド）

Visual Studio（「C++によるデスクトップ開発」または Build Tools）をインストール後、`mod_src/build.bat` をダブルクリック／実行してください。
`vswhere` でVSのエディション・バージョンを自動検出してビルドします（`..\dist\version.dll` を出力）。
出力された `version.dll` をゲームフォルダへ配置します。

## 翻訳について

- 中国語原文を基準に翻訳し、既存の機械翻訳の誤りは引き継いでいません
- 地名・陣営・キャラクター名などの固有名詞と、話者ごとの一人称・二人称・文末表現を統一しています
- 翻訳JSONは `[{"id": 数値, "jp": "訳文"}]` 形式で、ゲーム本体の更新時は該当IDのみ再生成してください

## 免責

- 本MODは非公式のファン翻訳であり、ゲームの開発元・販売元とは一切関係ありません
- ゲーム本体のファイルおよび著作権は各権利者に帰属します。本リポジトリにはゲーム本体のファイルおよび原文テキストは含まれません
- 本MODの利用は自己責任でお願いします
