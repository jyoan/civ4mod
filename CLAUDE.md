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
build.bat clean      CvGameCoreDLL\Release\ を削除（配置はしない）
```

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

以下、`<CIV4>` = `C:\Program Files (x86)\CYBERFRONT\Sid Meier's Civilization 4(J)`、`<BTS>` = `<CIV4>\Beyond the Sword(J)`。

| パス | 内容 |
|---|---|
| `<BTS>\Assets\XML\`、`<BTS>\Assets\Python\` | BtS の元ファイル。Mod で変更するときはここからコピーする |
| `<CIV4>\Assets\` | 無印 Civ4(J) の元ファイル。BtS に無いもの（テキスト XML の多くなど）はこちらにある。探す順は `<BTS>` → `<CIV4>` |
| `<BTS>\Assets\CvGameCoreDLL.dll` | 製品版 DLL。**上書きしない** |
| `<BTS>\CvGameCoreDLL\` | インストール済みの SDK。リポジトリの `2b1bf36` と同一 |
| `<BTS>\Mods\claude_sample\` | **Mod の配置先**（普通のフォルダ。ジャンクションではない） |

配置（build.bat が自動で行う）:

- `Assets\` → `Mods\claude_sample\Assets\` を robocopy `/MIR` で**ミラー同期**する。リポジトリから消したファイルは Mod 側からも消える（`*EXTRA File` と表示される）
- `claude_sample.ini` → `Mods\claude_sample\claude_sample.ini` を上書きコピーする。Mod フォルダ直下のその他のファイル（`claude_sample.ini.bak` など）には触れない
- **Mod フォルダ側で直接編集しない**。編集はすべてリポジトリで行う（Mod 側の `Assets\` の変更は次の配置で消える）
- Civ4 の起動中は DLL がロックされて `DEPLOY FAILED` になる。ゲームを終了して再実行する
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

- XML 内のタグの順序は Schema の順序に合わせる
- Mod の XML は本体の同名ファイルを丸ごと置き換える（`ModularLoading = 0` のため）。`<BTS>\Assets\XML\...`（無ければ `<CIV4>\Assets\XML\...`）から同じ相対パスで `Assets\XML\...` にコピーしてから編集する（Schema も同様）
- 元の XML はすべて ASCII・CRLF。Mod 側も ASCII・CRLF を保つ
- テキスト XML（`XML\Text\`）は `encoding="ISO-8859-1"` 宣言の ASCII ファイル。日本語は `<Japanese><Text>` の中に **10 進の数値文字参照**で書く（例: 「戦士」→ `&#25126;&#22763;`）。他の言語タグ（`<English>` など）も元ファイルに倣って残す

## Python（2.4）

- Python 2.4 の構文だけを使う。`with` 文、条件式（`a if c else b`）、`except E as e`、`str.format` は使えない
- `Assets\Python\` に置いたファイルは本体の同名ファイルを置き換える。`<BTS>\Assets\Python\` から同じ相対パス（`EntryPoints\`、`Screens\` など）でコピーしてから編集する
- 元の Python はすべて ASCII・CRLF。日本語を書く必要があるなら、ファイル先頭に coding 宣言を入れる
- DLL の機能を Python に公開するには `Cy*.cpp` と `Cy*Interface*.cpp`（boost::python）に追加する

## git

- `2b1bf36` が無改造の公式ソース。改造の全差分は `git diff 2b1bf36 -- CvGameCoreDLL` で見られる
- `Assets/CvGameCoreDLL.dll` も git で管理する。DLL を作り直したら、元になったソース変更と同じコミットに含める
- `core.autocrlf=false`（ファイルはそのまま保存される）。既存ファイルは CRLF なので、新しいテキストファイルも CRLF で作る

## 動作確認の記録

- 2026-10-31: 無改造でビルドした DLL を `Mods\claude_sample` として読み込み、ゲームが起動することを確認
- 2026-10-31: 製品版 DLL と比較。依存 DLL 7 つ・エクスポート 2432 個（名前も完全一致）・リンカ 7.10・LargeAddressAware が同じ。サイズ差 8,192 バイトはバージョン情報（`.rc`、製品版は 3.1.7.0）が無い分と考えられる
