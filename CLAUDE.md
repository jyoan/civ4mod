# MyMod — Civ4 Beyond the Sword（日本語版）DLL Mod

Civilization IV: Beyond the Sword 日本語版（CyberFront 版）向けの Mod。公式 SDK の CvGameCoreDLL を改造し、XML / Python も改修する。

## トークンを節約する読み方

- `docs/` の大きなファイル（`docs/tags/*`・`test-data*.md`・`test-log*.md`・`civ-design.md`・`civ4_mod_design_handoff.md`）は丸ごと Read しない。Grep で該当箇所を探し、`offset` / `limit` でその付近だけ読む（タグと検証用データは見出し 1 つが 1 項目）
- 大きなソース（`CvInfos.cpp` 約 2.7 万行、`CvPlayer.cpp`・`CvGameTextMgr.cpp` など）も同じ。関数名を Grep してから範囲を指定して読む
- 一度読んだファイルを同じ会話で読み直さない

## フォルダ構成

| パス | 内容 |
|---|---|
| `CvGameCoreDLL/` | 公式 SDK ソース（日本語版、`CYBERFRONT` 定義）。`Boost-1.32.0/` と `Python24/` 同梱。`Makefile` は nmake 用 |
| `build.bat` | ビルドと Mod フォルダへの配置の入口 |
| `claude_sample.ini` | Mod 設定（`ModularLoading = 0` など）。配置時に Mod フォルダへコピーされる |
| `Assets/` | Mod 本体（XML・Python・ビルドした `CvGameCoreDLL.dll`） |
| `tools/` | `check_refs.ps1`（名前の参照）・`check_xml.ps1`（XML の形式） |

## ビルド

```
build.bat            差分ビルド → Assets\CvGameCoreDLL.dll にコピー → Mod フォルダへ配置
build.bat rebuild    クリーン後に全ビルド → 配置
build.bat deploy     配置だけ（ビルドしない）
build.bat clean      CvGameCoreDLL\Release\ を削除（配置はしない）
```

- Claude のツールからはフルパスで実行する。Bash: `cmd //c 'C:\claudeCode\civ4mod\build.bat'`、PowerShell: `& 'C:\claudeCode\civ4mod\build.bat'`
- 終了コード: `0` = 完了、`1` = ビルドまたは配置に失敗、`2` = ビルドは成功したが **Civ4 が起動中なので配置を飛ばした**
- **Claude は Civ4 を終了させない**（強制終了になり、最後のオートセーブ以降の進行が失われる）。終了コード `2` のときは、ユーザーにゲームの終了を頼み、終了したと言われてから `build.bat deploy` を実行する
- XML / Python だけを変えたときも `build.bat` を実行する（DLL は 0 秒で終わり、配置だけが行われる）
- 出力の `check_refs:` の行を見て、エラーが出ていたら直す（配置は続くので見落としやすい）
- 元ソースは `/W3` で**警告 0 件**。警告が出たら自分の変更が原因
- コンパイラの上限に達しているファイル（`.def` を足す先を変える）:
  - `CyPlayerInterface1.cpp`・`CyCityInterface1.cpp`（C1067）→ `CyPlayer` は `CyPlayerInterface2.cpp` の末尾、`CyCity` は `CyCityInterface2.cpp` に書く
  - `CyInfoInterface3.cpp`（C1204。同じファイルの別の関数に分けても出る）→ `CvTraitInfo` は `CyInfoInterface4.cpp` の `CyInfoPythonInterfaceTrait2` に書く
- 新しい `.cpp` を追加したら Makefile の `OBJS` に 1 行足す（ワイルドカードは使っていない）
- `Makefile` と `build.bat` は ASCII のみ・CRLF
- `CYBERFRONT` と `FINAL_RELEASE` の定義は外さない（日本語版 exe に合わせた構成）
- 詳細（出力・check_refs が調べる項目・配置の仕組み・ツールチェーンの場所）は `docs/build-env.md`

## ゲーム本体と Mod の配置先

`<CIV4>` = `C:\Program Files (x86)\CYBERFRONT\Sid Meier's Civilization 4(J)`、`<BTS>` = `<CIV4>\Beyond the Sword(J)`、`<WL>` = `<CIV4>\Warlords(J)`。

- 元ファイルを探す順は **`<BTS>` → `<WL>` → `<CIV4>`**（`Assets\XML\`・`Assets\Python\`）。最初に見つかったものが実際に使われている（BtS(J) は自分に無いファイルを Warlords(J)、無印から読む）
- 配置先は `<BTS>\Mods\claude_sample\`（`civ-design` ブランチは `Mods\better_trait_mod\`）。`Assets\` を robocopy `/MIR` でミラー同期する
- **Mod フォルダ側で直接編集しない**（次の配置で消える）。`<BTS>\Assets\CvGameCoreDLL.dll`（製品版）は上書きしない
- ゲームのログは `%USERPROFILE%\Documents\My Games\Beyond the Sword(J)\Logs`（`xml.log`・`PythonErr.log` など）

## ツールチェーン（MSVC 7.1 固定）

Civ4 本体が MSVC 7.1 ランタイムなので、**新しい MSVC や Windows Kits のヘッダ・ライブラリは使わない**。Visual C++ Toolkit 2003（cl 13.10）と Platform SDK（Windows Server 2003 SP1）を使う。パスは `build.bat` にだけ書いてある（nmake を直接実行しない）。dumpbin は無いので `link.exe /dump`（PowerShell から）。詳細は `docs/build-env.md`。

## C++ のルール（C++03 / MSVC 7.1）

- 使えない: `auto`（型推論）、`nullptr`、ラムダ、range-based for、`override` / `final`、`enum class`、`static_assert`、`{}` による一様初期化と `std::initializer_list`（配列・構造体の集成体初期化は可）、`std::unique_ptr` / `std::shared_ptr` / `std::unordered_map`、可変引数マクロ（`__VA_ARGS__`）
- 入れ子テンプレートの閉じ括弧は `> >` と空白を入れる
- `for` のループ変数のスコープは非標準（`/Zc:forScope` なし）。ループ変数は関数の先頭で宣言する（例: `int iI;`）
- **ソースは ASCII のみ**。コメントも文字列も英語で書く（コードページ 932 なので、UTF-8 の日本語を cl 7.1 が誤解釈する）
- 既存のスタイルに合わせる: ハンガリアン記法（`m_iFoo`, `eUnit`, `pCity`, `szText`）、タブインデント、関数の波括弧は改行

## マルチプレイ同期（OOS 防止）

- ゲーム状態に影響する乱数は必ず `GC.getGameINLINE().getSorenRandNum(iNum, "reason")`（0〜iNum-1）。Python からは `CyGame().getSorenRandNum(iNum, "reason")`
- `rand()` / `srand()` や `GC.getASyncRand()` をゲーム状態に使わない（ASyncRand は演出・UI 専用）
- 表示用のコード（`CvGameTextMgr`、`CvDLLWidgetData` など）からゲーム状態を変更しない。プレイヤー操作による変更は `CvMessageControl` 経由で送る

## セーブデータ

- `read(FDataStreamBase*)` / `write(FDataStreamBase*)` を持つクラス（`CvUnit`, `CvCity`, `CvPlayer` など）にメンバを追加したら、`reset()` で初期化し、`read` と `write` の両方に同じ順序で追加する
- 既存のセーブとの互換を保つ場合は、`write` の `uiFlag` を上げ、`read` で `if (uiFlag > N)` のように分岐する

## XML

タグを追加するときは、次をすべて更新する:

1. `CvInfos.h`: メンバ変数と getter
2. `CvInfos.cpp`（志向は `CvTraitInfo.cpp`）: コンストラクタでの初期化（配列ならデストラクタで解放）と、`read(CvXMLLoadUtility* pXML)` 内の `pXML->GetChildXmlValByName(&m_iFoo, "iFoo");`
3. Schema（`Assets/XML/<分類>/CIV4*Schema.xml`）: `ElementType` の宣言と、親要素への `element` の追加
4. その Info クラスに `read(FDataStreamBase*)` / `write(FDataStreamBase*)` もある場合（Unit, Building, Tech, Promotion, Civic, Bonus など）は、そちらにも追加し、`uiFlag` を上げる（現在の値は `docs/xml-rules.md`）
5. Python から使うなら `CyInfoInterface*.cpp` で公開する

- XML や Schema を変えたら、起動の前に `powershell -ExecutionPolicy Bypass -File tools\check_xml.ps1` を実行する（ゲームと同じ MSXML で形式を検証する。名前の参照は build.bat の check_refs が調べる）
- XML 内のタグの順序は Schema の順序に合わせる。新しいタグは Schema で `minOccurs="0"` にする
- 「なし」を表す値は型名ではなく **`NONE`** と書く（`UPKEEP_NONE` は存在しない）。既存の値を書き換えるときは、元の XML でその要素がどう書かれているかを先に確かめる
- **後に読み込まれる Info を参照するタグは `readPass3()`（`ProcessInfo` などは `readPass4()`）で番号に変える**。`FindInInfoClass` で引けるのは先に読み込まれた Info だけ（詳細は `docs/xml-rules.md`）
- Mod の XML は本体の同名ファイルを丸ごと置き換える。`<BTS>` → `<WL>` → `<CIV4>` の順に探して最初に見つかったものを、同じ相対パスで `Assets\XML\...` にコピーしてから編集する（Schema も同様）
- コピーした直後に**未改変のままコミット**し、編集は次のコミットにする
- 元の XML はすべて ASCII・CRLF。Mod 側も ASCII・CRLF を保つ
- テキスト XML は ASCII。日本語は `<Japanese>` の中に **10 進の数値文字参照**で書く。キーの置き場所、`%F2`、同じ引数を 2 回使わないことなどは `docs/xml-rules.md` の「テキスト」
- 日本語版の用語: Trait は「〜志向」

### XML キャッシュ

- **Info の数が変わる変更（志向・昇進・技術・ユニットなどの追加や削除）をしたら、必ずキャッシュを消す**。Civ4 が起動していないことを確認してから、`%LOCALAPPDATA%\My Games\Beyond the Sword(J)\cache` 内の `*.dat` だけを消す（フォルダは残す）
- キャッシュが原因と思われる不具合が出たときも同じ手順で消す。理由と観察は `docs/xml-rules.md`

## Python（2.4）

- Python 2.4 の構文だけを使う。`with` 文、条件式（`a if c else b`）、`except E as e`、`str.format` は使えない
- `Assets\Python\` に置いたファイルは本体の同名ファイルを置き換える。元ファイルを同じ相対パスでコピーしてから編集する。どのファイルを入れているかは `docs/python-screens.md`
- 元の Python はすべて ASCII・CRLF。日本語を書く必要があるなら、ファイル先頭に coding 宣言を入れる
- DLL の機能を Python に公開するには `Cy*.cpp` と `Cy*Interface*.cpp`（boost::python）に追加する
- 文法チェック: Python 3.14（`python -I`）。BtS のファイルには `print` 文があるので、`print` 行を `pass` に置き換えた複製で、**編集前と編集後の両方**をチェックして比べる

## Mod の設計方針

この Mod の狙いは、FFH2 のような大規模なソース改変ではなく、**XML から設定できる要素を増やすこと**。AI がその要素を使いこなせるかは考慮しない。

- 新しい効果は、**志向などの Info に専用のタグを足す方式**で実装する
- 志向のコードの置き場所: `CvTraitInfo` は `CvTraitInfo.cpp`、`CvPlayer` の志向の関数（`sumTraitValues`・`hasTraitFlag`・志向のタグの getter と効果）は `CvPlayerTraits.cpp`、志向の説明文の部品（`parseTrait〜`）は `CvGameTextMgrTrait.cpp`。ほかの Info と共有する XML 読み込みの関数は `CvInfosReadUtils.h`、説明文の関数は `CvGameTextMgrUtils.h` で宣言する（ファイル内だけで使う補助関数は `static`）
- **ダミー建物（全都市に無料で置く建物）で効果を代用しない**（建設候補に出る、未来の時代の開始で前提技術が与えられる、遷都に対応できない、イベントやスパイの対象になる）
- 志向の効果のうち `CvPlayer::init()` で 1 回だけ適用されるもの（健康・幸福・商業・偉人率・労働者速度など）は、既存のセーブには反映されない。動作確認は新しいゲームで行う（ゲーム中に得た志向は `CvPlayer::processTrait` が足し引きする）
- 志向の数値の合計は `CvPlayer::sumTraitValues(&CvTraitInfo::getFoo)`（引数 2 つまで）。ループを書かない
- 志向の「どれかが持っていれば有効」は `CvPlayer::hasTraitFlag(&CvTraitInfo::isFoo)`（引数 1 つまで）、チーム単位は `CvTeam::hasPlayerTraitFlag`。昇進は `CvUnit::hasPromotionFlag` / `countPromotions` / `sumPromotionValues`。セーブしない昇進の値は `CvUnit::updatePromotionCounts` で読み込み時に数え直す
- 志向の説明文は、Mod の部分を `CvGameTextMgr::parseTrait〜`（`CvGameTextMgrTrait.cpp`）に分けてある。1 行の追加は `appendTraitLine`（値が 0 でなければ出す行は `appendTraitValueLine`、フラグの行は `appendTraitFlagLine`）、リンクは `getLinkText`
- 志向の説明文は文明選択の画面（ゲーム開始前）からも呼ばれ、そこでは指導者が `NO_LEADER` なので `hasTrait` などを呼ぶと強制終了する。プレイヤーを見るときは `GC.getGameINLINE().isFinalInitialized()` と指導者を先に確かめる
- 社会制度・建物がすでに持つ「文明全体に効く数値」は、志向にも同じ名前のタグで持たせ、既存の `CvPlayer::change〜()` で適用する。説明文は社会制度のテキストキーを流用する（キーは `[ICON_BULLET]` 始まりで改行を含まないので、コード側で改行と字下げ 2 文字を付ける）
- 「首都の産出 +X%」系（`CapitalYieldModifiers` など）は計算のたびに現在の首都を参照するので、遷都しても自動で移る
- **価値がゲーム速度や開始時代で変わる値（開始時の資金など）は、固定値の XML タグにしない**。志向の Python フック（`PyOnGameStart` など。`EntryPoints\CvTraitInterface.py`）で計算式として書く
- **DLL のタグか Python のフックか**: 1 つの文明だけが使う、出来事で 1 回起きる効果（都市の建設・獲得、技術、宣戦、ゲーム開始など）は、志向の Python フックと `CvHookUtils.py` の共通関数で書く。複数の文明で使い回す数値や仕組みは DLL のタグにする
  - DLL のタグにするもの: ゲーム中ずっと効く数値（産出・戦闘力・維持費・移動力など）、タイルごと・戦闘ごと・産出の計算のたびに呼ばれるもの（Python では重い）、計算の途中に入るもの（建物の残存率など）、許可を決めるもの（生産・通行・緊急生産）、状態を保存するもの
  - Python のフックの説明文は自動で出ないので、志向の `Help` に手で書く。フックは式を書いた志向を持つプレイヤーにだけ呼ばれる
  - 動作確認済みの既存のタグ（`bConquestHeal` など）は Python に移さない

### 検証用データ

**動作確認は検証用データで行い、既存の Info（日本・兵舎・専制政治など）には検証用の値を入れない。** 新しいタグを足したら、検証用の項目（文明・指導者・志向は `docs/test-data.md`、それ以外は `docs/test-data-other.md`）に値を入れて確かめる。検証用の項目は各ファイルの末尾にあり、テキストは `CIV4GameText_ClaudeSample_Test.xml`（説明文は共通の `TXT_KEY_CLAUDE_TEST_PEDIA`）。アイコン・グラフィックは複製元のものを使う。

- 新しい Info は既存の番号を変えないため常に各ファイルの末尾に足す。末尾に目印のコメント（`Entries of the civ-design branch go below this line`）があるファイルでは、`master` の項目は目印の**上**、`civ-design` の項目は**下**に足す

## git

- **ブランチ**: `master` がベース（DLL・Schema・Python の機能、タグ、検証用データ）。実在の文明のデザインは `civ-design`。機能は `master` で実装・コミットしてから `civ-design` で `git merge master` する。`civ-design` では `CvGameCoreDLL/`・Schema・DLL を変更しない。詳細は `docs/git-workflow.md`
- コミットの前に今のブランチを確かめる。ブランチを切り替えたら配置し直し、キャッシュも消す
- `2b1bf36` が無改造の公式ソース（`git diff 2b1bf36 -- CvGameCoreDLL`）。Unofficial 3.19 Patch 1.6 を取り込んである（`UNOFFICIAL_PATCH` の印）
- Edit ツールは、書き換えた既存行の**行末の空白・タブを消す**ことがある。編集後は `git diff` で意図しない削除行がないか確認し、あれば元に戻す
- `Assets/CvGameCoreDLL.dll` も git で管理する。DLL を作り直したら、元になったソース変更と同じコミットに含める
- `core.autocrlf=false`。既存ファイルは CRLF なので、新しいテキストファイルも CRLF で作る
- リモートは `origin`（非公開）。**コミットはユーザーの指示があったときに行い、そのあとの push は Claude が行ってよい**

## 詳細ドキュメント（docs/）

該当する作業のときだけ、必要な部分を読む。

| ファイル | 内容 |
|---|---|
| `docs/xml-tags.md` | 追加した XML タグの索引。仕様は `docs/tags/` の分野ごとのファイル（タグ名で Grep する） |
| `docs/xml-rules.md` | XML の読み込み順、XML キャッシュと `uiFlag` の現在の値、テキストの書き方 |
| `docs/build-env.md` | ビルド・check_refs・配置・ツールチェーンの詳細 |
| `docs/git-workflow.md` | ブランチ運用とマージの目印 |
| `docs/test-data.md`・`test-data-other.md` | 検証用データの中身（文明・指導者・志向／それ以外） |
| `docs/city-radius.md` | 都市圏の半径（都市圏のタイルを扱うコードを書くとき） |
| `docs/spells.md` | 呪文 |
| `docs/python-screens.md` | 社会制度の画面、ペディア（画面番号の落とし穴）、Mod に入れている Python の一覧 |
| `docs/ai-hidden-behavior.md` | AI の隠れた態度補正・宣戦の判定・狙う文明と都市・世界遺産・難易度補正 |
| `docs/test-log.md` | 動作確認の記録 |
| `docs/civ-design.md` | 実在の文明のデザイン（`civ-design` ブランチだけにある） |

- タグを足したら `docs/tags/` の該当ファイルと `docs/test-data*.md` を更新する（項目の形は `docs/xml-tags.md`）。ユーザーが動作確認したら `docs/test-log.md` に 1 行足す（コミットの前）
- docs のファイルも UTF-8・CRLF
