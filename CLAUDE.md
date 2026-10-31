# MyMod — Civ4 Beyond the Sword（日本語版）DLL Mod

Civilization IV: Beyond the Sword 日本語版（CyberFront 版）向けの Mod。
公式 SDK の CvGameCoreDLL を改造する。今後 XML / Python も改修する。

## フォルダ構成

| パス | 内容 |
|---|---|
| `CvGameCoreDLL/` | 公式 SDK ソース（日本語版、`CYBERFRONT` 定義）。`Boost-1.32.0/` と `Python24/` 同梱 |
| `CvGameCoreDLL/Makefile` | nmake 用。`CvGameCoreDLL.vcproj` の **Final Release**（製品版 DLL と同じ構成）を再現 |
| `build.bat` | ビルドと Mod フォルダへの配置の入口 |
| `claude_sample.ini` | Mod 設定（`ModularLoading = 0` など）。配置時に Mod フォルダへコピーされる |
| `Assets/` | Mod 本体。`CvGameCoreDLL.dll`（ビルド成果物、git 管理）。今後 `XML/`・`Python/` を追加 |

`CvGameCoreDLL.vcproj` は設定の参照用。新しい Visual Studio で開くと別のコンパイラ用に変換されるので開かない。

## ビルド

```
build.bat            差分ビルド → Assets\CvGameCoreDLL.dll にコピー → Mod フォルダへ配置
build.bat rebuild    クリーン後に全ビルド → 配置
build.bat deploy     配置だけ（ビルドしない）
build.bat clean      CvGameCoreDLL\Release\ を削除（配置はしない）
```

- 終了コード: `0` = 完了、`1` = ビルドまたは配置に失敗、`2` = ビルドは成功したが **Civ4 が起動中なので配置を飛ばした**
- **Claude は Civ4 を終了させない**（強制終了になり、最後のオートセーブ以降の進行が失われるため）。終了コード `2` のときは、ユーザーにゲームの終了を頼み、終了したと言われてから `build.bat deploy` を実行する

- 出力: `CvGameCoreDLL\Release\CvGameCoreDLL.dll`（と `.pdb`）。成功時に `Assets\CvGameCoreDLL.dll` へコピーし、続けて `<BTS>\Mods\claude_sample\` へ配置する
- XML / Python だけを変えたときも `build.bat` を実行すればよい（DLL は 0 秒で終わり、配置だけが行われる）
- Claude のツールから実行するときはフルパスで:
  - Bash: `cmd //c 'C:\claudeCode\civ4mod\build.bat'`（相対パスだと cmd が見つけられない）
  - PowerShell: `& 'C:\claudeCode\civ4mod\build.bat'`
- 全ビルド約 2 分、変更なしなら 0 秒。ヘッダを 1 つでも変えると PCH 経由で全再コンパイルになる
- build.bat はビルド後に依存 DLL を検査し、MSVCR71.dll に依存しない／MSVCR80 以降に依存する DLL は失敗扱いにしてコピーしない
- 元ソースは `/W3` で**警告 0 件**。警告が出たら自分の変更が原因
- 新しい `.cpp` を追加したら Makefile の `OBJS` に 1 行足す（ワイルドカードは使っていない）
- Debug 構成は未整備（Release のみ）。Release では `FAssert` は無効
- `Makefile` と `build.bat` は ASCII のみ・CRLF（日本語を書くと cmd / nmake で壊れる可能性がある）
- ビルド対象外: `CvTextScreens.cpp`（vcproj 未登録）、`CvGameCoreDLL.rc`（MFC の afxres.h が必要。中身はバージョン情報のみ）
- `CYBERFRONT` と `FINAL_RELEASE` の定義は外さない（日本語版 exe に合わせた構成）

## ゲーム本体と Mod の配置先

以下、`<CIV4>` = `C:\Program Files (x86)\CYBERFRONT\Sid Meier's Civilization 4(J)`、`<BTS>` = `<CIV4>\Beyond the Sword(J)`、`<WL>` = `<CIV4>\Warlords(J)`。

| パス | 内容 |
|---|---|
| `<BTS>\Assets\XML\`、`<BTS>\Assets\Python\` | BtS の元ファイル。Mod で変更するときはここからコピーする |
| `<WL>\Assets\` | Warlords(J) の元ファイル。**BtS(J) は自分に無いファイルを Warlords(J) から読む**（例: `XML\Civilizations\CIV4TraitInfos.xml`、`TXT_KEY_UNIQUE_BUILDINGS` を含む `CIV4GameText_Warlords.xml`） |
| `<CIV4>\Assets\` | 無印 Civ4(J) の元ファイル。テキスト XML の多くはここにある |
| `<BTS>\Assets\CvGameCoreDLL.dll` | 製品版 DLL。**上書きしない** |
| `<BTS>\CvGameCoreDLL\` | インストール済みの SDK。リポジトリの `2b1bf36` と同一 |
| `<BTS>\Mods\claude_sample\` | **Mod の配置先**（普通のフォルダ。ジャンクションではない） |

- 元ファイルを探す順は **`<BTS>` → `<WL>` → `<CIV4>`**。最初に見つかったものが実際に使われている。`<CIV4>\Assets\XML\Civilizations\CIV4TraitInfos.xml` は 8 志向しかない古い版なので使わない

配置（build.bat が自動で行う）:

- `Assets\` → `Mods\claude_sample\Assets\` を robocopy `/MIR` で**ミラー同期**する。リポジトリから消したファイルは Mod 側からも消える（`*EXTRA File` と表示される）
- `claude_sample.ini` → `Mods\claude_sample\claude_sample.ini` を上書きコピーする。Mod フォルダ直下のその他のファイル（`claude_sample.ini.bak` など）には触れない
- **Mod フォルダ側で直接編集しない**。編集はすべてリポジトリで行う（Mod 側の `Assets\` の変更は次の配置で消える）
- Civ4（`Civ4BeyondSword.exe`）の起動中は DLL がロックされるので、build.bat は配置の前に起動を確認し、起動中なら配置を飛ばす（終了コード `2`）。それでもファイルがロックされていた場合は `DEPLOY FAILED`（終了コード `1`）。どちらも、ゲームを終了してから `build.bat deploy` を実行する
- Civ4 は DLL を起動時に読み込むので、起動中のゲームに新しい DLL は反映されない（反映には再起動が必要）
- `<BTS>\Mods` が無い環境では警告を出して配置をスキップする（ビルドは成功扱い）
- ゲーム本体のパスと Mod 名は `build.bat` の `BTS` / `MODNAME` で定義している
- `Mods\` は Users に書き込み権限があるので、管理者権限なしで配置できる（VirtualStore へのリダイレクトも起きない）

## ツールチェーン（MSVC 7.1 固定）

Civ4 本体が MSVC 7.1 ランタイムなので、**新しい MSVC や Windows Kits のヘッダ・ライブラリは使わない**。

| ツール | 場所 | 備考 |
|---|---|---|
| cl / link | `C:\Program Files (x86)\Microsoft Visual C++ Toolkit 2003\bin` | cl 13.10.3077 / link 7.10.3077 |
| CRT のインポートライブラリ | 同 `lib\` | msvcrt.lib / msvcprt.lib / msvcrtd.lib は後から追加したもの。MSVCR71 / MSVCP71 / MSVCR71D を参照することを確認済み |
| Platform SDK | `C:\Program Files\Microsoft Platform SDK` | Windows Server 2003 SP1 版。`windows.h`・`winmm.lib` など |
| nmake / rc | `C:\Program Files\Microsoft Platform SDK\Bin` | nmake 7.00.8882 |
| dumpbin の代わり | `link.exe /dump /dependents` `/exports` `/headers` | dumpbin.exe は入っていない |

- `C:\Program Files (x86)\Microsoft Visual Studio .NET 2003` は中身が空（PSDK のインストーラが作ったフォルダ）。使わない
- ライブラリの検索順は Toolkit → PSDK（両方に `kernel32.lib`・`libcp.lib` がある）
- パスの定義は `build.bat` の 1 か所だけ。Makefile は環境変数 `TOOLKIT` / `PSDK` を受け取る（nmake を直接実行するとエラーで止まる）

## C++ のルール（C++03 / MSVC 7.1）

- 使えない: `auto`（型推論）、`nullptr`、ラムダ、range-based for、`override` / `final`、`enum class`、`static_assert`、`{}` による一様初期化と `std::initializer_list`（配列・構造体の集成体初期化は可）、`std::unique_ptr` / `std::shared_ptr` / `std::unordered_map`、可変引数マクロ（`__VA_ARGS__`）
- 入れ子テンプレートの閉じ括弧は `> >` と空白を入れる
- `for` のループ変数のスコープは非標準（`/Zc:forScope` なし）。既存コードに倣い、ループ変数は関数の先頭で宣言する（例: `int iI;`）
- **ソースは ASCII のみ**（現状すべて ASCII）。コメントも文字列も英語で書く。システムのコードページが 932 なので、UTF-8 の日本語が入ると cl 7.1 が誤解釈する
- 既存のスタイルに合わせる: ハンガリアン記法（`m_iFoo`, `eUnit`, `pCity`, `szText`）、タブインデント、関数の波括弧は改行

## マルチプレイ同期（OOS 防止）

- ゲーム状態に影響する乱数は必ず `GC.getGameINLINE().getSorenRandNum(iNum, "reason")`（0〜iNum-1 を返す）を使う。Python からは `CyGame().getSorenRandNum(iNum, "reason")`
- `rand()` / `srand()` や `GC.getASyncRand()` をゲーム状態に使わない（ASyncRand はマシンごとに値が異なる。演出・UI 専用）
- 表示用のコード（`CvGameTextMgr`、`CvDLLWidgetData` など、各マシンで別々に走る処理）からゲーム状態を変更しない。プレイヤー操作による変更は `CvMessageControl` 経由で送る

## セーブデータ

- `read(FDataStreamBase*)` / `write(FDataStreamBase*)` を持つクラス（`CvUnit`, `CvCity`, `CvPlayer` など）にメンバを追加したら、`reset()` で初期化し、`read` と `write` の両方に同じ順序で追加する
- 既存のセーブとの互換を保つ場合は、`write` の `uiFlag` を上げ、`read` で `if (uiFlag > N)` のように分岐する（既存コードと同じパターン）

## XML

タグを追加するときは、次をすべて更新する:

1. `CvInfos.h`: メンバ変数と getter
2. `CvInfos.cpp`: コンストラクタでの初期化（配列ならデストラクタで解放）と、`read(CvXMLLoadUtility* pXML)` 内の `pXML->GetChildXmlValByName(&m_iFoo, "iFoo");`
3. Schema（`Assets/XML/<分類>/CIV4*Schema.xml`）: `ElementType` の宣言と、親要素への `element` の追加
4. その Info クラスに `read(FDataStreamBase*)` / `write(FDataStreamBase*)` もある場合（Unit, Building, Tech, Promotion, Civic, Bonus など）は、そちらにも追加
5. Python から使うなら `CyInfoInterface*.cpp` で公開する

- XML 内のタグの順序は Schema の順序に合わせる。新しいタグは Schema で `minOccurs="0"` にして、既存の項目には書かなくて済むようにする
- 「なし」を表す値は型名ではなく **`NONE`** と書く（例: 維持費なしの社会制度は `<Upkeep>NONE</Upkeep>`。`UPKEEP_NONE` は存在せず、起動時に「Tag: UPKEEP_NONE in Info class was incorrect」になった）。既存の値を書き換えるときは、元の XML でその要素がどう書かれているかを先に確かめる
- 書いた名前が実在するかは、本体（`<BTS>` / `<WL>` / `<CIV4>`）と Mod の XML の `<Type>` 一覧と照合して確かめられる。ただし `ARTSTYLE_` / `CONTACT_` / `FLAVOR_` / `FORMATION_` などは別の一覧で定義されているので、`<Type>` には出てこない
- XML は `CvXMLLoadUtility::LoadPreMenuGlobals`（`CvXMLLoadUtilitySet.cpp`）に書かれた順に読み込まれ、`FindInInfoClass` で引けるのは**先に読み込まれた Info だけ**。例えば志向より後に読み込まれるのは施設・建物・社会制度・指導者・ユニット・文明など、先に読み込まれるのは地形・地形の特徴・昇進・兵科など。後に読み込まれる Info を参照するタグは、種類を文字列のまま持っておき、`readPass3()` で番号に変える（`CvTraitInfo::readPass3` が例）
- Mod の XML は本体の同名ファイルを丸ごと置き換える（`ModularLoading = 0` のため）。`<BTS>` → `<WL>` → `<CIV4>` の順に探して最初に見つかったものを、同じ相対パスで `Assets\XML\...` にコピーしてから編集する（Schema も同様）
- コピーした直後に**未改変のままコミット**し、編集は次のコミットにする（git の差分で Mod の変更だけが見えるように）
- 新しいテキストキーは `Assets\XML\Text\CIV4GameText_ClaudeSample.xml` に追加する（`XML\Text\` の xml はすべて読み込まれる）
- 元の XML はすべて ASCII・CRLF。Mod 側も ASCII・CRLF を保つ
- テキスト XML（`XML\Text\`）は `encoding="ISO-8859-1"` 宣言の ASCII ファイル。日本語は `<Japanese>` の中に **10 進の数値文字参照**で書く（例: 「戦士」→ `&#25126;&#22763;`）。他の言語タグ（`<English>` など）も元ファイルに倣って残す
- 日本語版の用語: Trait は「〜志向」（例: `TRAIT_INDUSTRIOUS` = 勤労志向）。ペディアの見出し `TXT_KEY_PEDIA_TRAITS` は「特性」

### XML キャッシュ

- `CvCivilizationInfo` などストリーム版 `read` / `write` を持つ Info クラスは、exe が XML キャッシュ（`%LOCALAPPDATA%\My Games\Beyond the Sword(J)\cache`）経由でも読み込む。exe は `CvGlobals::read/write*InfoArray` を呼び、そこから各 Info の `read` / `write` が使われる
- これらのクラスに項目を足したら、`write` の `uiFlag` を上げ、`read` では `uiFlag` を見て古いキャッシュも読めるようにする（現在 `CvCivilizationInfo` は `uiFlag=1`、`CvCivicInfo` は `uiFlag=4`、`CvBuildingInfo` は `uiFlag=2`、`CvUnitInfo` は `uiFlag=1`）
- **Info の数が変わる変更（志向・昇進・技術・ユニットなどの追加や削除）をしたら、必ずキャッシュを消す**。ユニット・建物・指導者・文明などのキャッシュは「志向の数」などの数だけ配列を読むため、XML を変えていないファイルの古いキャッシュがずれて読まれ、データ破損や強制終了の原因になる
- キャッシュを消すときは、Civ4 が起動していないことを確認してから、上記の cache フォルダ内の `*.dat` だけを消す（フォルダは残す。次回起動時に作り直される。BtS 本体や他の Mod も次回の起動が少し遅くなるだけ）
- キャッシュが原因と思われる不具合が出たときも、同じ手順で消す
- 観察（2026-10-31）: cache フォルダにあるのは `catalogCiv4BeyondSword*.dat`（ファイル一覧のキャッシュ）だけで、XML の Info ごとのキャッシュは見当たらなかった。Mod 実行時は XML キャッシュが使われていない可能性が高い。ただし確証はないので、上記の `uiFlag` による形式の区別とキャッシュ削除のルールは守る
- exe がどの Info クラスを直接使っているかは `link.exe /dump /imports <BTS>\Civ4BeyondSword.exe` で確認できる。`CvCivilizationInfo` は getter 8 個だけで、コンストラクタは使っていない（メンバ追加は安全）。`CvUnitInfo`・`CvBuildingInfo`・`CvCivicInfo`・`CvTraitInfo`・`CvPromotionInfo`・`CvProjectInfo` は 1 つも使っていない（2026-10-31 確認）
- Git Bash から `link.exe /dump` を実行すると `/dump` がパスに変換されて失敗する。PowerShell から実行する

## Python（2.4）

- Python 2.4 の構文だけを使う。`with` 文、条件式（`a if c else b`）、`except E as e`、`str.format` は使えない
- `Assets\Python\` に置いたファイルは本体の同名ファイルを置き換える。`<BTS>\Assets\Python\` から同じ相対パス（`EntryPoints\`、`Screens\` など）でコピーしてから編集する
- 元の Python はすべて ASCII・CRLF。日本語を書く必要があるなら、ファイル先頭に coding 宣言を入れる
- DLL の機能を Python に公開するには `Cy*.cpp` と `Cy*Interface*.cpp`（boost::python）に追加する
- Mod に入れている Python: `Screens\CvPediaMain.py`・`Screens\CvScreenEnums.py`・`EntryPoints\CvScreensInterface.py`（BtS(J) のコピーに追記）、`Screens\CvCivicsScreen.py`（無印 Civ4(J) のコピーに追記。BtS(J)・Warlords(J) には無く、無印のものが使われている）、`Screens\CvPediaTrait.py`（新規）
- 社会制度の画面（`CvCivicsScreen.py`）: 元は各区分の枠が 5 行分の固定の高さで、6 行目がはみ出していた。`updateLayout()` で一番行数の多い区分に合わせて上の枠を伸ばし、大きいアイコンと説明の枠を下にずらす（5 行以下なら元と同じ座標）。9 行以上は説明の枠を 170 ピクセル残すため行の間隔を詰める。`isCivicShown()` で、前提の志向（`PrereqOrTraits`）を持たない社会制度は一覧から外す（今採用中のものは表示）
- 文法チェック: Python 3.14 が入っている（`python -I`）。Python 2.4 そのものではないが、インデントや括弧の誤りは見つかる。BtS のファイルには Python 2 の `print` 文があって途中で止まるので、`print` 行を `pass` に置き換えた複製で、**編集前と編集後の両方**をチェックして比べる

### 志向のペディア

- カテゴリ「志向」（`CIVILOPEDIA_PAGE_TRAIT`、テキスト `TXT_KEY_PEDIA_CATEGORY_TRAIT`）。一覧の位置は最後（ヒントの下）。`listCategories` の位置がそのまま `CivilopediaPageTypes` の値として使われるので、途中に挟めない
- 志向のページ（`CvPediaTrait.py`）: 左に「指導者」「文明」（その志向を持つもの。クリックで移動）、右上に「生産可能」（`PrereqOrTraits` にその志向を書いたユニット・建物・社会制度のボタン）、右下に「特殊能力」（`CyGameTextMgr().parseTraits()` の先頭の名前行を除いたもの）、右端に志向の一覧
- DLL 側: `WIDGET_PEDIA_JUMP_TO_TRAIT`（クリックで Python の `pediaJumpToTrait`、マウスオーバーで志向の効果）、`CyGameTextMgr.parseTraits(iTrait, iCiv, bDawnOfMan)` を追加。列挙型はどちらも末尾に足した（既存の値は変わらない。`NUM_WIDGET_TYPES` は DLL 側だけの定義）
- **画面番号の落とし穴**: `CvScreenEnums.py` では `PEDIA_X = CIVILOPEDIA_PAGE_X + 200` だが、`PEDIA_HISTORY = PEDIA_HINTS + 1` が決め打ち。末尾に足したページ種別に + 200 すると `PEDIA_HISTORY` と同じ値になるので、`PEDIA_TRAIT = PEDIA_HISTORY + 1` にしている
- 志向の名前は説明文の中で `<link=literal>` のリンクになっている（`parseTraits` の見出し、文明の説明の志向一覧）。ペディアでクリックすると志向のページへ移動する

## Mod の設計方針

この Mod の狙いは、FFH2 のような大規模なソース改変ではなく、**XML から設定できる要素を増やすこと**。AI がその要素を使いこなせるかは考慮しない。

- 新しい効果は、**志向などの Info に専用のタグを足す方式**で実装する
- **ダミー建物（全都市に無料で置く建物）で効果を代用しない**。過去に試して次の問題があった:
  - 建設候補に出てしまう
  - 前提を研究不可の技術にしても、未来の時代から始めるゲームではその技術が与えられてしまう
  - 文明全体に効かせたい能力を首都だけに置く方式は、遷都にうまく対応できなかった
  - 建物なので、ランダムイベントやスパイ工作の対象になる
- 志向の効果のうち、ゲーム開始時の `CvPlayer::init()` で 1 回だけ適用されるもの（健康・幸福・商業・偉人率・労働者速度など）は、**既存のセーブには反映されない**。動作確認は新しいゲームで行う
- 社会制度・建物がすでに持つ「文明全体に効く数値」は、志向にも同じ名前のタグで持たせ、既存の `CvPlayer::change〜()` で適用する。説明文は社会制度のテキストキーを流用する（キーは `[ICON_BULLET]` 始まりで改行を含まないので、志向の説明ではコード側で改行と字下げ 2 文字を付ける）
- 「首都の産出 +X%」系（`CapitalYieldModifiers` など）は、計算のたびに現在の首都を参照するので、**遷都しても自動で新しい首都に移る**
- **価値がゲーム速度や開始時代で変わる値（開始時の資金など）は、固定値の XML タグにしない**。BtS 本体の開始資金は「難易度 + 開始時代」の単純な合計で、ゲーム速度の補正がない。こうした値は、将来の汎用 Python フックで計算式として書く

### この Mod で追加した XML タグ

| ファイル | タグ | 内容 |
|---|---|---|
| `CIV4CivilizationInfos.xml` | `CivilizationInfo/Traits` | 文明が持つ志向（書式は指導者の `<Traits>` と同じ）。`CvPlayer::hasTrait()` は「指導者 または 文明」が持っていれば真。両方が同じ志向を持っても効果は 1 回分 |
| `CIV4TraitInfos.xml` | `TraitInfo/iWorkerSpeedModifier` | 労働者の作業速度 %（文明全体に 1 回だけ適用） |
| `CIV4TraitInfos.xml` | `TraitInfo/FreePromotionSets` | `<FreePromotionSet>` を何個でも書ける。各組は `<FreePromotions>` と `<FreePromotionUnitCombats>`（既存と同じ書式。どちらも省略不可）を持ち、組の中の全昇進を組の中の全兵科に付与する。既存の `FreePromotions` / `FreePromotionUnitCombats` と併用可。付与はユニット生成時（既存のセーブでも新しいユニットには効く）。その兵科が本来取れる昇進かはチェックしない |
| `CIV4TraitInfos.xml` | `TraitInfo/iFreeExperience`、`iMilitaryProductionModifier`、`iImprovementUpgradeRateModifier`、`iDistanceMaintenanceModifier`、`iNumCitiesMaintenanceModifier`、`iWarWearinessModifier`、`iFreeSpecialist`、`iTradeRoutes`、`iExpInBorderModifier` | 社会制度の同名タグと同じ効果（新ユニットの経験値、軍事ユニット生産 %、小屋などの成長速度 %、距離／都市数による維持費 %、厭戦感情 %、全都市の無償の専門家、全都市の交易路、国境内の戦闘経験値 %）。文明全体に 1 回だけ適用。`iFreeExperience` は社会制度と同じく**都市で生産したユニットだけ**に付く（ゲーム開始時に配られるユニットには付かない） |
| `CIV4TraitInfos.xml` | `TraitInfo/YieldModifiers`、`CapitalYieldModifiers`、`CapitalCommerceModifiers`、`SpecialistExtraCommerces` | 社会制度の同名タグと同じ効果（全都市／首都の産出 %、首都の商業 %、専門家 1 人あたりの商業）。`<iYield>` は食料・生産・商業、`<iCommerce>` は金・研究・文化・諜報の順。文明全体に 1 回だけ適用 |
| `CIV4TraitInfos.xml`、`CIV4CivicInfos.xml` | `TraitInfo/UnitCombatFreeExperiences`、`UnitCombatProductionModifiers`（`CivicInfo` にも同名タグ） | 兵科ごとの新ユニットの経験値（建物の同名タグと同じ書式: `UnitCombatType` + `iExperience`）と、兵科ごとのユニット生産 %（`UnitCombatType` + `iProductionModifier`）。既存の `iFreeExperience` / `iMilitaryProductionModifier` に上乗せされる。`CvPlayer::getUnitCombatFreeExperience()` / `getUnitCombatProductionModifier()` が「持っている志向 + 今の社会制度」を**そのつど合計**するので、セーブデータは変わらず、社会制度の切り替えも既存のセーブもそのまま反映される。経験値は都市で生産したユニットだけ。生産の内訳では、志向の分は既存の志向の行に含め、社会制度の分は `TXT_KEY_MISC_HELP_PROD_CIVIC_UNITCOMBAT` の行で表示 |
| `CIV4TraitInfos.xml`、`CIV4CivicInfos.xml` | `TraitInfo/UnitCombatUpgradeDiscounts`（`CivicInfo` にも同名タグ） | 兵科ごとのアップグレード費用の割引率 %（`UnitCombatType` + `iUpgradeDiscount`。マイナスで割増）。**アップグレード元**のユニットの兵科で判定する。ユニットの昇進による割引と、志向・社会制度の割引を**足し算**して 1 回で引く（`CvUnit::upgradePrice`）。合計 100% 以上で無料。上と同じくそのつど合計（`CvPlayer::getUnitCombatUpgradeDiscount()`）。説明文では費用の増減として「-50%」のように表示（`TXT_KEY_UNITCOMBAT_UPGRADE_COST_MODIFIER`） |
| `CIV4TraitInfos.xml`、`CIV4CivicInfos.xml`、`CIV4BuildingInfos.xml` | `TraitInfo/UpgradeAnywhereUnitCombats`（`CivicInfo`・`BuildingInfo` にも同名タグ） | 各 `<UpgradeAnywhereUnitCombat>` は `UnitCombatType` + `bUpgradeAnywhere` + 省略可の `bExcludeEnemyTerritory`（既定 0）。その兵科（アップグレード元）は**自チームの領土外でもアップグレードできる**。`bExcludeEnemyTerritory=1` なら交戦中のチームの領土では不可。有効になるのは「その志向を持つ」「その社会制度を採用中」「その建物を（陳腐化していない状態で）どこかの都市に持つ」のいずれか。複数が許可している場合、1 つでも「敵国でも可」なら敵国でも可。緩めるのは「領土内にいること」（`CvUnit::isReadyForUpgrade`）と「最寄りの都市が同じ大陸・水域にあること」（`CvUnit::getUpgradeCity` の非探索時）の 2 点だけで、移動力・費用・最寄りの自チーム都市でアップグレード先を生産できること（海洋は沿岸都市）は従来どおり必要。読み込みは 3 つの値を持つので、共通関数 `readUpgradeAnywhereUnitCombats()`（`CvInfos.cpp`）を使う |
| `CIV4UnitInfos.xml`、`CIV4BuildingInfos.xml`、`CIV4CivicInfos.xml` | `UnitInfo/PrereqOrTraits`（`BuildingInfo`・`CivicInfo` にも同名タグ） | `<TraitType>` を何個でも書ける。**どれか 1 つの志向を持つ**プレイヤーだけが生産・建設・採用できる（`CvPlayer::hasPrereqOrTrait()`。指導者・文明どちらの志向でも可）。ユニット・建物は、志向を持たないプレイヤーの生産候補に出ない（`canTrain` / `canConstruct` の `bTestVisible` より前で判定）。社会制度は採用できず（`canDoCivics`）、社会制度の画面の一覧にも出ない（`CvCivicsScreen.py`。技術の前提を外す建物の `CivicOption` でも志向の前提は外れない）。止めるのは生産・建設・採用の判定だけで、無料で配られるもの（開始ユニット、文明の無料建物、技術の無料ユニット、不思議の無料建物、イベント）には効かない。アップグレード先と偉人による建設は生産の判定を通るので制限される。説明文は「必要条件: A または B」（既存の `TXT_KEY_REQUIRES` / `TXT_KEY_OR`。志向名はリンク）で、ペディアでは常に、ゲーム中は条件を満たさないときだけ表示（`CvGameTextMgr::buildPrereqOrTraitsString`）。そのつど判定するので既存のセーブにも効く。Schema の位置はユニット・建物が `PrereqBonuses` の後、社会制度が `TechPrereq` の後。ユニットの Schema（`CIV4UnitSchema.xml`）はこのために Mod に追加した |
| `CIV4TraitInfos.xml` | `TraitInfo/TerrainYieldChanges`、`FeatureYieldChanges` | FFH2 の文明の同名タグと同じ書式（`<TerrainYieldChange>` / `<FeatureYieldChange>` が `TerrainType` / `FeatureType` + `<Yields>`（`<iYield>` は食料・生産・商業の順））。その文明の領土の、その地形・地形の特徴のタイルの産出に加算する（`CvPlot::calculateTraitYieldChange`。`calculateYield` で施設・道路と同じ段階に足すので、金融志向の閾値や黄金時代の判定にも効く）。地形と特徴は重なる。**`TERRAIN_HILL` / `TERRAIN_PEAK` は丘陵・山岳のタイル**を指す（どちらも `bGraphicalOnly` で、タイルの地形としては使われないため。丘陵のタイルは元の地形の分と丘陵の分の両方）。通行できないタイル（山岳・氷河）は BtS では産出 0 なので、山岳の分と、通行できなくしている特徴（氷河）の分だけが効き、下の地形の分は効かない。産出があれば `CvCity::canWork` を通るので、**山岳・氷河も都市で働かせられる**。`CvPlayer::getTraitTerrainYieldChange()` / `getTraitFeatureYieldChange()` がそのつど合計するのでセーブデータは変わらないが、タイルの産出の値はセーブに保存されているので、既存のセーブではタイルが計算し直されるまで反映されない。説明文は社会制度の `TXT_KEY_CIVIC_IMPROVEMENT_YIELD_CHANGE`（「次のものより[生産]+1: 雪原、ツンドラ」）を流用し、地形と特徴を産出ごとに 1 行にまとめる |
| `CIV4TraitInfos.xml` | `TraitInfo/ImprovementYieldChanges`、`RiverPlotYieldChanges` | 社会制度の `ImprovementYieldChanges`（`ImprovementType` + `ImprovementYields`）、建物の `RiverPlotYieldChanges`（`<iYield>` の並び）と同じ名前・書式。施設の分は `CvPlot::calculateImprovementYieldChange` で施設の持ち主の志向から足す（作業の説明「この施設で +○」にも反映される）。川の分は `calculateTraitYieldChange` で、建物と同じく川沿いの通行できるタイルに足す。そのつど合計（`CvPlayer::getTraitImprovementYieldChange()` / `getTraitRiverPlotYieldChange()`）。説明文は施設を地形・特徴と同じ行にまとめ、川は建物の `TXT_KEY_BUILDING_RIVER_PLOTS`（「河川スクエア: +1[食料]」）。**施設の XML は志向より後に読み込まれる**ので、`read()` では種類を文字列のまま持ち、施設の読み込み直後に `CvXMLLoadUtility::LoadPreMenuGlobals` から呼ぶ `CvTraitInfo::readPass3()` で番号に変える（名前の誤りは XML Error のメッセージで知らせる） |

BtS 本体の修正: `CvUnit::upgradePrice` は、割引の合計が 100% を超えると費用がマイナスになり、アップグレードで金が増えていた。この Mod では費用を 0 未満にしない

この Mod で追加した志向: `TRAIT_MARITIME`（海洋志向、短縮名「海」。昇進の組: 海洋ユニット → 航海術Ⅰ、白兵戦ユニット → 水陸両用。今はどの文明・指導者も持っていない）。`CIV4TraitInfos.xml` の末尾に追加（既存の志向の番号を変えないため、新しい志向は常に末尾に足す）

### 検証用データ

**動作確認は検証用データで行い、既存の Info（日本・兵舎・専制政治など）には検証用の値を入れない。** 新しいタグを足したら、下の検証用の項目に値を入れて確かめる。検証用の項目はすべて各ファイルの末尾にあり、テキストは `CIV4GameText_ClaudeSample.xml`（説明文は共通の `TXT_KEY_CLAUDE_TEST_PEDIA`）。アイコン・グラフィックは複製元のものを使う。

| 項目 | 複製元 | 内容 |
|---|---|---|
| `CIVILIZATION_TEST`（検証用文明） | 日本 | プレイ可・**AI は選ばない**（`bAIPlayable=0`）。色 `PLAYERCOLOR_GOLD_AND_BLACK`。固有ユニット・固有建物なし。文明の志向 `TRAIT_TEST`、首都に `BUILDING_TEST` が無料、開始ユニットに `UNIT_TEST_A`。初期の社会制度は他の文明と同じ（専制政治など）で、`CIVIC_TEST` は革命で採用する（無政府 1 ターン） |
| `LEADER_TEST`（検証用指導者） | 徳川 | **指導者の志向なし**（検証の邪魔をしないため） |
| `TRAIT_TEST`（検証用志向） | — | 志向のタグをすべて持つ: 労働者速度 +50%、社会制度風の数値 9 個と配列 4 個（新ユニット経験値 +2、軍事生産 +10% など）、白兵戦に経験値 +2・生産 +25%・アップグレード割引 50%・領土外アップグレード（敵国でも可）、昇進の組（白兵戦 → `PROMOTION_TEST`、海洋 → 航海術Ⅰ）、タイルの産出（砂漠・ツンドラ・雪原に生産 +1、丘陵に食料 +1、山岳に生産 +2、氷河に食料 +1・商業 +1、森に商業 +1、農場に生産 +1、鉱山に商業 +1、川沿いに食料 +1） |
| `CIVIC_TEST`（検証用社会制度） | 専制政治 | 政治の区分。前提技術なし・無政府期間 0・維持費なし・AI 評価 -10000。白兵戦に経験値 +1・生産 +10%・アップグレード割引 25%、弓兵に領土外アップグレード（敵国の領土を除く）。前提の志向 `TRAIT_TEST` |
| `BUILDING_TEST` / `BUILDINGCLASS_TEST`（検証用建物） | 兵舎 | 前提技術なし・元の効果（攻撃志向の生産 2 倍、陸上ユニット経験値 +3）は削除・AI 評価 -10000。騎乗に領土外アップグレード。前提の志向 `TRAIT_TEST` |
| `UNIT_TEST_A` / `UNIT_TEST_B`（検証用ユニット A・B） | 戦士・斧兵 | 白兵戦。前提技術・資源なし。A は B にだけアップグレードできる（1 ターン目からアップグレード関連を試せる）。AI 評価 -10000。B だけ前提の志向 `TRAIT_TEST` または `TRAIT_AGGRESSIVE`（「どれか 1 つ」の確認用。攻撃志向の徳川の日本でも作れる）。B を作れる文明では A は生産候補に出ない（BtS の仕様: アップグレード先がすべて作れるユニットは候補から外れる。`CvCity::canTrain` → `allUpgradesAvailable`）。A のアップグレードは検証用文明の開始ユニットで試す |
| `PROMOTION_TEST`（検証用昇進） | 戦闘術Ⅰ | 戦闘力 +10%。`UnitCombats` が空なので通常のレベルアップでは誰も選べず、昇進の組でだけ付く |

検証用文明が `CIVIC_TEST` を採用したあと、白兵戦ユニット（都市で生産）で期待される値: 経験値 +5（志向 +2、白兵戦 +2、社会制度 +1）、ユニット A → B のアップグレード割引 75%（志向 50% + 社会制度 25%）、`PROMOTION_TEST` が付く

## git

- `2b1bf36` が無改造の公式ソース。改造の全差分は `git diff 2b1bf36 -- CvGameCoreDLL` で見られる
- Edit ツールは、書き換えた既存行の**行末の空白・タブを消す**ことがある。編集後は `git diff` で意図しない削除行（`-` で始まる行）がないか確認し、あれば元に戻す
- `Assets/CvGameCoreDLL.dll` も git で管理する。DLL を作り直したら、元になったソース変更と同じコミットに含める
- `core.autocrlf=false`（ファイルはそのまま保存される）。既存ファイルは CRLF なので、新しいテキストファイルも CRLF で作る
- リモートは `origin` = https://github.com/jyoan/civ4mod（非公開）。**コミットはユーザーの指示があったときに行い、そのあとの push は Claude が行ってよい**（2026-10-31 にユーザーが許可）

## 動作確認の記録

- 2026-10-31: 無改造でビルドした DLL を `Mods\claude_sample` として読み込み、ゲームが起動することを確認
- 2026-10-31: 製品版 DLL と比較。依存 DLL 7 つ・エクスポート 2432 個（名前も完全一致）・リンカ 7.10・LargeAddressAware が同じ。サイズ差 8,192 バイトはバージョン情報（`.rc`、製品版は 3.1.7.0）が無い分と考えられる
- 2026-10-31: 文明の志向（`CivilizationInfo/Traits`）と `TraitInfo/iWorkerSpeedModifier` の動作をゲーム内で確認（検証用データ: 日本に勤労志向、勤労志向に労働者の作業速度 +50%）
- 2026-10-31: `TraitInfo/FreePromotionSets` と新しい志向 `TRAIT_MARITIME` の動作をゲーム内で確認（検証用データ: 日本に海洋志向を追加。海洋ユニット → 航海術Ⅰ、白兵戦ユニット → 水陸両用）
- 2026-10-31: 社会制度と同じ効果の志向タグ 13 個（`iFreeExperience` ほか数値 9 個、`YieldModifiers` ほか配列 4 個）の表示と効果をゲーム内で確認（検証用データ: 海洋志向に全タグを設定。値は検証用）
- 2026-10-31: `UnitCombatFreeExperiences` / `UnitCombatProductionModifiers` の志向側の動作をゲーム内で確認（検証用データ: 海洋志向で海洋ユニットに経験値 +2・生産 +25%）。社会制度側は検証用データなし（編集した Schema で元の `CIV4CivicInfos.xml` が読み込めることは確認済み）
- 2026-10-31: build.bat の「Civ4 起動中は配置を飛ばす（終了コード 2）」と `build.bat deploy` を、起動確認の対象を `explorer.exe` に差し替えた複製で確認
- 2026-10-31: `UnitCombatUpgradeDiscounts` の志向側の動作（表示・アップグレード費用・実際に引かれる金額）をゲーム内で確認（検証用データ: 海洋志向で海洋ユニット -50%）
- 2026-10-31: `UpgradeAnywhereUnitCombats`（第 1 段階: 志向・社会制度）の動作をゲーム内で確認（検証用データ: 海洋志向で海洋ユニット・敵国でも可、専制政治で白兵戦・騎乗ユニット・敵国の領土を除く）
- 2026-10-31: `UpgradeAnywhereUnitCombats`（第 2 段階: 建物）の動作をゲーム内で確認（検証用データ: 兵舎で弓兵ユニット・敵国でも可。兵舎の有無で領土外のアップグレード可否が切り替わる）
- 2026-10-31: 検証用データを専用の項目（検証用の文明・指導者・志向・社会制度・建物・ユニット・昇進）に移し、日本・海洋志向・勤労志向・専制政治・兵舎の検証用の値をリセット。XML エラーなく起動することを確認（最初は `UPKEEP_NONE` で起動時エラー → `NONE` に修正）。検証用文明でゲーム内も確認済み（開始ユニットに検証用昇進、ユニット A → B のアップグレード 75% 引き、首都で生産したユニットの経験値 5）
- 2026-10-31: 志向のペディア（カテゴリ「志向」、志向のページ、志向名のリンク）をゲーム内で確認
- 2026-10-31: `PrereqOrTraits`（ユニット・建物・社会制度）と社会制度の画面の改修をゲーム内で確認（検証用文明・日本・攻撃志向でない文明。ユニット B は日本でも作れ、攻撃志向でない文明では作れない。検証用建物・検証用社会制度は日本では出ない。社会制度の画面は検証用文明で 6 行、日本で 5 行）
- 2026-10-31: `TerrainYieldChanges` / `FeatureYieldChanges`（丘陵・山岳・氷河を含む）の説明文とタイルの産出をゲーム内で確認（検証用文明）
- 2026-10-31: `ImprovementYieldChanges` / `RiverPlotYieldChanges`（志向）の説明文・施設と川沿いのタイルの産出・作業ボタンの説明をゲーム内で確認（検証用文明）
