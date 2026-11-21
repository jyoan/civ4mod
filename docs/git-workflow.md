# git とブランチ運用の詳細

CLAUDE.md の「git」の補足。マージするとき、`civ-design` で作業するときに読む。

## ブランチ

- `master` がベース（DLL・Schema・Python の機能、タグ、検証用データ）。実在の文明のデザイン（日本の武士道など）は `civ-design` に入れる。`civ-design` は別の Mod（`Mods\better_trait_mod`）として配置する
- 機能を足すときは `master` で実装してコミットし、そのあと `civ-design` で `git merge master` する。文明のデザインの途中で新しいタグが要るとわかったときも、先に `master` に入れる
- `civ-design` では `CvGameCoreDLL/`・Schema・`Assets/CvGameCoreDLL.dll` を変更しない（DLL の衝突を防ぐ）
- `build.bat` が配置するのは**今のブランチ**の `Assets\` なので、ブランチを切り替えたら配置し直し、Info の数が違うのでキャッシュも消す
- `civ-design` のテキストは `CIV4GameText_ClaudeCivs.xml`、記録は `docs/civ-design.md` に書く（`master` の追記と衝突しないように）。`docs/test-log.md` は `.gitattributes` の `merge=union` で両方の追記が残る

## Info ファイルの末尾の目印

志向や昇進などの Info は両方のブランチで各ファイルの末尾に足すので、末尾の閉じタグの前に目印のコメントを置いてある:

```
<!-- Entries of the civ-design branch go below this line, entries of master above it -->
```

- **`master` の項目は目印の上、`civ-design` の項目は目印の下**に足す。両方の変更の間に目印の行が挟まるので、マージが自動で済む
- 目印のないファイルに `civ-design` で項目を足すときは、先に `master` で目印を置く
- それでも衝突したら、`master` のファイルに `civ-design` の項目を目印の下に足して作り直す
- `master` の Info が増えると `civ-design` の Info の番号がずれるので、古いセーブは使えなくなることがある。マージ後はキャッシュを消す

## その他

- `2b1bf36` が無改造の公式ソース。改造の全差分は `git diff 2b1bf36 -- CvGameCoreDLL` で見られる
- Unofficial 3.19 Patch 1.6 を取り込んである（`UNOFFICIAL_PATCH` の印）。取り込まなかった部分と、この Mod の改変と重なった箇所は `docs/tags/misc.md` の「BtS 本体の修正」
- リモートは `origin` = https://github.com/jyoan/civ4mod（非公開）
