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
- Mod に入れている Python: `Screens\CvPediaMain.py`・`Screens\CvScreenEnums.py`・`EntryPoints\CvScreensInterface.py`（BtS(J) のコピーに追記）、`Screens\CvCivicsScreen.py`（無印 Civ4(J) のコピーに追記。BtS(J)・Warlords(J) には無く、無印のものが使われている）、`Screens\CvPediaTrait.py`・`Screens\CvPediaSpell.py`・`EntryPoints\CvSpellInterface.py`（新規）
- 社会制度の画面（`CvCivicsScreen.py`）: 元は各区分の枠が 5 行分の固定の高さで、6 行目がはみ出していた。`updateLayout()` で一番行数の多い区分に合わせて上の枠を伸ばし、大きいアイコンと説明の枠を下にずらす（5 行以下なら元と同じ座標）。9 行以上は説明の枠を 170 ピクセル残すため行の間隔を詰める。`isCivicShown()` で、前提の志向（`PrereqOrTraits`）を持たない社会制度は一覧から外す（今採用中のものは表示）
- 文法チェック: Python 3.14 が入っている（`python -I`）。Python 2.4 そのものではないが、インデントや括弧の誤りは見つかる。BtS のファイルには Python 2 の `print` 文があって途中で止まるので、`print` 行を `pass` に置き換えた複製で、**編集前と編集後の両方**をチェックして比べる

### 志向のペディア

- カテゴリ「志向」（`CIVILOPEDIA_PAGE_TRAIT`、テキスト `TXT_KEY_PEDIA_CATEGORY_TRAIT`）。一覧の位置は最後（ヒントの下）。`listCategories` の位置がそのまま `CivilopediaPageTypes` の値として使われるので、途中に挟めない
- 志向のページ（`CvPediaTrait.py`）: 左に「指導者」「文明」（その志向を持つもの。クリックで移動）、右上に「生産可能」（`PrereqOrTraits` にその志向を書いたユニット・建物・社会制度のボタン）、右下に「特殊能力」（`CyGameTextMgr().parseTraits()` の先頭の名前行を除いたもの）、右端に志向の一覧
- DLL 側: `WIDGET_PEDIA_JUMP_TO_TRAIT`（クリックで Python の `pediaJumpToTrait`、マウスオーバーで志向の効果）、`CyGameTextMgr.parseTraits(iTrait, iCiv, bDawnOfMan)` を追加。列挙型はどちらも末尾に足した（既存の値は変わらない。`NUM_WIDGET_TYPES` は DLL 側だけの定義）
- **画面番号の落とし穴**: `CvScreenEnums.py` では `PEDIA_X = CIVILOPEDIA_PAGE_X + 200` だが、`PEDIA_HISTORY = PEDIA_HINTS + 1` が決め打ち。末尾に足したページ種別に + 200 すると `PEDIA_HISTORY` と同じ値になるので、`PEDIA_TRAIT = PEDIA_HISTORY + 1` にしている（呪文のペディアは `PEDIA_SPELL = PEDIA_TRAIT + 1`）
- 志向の名前は説明文の中で `<link=literal>` のリンクになっている（`parseTraits` の見出し、文明の説明の志向一覧）。ペディアでクリックすると志向のページへ移動する

### 都市圏の半径

BtS は全都市が半径 2（21 タイル）固定。この Mod では都市ごとに半径を持つ（既定 2、最大 3）。FFH2 の `bSprawling` と Denev さんの 056Patch12 の実装を参考にした（第 1 段階は土台だけで、半径を変えるタグはまだない）。

- `CvDefines.h`: `NUM_CITY_PLOTS` / `CITY_PLOTS_RADIUS` / `CITY_PLOTS_DIAMETER` は**わざと消した**（使う箇所をすべて見直すため）。代わりに `CITY_PLOTS_DEFAULT_RADIUS`（2）、`CITY_PLOTS_MAX_RADIUS`（3）、`NUM_DEFAULT_CITY_PLOTS`（21）、`NUM_MAX_CITY_PLOTS`（37）、`CITY_PLOTS_MAX_DIAMETER`（7）。タイル数は `calculateNumCityPlots(半径)`（`CvGameCoreUtils.h`。1: 9、2: 21、3: 37）
- 都市圏のタイル番号は「中心 → 1 周目（1〜8）→ 2 周目（9〜20）→ 3 周目（21〜36）」の順で、最初の 21 個は BtS と同じ。**半径 r の都市は番号 `calculateNumCityPlots(r)` 未満のタイルを使う**。表（`CvGlobals` の `aiCityPlotX` / `Y` / `Priority`、`aaiXYCityPlot` 7×7）は FFH2 と同じ並び
- `CvCity::getPlotRadius()` / `getNumCityPlots()`（Python にも公開）。`getCityIndexPlot(i)` は半径の外の番号なら NULL、`getCityPlotIndex(pPlot)`（`plotCityXY(pCity, pPlot)`）は半径の外なら -1。`plotCityXY(dx, dy)` と `plotCity(x, y, i)` は最大半径までの位置の変換だけ
- 新しく足すループの書き分け:
  - その都市自身のタイル: `for (i = 0; i < pCity->getNumCityPlots(); i++)`
  - あるタイルを働かせられる都市を探す（`CvPlot::updateWorkingCity`、`calculateCulturalOwner`）: `NUM_MAX_CITY_PLOTS` まで探し、`i < pLoopCity->getNumCityPlots()` を確かめる（番号の周回はタイルから見ても都市から見ても同じ）
  - まだない都市を評価する（AI の候補地 `AI_foundValue` など）: `CvPlayer::getNextCityRadius()`（その文明の新しい都市の半径）
  - マップ生成・開始位置の調整（`CvGame::normalize*`、`CvPlayer::addFreeUnit` / `startingPlotRange` / `findStartingArea`）: 既定の半径のまま（乱数の引き方が変わるとマップが変わるため）
- 「どれかの都市が働かせられるか」（`CvPlot::updatePotentialCityWork`。偽ならタイルの産出は 0）は `GC.getMaxCityPlotRadius()`（XML で都市が取りうる最大の半径。半径を広げるタグが無ければ 2）で判定する。文化の広がり（`CvCity::doPlotCulture` の `isPotentialCityWorkForArea`）は文化を広げる都市自身の半径で判定する。最大の半径（3）で判定すると、半径 3 の都市がいなくても国境が海へ広がってしまうため（056 はそうなっている）
- セーブ: `CvCity`（`uiFlag=1`）は半径と 37 個分の働き先を、`CvCityAI`（`uiFlag=1`）は 37 個分の最善の改善を保存する。`uiFlag=0`（この変更より前のセーブ）は 21 個分を読み、半径は 2
- exe が使う都市圏の関数は、都市画面のクリック（`CvGame::handleCityScreenPlotPicked` など）だけで、表（`getXYCityPlot` など）や `getNUM_CITY_PLOTS` は使っていない（`link.exe /dump /imports` で確認）。都市圏の表示も DLL が決めるので、変更は DLL だけで済む。BtS の Python も都市圏のタイルを扱っていない
- 半径の決まり方（第 2 段階）: 新しい都市は `CvCity::init` で持ち主の `CvPlayer::getNextCityRadius()`（既定 2 + 持っている志向の `iCityRadiusChange` の合計、1〜3）になる（`setPlotCity` が都市圏のタイルを数える前に設定する）。占領した都市も新しい持ち主の半径になる。既存の都市は `CvCity::doTurn` の最初の `updatePlotRadius()` で持ち主の半径に合わせる（既存のセーブの都市も次のターンから広がる）
- `CvCity::setPlotRadius()`: 狭まるときは先に外れるタイルの市民を外し、増減した輪のタイルの都市圏の数（`changeCityRadiusCount` / `changePlayerCityRadiusCount`）を直し、都市圏の全タイルで働かせる都市を決め直す（`updateWorkingCity`）。056 は狭まるときのループの上限に半径の値をそのまま使っていて誤り。`CvCity::setWorkingPlot(CvPlot*)` は半径の外のタイル（番号 -1）なら何もしない（狭まった直後に `updateWorkingCity` から呼ばれるため）
- 建物の `iPlotRadius`（第 3 段階）: 都市の半径は「持ち主の半径」と「その都市の陳腐化していない建物の `iPlotRadius`」の大きい方（`CvCity::updatePlotRadius`）。建物ができた・無くなった・陳腐化したときは `CvCity::processBuilding` の最後で計算し直す（建物の数と陳腐化の状態は更新済み）。FFH2 は「半径がすでに同じか広い都市では建てられない」「建物が無くなると半径 2 に戻す」だが、この Mod では建設を止めず（遺産などほかの効果のため）、半径は毎回計算する
- `GC.getMaxCityPlotRadius()` は `LoadPreMenuGlobals` の最後に、既定 2 + 全志向の正の `iCityRadiusChange` の合計と、全建物の `iPlotRadius` の大きい方（最大 3）で決まる。検証用志向が +1 なので、この Mod では 3（陸から 3 タイル離れた海のタイルにも産出が表示される）
- 3 周目のタイルも、BtS と同じく自分の国境の中でないと働かせられない。国境は文化レベル 3（標準速度で文化 100）で 3 タイルまで広がる
- 都市画面のカメラ: exe は都市に寄るとき、GlobalDefines の `CAMERA_CITY_ZOOM_IN_DISTANCE`（BtS は 3500。FFH2 / OAC は全都市 4250）を使う。この Mod では、都市を選んだとき（`CvGame::selectCity` と、exe が呼ぶ `CvCity::updateSelectedCity`）に `CvCity::updateCityScreenCamera()` がこの値を書き換える。3 周目に働かせられるタイル（`getWorkingCity() == this`）がある都市は `CAMERA_CITY_ZOOM_IN_DISTANCE_RADIUS_3`（Mod の `GlobalDefinesAlt.xml`、4500）、それ以外は XML の元の値（`LoadPreMenuGlobals` の最後で `CAMERA_CITY_ZOOM_IN_DISTANCE_DEFAULT` に控える）。カメラは各マシンの表示だけなので、マルチプレイの同期には影響しない
- `GlobalDefinesAlt.xml` は無印 Civ4(J) のもの（ダミーの値 1 つだけ）をコピーした。BtS は `GlobalDefines.xml` の後にこれを読むので、Mod の設定値の追加・上書きはここに書く（大きな `GlobalDefines.xml` をコピーしなくて済む）

### 呪文

FFH2 の呪文（spell）の仕組みを移植した。タグ・ファイル・関数の名前は FFH2 と同じ。使えるのはユニットだけ。第 1 段階は土台と基本の条件、第 2 段階 a は XML だけで書ける効果のうち、術者自身・今いるタイル・今いる都市へのもの、第 2 段階 b は周りのユニットへのもの、第 2 段階 c は召喚・失敗・世界呪文（1 ゲーム 1 回）・遅延、第 3 段階は AI、第 4 段階はペディア。

- XML: `Units\CIV4SpellInfos.xml`。Schema は FFH2 と同じく別ファイルの `Units\CIV4SpellSchema.xml`。昇進・ユニット・技術・文明を参照するので、`LoadPreMenuGlobals` で文明の後に読み込む。`CvSpellInfo` はストリーム版の `read` / `write` を持たない（キャッシュを使わない）
- 行動ボタン: `SetGlobalActionInfo` が呪文ごとに `ACTIONSUBTYPE_SPELL` の行動を作る。コマンドは `COMMAND_CAST`、データは呪文の番号。ボタンを押すと、既存のコマンドのネットメッセージ（`GAMEMESSAGE_DO_COMMAND`）で全マシンの `CvUnit::doCommand` → `cast` が呼ばれる（マルチプレイでも同期する）。選択中のユニットはすべて実行する
- `COMMAND_CAST` は `CvEnums.h` の `CommandTypes` の末尾（`COMMAND_HOTKEY` の後）と、`CIV4CommandInfos.xml`（無印 Civ4(J) のコピー）の末尾に足した。**XML の並びは列挙型と同じでなければならない**。`<Help/>` は空にしてある（中身があると、全呪文の説明に同じ文が付く）
- `CvUnit::canCast(iSpell, bTestVisible)` の判定順（FFH2 と同じ）:
  1. 移動できないタイマー（`getImmobileTimer`）。AI のユニットは `bAllowAI` の呪文だけ（FFH2 と同じ）
  2. ユニットの条件: `PromotionPrereq1` / `2`・`UnitClassPrereq`・`UnitPrereq`・`UnitCombatPrereq`
  3. 文明・都市・タイルの条件: `BuildingPrereq`（今いる都市にその建物）・`BuildingClassOwnedPrereq`（自国のどこかにその建物）・`CivilizationPrereq`・`CorporationPrereq`（今いる都市にその企業）・`ImprovementPrereq`（今いるタイルの施設）・`StateReligionPrereq`・`TechPrereq`。`ConvertUnitType` の呪文は、すでにその種類のユニットなら使えない。`bGlobal`（世界呪文をまだ使っていない）。`iCasterMinLevel`。`bCasterNoDuration`（召喚した、いずれ消えるユニットでない）
  4. `PromotionInStackPrereq` / `UnitInStackPrereq`: 同じタイルに、その昇進を持つ・その種類の、自国のほかのユニットがいる（術者自身がその昇進を持つ・その種類だと使えない。FFH2 と同じ）
  5. ボタンを出すかの判定（`bTestVisible`）で `bDisplayWhenDisabled` なら、ここで「出す」と決まる。この後の条件を満たさないと無効のボタンになる。`bDisplayWhenDisabled` でない呪文は、使えないときはボタン自体が出ない
  6. `FeatureOrPrereq1` / `2`（今いるタイルの地形の特徴。どちらか）
  7. `bHasCasted`: `bIgnoreHasCasted` でなければ、このターンに実行済みのユニットは使えない
  8. `bAdjacentToWaterOnly`（周りの 8 タイルに水のタイル（海・湖）がある。川・オアシスは含まない。`CvPlot::isAdjacentToWater` は FFH2 と同じものを足した）・`bInBordersOnly`（自分の領土）・`bInCityOnly`（都市。他国の都市でも可）
  9. 効果の前提: `iChangePopulation` は自分の都市で、人口が 1 以上残ること（FFH2 はどの都市でも可）。`iCost` が正なら資金が足りること。`bRemoveHasCasted` は実行済みのユニットだけ（召喚した、いずれ消えるユニットは不可。FFH2 と同じ）。`CreateUnitType` は召喚できること・`CreateBuildingType` は建てられること（下記）
  10. `PyRequirement` があれば、その結果で決まる
  11. なければ、効果のどれかがここで効くときだけ使える（効果のない呪文は使えない）: 召喚、昇進の付与（まだ持っていない）・除去（持っている）、変身、建物、地形の特徴・施設・宗教（作れる・広められる）、人口の増加、資金を得る（`iCost` が負。数えるのはこの Mod の追加）、ダメージ・移動不能（効く相手がいる。FFH2 のダメージは相手がいなくても使える）、`bRemoveHasCasted`、`PyResult`
- `CvUnit::cast` の順（FFH2 と同じ）: 実行済みにする（`bHasCasted`）→ 世界呪文を使用済みにする（`bGlobal`）→ 失敗（`iMiscastChance`。失敗したらここで終わり）→ 資金の消費（`iCost` が正。この Mod では効果の前に払う）→ 遅延（`iDelay`。あればここで止め、遅延が終わったときに続きを実行）→ ここから `CvUnit::castEffects`: 召喚 → 昇進の付与・除去 → 変身 → 建物 → 地形の特徴 → 施設 → 宗教 → ダメージ → 移動不能 → 実行済みの取り消し（`bRemoveHasCasted`）→ 資金を得る（`iCost` が負）→ 人口 → 演出（`Effect`・`Sound`）とメッセージ（呪文の名前とボタン）→ `PyResult` → 術者が消える（`bSacrificeCaster`、遅延）
- 効果の細部（第 2 段階 a）:
  - `AddPromotionType1`〜`3` / `RemovePromotionType1`〜`3` は、`bBuffCasterOnly=1` なら術者自身、0 なら `iRange` の範囲のユニット（下記）
  - `ConvertUnitType`: 新しいユニットを作り、`CvUnit::convert` で経験値・昇進・実行済み・積み荷を引き継ぐ。新しいユニットはそのターン動けない（移動できないタイマー 1）。変身より後の効果と `PyResult` は元のユニット（遅延で消える）に対して働く
  - `CreateBuildingType`: 自分の都市だけ（FFH2 はどの都市でも可）。すでにその建物がある都市では、呪文全体が使えない。世界遺産などの数の制限は見ない
  - `CreateFeatureType`: 都市でなく、他チームの領土でなく、地形の特徴も施設もないタイル（FFH2 は施設があっても、文明が地形の特徴を保つなら可）。地形との組み合わせは `canHaveFeature`
  - `CreateImprovementType`: 都市でなく、他チームの領土でなく、施設のないタイル（FFH2 は他国の領土でも可）。`canHaveImprovement`
  - `SpreadReligion`: 今いる都市（他国の都市でも可。まだ創始されていない宗教でも広まる。FFH2 と同じ）
  - `iCost`: 正なら消費、負なら得る
  - `Effect`（`CIV4EffectInfos.xml` の `EFFECT_`）と `Sound`（`AS2D_` などのスクリプト）は、タイルが見えているマシンでだけ出る（演出なので向きの乱数は `ASyncRand`）
- 周りのユニットへの効果（第 2 段階 b）:
  - 範囲: 術者から縦横 `iRange` タイル以内の正方形（FFH2 と同じ。0 なら術者のタイルだけ）。昇進の付与・除去は術者のタイル（術者自身を含む）も含み、ダメージと移動不能は術者のタイルを除く（FFH2 と同じ）。対象は `CvUnit::getSpellTargets` で先に ID で集めてから効果を与える（輸送ユニットが倒れると積み荷がその場で消えるため）
  - 対象にならないユニット（`CvUnit::isImmuneToSpell`）: `bImmuneTeam` なら自チーム、`bImmuneEnemy` なら交戦中の相手、`bImmuneFlying` なら航空ユニット（FFH2 の飛行ユニットの代わり）。**交戦中でない他チームのユニットは常に対象外**（FFH2 の `bImmuneNeutral` が常に有効な扱い。FFH2 は中立のユニットにダメージを与えると宣戦する。`bImmuneNeutral`・`bCausesWar` は入れていない）。私掠船など国籍を隠すユニットは、BtS の戦闘と同じく交戦中の扱い（`CvUnit::isEnemy`）。他チームのユニットは、術者のチームから見えているタイルにいて、見えているもの（潜水艦などの見えないユニットは除く）だけ
  - 昇進の付与は兵科のあるユニットだけ（FFH2 と同じ。開拓者・労働者には付かない）。昇進の `UnitCombats` は見ない（この Mod のほかの昇進のタグと同じ。FFH2 は見る）。除去は兵科を問わない
  - `iDamage`: 対象ごとに `iDamage` の 50〜149%（`iDamage / 2 + 乱数(iDamage)`）のダメージ。ただし `iDamageLimit`（省略時 100）を超えない。100 に達すると倒れる（倒したのは術者の持ち主）。FFH2 の、防御力・レベル・都市の防御・ダメージの種類（`DamageType`）による軽減は入れていない。メッセージは両方の持ち主に出る（`TXT_KEY_MESSAGE_SPELL_DAMAGED` / `KILLED`）
  - `iImmobileTurns`: まだ移動できないタイマーが 0 の対象を、**その持ち主のターンで数えて** ○ ターン動けなくする（今がその持ち主のターンなら、そのターンを含む）。タイマーは持ち主のターンの開始時に減るので、持ち主のターンがまだ来ていなければ 1 多く設定する。FFH2 はそのまま設定するので、1 だと後で手番の来る相手には効かない。動けないユニットも防御はする。ユニットの説明に「移動不能」（`TXT_KEY_UNIT_IMMOBILE`）と出る
  - 説明文: 範囲の昇進・ダメージ・移動不能の行と、対象外の行（自チーム・交戦中の相手・交戦中でない他国（常に表示）・航空ユニット）
- 召喚・失敗・世界呪文・遅延（第 2 段階 c）:
  - `CreateUnitType` を `iCreateUnitNum`（省略時 1。FFH2 は 0 で、召喚の呪文には必ず書いてある）体、術者のタイルに作る（`CvUnit::castCreateUnit`。1 体ずつ `canCreateUnit` を確かめる）。召喚できない（`CvUnit::canCreateUnit`）: 消えるユニットが自分と同じ種類を召喚する（FFH2 と同じ）・術者のタイルに見えている敵がいる（FFH2 と同じ）・そのユニットがいられないタイル（陸のユニットを水上に、海のユニットを陸に。都市・砦は可。この Mod の追加）・ユニットクラスの数の上限（国家・チーム・世界。この Mod の追加）
  - `bPermanentUnitCreate` でなければ、召喚したユニットは `1 + 志向の iSummonDuration の合計`（`CvPlayer::getSummonDuration`）ターンで消える（`CvUnit::m_iDuration`。持ち主のターンの開始時に `CvUnit::doTurn` で 1 減り、0 で消える）。召喚したターンは動ける。消えるユニットは: 維持費がかからない（`CvPlayer::calculateUnitCost` で無料のユニットに数える。FFH2 と同じ）、倒されても厭戦感情・戦争の成果にならない（FFH2 と同じ）、捕獲されない（BtS の非戦闘ユニットの捕獲と、この Mod の `iCombatCapturePercent`）、消えるユニットが `iCombatCapturePercent` で捕獲したユニットは同じターン数で消える（FFH2 と同じ）、アップグレード・変身しても残りのターン数を引き継ぐ（`CvUnit::convert`）、`bRemoveHasCasted` と `bCasterNoDuration` の呪文を使えない。領土外の維持費（`NumOutsideUnits`）は FFH2 と違い普通のユニットと同じ。ユニットの説明に「あと○ターンで消える」（`TXT_KEY_UNIT_DURATION`）
  - `bPermanentUnitCreate`: 消えない。召喚したターンは動けない（FFH2 と同じ）。数の上限は「呪文のユニットの条件（昇進・ユニット・ユニットクラス・兵科）を満たす自国のユニットの数 > そのユニットクラスの数」（術者になれるユニット 1 体につき 1 体。FFH2 は昇進と宗教で数える）
  - `bCopyCastersPromotions`: 術者の昇進をすべて付ける（大将軍の指導者の昇進（`isLeader`）を除く。FFH2 は装備・種族などを除く）。`CreateUnitPromotion`: その昇進を付ける。どちらも昇進の `UnitCombats` は見ない
  - `iMiscastChance`: ○% で失敗する（乱数は `getSorenRandNum`）。失敗すると効果はなく、資金も消費しない。実行済みにはなり、世界呪文は使用済みになる（FFH2 と同じ）。`PyMiscast` があれば、失敗したときに `CvSpellInterface.miscast` が `eval` する。メッセージ `TXT_KEY_MESSAGE_SPELL_MISCAST`。FFH2 のユニットの `iMiscastChance` は入れていない
  - `bGlobal`（世界呪文）: 各プレイヤーが 1 ゲームに 1 回。世界呪文すべてで合わせて 1 回（FFH2 と同じ。`FEAT_GLOBAL_SPELL` を `FeatTypes` の末尾に足した）。ボタンを押すと確認のポップアップが出る（`CvActionInfo::isConfirmCommand` が世界呪文で真になり、BtS の `BUTTONPOPUP_CONFIRMCOMMAND` を使う。文は `TXT_KEY_POPUP_CONFIRM_CAST_GLOBAL`）。実行すると全プレイヤーにメッセージ（出会っていなければ「未知の指導者」）。FFH2 のゲームオプション「世界呪文なし」は入れていない
  - `iDelay`: 術者が ○ ターン動けなくなり（移動できないタイマー）、効果は ○ ターン後の持ち主のターンの開始時に出る（`CvUnit::m_iDelayedSpell`）。効果が出るのは `CvPlayer::doTurnUnits` で全ユニットのターンの開始の処理が終わった後（召喚したユニットがそのターンに `doTurn` を受けて、すぐ消えたりしないため）。効果はそのときに、一つずつ効くかを確かめ直す（建物がすでにある、など）。資金は実行したときに払い、遅延の間に術者が倒されると戻らない。FFH2 は遅延の後にもう一度失敗の判定と世界呪文のメッセージを行うが、この Mod では 1 回だけ。ユニットの説明に「実行中: ○（残り○ターン）」（`TXT_KEY_UNIT_HELP_DELAYED_CASTING`）
  - 説明文: 世界呪文（先頭）、`bCasterNoDuration`（前提）、召喚の行（何ターンで消えるかは今のプレイヤーの志向で計算）、遅延・失敗率（費用の後）
  - セーブ: `CvUnit` の `uiFlag=4`（召喚の残りターンと遅延中の呪文。3 以下のセーブは召喚なし・遅延なし）、`CvPlayer` の `uiFlag=2`（世界呪文の使用済み。1 以下のセーブは未使用）
- 実行済み（`m_bHasCasted`）はターンの開始時（`CvUnit::doTurn`）に戻り、アップグレードでは引き継ぐ（`CvUnit::convert`）。ユニットの説明に「実行済み」（`TXT_KEY_UNIT_HAS_CASTED`）と出る。セーブは `CvUnit` の `uiFlag=3`（2 以下のセーブは実行済みでない扱い）
- Python: `EntryPoints\CvSpellInterface.py`（`CvDefines.h` の `PYSpellModule`）。DLL は `canCast([pCaster, eSpell])` / `cast([pCaster, eSpell])` を呼び、それぞれが `PyRequirement` / `PyResult` の文字列を `eval` する（FFH2 0.41 と同じ。インストール済みの FFH2（Denev さんの版）は関数名だけを書く別の方式）。式の中では `pCaster`（CyUnit）と `eSpell` が使えるので、引数つきの共通関数を XML から呼べる（例: `spellGainGold(pCaster, 25)`）。今ある共通関数は `reqNotHasPromotion`・`spellAddPromotion`・`spellGainGold`・`spellDamageCaster`。失敗したときは `miscast([pCaster, eSpell])` が `PyMiscast` を `eval` する
- `PyRequirement` はボタンを描くたびに各マシンで別々に呼ばれるので、ゲーム状態を変えない。`PyResult` で術者を消すときは遅延（`pCaster.kill(True, -1)`）にする（`cast` の後で `doCommand` がまだユニットを使うため）
- 説明文（`CvGameTextMgr::parseSpellHelp`。行動ボタンのマウスオーバーとペディア）: 世界呪文 → 前提（文明・技術・国教・ユニット・兵科・ユニットクラス・昇進・レベル・建物・企業・地形の特徴・施設・同じタイルのユニット・召喚したユニットでない）→ 効果 → `Help` → 場所の条件 → 実行済みの取り消し・費用・遅延・失敗率・術者が消える。見出しは「実行 (呪文名)」（`TXT_KEY_COMMAND_CAST`）。テキストキーは、FFH2 にあるものは同じ名前（`TXT_KEY_SPELL_*`）。Python の効果は自動では説明されないので、`Help` に書く
- Python への公開: `gc.getNumSpellInfos()` / `gc.getSpellInfo(i)`（`CvSpellInfo` の getter）、`CyUnit.canCast` / `cast`（`canCast` の判定を通さない）/ `isHasCasted` / `setHasCasted` / `getDuration` / `setDuration` / `changeDuration` / `getDelayedSpell`、`CyPlayer.getSummonDuration`、`FeatTypes.FEAT_GLOBAL_SPELL`、`CommandTypes.COMMAND_CAST`、`ActionSubTypes.ACTIONSUBTYPE_SPELL`、`SpellTypes.NO_SPELL`、`CyGameTextMgr.getSpellHelp(iSpell, bCivilopediaText)`、`WidgetTypes.WIDGET_PEDIA_JUMP_TO_SPELL`、`CivilopediaPageTypes.CIVILOPEDIA_PAGE_SPELL`
- ペディア（第 4 段階）:
  - カテゴリ「呪文」（`CIVILOPEDIA_PAGE_SPELL`、テキスト `TXT_KEY_PEDIA_CATEGORY_SPELL`。名前は FFH2 と同じ）。一覧の位置は「志向」の下（最後）。一覧は呪文のボタンと名前の表
  - 呪文のページ（`CvPediaSpell.py`。昇進のページと同じ配置）: 左上に呪文のボタン、右上に「必要条件」（文明・技術・国教・ユニット・兵科・ユニットクラス・昇進・建物・企業・地形の特徴・施設・同じタイルの昇進とユニットのボタン。説明文と同じ順。ユニットクラス・建物クラスは呪文の `CivilizationPrereq` の文明、なければ今のプレイヤーの文明のもの、なければ既定のもの）、その下に「効果」（召喚するユニットとその昇進・与える昇進・変身先のユニット・建物・地形の特徴・施設・宗教のボタン。外す昇進は説明文だけ）、右下に「特殊能力」（行動ボタンと同じ説明文から見出しを除いたもの）、左下に「歴史」（`Civilopedia`）、右端に呪文の一覧
  - DLL 側: `WIDGET_PEDIA_JUMP_TO_SPELL`（クリックで Python の `pediaJumpToSpell`、マウスオーバーで呪文の名前と説明文。昇進と同じく `iData2` が 0 なら説明を出さない（右端の一覧））、`CvGameTextMgr::setSpellHelp`（`setPromotionHelp` と同じく、ペディア用でなければ名前を先頭に付ける）、`CyGameTextMgr.getSpellHelp`。列挙型はどちらも末尾（志向の後）に足した。説明文の中の `[LINK=literal]` の呪文名もペディアで呪文のページへ移動する（`CvPediaMain.link`）
  - 召喚したユニットが消えるまでのターン数は、今のプレイヤーの志向で計算する（ゲームのプレイヤーがいないとき（メインメニューから開いたペディア）は 1）
  - 昇進・ユニットのページから呪文へのリンク（FFH2 の昇進のページの「呪文」）は入れていない（BtS の `CvPediaPromotion.py` などを Mod にコピーする必要がある）
- AI（第 3 段階）:
  - AI（蛮族を含む）は `bAllowAI`（省略時 0）の呪文だけを使う（FFH2 と同じ）。ユニットごとに、使える呪文のうち評価値が最大で 0 より大きいものを 1 つ実行する（`CvUnitAI::AI_chooseSpell`）。実行するのは自分のターンの初め（`CvPlayerAI::AI_doTurnUnitsPost` の最後。遅延の呪文・昇進・アップグレードの後）と終わり（`CvGame::updateMoves` で全ユニットが動き終わった後。FFH2 はここだけ）の 2 回（`CvPlayerAI::AI_doSpells`）。ネットメッセージを通さず `cast` を直接呼ぶ（FFH2 と同じ。AI の処理は全マシンで同じように動くので同期する）。世界呪文の確認のポップアップは出ない
  - 評価値（`CvUnitAI::AI_spellValue`）は FFH2 の `chooseSpell` の係数に `iAIWeight` を足したもの: 召喚は、召喚するユニットの移動力の範囲に見えている敵がいるときだけ「戦闘力 × 100 × 体数」（`bPermanentUnitCreate` でも同じ）。昇進は術者から見た昇進の評価（BtS の `AI_promotionValue`）。ダメージは敵 1 体につき「ダメージ × 10」（`iDamageLimit` に達した敵は 0）、自チーム 1 体につき「ダメージ × -20」。移動不能は動ける敵 1 体につき「ターン数 × 20」（自チームは -20）。変身は新旧のユニットの評価の差（`AI_unitValue`）。建物は都市の建物の評価（`AI_buildingValue`）。地形の特徴・施設は 10。人口 1 につき 50。資金は 1 につき -4（得るなら +4）。術者が消えるなら「レベル × 術者のユニットの評価」を引く。宗教・実行済みの取り消し・遅延・世界呪文・`PyResult` は評価しないので、`iAIWeight` で決める
  - FFH2 との違い: 昇進と移動不能は範囲の対象の数で数える（FFH2 は昇進が術者 1 体分、移動不能が範囲のタイルの数）。範囲の昇進は、自チームのユニットが得ると +、敵が得ると −（除去は逆）。今いる場所で効かない効果（地形の特徴を作れない、など）は数えない。効果の評価は失敗率の分だけ下げる（失敗すると効果も費用もないため。`iAIWeight` は下げない）。変身と術者が消えるときのユニットの評価は、FFH2 の `UNITAI_ATTACK` の代わりに術者のユニット AI で計る（変身後のユニットは術者のユニット AI を引き継ぐため）
  - AI のユニットは呪文を使うために移動しない（FFH2 の `UNITAI_MAGE` などは入れていない）。FFH2 の自動化のタグ（`bAllowAutomateTerrain` など）も入れていない

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
| `CIV4BuildingInfos.xml` | `BuildingInfo/iPlotRadius` | その建物（陳腐化していないもの）がある都市の都市圏の半径を ○ にする（持ち主の半径の方が広ければそちら。最大 3）。FFH2 と同じ名前。説明文は `TXT_KEY_BUILDING_PLOT_RADIUS`（「都市圏の半径3」）。建設は止めない |
| `CIV4TraitInfos.xml` | `TraitInfo/iCityRadiusChange` | 都市圏の半径 +○（既定 2、最大 3）。FFH2 の `bSprawling` から都市圏の部分だけを取り出したもの（都市数の上限は別のタグにする予定）。仕組みは「都市圏の半径」の節。説明文は `TXT_KEY_TRAIT_CITY_RADIUS_CHANGE`（「都市圏の半径+1（最大3）」） |
| `CIV4PromotionInfos.xml` | `PromotionInfo/iExpireChance`、`bRemovedByCombat`、`PromotionVictory`、`iPromotionVictoryChance` | 状態の昇進と戦闘結果（第 1 段階。名前は FFH2 と同じ。`PromotionVictory` / `iPromotionVictoryChance` だけ新規）。`iExpireChance`: 自分のターンの開始時に ○% で消える（`CvUnit::doTurnPromotions`）。`bRemovedByCombat`: 戦闘で生き残ると消える。`PromotionVictory`: 戦闘に勝利すると `iPromotionVictoryChance`%（省略時 100）でその昇進を得る。戦闘の処理は `CvUnit::doCombatPromotions`（`updateCombat` の決着時、両ユニットがまだ存在するうちに呼ぶ）で、**戦闘中に持っていた昇進**で判定し、戦闘で消える昇進を外してから勝利の効果を与える（勝利で得た昇進はその戦闘では消えない）。勝利は「相手が倒れて自分が生き残った」とき（自爆ユニットは除く。撤退は勝者なし）。対象は通常の攻撃・防御だけで、空中戦・迎撃・爆撃・付随ダメージ・核は含まない。昇進がその兵科で取れるかはチェックしない。状態用の昇進は `UnitCombats` を空にしてレベルアップで選べないようにする。乱数は `getSorenRandNum`。セーブデータは変わらない。`PromotionVictory` は昇進を参照するので `readPass2` で読む |
| `CIV4PromotionInfos.xml` | `PromotionInfo/bRemovedWhenHealed`、`PromotionCombatApply`、`iPromotionCombatApplyChance`、`iFreeXPPerTurn`、`iFreeXPCap` | 第 2 段階（名前は FFH2 と同じ。`iPromotionCombatApplyChance` / `iFreeXPCap` だけ新規）。ターンの開始時（`doTurnPromotions`、回復の後）に、`iFreeXPPerTurn` の経験値を足し（戦闘の経験値ではないので補正・大将軍ポイントなし。**ユニットの経験値が `iFreeXPCap`（省略時 100）に達したらそれ以上増えない**）、`bRemovedWhenHealed` はダメージ 0 なら消える。`PromotionCombatApply`: 戦闘の後、生き残った相手に `iPromotionCombatApplyChance`%（省略時 100）でその昇進を与える（与える側が倒れていても与える。戦闘で消える昇進を外した後、勝利の効果の前）。`PromotionCombatApply` も `readPass2` で読む |
| `CIV4PromotionInfos.xml` | `PromotionInfo/iGoldFromCombat`、`iCombatCapturePercent` | 第 3 段階（名前は FFH2 と同じ）。どちらも勝者の（戦闘中に持っていた）昇進の値を**足し算**する。`iGoldFromCombat`: 勝利すると合計が正なら資金を得る（メッセージ `TXT_KEY_MISC_GOLD_FROM_COMBAT`）。`iCombatCapturePercent`: 勝利すると合計 % で、倒したユニットと同じ種類のユニットを勝者のタイルに新しく作る（倒したユニットの昇進・経験値は引き継がない。新しいユニットと同じく志向の昇進の組などは付く。そのターンは動けない。メッセージは BtS の `TXT_KEY_MISC_YOU_CAPTURED_UNIT`）。捕獲しないのは、倒したのが動物（領土に入れないため）、陸と海などドメインが違う、陸のユニットが水上から勝った（船からの上陸攻撃）、勝者が蛮族のとき |
| `CIV4SpellInfos.xml`（新規） | `SpellInfo` の `PromotionPrereq1` / `PromotionPrereq2`、`UnitPrereq`、`UnitClassPrereq`、`UnitCombatPrereq`、`CivilizationPrereq`、`TechPrereq`、`iCasterMinLevel`、`bInBordersOnly`、`bInCityOnly`、`bHasCasted`、`bIgnoreHasCasted`、`bDisplayWhenDisabled`、`PyRequirement`、`PyResult`（と `Type`・`Description`・`Civilopedia`・`Help`・`Button`・ホットキー） | 呪文（第 1 段階。名前は FFH2 と同じ）。仕組みは「呪文」の節 |
| `CIV4SpellInfos.xml` | `SpellInfo` の `bAllowAI`、`iAIWeight` | 呪文の第 3 段階（名前は FFH2 と同じ）。AI が使ってよい呪文と、AI の評価値に足す値。仕組みは「呪文」の節 |
| `CIV4SpellInfos.xml` | `SpellInfo` の `CreateUnitType`、`iCreateUnitNum`、`bPermanentUnitCreate`、`bCopyCastersPromotions`、`CreateUnitPromotion`、`bCasterNoDuration`、`iMiscastChance`、`PyMiscast`、`bGlobal`、`iDelay` | 呪文の第 2 段階 c（名前は FFH2 と同じ）。召喚・失敗・世界呪文・遅延。FFH2 との違いは「呪文」の節 |
| `CIV4TraitInfos.xml` | `TraitInfo/iSummonDuration` | 呪文で召喚したユニットが消えるまでのターン +○（FFH2 と同じ名前）。そのつど合計（`CvPlayer::getSummonDuration`）。説明文は `TXT_KEY_TRAIT_SUMMON_DURATION` |
| `CIV4SpellInfos.xml` | `SpellInfo` の `iRange`、`iDamage`、`iDamageLimit`、`iImmobileTurns`、`bImmuneTeam`、`bImmuneEnemy`、`bImmuneFlying`（と、`bBuffCasterOnly=0` の `AddPromotionType1`〜`3` / `RemovePromotionType1`〜`3`） | 呪文の第 2 段階 b（名前は FFH2 と同じ）。周りのユニットへの効果。交戦中でない他国のユニットは常に対象外など、FFH2 との違いは「呪文」の節 |
| `CIV4SpellInfos.xml` | `SpellInfo` の前提 `UnitInStackPrereq`、`BuildingPrereq`、`BuildingClassOwnedPrereq`、`CorporationPrereq`、`FeatureOrPrereq1` / `2`、`ImprovementPrereq`、`PromotionInStackPrereq`、`StateReligionPrereq`、`bAdjacentToWaterOnly` と、効果 `iCost`、`AddPromotionType1`〜`3`、`RemovePromotionType1`〜`3`、`bBuffCasterOnly`、`ConvertUnitType`、`CreateBuildingType`、`CreateFeatureType`、`CreateImprovementType`、`SpreadReligion`、`iChangePopulation`、`bSacrificeCaster`、`bRemoveHasCasted`、`Effect`、`Sound` | 呪文の第 2 段階 a（名前は FFH2 と同じ）。術者自身・今いるタイル・今いる都市への効果。FFH2 との違い（自分の都市・領土に限るものなど）は「呪文」の節 |

BtS 本体の修正:

- `CvUnit::upgradePrice` は、割引の合計が 100% を超えると費用がマイナスになり、アップグレードで金が増えていた。この Mod では費用を 0 未満にしない
- `CvUnit::isPromotionValid` は「ユニットの撤退率 + その昇進の撤退率」が `MAX_WITHDRAWAL_PROBABILITY`（90）を超える昇進を選べなくする（迎撃率・回避率も同じ）。ユニットがすでに上限を超えていると、撤退率を上げない昇進や大将軍（指導者の昇進）まで**すべて**選べなくなっていた。BtS では昇進で上限を超えられないので起きないが、この Mod では選ばずに得る昇進（志向の昇進の組、`PromotionVictory`、`PromotionCombatApply`）で超えられる。この Mod では、その昇進が値を上げる（> 0）ときだけ上限で止める

この Mod で追加した志向: `TRAIT_MARITIME`（海洋志向、短縮名「海」。昇進の組: 海洋ユニット → 航海術Ⅰ、白兵戦ユニット → 水陸両用。今はどの文明・指導者も持っていない）。`CIV4TraitInfos.xml` の末尾に追加（既存の志向の番号を変えないため、新しい志向は常に末尾に足す）

### 検証用データ

**動作確認は検証用データで行い、既存の Info（日本・兵舎・専制政治など）には検証用の値を入れない。** 新しいタグを足したら、下の検証用の項目に値を入れて確かめる。検証用の項目はすべて各ファイルの末尾にあり、テキストは `CIV4GameText_ClaudeSample.xml`（説明文は共通の `TXT_KEY_CLAUDE_TEST_PEDIA`）。アイコン・グラフィックは複製元のものを使う。

| 項目 | 複製元 | 内容 |
|---|---|---|
| `CIVILIZATION_TEST`（検証用文明） | 日本 | プレイ可・**AI は選ばない**（`bAIPlayable=0`）。色 `PLAYERCOLOR_GOLD_AND_BLACK`。固有ユニット・固有建物なし。文明の志向 `TRAIT_TEST`、首都に `BUILDING_TEST` が無料、開始ユニットに `UNIT_TEST_A`。初期の社会制度は他の文明と同じ（専制政治など）で、`CIVIC_TEST` は革命で採用する（無政府 1 ターン） |
| `LEADER_TEST`（検証用指導者） | 徳川 | **指導者の志向なし**（検証の邪魔をしないため） |
| `TRAIT_TEST`（検証用志向） | — | 志向のタグをすべて持つ: 労働者速度 +50%、社会制度風の数値 9 個と配列 4 個（新ユニット経験値 +2、軍事生産 +10% など）、白兵戦に経験値 +2・生産 +25%・アップグレード割引 50%・領土外アップグレード（敵国でも可）、昇進の組（白兵戦 → `PROMOTION_TEST`、海洋 → 航海術Ⅰ）、タイルの産出（砂漠・ツンドラ・雪原に生産 +1、丘陵に食料 +1、山岳に生産 +2、氷河に食料 +1・商業 +1、森に商業 +1、農場に生産 +1、鉱山に商業 +1、川沿いに食料 +1）、都市圏の半径 +1、召喚したユニットが消えるまで +1 ターン |
| `CIVIC_TEST`（検証用社会制度） | 専制政治 | 政治の区分。前提技術なし・無政府期間 0・維持費なし・AI 評価 -10000。白兵戦に経験値 +1・生産 +10%・アップグレード割引 25%、弓兵に領土外アップグレード（敵国の領土を除く）。前提の志向 `TRAIT_TEST` |
| `BUILDING_TEST` / `BUILDINGCLASS_TEST`（検証用建物） | 兵舎 | 前提技術なし・元の効果（攻撃志向の生産 2 倍、陸上ユニット経験値 +3）は削除・AI 評価 -10000。騎乗に領土外アップグレード。前提の志向 `TRAIT_TEST` |
| `BUILDING_TEST_RADIUS` / `BUILDINGCLASS_TEST_RADIUS`（検証用建物（都市圏）） | 検証用建物 | `iPlotRadius` 3。前提の志向なし（どの文明も建てられる）・費用 20。検証用文明は志向で半径 3 なので、建物の効果は他の文明（日本など）で確かめる |
| `UNIT_TEST_A` / `UNIT_TEST_B`（検証用ユニット A・B） | 戦士・斧兵 | 白兵戦。前提技術・資源なし。A は B にだけアップグレードできる（1 ターン目からアップグレード関連を試せる）。AI 評価 -10000。B だけ前提の志向 `TRAIT_TEST` または `TRAIT_AGGRESSIVE`（「どれか 1 つ」の確認用。攻撃志向の徳川の日本でも作れる）。B を作れる文明では A は生産候補に出ない（BtS の仕様: アップグレード先がすべて作れるユニットは候補から外れる。`CvCity::canTrain` → `allUpgradesAvailable`）。A のアップグレードは検証用文明の開始ユニットで試す |
| `PROMOTION_TEST`（検証用昇進） | 戦闘術Ⅰ | 戦闘力 +10%。`UnitCombats` が空なので通常のレベルアップでは誰も選べず、昇進の組でだけ付く。勝利すると `PROMOTION_TEST_STATE1` を得る。生き残った戦闘相手に `PROMOTION_TEST_POISON` を与える。毎ターン経験値 +1（10 まで）。撤退率 +100%（攻撃で負けそうになると必ず撤退するので、相手が生き残って毒を受ける）。勝利すると資金 +5、50% で捕獲 |
| `PROMOTION_TEST_STATE1` / `STATE2`（検証用状態Ⅰ・Ⅱ） | 検証用昇進 | 状態の昇進（`UnitCombats` は空）。Ⅰ: 戦闘力 +20%、毎ターン 50% で消える、勝利すると 50% でⅡを得る・資金 +3・捕獲 50%（検証用昇進と合わせて資金 +8・捕獲 100%）。Ⅱ: 戦闘力 +10%、戦闘すると消える。ボタンは戦闘術Ⅱ・Ⅲ |
| `PROMOTION_TEST_POISON`（検証用毒） | 検証用状態Ⅱ | 状態の昇進。戦闘力 -20%、完全に回復すると消える、生き残った戦闘相手に 50% でうつる（毒を受けた敵と戦うと自分も受けることがある）。ボタンは戦闘術Ⅳ |
| `SPELL_TEST_GOLD`（検証用呪文（資金）） | — | 検証用文明の白兵戦ユニットが、自国の領土で資金 +25（`spellGainGold(pCaster, 25)`。`PyRequirement` なし）。使えないときも無効のボタンで出る（`bDisplayWhenDisabled`）。実行済みになる。ボタンは「富」 |
| `SPELL_TEST_PROMOTION`（検証用呪文（昇進）） | — | 検証用昇進を持つユニットが、都市で、検証用状態Ⅰを持っていなければ得る（`reqNotHasPromotion` / `spellAddPromotion`）。使えないときはボタンが出ない。実行済みになる。ボタンは戦闘術Ⅱ |
| `SPELL_TEST_CONVERT`（検証用呪文（変身）） | — | ユニット A が、資金 10 で検証用状態Ⅱを得てからユニット B に変わる（状態Ⅱ・経験値・実行済みは B に引き継ぐ。B はそのターン動けない）。演出 `EFFECT_SETTLERSMOKE`、音 `AS2D_GOODY_WARRIOR`。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_CITY`（検証用呪文（都市））| — | 検証用建物のある都市（検証用文明の首都）で、オベリスクを建て、仏教を広め、人口 +1、資金 +50（`iCost` -50）。実行したユニットは消える。オベリスクがあると使えない（ボタンも出ない）。音 `AS2D_GOODY_GOLD` |
| `SPELL_TEST_FOREST`（検証用呪文（森）） | — | 検証用建物を持つ文明が、自国の領土の水に隣接したタイルで、資金 5 で森を作り、検証用毒を外す（どちらかが効けば使える）。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_FORT`（検証用呪文（砦）） | — | 検証用文明の、検証用昇進を持たないユニット（開拓者など）が、検証用昇進を持つ自国のユニットと同じタイルにいるとき、森かジャングルのタイルに砦を作る。使えないときも無効のボタンで出る（同じタイルの条件を満たさないとボタンは出ない）。実行済みになる |
| `SPELL_TEST_REFRESH`（検証用呪文（再実行）） | — | 検証用文明のユニットが、砦のタイルで、資金 20 で実行済みを取り消す（`bIgnoreHasCasted`・`bRemoveHasCasted`）。実行済みでないときは無効のボタン（砦のタイルでないとボタンは出ない） |
| `SPELL_TEST_BLESS`（検証用呪文（祝福）） | — | 検証用文明のユニットが、1 タイル以内の自国（と同じチーム）のユニットに検証用状態Ⅱを与え、検証用毒を外す（`bImmuneEnemy`。兵科のないユニットには状態Ⅱは付かない）。演出 `EFFECT_PROMOTION_UNIT`。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_FIREBALL`（検証用呪文（火球）） | — | 検証用文明のユニットが、周囲 1 タイル（自分のタイルを除く）の交戦中の相手（動物・蛮族）に 40 前後のダメージ（`bImmuneTeam`。上限なしなので倒すこともある）。演出 `EFFECT_ARTILLERY_SHELL_EXPLODE`。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_SNARE`（検証用呪文（足止め）） | — | 検証用文明のユニットが、周囲 2 タイル（自分のタイルを除く）の交戦中の相手を 2 ターン動けなくし、10 前後のダメージ（25 まで）を与える（`bImmuneTeam`・`bImmuneFlying`）。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_SUMMON`（検証用呪文（召喚）） | — | 検証用文明の、検証用昇進を持つユニットが、戦士を 2 体召喚する（2 ターンで消える: 既定 1 + 検証用志向 1）。戦士は術者の昇進をすべて得る（`bCopyCastersPromotions`）。召喚した戦士は実行できない（`bCasterNoDuration`）。25% で失敗し、術者が 20 のダメージ（`spellDamageCaster`）。演出 `EFFECT_SETTLERSMOKE`。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_PERMANENT`（検証用呪文（永続召喚）） | — | 検証用文明の、検証用昇進を持つユニットが、資金 10 で、次のターンの初めに探検家を召喚する（`iDelay` 1。消えない。検証用状態Ⅱが付く。召喚したターンは動けない）。検証用昇進を持つ自国のユニットの数まで。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_WORLD`（検証用呪文（世界）） | — | 検証用文明のユニットが、資金 +100（世界呪文。確認のポップアップ。使うとボタンが出なくなる）。音 `AS2D_GOODY_GOLD`。実行済みになる |

検証用呪文は、検証用呪文（砦）以外すべて `bAllowAI`（AI も使う。確かめるには AI に検証用文明を割り当てる）。Python の効果しかない資金・昇進と、敵がいないと評価が 0 の永続召喚は `iAIWeight` を持つ（資金 100・昇進 50・永続召喚 100）。砦は `bAllowAI` がない（評価は正なので、AI が使わないことで `bAllowAI` を確かめる）。再実行は評価が負（資金 20）なので AI は使わない

検証用文明が `CIVIC_TEST` を採用したあと、白兵戦ユニット（都市で生産）で期待される値: 生産直後の経験値 5（志向 +2、白兵戦 +2、社会制度 +1。その後は毎ターン +1 で 10 まで増える）、ユニット A → B のアップグレード割引 75%（志向 50% + 社会制度 25%）、`PROMOTION_TEST` が付く

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
- 2026-10-31: 昇進の `iExpireChance` / `bRemovedByCombat` / `PromotionVictory`（D・E の第 1 段階）の説明文と動作をゲーム内で確認（検証用文明のユニット A で勝利 → 検証用状態Ⅰ・Ⅱ）
- 2026-10-31: 昇進の `bRemovedWhenHealed` / `PromotionCombatApply` / `iFreeXPPerTurn`・`iFreeXPCap`（D・E の第 2 段階）と、撤退率などが上限を超えたユニットでも昇進・大将軍の合体ができる修正（`CvUnit::isPromotionValid`）をゲーム内で確認
- 2026-10-31: 昇進の `iGoldFromCombat` / `iCombatCapturePercent`（D・E の第 3 段階）の説明文・資金・捕獲（足し算、動物は捕獲しない）をゲーム内で確認
- 2026-10-31: 都市圏の半径の土台（第 1 段階。都市ごとの半径、既定 2）で、既存のセーブの読み込みと、新しいゲームが今までどおり動くことを確認
- 2026-11-01: 志向の `iCityRadiusChange`（都市圏の半径 第 2 段階）と都市画面のカメラ（3 周目を働かせられる都市だけ引く）をゲーム内で確認。近くの都市の文化で広がった国境のタイルを 3 周目で働かせられる都市も引く（文明単位の国境なので仕様どおり）
- 2026-11-01: 建物の `iPlotRadius`（都市圏の半径 第 3 段階）をゲーム内で確認（日本で検証用建物（都市圏）を建てた都市だけ 3 周目まで広がり、取り除くと戻る）
- 2026-11-01: 呪文の第 1 段階（`CIV4SpellInfos.xml`、行動ボタン、基本の条件、`PyRequirement` / `PyResult`、実行済み）をゲーム内で確認（検証用呪文（資金）・検証用呪文（昇進））
- 2026-11-01: 呪文の第 2 段階 a（術者自身・今いるタイル・今いる都市への効果と、建物・地形の特徴・施設・同じタイルのユニットなどの前提）をゲーム内で確認（検証用呪文（変身）・（都市）・（森）・（砦）・（再実行））
- 2026-11-01: 呪文の第 2 段階 b（周りのユニットへの昇進の付与・除去、ダメージ、移動不能と、対象外の条件）をゲーム内で確認（検証用呪文（祝福）・（火球）・（足止め））
- 2026-11-01: 呪文の第 2 段階 c（召喚・消えるまでのターン・昇進の引き継ぎ、失敗と `PyMiscast`、世界呪文と確認のポップアップ、遅延）をゲーム内で確認（検証用呪文（召喚）・（永続召喚）・（世界））
- 2026-11-01: 呪文の第 3 段階（AI の `bAllowAI` と評価値。ターンの初めと終わりに呪文を使う）をゲーム内で確認（AI に検証用文明を割り当てたゲーム）
- 2026-11-01: 呪文の第 4 段階（ペディアのカテゴリ「呪文」、呪文のページ、ボタンとリンク）をゲーム内で確認
