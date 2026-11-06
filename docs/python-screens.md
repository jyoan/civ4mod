# 画面とペディア（Python / DLL）

## 社会制度の画面

- 社会制度の画面（`CvCivicsScreen.py`）: 元は各区分の枠が 5 行分の固定の高さで、6 行目がはみ出していた。`updateLayout()` で一番行数の多い区分に合わせて上の枠を伸ばし、大きいアイコンと説明の枠を下にずらす（5 行以下なら元と同じ座標）。9 行以上は説明の枠を 170 ピクセル残すため行の間隔を詰める。`isCivicShown()` で、前提の志向（`PrereqOrTraits`）を持たない社会制度は一覧から外す（今採用中のものは表示）。`bFavoriteCivic` で志向が付く制度の名前は黄色（`CyPlayer.isFavoriteCivicWithTraits`）

## 志向のペディア

- カテゴリ「志向」（`CIVILOPEDIA_PAGE_TRAIT`、テキスト `TXT_KEY_PEDIA_CATEGORY_TRAIT`）。一覧の位置は最後（ヒントの下）。`listCategories` の位置がそのまま `CivilopediaPageTypes` の値として使われるので、途中に挟めない
- 志向のページ（`CvPediaTrait.py`）: 左に「指導者」「文明」（その志向を持つもの。クリックで移動）、右上に「生産可能」（`PrereqOrTraits` にその志向を書いたユニット・建物・社会制度のボタン）、右下に「特殊能力」（`CyGameTextMgr().parseTraits()` の先頭の名前行を除いたもの）、右端に志向の一覧
- DLL 側: `WIDGET_PEDIA_JUMP_TO_TRAIT`（クリックで Python の `pediaJumpToTrait`、マウスオーバーで志向の効果）、`CyGameTextMgr.parseTraits(iTrait, iCiv, bDawnOfMan)` を追加。列挙型はどちらも末尾に足した（既存の値は変わらない。`NUM_WIDGET_TYPES` は DLL 側だけの定義）
- **画面番号の落とし穴**: `CvScreenEnums.py` では `PEDIA_X = CIVILOPEDIA_PAGE_X + 200` だが、`PEDIA_HISTORY = PEDIA_HINTS + 1` が決め打ち。末尾に足したページ種別に + 200 すると `PEDIA_HISTORY` と同じ値になるので、`PEDIA_TRAIT = PEDIA_HISTORY + 1` にしている（呪文のペディアは `PEDIA_SPELL = PEDIA_TRAIT + 1`）
- 志向の名前は説明文の中で `<link=literal>` のリンクになっている（`parseTraits` の見出し、文明の説明の志向一覧）。ペディアでクリックすると志向のページへ移動する
