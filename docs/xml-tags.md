# この Mod で追加した XML タグ（索引）

タグの仕様は `docs/tags/` に分野ごとに分けてある。**丸ごと読まず、タグ名で Grep してから該当の行だけ読む**（例: `Grep "iCityRadiusChange" docs/tags`）。1 行が 1 つのタグ（または関連するタグの組）の仕様。

| ファイル | 内容 |
|---|---|
| `docs/tags/trait-economy.md` | 志向: 産出・商業・幸福・健康・偉人・黄金時代・交易路 |
| `docs/tags/trait-military.md` | 志向: ユニット・戦闘・征服・昇進・経験値・維持費 |
| `docs/tags/trait-territory.md` | 志向: 地形・地形の特徴・都市・都市圏・国境・移動・蛮族・資源の発見 |
| `docs/tags/trait-rules.md` | 志向: 社会制度・技術・陳腐化・外交・固有ユニット・緊急生産・`ConditionalFreeTraits`・Python フック（`Py...`） |
| `docs/tags/espionage.md` | 諜報任務: 志向による使用の制限（`PrereqTrait`・`DisableTrait`）、この Mod で足した任務（要人の殺害・誘拐） |
| `docs/tags/promotion.md` | 昇進・呪文 |
| `docs/tags/building-civ.md` | 建物・文明 |
| `docs/tags/improvement-civic.md` | 施設・作業、社会制度・資源・ユニット・イベント |
| `docs/tags/misc.md` | ゲームオプション、タグのない機能（地域区分など）、志向の付け外しの仕組み、ゲームルール（`GlobalDefinesAlt.xml`）、BtS 本体の修正（Unofficial Patch を含む）、画面の表示の追加、この Mod で追加した志向 |

- 各表の 1 列目はタグを書く XML ファイル。複数のファイルに同じタグがあるものは、最初のファイルの分野に書く（例: 志向と社会制度にある `UnitCombatUpgradeDiscounts` は志向のファイル）
- 新しいタグの行は、該当する分野のファイルの表の末尾に足す。どの分野にも合わない志向のタグは `trait-rules.md` に足す
- 1 ファイルが 40KB を超えたら、さらに分ける（この索引も直す）
