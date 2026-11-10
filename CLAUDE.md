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
- `CyPlayerInterface1.cpp` は MSVC 7.1 のデバッグ情報の上限に達している（`.def` を足すと `fatal error C1067: compiler limit : debug information module size exceeded`）。この Mod で足す `CyPlayer` の `.def` は `CyPlayerInterface2.cpp` の末尾に書く。`CyCityInterface1.cpp` も同じ上限に達している。`CyCity` の `.def` は `CyCityInterface2.cpp` に書く
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

- XML や Schema を変えたら、起動の前に `tools\check_xml.ps1` を実行する（`powershell -ExecutionPolicy Bypass -File tools\check_xml.ps1`）。ゲームと同じ MSXML で検証するので、Schema の二重宣言や必須要素の抜けなど、起動時の XML エラーを先に見つけられる。ただし検証するのは形式だけで、名前の参照の誤りは見つからない
- ゲームのログは `%USERPROFILE%\Documents\My Games\Beyond the Sword(J)\Logs`（`xml.log`・`PythonErr.log` など）
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
- これらのクラスに項目を足したら、`write` の `uiFlag` を上げ、`read` では `uiFlag` を見て古いキャッシュも読めるようにする（現在 `CvCivilizationInfo` は `uiFlag=2`、`CvCivicInfo` は `uiFlag=7`、`CvBuildingInfo` は `uiFlag=17`、`CvUnitInfo` は `uiFlag=1`、`CvPromotionInfo` は `uiFlag=10`、`CvTechInfo` は `uiFlag=1`、`CvImprovementInfo` は `uiFlag=4`、`CvEventTriggerInfo` は `uiFlag=1`）
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
- Mod に入れている Python: `Screens\CvPediaMain.py`・`Screens\CvPediaCivilization.py`・`Screens\CvScreenEnums.py`・`EntryPoints\CvScreensInterface.py`・`Screens\CvVictoryScreen.py`・`Screens\CvMainInterface.py`・`Screens\CvTechChooser.py`・`CvEventManager.py`（BtS(J) のコピーに追記）、`Screens\CvCivicsScreen.py`（無印 Civ4(J) のコピーに追記。BtS(J)・Warlords(J) には無く、無印のものが使われている）、`Screens\CvFinanceAdvisor.py`（Warlords(J) のコピーに追記。BtS(J) には無い）、`Screens\CvPediaTrait.py`・`Screens\CvPediaSpell.py`・`EntryPoints\CvSpellInterface.py`・`EntryPoints\CvTraitInterface.py`（新規）
- 文法チェック: Python 3.14 が入っている（`python -I`）。Python 2.4 そのものではないが、インデントや括弧の誤りは見つかる。BtS のファイルには Python 2 の `print` 文があって途中で止まるので、`print` 行を `pass` に置き換えた複製で、**編集前と編集後の両方**をチェックして比べる

## Mod の設計方針

この Mod の狙いは、FFH2 のような大規模なソース改変ではなく、**XML から設定できる要素を増やすこと**。AI がその要素を使いこなせるかは考慮しない。

- 新しい効果は、**志向などの Info に専用のタグを足す方式**で実装する
- **ダミー建物（全都市に無料で置く建物）で効果を代用しない**。過去に試して次の問題があった:
  - 建設候補に出てしまう
  - 前提を研究不可の技術にしても、未来の時代から始めるゲームではその技術が与えられてしまう
  - 文明全体に効かせたい能力を首都だけに置く方式は、遷都にうまく対応できなかった
  - 建物なので、ランダムイベントやスパイ工作の対象になる
- 志向の効果のうち、ゲーム開始時の `CvPlayer::init()` で 1 回だけ適用されるもの（健康・幸福・商業・偉人率・労働者速度など）は、XML で指導者・文明に志向を足しても**既存のセーブには反映されない**。動作確認は新しいゲームで行う（ゲーム中に `FreeTraits` などで得た志向は、その時点で `CvPlayer::processTrait` が足し引きする）
- 志向の数値の合計は `CvPlayer::sumTraitValues(&CvTraitInfo::getFoo)` で求める（引数は 2 つまで）。ループを書かない
- 志向の説明文は、Mod の部分を `CvGameTextMgr::parseTrait〜` に分けてある。1 行の追加は `appendTraitLine`、リンクは `getLinkText` を使う
- 志向の説明文は、文明選択の画面（ゲーム開始前）からも呼ばれる。そこではアクティブプレイヤーが設定済みで指導者が `NO_LEADER` なので、`hasTrait` などを呼ぶと強制終了する。説明文でプレイヤーを見るときは `GC.getGameINLINE().isFinalInitialized()` と指導者を先に確かめる
- 社会制度・建物がすでに持つ「文明全体に効く数値」は、志向にも同じ名前のタグで持たせ、既存の `CvPlayer::change〜()` で適用する。説明文は社会制度のテキストキーを流用する（キーは `[ICON_BULLET]` 始まりで改行を含まないので、志向の説明ではコード側で改行と字下げ 2 文字を付ける）
- 「首都の産出 +X%」系（`CapitalYieldModifiers` など）は、計算のたびに現在の首都を参照するので、**遷都しても自動で新しい首都に移る**
- **価値がゲーム速度や開始時代で変わる値（開始時の資金など）は、固定値の XML タグにしない**。BtS 本体の開始資金は「難易度 + 開始時代」の単純な合計で、ゲーム速度の補正がない。こうした値は、志向の Python フック（`PyOnGameStart` など。`EntryPoints\CvTraitInterface.py`）で計算式として書く

### 検証用データ

**動作確認は検証用データで行い、既存の Info（日本・兵舎・専制政治など）には検証用の値を入れない。** 新しいタグを足したら、検証用の項目（`docs/test-data.md`）に値を入れて確かめる。検証用の項目はすべて各ファイルの末尾にあり、テキストは `CIV4GameText_ClaudeSample.xml`（説明文は共通の `TXT_KEY_CLAUDE_TEST_PEDIA`）。アイコン・グラフィックは複製元のものを使う。

- 新しい志向・昇進などの Info は、既存の番号を変えないため常に各ファイルの末尾に足す。検証用の項目がある Info ファイルでは、末尾の目印のコメント（`Entries of the civ-design branch go below this line`）の**上**に足す（git の節）

## git

- **ブランチ**: `master` がベース（DLL・Schema・Python の機能、タグ、検証用データ）。実在の文明のデザイン（日本の武士道など）は `civ-design` に入れる
  - 機能を足すときは `master` で実装してコミットし、そのあと `civ-design` で `git merge master` する。文明のデザインの途中で新しいタグが要るとわかったときも、先に `master` に入れる
  - `civ-design` では `CvGameCoreDLL/`・Schema・`Assets/CvGameCoreDLL.dll` を変更しない（DLL の衝突を防ぐ）
  - コミットの前に今のブランチを確かめる。`build.bat` が配置するのは**今のブランチ**の `Assets\` なので、ブランチを切り替えたら配置し直し、Info の数が違うのでキャッシュも消す
  - `civ-design` のテキストは `CIV4GameText_ClaudeCivs.xml`、記録は `docs/civ-design.md` に書く（`master` の追記と衝突しないように）。`docs/test-log.md` は `.gitattributes` の `merge=union` で両方の追記が残る
  - 志向や昇進などの Info は両方のブランチで各ファイルの末尾に足すので、末尾の閉じタグの前に目印のコメント `<!-- Entries of the civ-design branch go below this line, entries of master above it -->` を置いてある。**`master` の項目は目印の上、`civ-design` の項目は目印の下**に足す。両方の変更の間に目印の行が挟まるので、マージが自動で済む。目印のないファイルに `civ-design` で項目を足すときは、先に `master` で目印を置く。それでも衝突したら、`master` のファイルに `civ-design` の項目を目印の下に足して作り直す。`master` の Info が増えると `civ-design` の Info の番号がずれるので、古いセーブは使えなくなることがある。マージ後はキャッシュを消す
- `2b1bf36` が無改造の公式ソース。改造の全差分は `git diff 2b1bf36 -- CvGameCoreDLL` で見られる
- Edit ツールは、書き換えた既存行の**行末の空白・タブを消す**ことがある。編集後は `git diff` で意図しない削除行（`-` で始まる行）がないか確認し、あれば元に戻す
- `Assets/CvGameCoreDLL.dll` も git で管理する。DLL を作り直したら、元になったソース変更と同じコミットに含める
- `core.autocrlf=false`（ファイルはそのまま保存される）。既存ファイルは CRLF なので、新しいテキストファイルも CRLF で作る
- リモートは `origin` = https://github.com/jyoan/civ4mod（非公開）。**コミットはユーザーの指示があったときに行い、そのあとの push は Claude が行ってよい**（2026-10-31 にユーザーが許可）

## 詳細ドキュメント（docs/）

仕様の詳細は `docs/` に分けてある。該当する作業のときだけ読む。

| ファイル | 内容 | 読むとき |
|---|---|---|
| `docs/xml-tags.md` | この Mod で追加した XML タグの一覧と仕様、BtS 本体の修正、追加した志向 | タグを追加・変更するとき、既存のタグの動作を調べるとき |
| `docs/test-data.md` | 検証用データ（検証用の文明・志向・建物・昇進・呪文など）の中身 | 検証用データに値を足すとき、確認の手順を書くとき |
| `docs/city-radius.md` | 都市圏の半径（都市ごとの半径、ループの書き分け、セーブ、カメラ） | 都市圏のタイルを扱うコードを書くとき |
| `docs/spells.md` | 呪文（判定順、効果、召喚、AI、ペディア） | 呪文に触れるとき |
| `docs/python-screens.md` | 社会制度の画面、志向のペディア（画面番号の落とし穴を含む） | 画面・ペディアの Python に触れるとき |
| `docs/test-log.md` | 動作確認の記録 | 確認済みの範囲を調べるとき |
| `docs/civ-design.md` | 実在の文明のデザイン（`civ-design` ブランチだけにある） | 文明をデザインするとき |

- タグを足したら `docs/xml-tags.md` の表と `docs/test-data.md` を更新する。ユーザーが動作確認したら `docs/test-log.md` に 1 行足す（コミットの前）
- docs のファイルも UTF-8・CRLF
