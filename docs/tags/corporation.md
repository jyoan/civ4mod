# 企業のタグ

2026-11-28 に作った。`CIV4CorporationInfo.xml` は BtS のものを Mod に入れた（Schema は `GameInfo\CIV4GameInfoSchema.xml`）。

## `CorporationInfo/PrereqOrTraits`

- ファイル: `CIV4CorporationInfo.xml`
- 志向のどれかを持つプレイヤーだけが、その企業を設立できる（オランダの東インド会社のため。2026-11-28）。
- 本社の建物を建てられるか（`CvCity::canConstruct` の `getFoundsCorporation`。大商人などの行動「本社を建てる」）で判定する。
- 本社を征服で得る・重役で広めることは止めない。
- 企業の説明（ペディア・マウスオーバー）に「必要: 〜志向」（`buildPrereqOrTraitsString`）。
- 志向は企業より先に読み込まれる。`CvCorporationInfo` はストリーム版の `read` / `write` を持たないので、キャッシュの形式は変わらない。
- Python には公開していない（`CyInfoInterface3.cpp` がコンパイラの上限のため）
