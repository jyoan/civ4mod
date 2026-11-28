# 昇進のタグ: 戦闘力

2026-11-28 に `docs/tags/promotion.md` から分けた。戦闘の相手・場所・状態による戦闘力の % のタグ。戦闘の結果（金・捕獲・経験値など）とそのほかの昇進のタグは `docs/tags/promotion.md`。

### `PromotionInfo/iCombatPercentInBorders`

- ファイル: `CIV4PromotionInfos.xml`
- FFH2 と同じ名前（Civ5 のショショーニ）。
- 自国の領土（ユニットのいるタイルの持ち主が自分）にいるとき戦闘力 ○%。
- 攻撃でも防御でも、そのユニット自身のいるタイルで判定する（`CvUnit::getExtraCombatPercent` に足すので、ユニットの説明の「戦闘力 +○%」と戦闘の勝率の内訳にも含まれる）。
- ユニットごとの合計 `CvUnit::m_iCombatPercentInBorders` はセーブに保存せず、読み込み時に昇進から計算する。
- 説明文 `TXT_KEY_PROMOTION_COMBAT_PERCENT_IN_BORDERS`（「自国の領土で戦闘力+15%」）

### `PromotionInfo/iCombatPercentOverseas`

- ファイル: `CIV4PromotionInfos.xml`
- 海外（ユニットのいるタイルが、持ち主の首都と別の地域）で戦闘力 %。
- `iCombatPercentInBorders` と同じく、攻撃でも防御でも**ユニット自身のタイル**で判定する（`CvUnit::getExtraCombatPercent`）。
- 外洋はどの地域にも属さないので海外にならない。
- セーブしない値で、`updatePromotionCounts` で数え直す。
- キャッシュの形式: `CvPromotionInfo` `uiFlag=13`

### `PromotionInfo/iOtherReligionCombatPercent`、`iStateReligionCityAttack`

- ファイル: `CIV4PromotionInfos.xml`
- 異教徒との戦闘のボーナス。
- `iOtherReligionCombatPercent`: 持ち主に国教があり、戦う相手の文明の国教がそれと違う（国教なしも違う扱いなので蛮族にも効く）ときの戦闘力 %。
- 攻撃でも防御でも効く（兵科ボーナスと同じく、両方のユニットがわかっている戦闘で `CvUnit::maxCombatStr` に足す）。
- `iStateReligionCityAttack`: 持ち主の国教が広まっている都市を攻撃するときの都市攻撃 %（`iCityAttack` と同じ位置）。
- 持ち主に国教がなければどちらも効かない。
- 戦闘のオッズの説明に行を出す（BtS の `CombatDetails` は exe と共有の構造体なので変えていない）。
- キャッシュの形式: `CvPromotionInfo` `uiFlag=14`

### `PromotionInfo/BuildingCombatPercents`、`iBuildingCombatMaxPercent`

- ファイル: `CIV4PromotionInfos.xml`
- 自国の建物の数に応じた戦闘力 %。
- `<BuildingCombatPercent>` を何個でも書け、各項目は `BuildingClassType` か `SpecialBuildingType` のどちらか（両方書けば建物クラス）+ `iCombatPercent`（建物 1 つあたり）+ `bStateReligion`（省略時 0）。
- 数えるのは持ち主の全都市の建物（`CvPlayer::getNumActiveBuildingsForCombat`）。
- 特殊建造物は、既定の建物（`DefaultBuilding`）がその特殊建造物に属する建物クラスをすべて合計する（`SPECIALBUILDING_TEMPLE` なら 7 宗教の寺院）。
- **陳腐化した建物は数えない**（`CvPlayer::getBuildingClassCount` が `processBuilding` で陳腐化した建物を除くため）。
- 陳腐化させない志向（`bIgnoreBuildingObsolescence`・`NoObsoleteBuildingClasses`・`NoObsoleteSpecialBuildings`）で残っている建物は数える。
- `bStateReligion`: 既定の建物の宗教（`ReligionType`、なければ `PrereqReligion`）が持ち主の国教と同じ建物クラスだけを数える。
- 国教がなければ 0、宗教のない建物クラスに付けると常に 0。
- `iBuildingCombatMaxPercent`（省略時 -1 = 上限なし）: その昇進の項目の合計の上限。
- 複数の昇進を持てば、昇進ごとに上限をかけてから足す。
- 攻撃でも防御でも、場所を問わず効く（`CvUnit::getExtraCombatPercent` に足すので、ユニットの説明の「戦闘力 +○%」と戦闘の勝率の内訳に含まれる）。
- 数はそのつど数える（`CvUnit::getBuildingCombatPercent`）ので、建物が増減するとすぐ変わる。
- 持っている昇進の数（`m_iBuildingCombatCount`）はセーブせず、`updatePromotionCounts` で数え直す。
- 建物クラス・特殊建造物は昇進より後に読み込まれるので `readPass3` で番号に変える（昇進の `readPass3` を呼ぶ位置を、資源の後から建物クラスの後に移した）。
- 説明文 `TXT_KEY_PROMOTION_BUILDING_COMBAT`（「自国の兵舎 1つにつき戦闘力+5%」。建物クラスは既定の建物へのリンク、特殊建造物はリンクなし）・`_STATE_RELIGION`（「（国教のもの）」）・`_MAX`（「自国の建造物による戦闘力は最大+20%」）、ユニットのマウスオーバーの戦闘力の行に `TXT_KEY_UNIT_BUILDING_COMBAT_NOW`（「（うち建造物から+8%）」）。
- AI の評価は変えていない（戦闘力には含まれる）。
- キャッシュの形式: `CvPromotionInfo` `uiFlag=19`。
- Python は `CvPromotionInfo.getNumBuildingCombatPercents()` / `getBuildingCombatBuildingClass(i)` / `SpecialBuilding(i)` / `Percent(i)` / `isBuildingCombatStateReligion(i)` / `getBuildingCombatMaxPercent()`、`CyUnit.getBuildingCombatPercent()`

### `PromotionInfo/Encirclement`（`iSidePercent`・`iRearPercent`・`iMaxUnitsPerPlot`）

- ファイル: `CIV4PromotionInfos.xml`
- 2026-11-26。包囲と支援（Civ6 の側面攻撃・支援ボーナスを Civ4 向けにしたもの）。シャカの「牛の角の陣」用。
- 中心のタイルの周囲 8 タイルのうち、攻撃側のいるタイルを除く 7 タイルにいる、自分と同じチームのユニットを数える。中心は、攻撃するときは守る側のタイル、守るときは自分のタイル。
- 攻撃側のいるタイルから見て中心の向こう側の 3 タイル（135° と 180°）が背後で、1 体につき `iRearPercent`。残りの 4 タイルが横で、1 体につき `iSidePercent`。
- 1 タイルで数えるのは `iMaxUnitsPerPlot` 体まで（省略時 -1 = 上限なし）。全体の上限はない（1・2・10 なら横 4 × 10% + 背後 3 × 20% で最大 +100%）。
- 数えるユニット: 戦闘力が 1 以上（`canFight`）、自分と同じ種類（`DomainType`。陸なら陸、海なら海。航空は数えない）、輸送中でない（`isCargo`）。移動を終えたユニット、要塞化・負傷したユニットも数える。同盟国・属国のユニットは数えない。
- 攻撃でも防御でも同じ値が効く。攻撃側の値は `CvUnit::maxCombatStr` の攻撃側の補正（未知の守備側への見積もりにも入る）、守る側の値は既知の攻撃側に対する補正に足す。都市への攻撃、都市での防御にも効く。
- 複数の昇進を持てば % は合計し、1 タイルの体数の上限は最も大きいもの（どれかが -1 なら上限なし）。
- 数はそのつど数える（`CvUnit::getEncirclementPercent`）。持っている昇進の数（`m_iEncirclementCount`）はセーブせず、`updatePromotionCounts` で数え直す。
- 戦闘の勝率の内訳に `TXT_KEY_COMBAT_PLOT_ENCIRCLEMENT_MOD`（攻撃側「包囲 +30%（横 10%、後 20%）」）・`TXT_KEY_COMBAT_PLOT_SUPPORT_MOD`（守る側「支援 …」）。昇進の説明文 `TXT_KEY_PROMOTION_ENCIRCLEMENT`・`_MAX`。
- BtS の `FlankingStrikes`（騎乗ユニットの側面攻撃）とは別物なので、名前を `Encirclement` にした。
- AI の評価は変えていない（勝率の計算には含まれる）。
- キャッシュの形式: `CvPromotionInfo` `uiFlag=20`。
- Python は `CvPromotionInfo.getEncirclementSidePercent()` / `getEncirclementRearPercent()` / `getEncirclementMaxUnitsPerPlot()`、`CyUnit.getEncirclementPercent(pCenter, pFront)`（中心のタイルと攻撃側のタイル）

### `PromotionInfo/iGoldenAgeAdvantageCombatPercent`

- ファイル: `CIV4PromotionInfos.xml`
- 2026-11-26（ペルシャの案）。
- ユニットの持ち主が黄金期で、戦闘の相手のプレイヤーが黄金期でないときの戦闘力 %（`CvUnit::getGoldenAgeAdvantageCombatModifier`。`iOtherReligionCombatPercent` と同じく、攻撃・防御の両方で `maxCombatStr` に入る）。
- 蛮族は黄金期にならないので、自分が黄金期なら常に対象。
- 戦闘のたびに昇進の合計を数える（ユニットに値を持たせず、セーブしない）。
- 戦闘の内訳に「黄金期でない相手（自国は黄金期）: +N%」（`TXT_KEY_COMBAT_PLOT_GOLDEN_AGE_ADVANTAGE_MOD`）。
- 説明文 `TXT_KEY_PROMOTION_GOLDEN_AGE_ADVANTAGE_COMBAT`。
- 昇進のキャッシュの `uiFlag` 21。
- Python: `CvPromotionInfo.getGoldenAgeAdvantageCombatPercent()`

### `PromotionInfo/AttackFromTerrains`、`AttackFromFeatures`

- ファイル: `CIV4PromotionInfos.xml`
- 攻撃する側のいるタイルの地形・地形の特徴による攻撃の戦闘力 %（2026-11-28。ハンニバル）。
- 書式は `<AttackFromTerrain>` が `TerrainType` + `iAttackFromTerrain`、`<AttackFromFeature>` が `FeatureType` + `iAttackFromFeature`。
- 山岳は `TERRAIN_PEAK` で書く（その下の地形は数えない）。
- 丘陵は下の地形と `TERRAIN_HILL` の両方を数える。地形の特徴の分も足す。
- 持っている昇進の合計（`CvUnit::getAttackFromPlotPercent`。昇進の数は `m_iAttackFromPlotCount` で覚え、セーブしない）。
- 戦闘力の計算（`CvUnit::maxCombatStr` の攻撃する側の補正）に入るので、AI の勝率にも入る。
- 勝率の内訳に `TXT_KEY_COMBAT_PLOT_ATTACK_FROM_MOD`、昇進の説明に `TXT_KEY_PROMOTION_ATTACK_FROM`。
- Python は `CvPromotionInfo.getAttackFromTerrainPercent(i)` / `getAttackFromFeaturePercent(i)`、`CyUnit.getAttackFromPlotPercent(pPlot)`。
- キャッシュの形式: `CvPromotionInfo` `uiFlag=22`

### `PromotionInfo/BonusCombatPercents`、`iBonusCombatMaxPercent`

- ファイル: `CIV4PromotionInfos.xml`
- 持ち主がその資源を持つ数 1 つにつき戦闘力 %（2026-11-28。モンゴル）。
- 書式は `<BonusCombatPercent>` が `BonusType` + `iCombatPercent`。
- 数は `BonusAffinities` と同じく首都の交易網の中の数（`CvPlayer::getNumAvailableBonuses`。輸入も含む）。
- `iBonusCombatMaxPercent` は昇進ごとの合計の上限（0 なら上限なし）。昇進が複数あれば、それぞれの値を足す。
- 追加の戦闘力（`CvUnit::getExtraCombatPercent`）に含めるので、そのつど計算され、資源の数が変わるとすぐに変わる。
- ユニットのマウスオーバーに `TXT_KEY_UNIT_BONUS_COMBAT_NOW`（「（うち資源から +N%）」）。
- 昇進の説明に `TXT_KEY_PROMOTION_BONUS_COMBAT`・`TXT_KEY_PROMOTION_BONUS_COMBAT_MAX`。
- 資源は昇進より後に読み込まれるので、`CvPromotionInfo::readPass3` で番号に変える。
- Python は `CvPromotionInfo.getBonusCombatPercent(i)` / `getBonusCombatMaxPercent()`、`CyUnit.getBonusCombatPercent()`。
- キャッシュの形式: `CvPromotionInfo` `uiFlag=22`

### `PromotionInfo/iWarWearinessCombatPercent`、`iWarWearinessCombatMaxPercent`

- ファイル: `CIV4PromotionInfos.xml`
- その相手との厭戦感情による戦闘力 %（2026-11-28。ブーディカ。最初は志向のタグにしたが、コミット前に昇進に移した）。
- 厭戦感情（`CvTeam::getWarWeariness`）を、それが生むはずの不満の割合（%）にする（`CvPlayer::getWarWearinessAngerPercentAgainst`）:
  - `updateWarWearinessPercentAnger` と同じ計算だが、相手のチームだけ
  - 相手の `iEnemyWarWearinessModifier` は入れない
  - 自分の志向・社会制度・建物の `iWarWearinessModifier` をかける前（-100% でも数える）
  - 厭戦感情は不満が 0 でも数え続け、戦争中は少しずつ減り、和平で 99% 減る
- その割合 × `iWarWearinessCombatPercent` / 100 が戦闘力 %。`iWarWearinessCombatMaxPercent` が上限（0 なら上限なし）。
- どちらも持っている昇進の合計（`CvUnit::getWarWearinessCombatModifier`。セーブせず読み込み時に数え直す）。
- 攻撃・防御のどちらでも効き、勝率の内訳に `TXT_KEY_COMBAT_PLOT_WAR_WEARINESS_MOD`。
- 昇進の説明に `TXT_KEY_PROMOTION_WAR_WEARINESS_COMBAT`・`_MAX`。
- Python は `CvPromotionInfo` の getter、`CyUnit.getWarWearinessCombatModifier(ePlayer)`、`CyPlayer.getWarWearinessAngerPercentAgainst(ePlayer)`。
- キャッシュの形式: `CvPromotionInfo` `uiFlag=22`

### `PromotionInfo/iStrongerEnemyCombatPercent`

- ファイル: `CIV4PromotionInfos.xml`
- 軍事力が持ち主より上の文明との戦闘力 %（2026-11-28。フリードリッヒ。最初は志向のタグにしたが、コミット前に昇進に移した）。
- 軍事力は `CvPlayer::getPower`（ゲーム情報の画面の軍事力）。蛮族と同じチームは対象外。
- 持っている昇進の合計（`CvUnit::getStrongerEnemyCombatModifier`）。攻撃・防御のどちらでも効く。
- 勝率の内訳に `TXT_KEY_COMBAT_PLOT_STRONGER_ENEMY_MOD`、昇進の説明に `TXT_KEY_PROMOTION_STRONGER_ENEMY_COMBAT`。
- Python は `CvPromotionInfo.getStrongerEnemyCombatPercent()`、`CyUnit.getStrongerEnemyCombatModifier(ePlayer)`。
- キャッシュの形式: `CvPromotionInfo` `uiFlag=22`

### `PromotionInfo/iFormerMasterCombatPercent`

- ファイル: `CIV4PromotionInfos.xml`
- 持ち主のチームの、現在とかつての宗主国のユニットとの戦闘力 %（2026-11-28。ワシントン。志向が `FreePromotionSets` の `bFreePromotionAllUnitCombats` で全戦闘ユニットに配る）。
- 属国は宗主国と戦争できないので、実際に効くのはかつての宗主国。
- かつての宗主国の記録: チームごとに、属国の状態が終わったとき（独立、降伏の解消、解放）にその宗主国を記録し、ずっと残す（`CvTeam::m_abFormerMaster`、`isFormerMaster`・`isMasterOrFormerMaster`。セーブに保存、`CvTeam` の `uiFlag=3`。古いセーブは記録なし）。
- 持っている昇進の合計（`CvUnit::getFormerMasterCombatModifier`）。攻撃・防御のどちらでも効く。
- 勝率の内訳に `TXT_KEY_COMBAT_PLOT_FORMER_MASTER_MOD`、昇進の説明に `TXT_KEY_PROMOTION_FORMER_MASTER_COMBAT`。
- Python は `CvPromotionInfo.getFormerMasterCombatPercent()`、`CyUnit.getFormerMasterCombatModifier(ePlayer)`、`CyTeam.isFormerMaster(eTeam)`・`isMasterOrFormerMaster(eTeam)`。
- キャッシュの形式: `CvPromotionInfo` `uiFlag=24`
