# 実在の文明のデザイン（civ-design ブランチ）

`master` の機能を使って、実在の文明を XML でデザインする。機能の追加は `master` に入れてからマージする（`CLAUDE.md` の git の節）。

## 文明

| 文明 | 追加した要素 | 内容 |
|---|---|---|
| `CIVILIZATION_JAPAN`（日本） | 文明の `Traits` に `TRAIT_BUSHIDO` | Civ5 の文明能力「武士道」。指導者（徳川家康）の志向は元のまま |

## 志向と昇進

| 項目 | 内容 |
|---|---|
| `TRAIT_BUSHIDO`（武士道志向、短縮名「武」） | `FreePromotionSets` で、航空以外の全戦闘ユニット（偵察・弓兵・騎乗・白兵戦・攻城・銃・装甲・ヘリコプター・海洋）に `PROMOTION_BUSHIDO` を配る |
| `PROMOTION_BUSHIDO`（武士道） | `bNoDamagePenalty` 1。`UnitCombats` は空でレベルアップでは選べない。アイコンはコマンドーの流用 |

## 確認の記録

- 2026-11-07: 日本の武士道をゲーム内で確認（ユーザーの確認）
