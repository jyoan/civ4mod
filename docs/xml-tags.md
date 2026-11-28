# この Mod で追加した XML タグ（索引）

タグの仕様は `docs/tags/` に分野ごとに分けてある。**丸ごと読まず、タグ名で Grep してから該当の項目だけ読む**（例: `Grep "iCityRadiusChange" docs/tags`）。1 つの見出し（`##` か `###`）が 1 つのタグ（または関連するタグの組）の仕様で、次の見出しまでがその項目。

| ファイル | 内容 |
|---|---|
| `docs/tags/trait-economy.md` | 志向: 産出・商業・幸福・健康・偉人・黄金時代・交易路 |
| `docs/tags/trait-military.md` | 志向: ユニット・戦闘・征服・昇進・経験値・維持費 |
| `docs/tags/trait-territory.md` | 志向: 地形・地形の特徴・国境・移動・運河・道・蛮族・資源の発見 |
| `docs/tags/trait-religion.md` | 志向: 宗教の数による効果、宣教師、宗教の伝播（`PyOnOtherReligionFounded` も）、文化による不満 |
| `docs/tags/trait-cities.md` | 志向: 都市・都市圏・都市の数・文化圏・新しい都市・文化による反乱・都市圏の地形による建物の生産・世界遺産の完成時 |
| `docs/tags/trait-rules.md` | 志向: 社会制度・技術・陳腐化・固有ユニット・緊急生産・ランダムイベント・施設の固定 |
| `docs/tags/trait-diplomacy.md` | 志向: 外交（属国 `bHumanCanBeVassal`・`FoundingVassal`、情報の公開 `bReveal...`、取引 `Deals`、レンドリース `LendLease`、宣戦した側の記録と非難 `iDenounceAttitudeChange`） |
| `docs/tags/trait-hooks.md` | 志向: Python フック（`Py...`）と `CvHookUtils.py` の共通関数（暦の偉人・優先地域）、`ConditionalFreeTraits`、`bLongCountCalendar` |
| `docs/tags/espionage.md` | 諜報任務: 志向による使用の制限（`PrereqOrTraits`・`DisableTraits`）、この Mod で足した任務（要人の殺害・誘拐、任意の社会制度を押し付ける体制転覆 `bSwitchAnyCivic`、任意の宗教か国教なしを押し付ける国教転覆 `bSwitchAnyReligion`・`bSwitchNoReligion`） |
| `docs/tags/promotion.md` | 昇進 |
| `docs/tags/spell.md` | 呪文 |
| `docs/tags/building-civ.md` | 建物・文明 |
| `docs/tags/improvement-civic.md` | 施設・作業、社会制度・資源・ユニット・イベント・生産の種類・プロジェクト |
| `docs/tags/corporation.md` | 企業（`PrereqOrTraits`） |
| `docs/tags/misc.md` | ゲームオプション、タグのない機能（地域区分など）、志向の付け外しの仕組み、ゲームルール（`GlobalDefinesAlt.xml`）、BtS 本体の修正（Unofficial Patch を含む）、画面の表示の追加、この Mod で追加した志向 |

- 項目の形: 見出しはタグ（`` `TraitInfo/iFoo` ``。組なら `、` で並べる）、1 行目は `- ファイル: ` とタグを書く XML ファイル、続けて 1 文ずつ `- ` の箇条書き。列挙が長くなる文は `- 〜:` の下に `  - ` で並べる。**1 行を長くしない**（Grep が長い行を省略して表示しない）
- 複数のファイルに同じタグがあるものは、最初のファイルの分野に書く（例: 志向と社会制度にある `UnitCombatUpgradeDiscounts` は志向のファイル）
- 新しいタグの項目は、該当する分野のファイルの末尾（そのファイルが分野ごとに `##` で分かれていれば、その分野の末尾）に足す。どの分野にも合わない志向のタグは `trait-rules.md` に足す
- 1 ファイルが 40KB を超えたら、さらに分ける（この索引も直す）
