# ユニットのタグ

### `UnitInfo/PrereqOrTraits`（`BuildingInfo`・`CivicInfo` にも同名タグ）

- ファイル: `CIV4UnitInfos.xml`、`CIV4BuildingInfos.xml`、`CIV4CivicInfos.xml`
- `<TraitType>` を何個でも書ける。
- **どれか 1 つの志向を持つ**プレイヤーだけが生産・建設・採用できる（`CvPlayer::hasPrereqOrTrait()`。指導者・文明どちらの志向でも可）。
- ユニット・建物は、志向を持たないプレイヤーの生産候補に出ない（`canTrain` / `canConstruct` の `bTestVisible` より前で判定）。
- 社会制度は採用できず（`canDoCivics`）、社会制度の画面の一覧にも出ない（`CvCivicsScreen.py`。技術の前提を外す建物の `CivicOption` でも志向の前提は外れない）。
- 止めるのは生産・建設・採用の判定だけで、無料で配られるもの（開始ユニット、文明の無料建物、技術の無料ユニット、不思議の無料建物、イベント）には効かない。
- アップグレード先と偉人による建設は生産の判定を通るので制限される。
- 説明文は「必要条件: A または B」（既存の `TXT_KEY_REQUIRES` / `TXT_KEY_OR`。志向名はリンク）で、ペディアでは常に、ゲーム中は条件を満たさないときだけ表示（`CvGameTextMgr::buildPrereqOrTraitsString`）。
- そのつど判定するので既存のセーブにも効く。
- Schema の位置はユニット・建物が `PrereqBonuses` の後、社会制度が `TechPrereq` の後。
- ユニットの Schema（`CIV4UnitSchema.xml`）はこのために Mod に追加した

### `UnitInfo/iPrereqPopulation`、`PrereqCultureLevel`、`PrereqOrTerrains`、`PrereqOrFeatures`、`PrereqRawVicinityBonuses`、`PrereqVicinityBonuses`、`PrereqOrImprovements`

- ファイル: `CIV4UnitInfos.xml`
- ユニットの都市の前提。
- 建物の同名タグ（建物の節）と同じ書式・同じ判定で、判定（`CvCity::hasPrereqOrTerrains` など）と説明文（`CvGameTextMgr::buildPrereqVicinityString`・`buildPrereqCityLevelString`）は建物と共通。
- 地形・地形の特徴・未開発の資源は満たさないと生産の一覧に出ない。
- 開発済みの資源・施設・文化レベル・人口は灰色で出る（`CvCity::hasCityPrereqs`。`canTrain` の `bTestVisible`）。
- 人口は生産の列に入れるときだけ確かめる（`bContinue`）。
- ほかは生産中も確かめ、満たさなくなると列から外れる。
- アップグレードも BtS と同じく、そのユニットを生産できる都市（`CvUnit::getUpgradeCity` の `canTrain`）が要るので、都市の中・近くでアップグレードするときは前提を満たす都市が要る。
- AI は `canTrain` を通るので前提を守る。
- そのつど判定するのでセーブデータは変わらない。
- 説明文は建物と同じキー（`TXT_KEY_BUILDING_PREREQ_VICINITY` など）で、ペディアでは常に、ゲーム中は満たしていない条件だけ。
- Python は `CvUnitInfo` の同名の関数（`getPrereqPopulation()`、`getNumPrereqOrTerrains()` / `getPrereqOrTerrain(i)` など）。
- キャッシュの形式: `CvUnitInfo` `uiFlag=2`

### `UnitInfo/iStationedAttitudeChange`

- ファイル: `CIV4UnitInfos.xml`
- 2026-11-24。諸子百家の張儀（縦横家）。
- そのユニットがほかのプレイヤーの都市のタイルにいる間、その都市の持ち主（AI）のユニットの持ち主への態度 +値（負なら悪化）。
- 都市ごと・ユニットごとに足し合わせる。見えないユニット（スパイと同じ `InvisibleType`）も数える（`CvPlayerAI::AI_getStationedUnitAttitude`。`AI_getAttitudeVal` に入る）。
- 外交画面の態度の内訳に `TXT_KEY_MISC_ATTITUDE_STATIONED_UNITS`（「+3: 都市に滞在する使節による印象」）。
- ユニットの説明（ペディア・生産の候補・マップ上のユニット）に `TXT_KEY_UNIT_STATIONED_ATTITUDE`（「ほかの文明の都市にいる間、その文明の態度+3」）。
- 都市の様子は既存の `bInvestigate`（旧スパイ）、強制和平は呪文（`reqInWarEnemyCity`・`spellForcePeaceWithCityOwner`。`docs/spells.md`）。
- `CvUnitInfo` の `uiFlag` を 5 にした。
- Python: `CvUnitInfo.getStationedAttitudeChange()`

### `UnitInfo/iUpgradeDiscount`

- ファイル: `CIV4UnitInfos.xml`
- 2026-11-26（シャルルマーニュの聖騎士の案）。
- そのユニットからアップグレードするときの費用の割引率 %（100 で無料。マイナスで割増）。
- 昇進の割引と志向・社会制度の `UnitCombatUpgradeDiscounts` に**足し算**して 1 回で引く（`CvUnit::upgradePrice`）。合計 100% 以上で無料。
- `UnitCombatUpgradeDiscounts` と違い、兵科のないユニット（偉人など）にも効く。
- Python の `getUpgradePriceOverride` が値を返したときは、そちらが優先される（BtS どおり）。
- ユニットの説明（ペディア・生産の候補・マップ上のユニット）に `TXT_KEY_UNIT_UPGRADE_COST_MODIFIER`（「アップグレードに必要な[ICON_GOLD] -100%」）。
- キャッシュの形式: `CvUnitInfo` `uiFlag=6`。
- Python: `CvUnitInfo.getUpgradeDiscount()`

### `UnitInfo/iGreatWorkOccupationTurns`

- ファイル: `CIV4UnitInfos.xml`
- 2026-11-28（アステカの労働者の生贄）。
- 偉人の傑作（`iGreatWorkCulture`）を作ったときの、都市の反乱（占領）の期間の扱い。
- 0（省略時）は BtS どおり反乱を終わらせる。N（1 以上）なら反乱の期間を N ターン短くするだけ（`CvUnit::greatWork`）。
- 傑作の文化の量と文化の更新のタイマーは BtS のまま。
- ユニットの説明（ペディア・生産の候補）の傑作の行の下に `TXT_KEY_UNIT_GREAT_WORK_OCCUPATION`（「大作は反乱を終わらせず、反乱の期間を N ターン短くする」）。
- 偉人の 3 つの任務（`iBaseHurry`・`iBaseTrade`・`iGreatWorkCulture`）は BtS でも偉人かどうかを見ないので、値を書けば労働者など偉人でないユニットも使える（新しいコードは不要）。
