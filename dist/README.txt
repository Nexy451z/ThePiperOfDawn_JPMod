======================================================================
『夜明けの笛吹き者 (The Piper of Dawn)』日本語改善Mod (Proxy DLL版)
======================================================================

【インストール方法】
ゲームのインストールフォルダ（The Piper Of Dawn.exe があるフォルダ）に
以下のファイルとフォルダをそのままコピーしてください。

  - version.dll
  - JapaneseMod\Language.json
  - JapaneseMod\LanguageTalk.json

※元のゲームファイルは一切書き換えません。
※アンインストールする場合は version.dll と JapaneseMod フォルダを削除するだけです。

【ゲーム内ショートカットキー】
  - [F5] キー : JapaneseMod フォルダ内の JSON ファイルを再読み込み（ホットリロード）
  - [F6] キー : 日本語改善Modの ON / OFF 切り替え（元のテキストとの比較用）
  - [F7] キー : 直近に表示されたテキストID一覧を JapaneseMod\recent_ids.txt に出力

【カスタム上書き機能】
JapaneseMod フォルダ内に以下のファイルを作成すると、特定のIDだけ優先的に上書きできます：
  - JapaneseMod\override_Language.json
  - JapaneseMod\override_LanguageTalk.json
  形式例: { "1000001": "セーブデータをロード" }
======================================================================
