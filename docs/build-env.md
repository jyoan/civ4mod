# ビルド環境・配置の詳細

CLAUDE.md の「ビルド」「配置」の補足。ビルドや配置で困ったとき、ツールチェーンに触れるときに読む。

## ビルド

- 出力: `CvGameCoreDLL\Release\CvGameCoreDLL.dll`（と `.pdb`）。成功時に `Assets\CvGameCoreDLL.dll` へコピーし、続けて `<BTS>\Mods\claude_sample\` へ配置する
- 全ビルド約 2 分、変更なしなら 0 秒。ヘッダを 1 つでも変えると PCH 経由で全再コンパイルになる
- build.bat はビルド後に依存 DLL を検査し、MSVCR71.dll に依存しない／MSVCR80 以降に依存する DLL は失敗扱いにしてコピーしない
- `Makefile` は `CvGameCoreDLL.vcproj` の **Final Release**（製品版 DLL と同じ構成）を再現したもの。`CvGameCoreDLL.vcproj` は設定の参照用。新しい Visual Studio で開くと別のコンパイラ用に変換されるので開かない
- Debug 構成は未整備（Release のみ）。Release では `FAssert` は無効
- ビルド対象外: `CvTextScreens.cpp`（vcproj 未登録）、`CvGameCoreDLL.rc`（MFC の afxres.h が必要。中身はバージョン情報のみ）

## check_refs.ps1

build.bat がビルド後（配置の前）に実行する（約 2 秒。`build.bat deploy` では実行しない）。エラーがあっても配置は続け、終了コードも変えない。

調べるもの: ゲームが読む XML（`<BTS>` → `<WL>` → `<CIV4>` の順で最初のもの）の `<Type>` と `GlobalTypes.xml`（`ARTSTYLE_` / `CONTACT_` / `FLAVOR_` / `FORMATION_` など）、テキストの `<Tag>` と照合して、

- 定義されていない名前（接頭辞が既知のもの）、無いテキストキー、`<Type>` の重複
- 要素の種類と違う名前（警告。`<TraitType>` に `UNIT_` など）
- Mod のテキストキーの重複（警告）
- 志向の `ConditionalFreeTraits` の輪（警告。条件が外れても志向が残る）
- 同じ引数を 2 回使うテキスト
- Python と XML の `Py...` の式の中の引用符で囲んだ名前（コメント行を除く）
- ソースの形式（ASCII のみ・CRLF）

本体からコピーしたファイルにある本体と同じ誤りは数えるだけで表示しない。

## 配置

- `Assets\` → `Mods\claude_sample\Assets\` を robocopy `/MIR` で**ミラー同期**する。リポジトリから消したファイルは Mod 側からも消える（`*EXTRA File` と表示される）
- `claude_sample.ini`（`ModularLoading = 0` など）→ `Mods\claude_sample\claude_sample.ini` を上書きコピーする。Mod フォルダ直下のその他のファイル（`claude_sample.ini.bak` など）には触れない
- `Mods\claude_sample\` は普通のフォルダ（ジャンクションではない）
- Civ4 の起動中に、それでもファイルがロックされていた場合は `DEPLOY FAILED`（終了コード `1`）。ゲームを終了してから `build.bat deploy` を実行する
- Civ4 は DLL を起動時に読み込むので、起動中のゲームに新しい DLL は反映されない（反映には再起動が必要）
- `<BTS>\Mods` が無い環境では警告を出して配置をスキップする（ビルドは成功扱い）
- ゲーム本体のパスと Mod 名は `build.bat` の `BTS` / `MODNAME` で定義している
- `Mods\` は Users に書き込み権限があるので、管理者権限なしで配置できる（VirtualStore へのリダイレクトも起きない）
- `<BTS>\Assets\CvGameCoreDLL.dll` は製品版 DLL。上書きしない。`<BTS>\CvGameCoreDLL\` はインストール済みの SDK で、リポジトリの `2b1bf36` と同一

## ツールチェーン（MSVC 7.1 固定）

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
- Git Bash から `link.exe /dump` を実行すると `/dump` がパスに変換されて失敗する。PowerShell から実行する
