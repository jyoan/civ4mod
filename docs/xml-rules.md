# XML の詳細（読み込み順・キャッシュ・テキスト）

CLAUDE.md の「XML」の補足。Info の読み込み順に関わるタグを足すとき、キャッシュを疑うとき、テキストを書くときに読む。

## 読み込み順

- XML は `CvXMLLoadUtility::LoadPreMenuGlobals`（`CvXMLLoadUtilitySet.cpp`）に書かれた順に読み込まれ、`FindInInfoClass` で引けるのは**先に読み込まれた Info だけ**
- 例えば志向より後に読み込まれるのは施設・建物・社会制度・指導者・ユニット・文明など、先に読み込まれるのは地形・地形の特徴・昇進・兵科など
- 後に読み込まれる Info を参照するタグは、種類を文字列のまま持っておき、`readPass3()` で番号に変える（`CvTraitInfo::readPass3` が例）
- 志向の `readPass3()` は指導者・文明より**前**に呼ばれる（`CvXMLLoadUtility::LoadPreMenuGlobals`）。指導者・文明を参照する志向のタグは、文明の読み込みの直後に呼ぶ `CvTraitInfo::readFoundingVassalTypes()` で番号に変える（2026-11-23。`readPass3()` で引くと「Tag: ... was incorrect」のエラーが出た）
- 生産の種類（`ProcessInfo`）など `LoadPostMenuGlobals`（ゲームを始めるとき）で読み込まれる Info は `readPass3()` の時点でもまだ無いので、`CvTraitInfo::readPass4()` で番号に変える（ゲームの前の画面ではその数が 0）

## 元ファイル

- `<CIV4>\Assets\XML\Civilizations\CIV4TraitInfos.xml` は 8 志向しかない古い版なので使わない。BtS(J) は自分に無いファイルを Warlords(J) から読む（例: `XML\Civilizations\CIV4TraitInfos.xml`、`TXT_KEY_UNIQUE_BUILDINGS` を含む `CIV4GameText_Warlords.xml`）。テキスト XML の多くは `<CIV4>\Assets\` にある
- 「なし」の値の失敗例: 維持費なしの社会制度に `UPKEEP_NONE` と書いて、起動時に「Tag: UPKEEP_NONE in Info class was incorrect」になった（正しくは `<Upkeep>NONE</Upkeep>`）
- 志向（`TraitInfo`）で省略できないのは `Type`・`Description`・`ShortDescription` だけ。BtS では必須だった `iHealth`〜`iMaxPlayerBuildingProductionModifier` の 11 個と `ExtraYieldThresholds`・`TradeYieldModifiers`・`CommerceChanges`・`CommerceModifiers`・`FreePromotions`・`FreePromotionUnitCombats` も、2026-11-23 に Schema で `minOccurs="0"` にした。省略すると 0 か空になる。ただし `iMaxAnarchy` は省略すると -1（上限なし）で、0 は「無政府期間なし」（精神志向）。志向の XML には、0・空・`iMaxAnarchy` の -1 を書かない

## テキスト

- テキスト XML（`XML\Text\`）は `encoding="ISO-8859-1"` 宣言の ASCII ファイル。日本語は `<Japanese>` の中に **10 進の数値文字参照**で書く（例: 「戦士」→ `&#25126;&#22763;`）。他の言語タグ（`<English>` など）も元ファイルに倣って残す
- 新しいテキストキーの置き場所（`Assets\XML\Text\`。`XML\Text\` の xml はすべて読み込まれる）:
  - 志向（`TXT_KEY_TRAIT_*`・`TXT_KEY_CONDITION_*`・`TXT_KEY_START_BIAS*`）: `CIV4GameText_ClaudeSample_Traits.xml`
  - 昇進: `_Promotions.xml`、呪文: `_Spells.xml`、検証用の項目の名前: `_Test.xml`
  - それ以外（メッセージ、建物、社会制度、ゲームオプションなど）: `CIV4GameText_ClaudeSample.xml`
  - `civ-design` ブランチ: `CIV4GameText_ClaudeCivs.xml`
- 商業・産出のアイコン（`GC.getCommerceInfo(e).getChar()` など）を引数で渡すときは `%F2` と書く（`%c2` ではアイコンが「?」になる。BtS の「建造物により%F2%D1」と同じ）
- テキストの中で同じ引数を 2 回使わない（`%s1_Feature … %s1_Feature`）。ゲームは片方しか置き換えず、もう片方が `%s1_Feature` のまま出る。2 回目はコードから同じ値を別の番号（`%s3`）で渡す
- 日本語版の用語: Trait は「〜志向」（例: `TRAIT_INDUSTRIOUS` = 勤労志向）。ペディアの見出し `TXT_KEY_PEDIA_TRAITS` は「特性」

## XML キャッシュ

- `CvCivilizationInfo` などストリーム版 `read` / `write` を持つ Info クラスは、exe が XML キャッシュ（`%LOCALAPPDATA%\My Games\Beyond the Sword(J)\cache`）経由でも読み込む。exe は `CvGlobals::read/write*InfoArray` を呼び、そこから各 Info の `read` / `write` が使われる
- これらのクラスに項目を足したら、`write` の `uiFlag` を上げ、`read` では `uiFlag` を見て古いキャッシュも読めるようにする。現在の値: `CvCivilizationInfo`=3、`CvCivicInfo`=9、`CvBuildingInfo`=18、`CvUnitInfo`=6、`CvPromotionInfo`=22、`CvTechInfo`=2、`CvImprovementInfo`=7、`CvEventTriggerInfo`=1、`CvBonusInfo`=2（上げたらここも直す）
- Info の数が変わるとキャッシュを消す理由: ユニット・建物・指導者・文明などのキャッシュは「志向の数」などの数だけ配列を読むため、XML を変えていないファイルの古いキャッシュがずれて読まれ、データ破損や強制終了の原因になる
- キャッシュを消すときは、Civ4 が起動していないことを確認してから、cache フォルダ内の `*.dat` だけを消す（フォルダは残す。次回起動時に作り直される。BtS 本体や他の Mod も次回の起動が少し遅くなるだけ）
- 観察（2026-10-31）: cache フォルダにあるのは `catalogCiv4BeyondSword*.dat`（ファイル一覧のキャッシュ）だけで、XML の Info ごとのキャッシュは見当たらなかった。Mod 実行時は XML キャッシュが使われていない可能性が高い。ただし確証はないので、`uiFlag` による形式の区別とキャッシュ削除のルールは守る
- exe がどの Info クラスを直接使っているかは `link.exe /dump /imports <BTS>\Civ4BeyondSword.exe` で確認できる（PowerShell から）。`CvCivilizationInfo` は getter 8 個だけで、コンストラクタは使っていない（メンバ追加は安全）。`CvUnitInfo`・`CvBuildingInfo`・`CvCivicInfo`・`CvTraitInfo`・`CvPromotionInfo`・`CvProjectInfo` は 1 つも使っていない（2026-10-31 確認）
