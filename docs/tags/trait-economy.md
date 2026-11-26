# 志向のタグ（経済）

産出・商業・幸福・健康・偉人・黄金時代・交易路。

## `TraitInfo/iFreeExperience`、`iMilitaryProductionModifier`、`iImprovementUpgradeRateModifier`、`iDistanceMaintenanceModifier`、`iNumCitiesMaintenanceModifier`、`iWarWearinessModifier`、`iFreeSpecialist`、`iTradeRoutes`、`iExpInBorderModifier`

- ファイル: `CIV4TraitInfos.xml`
- 社会制度の同名タグと同じ効果（新ユニットの経験値、軍事ユニット生産 %、小屋などの成長速度 %、距離／都市数による維持費 %、厭戦感情 %、全都市の無償の専門家、全都市の交易路、国境内の戦闘経験値 %）。
- 文明全体に 1 回だけ適用。
- `iFreeExperience` は社会制度と同じく**都市で生産したユニットだけ**に付く（ゲーム開始時に配られるユニットには付かない）

## `TraitInfo/YieldModifiers`、`CapitalYieldModifiers`、`CapitalCommerceModifiers`、`SpecialistExtraCommerces`

- ファイル: `CIV4TraitInfos.xml`
- 社会制度の同名タグと同じ効果（全都市／首都の産出 %、首都の商業 %、専門家 1 人あたりの商業）。
- `<iYield>` は食料・生産・商業、`<iCommerce>` は金・研究・文化・諜報の順。
- 文明全体に 1 回だけ適用

## `TraitInfo/SpecialistYieldChanges`、`SpecialistCommerceChanges`

- ファイル: `CIV4TraitInfos.xml`
- 専門家の**種類ごと**の産出・商業（三国志 Mod（The History of Three Kingdoms）の曹操の志向と同じ名前。`SpecialistExtraCommerces` は全専門家で共通）。
- `SpecialistYieldChanges` は建物の同名タグと同じ書式（`<SpecialistYieldChange>` が `SpecialistType` + `<YieldChanges>`）、`SpecialistCommerceChanges` は `<SpecialistCommerceChange>` が `SpecialistType` + `<CommerceChanges>`（金・研究・文化・諜報の順）。
- その種類の専門家と、都市に定住した偉人（無償の専門家。`getFreeSpecialistCount`）1 人につき足す。
- 産出は BtS の `CvPlayer::changeSpecialistExtraYield`（建物と同じ。セーブに入る）、商業はこの Mod の `CvPlayer::changeSpecialistTypeExtraCommerce`（種類 × 商業の配列。セーブに入る。`CvPlayer` の `uiFlag` 8）に、志向を得たとき・失ったとき（`processTrait`）に足し引きする。
- そのため既存のセーブには、ゲーム開始時からの志向の分が入らない（新しいゲームで確かめる）。
- 都市は基本の商業に足す（`CvCity::getSpecialistTypeExtraCommerce`。都市画面の商業の内訳と AI の見積もりにも足した）。
- 専門家の説明・AI の評価は `CvPlayer::specialistCommerce` に足した値を使う。
- 説明文は「科学者: +1[ICON_FOOD]」「科学者: +2[ICON_GOLD]」のように専門家ごとに 1 行（産出と商業は別の行）。
- Python は `CvTraitInfo.getSpecialistYieldChange(iSpecialist, iYield)` / `getSpecialistCommerceChange(iSpecialist, iCommerce)`、`CyPlayer.getSpecialistTypeExtraCommerce(eSpecialist, eCommerce)`

## `TraitInfo/TerrainYieldChanges`、`FeatureYieldChanges`

- ファイル: `CIV4TraitInfos.xml`
- FFH2 の文明の同名タグと同じ書式（`<TerrainYieldChange>` / `<FeatureYieldChange>` が `TerrainType` / `FeatureType` + `<Yields>`（`<iYield>` は食料・生産・商業の順））。
- その文明の領土の、その地形・地形の特徴のタイルの産出に加算する（`CvPlot::calculateTraitYieldChange`。`calculateYield` で施設・道路と同じ段階に足すので、金融志向の閾値や黄金時代の判定にも効く）。
- 地形と特徴は重なる。
- **`TERRAIN_HILL` / `TERRAIN_PEAK` は丘陵・山岳のタイル**を指す（どちらも `bGraphicalOnly` で、タイルの地形としては使われないため。丘陵のタイルは元の地形の分と丘陵の分の両方）。
- 通行できないタイル（山岳・氷河）は BtS では産出 0 なので、山岳の分と、通行できなくしている特徴（氷河）の分だけが効き、下の地形の分は効かない。
- 産出があれば `CvCity::canWork` を通るので、**山岳・氷河も都市で働かせられる**。
- `CvPlayer::getTraitTerrainYieldChange()` / `getTraitFeatureYieldChange()` がそのつど合計するのでセーブデータは変わらないが、タイルの産出の値はセーブに保存されているので、既存のセーブではタイルが計算し直されるまで反映されない。
- 説明文は社会制度の `TXT_KEY_CIVIC_IMPROVEMENT_YIELD_CHANGE`（「次のものより[生産]+1: 雪原、ツンドラ」）を流用し、地形と特徴を産出ごとに 1 行にまとめる

## `TraitInfo/TerrainYieldChangesPerEra`

- ファイル: `CIV4TraitInfos.xml`
- 時代が 1 つ進むごとの地形の産出（2026-11-19。インカの山岳の商業の案）。
- `TerrainYieldChanges` と同じ書式（`<TerrainYieldChangePerEra>` が `TerrainType` + `<Yields>`）で、値 × そのプレイヤーの今の時代（古代 0、古典 1 …）を同じタイルに足す（`CvPlot::calculateTraitYieldChange`。`TERRAIN_HILL` / `TERRAIN_PEAK` の扱いも同じ）。
- 時代が変わると領土のタイルの産出を計算し直す（`CvPlayer::setCurrentEra`）。
- 説明文は別の行（`TXT_KEY_TRAIT_YIELD_CHANGE_PER_ERA`「時代が 1 つ進むごとに、次のものより[商業]+1: 山岳」）。
- Python は `CvTraitInfo.getTerrainYieldChangePerEra(iTerrain, iYield)`

## `TraitInfo/ImprovementYieldChanges`、`RiverPlotYieldChanges`

- ファイル: `CIV4TraitInfos.xml`
- 社会制度の `ImprovementYieldChanges`（`ImprovementType` + `ImprovementYields`）、建物の `RiverPlotYieldChanges`（`<iYield>` の並び）と同じ名前・書式。
- 施設の分は `CvPlot::calculateImprovementYieldChange` で施設の持ち主の志向から足す（作業の説明「この施設で +○」にも反映される）。
- 川の分は `calculateTraitYieldChange` で、建物と同じく川沿いの通行できるタイルに足す。
- そのつど合計（`CvPlayer::getTraitImprovementYieldChange()` / `getTraitRiverPlotYieldChange()`）。
- 説明文は施設を地形・特徴と同じ行にまとめ、川は建物の `TXT_KEY_BUILDING_RIVER_PLOTS`（「河川スクエア: +1[食料]」）。
- **施設の XML は志向より後に読み込まれる**ので、`read()` では種類を文字列のまま持ち、施設の読み込み直後に `CvXMLLoadUtility::LoadPreMenuGlobals` から呼ぶ `CvTraitInfo::readPass3()` で番号に変える（名前の誤りは XML Error のメッセージで知らせる）

## `TraitInfo/RiverEdgeYieldPercents`

- ファイル: `CIV4TraitInfos.xml`
- タイルの辺のうち川が流れている本数（0〜4。`CvPlot::getRiverEdgeCount`: 東西南北の 4 方向の川の渡り `isRiverCrossing`。陸のタイルどうしの辺だけ）に応じた産出（`<iYield>` は食料・生産・商業の順）。
- 値は川の辺 1 本につき産出の ○%で、本数を掛けて 100 で割り、切り捨てる（100 なら 1 本につき +1、34 なら 3 本で +1、50 なら 2 本で +1）。
- その文明の領土の、川沿いの通行できるタイルに足す（`RiverPlotYieldChanges` と同じ条件と段階。`CvPlot::calculateTraitYieldChange`）。
- そのつど合計（`CvPlayer::getTraitRiverEdgeYieldPercent`）。
- 志向が変わると `updateTraitDependents` が領土のタイルの産出を計算し直す。
- 説明文は本数ごとの値（`TXT_KEY_TRAIT_RIVER_EDGE_YIELD`「川の流れる辺が1・2・3・4本のタイル: 0・0・+1・+1[食料]」）。
- Python は `CyPlot.getRiverEdgeCount()`、`CyPlayer.getTraitRiverEdgeYieldPercent(eYield)`、`CvTraitInfo.getRiverEdgeYieldPercent(i)`

## `TraitInfo/LakePlotYieldChanges`

- ファイル: `CIV4TraitInfos.xml`
- 自国の領土の湖のタイルの産出に加算（`<iYield>` は食料・生産・商業の順。東方叙事詩の特定の志向の直書きを汎用のタグにしたもの）。
- `CvPlot::calculateTraitYieldChange` で足す。
- 湖は BtS の `CvPlot::isLake`（小さな水域。`LAKE_MAX_AREA_SIZE` 以下）。
- そのつど合計（`CvPlayer::getTraitLakePlotYieldChange()`）。
- 説明文は `TXT_KEY_TRAIT_LAKE_PLOTS`（「湖スクエア: +1[生産]」）

## `TraitInfo/LakePlotYieldChangesPerEra`

- ファイル: `CIV4TraitInfos.xml`
- 時代が 1 つ進むごとの湖のタイルの産出（2026-11-19。アステカの案）。
- `LakePlotYieldChanges` と同じ書式で、値 × 今の時代を足す（`TerrainYieldChangesPerEra` と同じく、時代が変わると領土の産出を計算し直す）。
- 説明文「時代が 1 つ進むごとに湖スクエア: +1[商業]」（`TXT_KEY_TRAIT_LAKE_PLOTS_PER_ERA`）。
- Python は `CvTraitInfo.getLakePlotYieldChangePerEra(iYield)`

## `TraitInfo/GoldInterest`

- ファイル: `CIV4TraitInfos.xml`
- `<GoldInterest>` に `iPercent`（必須）と `iMax`（省略可）を書く。
- 2026-11-21 に親タグにまとめた。
- 国庫の利子（ex 氏の CivGoldEmpires の提携志向「国庫の 3%（最大 25）」）。
- 毎ターン、国庫（その時点の資金）の ○%（切り捨て）を得る。
- `iMax` は 1 ターンの上限（0 で上限なし）。
- 複数の志向があれば志向ごとに計算して足す。
- 国庫が 0 以下なら 0。
- `CvPlayer::calculateGoldRate` に足す（`CvPlayer::getGoldInterest()`）ので、ターンの資金の処理（`doGold`）、画面上部の 1 ターンの資金、AI の資金の見積もりに含まれる。
- 財務アドバイザー（`Screens\CvFinanceAdvisor.py`。Warlords(J) のコピー）の収入に「利子」の行（`TXT_KEY_FINANCIAL_ADVISOR_INTEREST`）を足した。
- 説明文は `TXT_KEY_TRAIT_GOLD_INTEREST_MAX`（「毎ターン国庫の3%の[金]を得る（最大25）」）/ `TXT_KEY_TRAIT_GOLD_INTEREST`

## `TraitInfo/iStateReligionHappiness`、`iStateReligionHealth`

- ファイル: `CIV4TraitInfos.xml`
- `iStateReligionHappiness`: 社会制度の同名タグと同じ（国教の広まった都市の幸福。`CvPlayer::processTrait` で `changeStateReligionHappiness`。説明文は社会制度のキー `TXT_KEY_CIVIC_RELIGION_HAPPINESS`）。
- `iStateReligionHealth`: 国教の広まった都市の衛生（マイナスなら不衛生）。
- そのつど計算する（`CvCity::getStateReligionTraitHealth`、`goodHealth` / `badHealth`）。
- 衛生の内訳に「国教（志向）により」の行（`TXT_KEY_MISC_GOOD_HEALTH_FROM_STATE_RELIGION_TRAIT` / `TXT_KEY_MISC_HEALTH_FROM_STATE_RELIGION_TRAIT`）。
- 説明文は `TXT_KEY_TRAIT_STATE_RELIGION_HEALTH`

## `TraitInfo/GreatPeopleClassRateModifiers`

- ファイル: `CIV4TraitInfos.xml`
- 偉人の種類（ユニットクラス）ごとの偉人ポイント %。
- `<GreatPeopleClassRateModifier>` が `UnitClassType` + `iGreatPeopleRateModifier`。
- 都市の専門家・建物が出すその偉人のポイント（`getGreatPeopleUnitRate`）に % を掛けたもの（`CvCity::getGreatPeopleUnitRateWithTraits`）を、その偉人が出る確率の重み（`doGreatPeople` の `changeGreatPeopleUnitProgress`）と、都市の偉人ポイントの合計（`getGreatPeopleRate` に `getTraitGreatPeopleRateExtra` を足す）の両方に使う。
- 偉人ポイントの内訳に「専門家ごとの補正（志向）」の行（`TXT_KEY_MISC_HELP_GREATPEOPLE_TRAIT_CLASS`）。
- 説明文は `TXT_KEY_TRAIT_GREAT_PEOPLE_CLASS_RATE`

## `TraitInfo/SpecialBuildingProductionModifiers`

- ファイル: `CIV4TraitInfos.xml`
- 特殊建造物（`SPECIALBUILDING_TEMPLE` など）ごとの生産 %。
- `<SpecialBuildingProductionModifier>` が `SpecialBuildingType` + `iProductionModifier`。
- BtS の特殊建造物の `ProductionTraits`（宗教志向の寺院・大聖堂 +100% など）を志向の側に書くもの。
- `CvPlayer::getProductionModifier(BuildingTypes)` で `ProductionTraits` と一緒に足し、生産の内訳と志向の説明文も同じ行に出る（「生産速度 2 倍」など）。
- 特殊建造物は志向より後に読み込まれるので `readPass3` で番号に変える

## `TraitInfo/iGoldenAgeModifier`、`iGoldenAgeGreatPeopleRateModifier`、`iCapitalBuildingProductionModifier`、`iPillageGoldModifier`、`Conquest/iGoldModifier`

- ファイル: `CIV4TraitInfos.xml`
- Civ5 の文明の能力（ペルシャ・ブラジル・ローマ・ソンガイ）。
- `iGoldenAgeModifier`: 黄金時代の期間 ○%（建物の同名タグと同じ。`CvPlayer::init` で 1 回だけ適用するので、既存のセーブには効かない。説明文は建物の `TXT_KEY_BUILDING_GOLDENAGE_MOD`）。
- `iGoldenAgeGreatPeopleRateModifier`: 黄金時代の間、全都市の偉人ポイント ○%（BtS の黄金時代の +100% に足す。`CvCity::getGreatPeopleRateModifier`。偉人ポイントの内訳では「黄金時代のため」の行に含める）。
- `iCapitalBuildingProductionModifier`: 首都にある建物を首都以外の都市で建てるとき生産 ○%（`CvCity::getCapitalBuildingProductionModifier`。首都にその種類の建物があるかで判定。生産の内訳 `TXT_KEY_MISC_HELP_PROD_CAPITAL_BUILDING`）。
- `iPillageGoldModifier`: 施設の略奪で得る資金 ○%（`CvUnit::pillage`）。
- `iGoldModifier`（2026-11-23 に `<Conquest>` の子に移し、名前を `iCityCaptureGoldModifier` から変えた（getter と Python の名前は元のまま）。）: 都市を征服したときの資金 ○%（`CvPlayer::acquireCity`）。
- 資金はどちらも Python（`doPillageGold` / `doCityCaptureGold`）の結果に掛け、-100 以下で 0。
- `iGoldenAgeModifier` 以外はそのつど合計（`CvPlayer::getGoldenAgeGreatPeopleRateModifier()` など）するので、既存のセーブにも効く

## `TraitInfo/BonusYieldChanges`

- ファイル: `CIV4TraitInfos.xml`
- Civ5 のロシア。
- `TerrainYieldChanges` と同じ書式（`<BonusYieldChange>` が `BonusType` + `<Yields>`）。
- その文明の領土の、その資源のタイルの産出に加算する（`CvPlot::calculateTraitYieldChange`）。
- 資源はタイルの持ち主から見えているもの（公開の技術を持つ）だけで、資源を使えるか（施設・技術）は問わない。
- そのつど合計（`CvPlayer::getTraitBonusYieldChange()`）。
- 資源は志向より後に読み込まれるので、`readPass3()` で番号に変える。
- 説明文は地形・地形の特徴・施設と同じ行にまとめる

## `TraitInfo/iHappinessPerReligion`、`CommercePerReligion`

- ファイル: `CIV4TraitInfos.xml`
- 都市にある宗教 1 つにつき、その都市の幸福（マイナスなら不満）と商業（`<iCommerce>` は金・研究・文化・諜報の順）を足す（インドの「多宗教の受容」）。
- 幸福はその都度数える（`CvCity::getTraitReligionHappiness`。`happyLevel`・`unhappyLevel`、都市の幸福の内訳に「都市の宗教の数により」の行 `TXT_KEY_HAPPY_TRAIT_RELIGIONS` / `TXT_KEY_ANGER_TRAIT_RELIGIONS`）。
- 商業は BtS の宗教の商業（聖都の聖堂など）の合計に足す（`CvCity::getTraitReligionCommerce`、`updateReligionCommerce`）ので、宗教が広まった・消えたときに計算し直され、都市画面の内訳では宗教の分に入る。
- 志向が変わったときは `updateTraitDependents` が全都市の宗教の商業を計算し直す。
- 志向の値の合計（`CvPlayer::getHappinessPerReligion` / `getCommercePerReligion`）。
- 説明文 `TXT_KEY_TRAIT_HAPPINESS_PER_RELIGION`・`TXT_KEY_TRAIT_COMMERCE_PER_RELIGION`（「都市の宗教1つにつき+1[文化]」）。
- Python は `CyPlayer` / `CvTraitInfo` の同名の関数

## `TraitInfo/WorldWonderCommerceChanges`、`GlobalWorldWonderCommerceChanges`

- ファイル: `CIV4TraitInfos.xml`
- 建物の同名タグの志向版（`<iCommerce>` は金・研究・文化・諜報の順）。
- `WorldWonderCommerceChanges`: そのプレイヤーの各都市で、その都市にある世界遺産 1 つにつき商業を足す。
- `GlobalWorldWonderCommerceChanges`: 全都市で、そのプレイヤーが持つ世界遺産 1 つにつき足す。
- 世界遺産は陳腐化したものも数える（建物のタグと同じ `getNumWorldWonders`）。
- 都市の基本の商業に足す（`CvCity::getTraitWorldWonderCommerce`、`getBaseCommerceRateTimes100`）。
- 建物の商業とは別の要素で、商業の内訳に「世界遺産（志向）により」の行（`TXT_KEY_MISC_HELP_TRAIT_WONDER_COMMERCE`）。
- 世界遺産の数が変わると `CvPlayer::updateWorldWonderEffects` が、志向が変わると `updateTraitDependents` が全都市の商業を計算し直す。
- 志向の値の合計（`CvPlayer::getTraitWorldWonderCommerce` / `getTraitGlobalWorldWonderCommerce`）。
- セーブデータは変わらない。
- ルイ14世の案。
- 説明文 `TXT_KEY_TRAIT_PER_CITY_WORLD_WONDER`・`TXT_KEY_TRAIT_PER_WORLD_WONDER`。
- Python は `CyPlayer` / `CvTraitInfo` の同名の関数

## `TraitInfo/iWeLoveTheKingChance`、`SpecialistSlotChanges`

- ファイル: `CIV4TraitInfos.xml`
- DoC のブラジル・朝鮮の UP から。
- `iWeLoveTheKingChance`: 国王感謝祭の確率 %。
- 国王感謝祭は毎ターン決め直す。
- BtS の条件（反乱中でない、怒っている市民がいない、不衛生でない、人口 `WE_LOVE_THE_KING_POPULATION_MIN_POPULATION`（8）以上）を満たす都市は、先にこの % で判定し、外れたら BtS の判定（人口 / `WE_LOVE_THE_KING_RAND` = 1000）をする（`CvCity::isTraitWeLoveTheKing`）。
- 100 なら必ず開く（乱数を使わない）。
- 志向の合計（0〜100）。
- `SpecialistSlotChanges`: `<SpecialistSlotChange>`（`SpecialistType` + `iSlots`）を何個でも。
- 全都市で、その専門家の枠を足す（建物の `SpecialistCounts` の枠に足す。`CvCity::getMaxSpecialistCount`。都市画面・AI・Python もこれを使う）。
- `-1`（マイナスの値）で無制限（カースト制の `SpecialistValids` と同じ。`CvPlayer::isSpecialistValid`）。
- 志向の合計で、どれかの志向が `-1` なら無制限（`CvPlayer::getTraitSpecialistSlotChange`）。
- 志向を失って枠を超えたときは BtS の仕組みで市民に戻る。
- DoC の朝鮮（首都だけ 2 倍）のような首都だけの指定はない。
- どちらもそのつど判定するのでセーブデータは変わらない。
- 説明文 `TXT_KEY_TRAIT_WE_LOVE_THE_KING_ALWAYS` / `_CHANCE`、`TXT_KEY_TRAIT_SPECIALIST_SLOTS` / `_UNLIMITED`。
- Python は `CyPlayer.getWeLoveTheKingChance()`・`getTraitSpecialistSlotChange(eSpecialist)`、`CvTraitInfo.getWeLoveTheKingChance()`・`getSpecialistSlotChange(i)`

## `TraitInfo/ImprovedBonusYieldChanges`

- ファイル: `CIV4TraitInfos.xml`
- DoC のオーストラリア・満洲の UP から。
- `BonusYieldChanges` と同じ書式（`<ImprovedBonusYieldChange>` が `BonusType` + `<Yields>`）。
- その資源のタイルに、その資源を使える施設（`isImprovementBonusTrade`。金なら鉱山、小麦なら農場）があるとき、施設の産出に足す（`CvPlot::calculateImprovementYieldChange`。施設の持ち主の志向。作業の説明「この施設で +○」にも出る）。
- 資源は持ち主から見えているもの（BtS の施設の資源の産出と同じ）。
- `BonusYieldChanges` は施設を問わない。
- 満洲の「平和なとき」は `ConditionalFreeTraits` の `bAtPeace` で書く（「怒っている市民のいない都市」の条件はない）。
- そのつど合計（`CvPlayer::getTraitImprovedBonusYieldChange`）。
- 志向が変わると `updateTraitDependents` が領土のタイルを計算し直す。
- 資源は志向より後に読み込まれるので `readPass3()` で番号に変える。
- 説明文 `TXT_KEY_TRAIT_IMPROVED_BONUS_YIELD_CHANGE`（「開発済みの次の資源より[商業]+3: 銅, …」。産出と値ごとに 1 行）。
- Python は `CvTraitInfo.getImprovedBonusYieldChange(iBonus, iYield)`

## `TraitInfo/HappyYieldPercents`、`HappyCommercePercents`、`HealthyYieldPercents`、`HealthyCommercePercents`

- ファイル: `CIV4TraitInfos.xml`
- DoC のハラッパーの UP（余剰の衛生が食料になる）から。
- 余剰の幸福（幸福 − 不満、正のとき）・余剰の衛生（衛生 − 不衛生、正のとき）1 につき、都市の産出（`<iYield>` は食料・生産・商業の順）・商業（`<iCommerce>` は金・研究・文化・諜報の順）を足す。
- 値は 1/100 単位（100 で 1 につき +1、50 で 2 につき +1）。
- 志向の値を合計してから、余剰の数を掛けて 100 で割り、切り捨てる。
- `AngryYieldChanges` などの逆で、同じ仕組み（`CvCity::updateMiseryChanges`。都市のターンの初めと、人間のプレイヤーの都市の作業が変わったとき。志向が変わったときは `updateTraitDependents`）。
- 怒りと余剰の幸福、不衛生と余剰の衛生は同時に起きないので、保存している値（`m_aiAngryYield` など）を共用し、セーブの形式は変わらない。
- 都市の産出・商業の内訳の行は、怒っている市民がいれば「怒り」、いなければ「余剰の幸福」（`TXT_KEY_MISC_HELP_YIELD_HAPPY`・`TXT_KEY_MISC_HELP_HAPPY_COMMERCE`。衛生も同じく `_HEALTHY`）。
- 志向だけ（建物・社会制度にはない）。
- ハラッパーの「古代だけ」は `ConditionalFreeTraits` の `EraMax`。
- 説明文 `TXT_KEY_TRAIT_HAPPY_PERCENT` / `TXT_KEY_TRAIT_HEALTHY_PERCENT`（「余剰の[幸福] 1 につき[金]+1」「…[食料]+0.50」）。
- Python は `CvTraitInfo` の同名の関数（`getHappyYieldPercent(i)` など）

## `TraitInfo/YieldChanges`、`WorldWonderYieldChanges`、`GlobalWorldWonderYieldChanges`、`iHealthPerReligion`、`YieldPerReligion`

- ファイル: `CIV4TraitInfos.xml`
- 片方しかなかったタグの対（2026-11-19）。
- `YieldChanges`（`<iYield>` の並び）: 全都市の食料・生産・商業 +N（`CommerceChanges` の産出版。建物の `YieldChanges` と同じ）。
- `WorldWonderYieldChanges` / `GlobalWorldWonderYieldChanges`: 商業版と同じく、その都市の世界遺産 1 つにつき / 持っている世界遺産 1 つにつき全都市の産出 +N。
- `YieldPerReligion`: 都市の宗教 1 つにつき産出 +N。
- この 4 つは `CvCity::getTraitYield` で合計し、建物の動的な産出（`calculateDynamicBuildingYield`、セーブ済みの `m_aiDynamicBuildingYield`）に含めて基本産出に足すので、セーブ形式は変わらない。
- 更新は都市の建設、宗教の伝播・消滅（`setHasReligion`）、世界遺産の数の変化（`updateWorldWonderEffects`）、志向の変化（`updateTraitDependents`）とターンの初め。
- 産出の内訳では建造物と分け、由来ごとに「都市の宗教（志向）」「世界遺産（志向）」「志向」（`YieldChanges`）の行に出す（`TXT_KEY_MISC_HELP_YIELD_TRAIT_RELIGIONS`・`_TRAIT_WONDERS`・`TXT_KEY_MISC_HELP_YIELD_TRAITS`）。
- 都市画面の宗教のアイコンの説明には宗教ごとの値（`YieldPerReligion`・`iHealthPerReligion`、既存の `CommercePerReligion`・`iHappinessPerReligion` も。`CvGameTextMgr::setReligionHelpCity`）、建造物の欄の各世界遺産には遺産ごとの値（`WorldWonderYieldChanges`・既存の `WorldWonderCommerceChanges`。陳腐化した遺産も数える。`CvMainInterface.py`）を出す。
- 持っている遺産の数による値（`Global…`）は全都市に効くので内訳の行だけ。
- Python は `CyPlayer.getYieldPerReligion(eYield)`・`getHealthPerReligion()`・`getTraitYieldChange(eYield)`・`getTraitWorldWonderYield(eYield)`・`getTraitGlobalWorldWonderYield(eYield)` も。
- `iHealthPerReligion`: 都市の宗教 1 つにつき衛生 +N（`CvCity::getTraitReligionHealth`、そのつど計算。衛生の内訳 `TXT_KEY_MISC_GOOD_HEALTH_FROM_TRAIT_RELIGIONS` / `TXT_KEY_MISC_HEALTH_FROM_TRAIT_RELIGIONS`）。
- 説明文は `TXT_KEY_CIVIC_IN_ALL_CITIES`、商業版のキー（`TXT_KEY_TRAIT_COMMERCE_PER_RELIGION`・`TXT_KEY_TRAIT_PER_CITY_WORLD_WONDER`・`TXT_KEY_TRAIT_PER_WORLD_WONDER`）に産出のアイコンを渡す、`TXT_KEY_TRAIT_HEALTH_PER_RELIGION`。
- Python は `CvTraitInfo.getYieldChange(i)` などの同名の getter

## `TraitInfo/SpecialistExtraYields`、`iNonStateReligionHappiness`、`FeatureHappinessChanges`

- ファイル: `CIV4TraitInfos.xml`
- 片方しかなかったタグの対（2026-11-19）。
- `SpecialistExtraYields`（`<iYield>` の並び）: 全種類の専門家の産出 +N（`SpecialistExtraCommerces` の産出版。`processTrait` で専門家ごとの `changeSpecialistExtraYield` に足す。説明文 `TXT_KEY_CIVIC_PER_SPECIALIST`）。
- `iNonStateReligionHappiness`: 社会制度と同じく、国教以外の宗教 1 つにつき幸福 +N（`CvPlayer::getNonStateReligionHappiness` に志向の合計を足す。説明文は社会制度の `TXT_KEY_CIVIC_NON_STATE_REL_HAPPINESS_WITH_STATE`）。
- `FeatureHappinessChanges`: 社会制度と同じ書式（`<FeatureHappinessChange>` が `FeatureType` + `iHappinessChange`）。
- 都市圏のその地形の特徴 1 つにつき幸福 +N（`CvPlayer::getFeatureHappiness` に志向の合計を足す。説明文は社会制度の `TXT_KEY_CIVIC_FEATURE_HAPPINESS`）。
- 志向が変わると `updateTraitDependents` が全都市の宗教の幸福・地形の特徴の幸福を計算し直す

## `TraitInfo/BuildingHappinessChanges`、`BuildingHealthChanges`

- ファイル: `CIV4TraitInfos.xml`
- 社会制度の同名のタグの志向版（2026-11-19）。
- `<BuildingHappinessChange>` が `BuildingClassType` + `iHappinessChange`（衛生は `<BuildingHealthChange>` の `iHealthChange`）。
- その建物クラスの建物に幸福・衛生 +N。
- 社会制度と違い建物クラスで引くので、固有建造物や、学んだ・占領した他の文明の建物にも効く（`CvPlayer::getExtraBuildingHappiness` / `getExtraBuildingHealth` に志向の合計を足す）。
- 建物クラスは志向より後に読み込まれるので `readPass3()` で番号に変える。
- 説明文は社会制度の `TXT_KEY_CIVIC_BUILDING_HAPPINESS`（衛生も同じキーにアイコンを渡す）

## `TraitInfo/BonusHappinessChanges`、`BonusHealthChanges`

- ファイル: `CIV4TraitInfos.xml`
- 建物の同名のタグの志向版（2026-11-19）。
- `<BonusHappinessChange>` が `BonusType` + `iHappinessChange`（衛生は `<BonusHealthChange>` の `iHealthChange`）。
- その資源を持つ全都市に幸福・衛生 +N（`CvCity::getTraitBonusHappiness` / `getTraitBonusHealth`、そのつど計算するのでセーブは変わらない。資源ごとに正の値は幸福、負の値は不満として数える）。
- 幸福・衛生の内訳に「資源（志向）により」の行（`TXT_KEY_HAPPY_TRAIT_BONUSES`・`TXT_KEY_ANGER_TRAIT_BONUSES`・`TXT_KEY_MISC_GOOD_HEALTH_FROM_TRAIT_BONUSES`・`TXT_KEY_MISC_HEALTH_FROM_TRAIT_BONUSES`）。
- 都市画面の資源の欄の値（`CvCity::getBonusHappiness` / `getBonusHealth`）にも含める。
- 資源は志向より後に読み込まれるので `readPass3()` で番号に変える。
- 説明文は建物の `TXT_KEY_BUILDING_HEALTH_HAPPINESS_CHANGE`

## `TraitInfo/iStateReligionGreatPeopleRateModifier`、`iStateReligionUnitProductionModifier`、`iStateReligionBuildingProductionModifier`、`iStateReligionFreeExperience`

- ファイル: `CIV4TraitInfos.xml`
- 社会制度（神権政治・神聖政治など）の同名のタグの志向版（2026-11-19）。
- 国教が広まった都市の偉人ポイント %、ユニットの生産 %、建物の生産 %、作ったユニットの経験値 +N。
- `CvPlayer::processTrait` で社会制度と同じ `changeStateReligion…()` に足すので、社会制度の値と合算され、都市の判定・表示も BtS のまま（宗教のアイコンの説明にも出る）。
- ゲーム開始時の `CvPlayer::init` で適用する値なので、既存のセーブの指導者・文明に足しても反映されない（ゲーム中に得た志向は `changeFreeTraitCount` で反映）。
- 説明文は社会制度の `TXT_KEY_CIVIC_GREAT_PEOPLE_MOD_STATE_RELIGION`・`TXT_KEY_CIVIC_STATE_REL_TRAIN_BONUS`・`TXT_KEY_CIVIC_STATE_REL_BUILDING_BONUS`・`TXT_KEY_CIVIC_STATE_REL_FREE_XP`。
- Python は `CvTraitInfo` の同名の getter

## `TraitInfo/iCoastalTradeRoutes`、`iTradeRouteModifier`、`iForeignTradeRouteModifier`、`iAllCityDefense`、`iBombardDefense`、`iEspionageDefense`

- ファイル: `CIV4TraitInfos.xml`
- 建物の同名のタグの志向版（2026-11-19）。
- 全都市（`iCoastalTradeRoutes` は沿岸の全都市）に効く。
- `iCoastalTradeRoutes`（交易路 +N）と `iAllCityDefense`（都市の防御 %）は `CvPlayer::processTrait` で建物と同じ `changeCoastalTradeRoutes` / `changeCityDefenseModifier` に足す（ゲーム開始時の `init` で適用するので、既存のセーブの指導者・文明に足しても反映されない）。
- `iTradeRouteModifier`（交易路の収入 %）は `CvCity::totalTradeModifier` に足し、交易路の説明に「志向により」の行（`TXT_KEY_TRADE_ROUTE_MOD_TRAIT`）。
- `iForeignTradeRouteModifier`（外国との交易路の収入 %）は `CvCity::getForeignTradeRouteModifier` に、`iBombardDefense`（砲撃による防御の減少 -%。BtS と同じく火薬ユニットの砲撃には効かない）は `getBuildingBombardDefense` に、`iEspionageDefense`（諜報への防御 %）は `getEspionageDefenseModifier` に、そのつど志向の合計を足す（セーブは変わらない。志向が変わると `updateTraitDependents` が交易路を計算し直す）。
- 説明文は建物の `TXT_KEY_BUILDING_COASTAL_TRADE_ROUTES`・`TXT_KEY_BUILDING_TRADE_ROUTE_MOD`・`TXT_KEY_BUILDING_FOREIGN_TRADE_ROUTE_MOD`・`TXT_KEY_BUILDING_DEFENSE_MOD_ALL_CITIES`・`TXT_KEY_BUILDING_BOMBARD_DEFENSE_MOD`・`TXT_KEY_BUILDING_ESPIONAGE_DEFENSE_MOD`。
- Python は `CvTraitInfo` の同名の getter、`CyPlayer` は未公開

## `TraitInfo/iLargestCityHappiness`、`iHappyPerMilitaryUnit`、`bNoUnhealthyPopulation`、`bBuildingOnlyHealthy`、`iFoodKept`

- ファイル: `CIV4TraitInfos.xml`
- 社会制度・建物の同名のタグの志向版（2026-11-19）。
- `iLargestCityHappiness`（人口の多い順にマップの目標都市数までの都市の幸福 +N）、`iHappyPerMilitaryUnit`（都市にいる軍事ユニット 1 つにつき幸福 +N）、`bNoUnhealthyPopulation`（人口による不衛生なし）、`bBuildingOnlyHealthy`（建物による不衛生なし）は `CvPlayer::processTrait` で社会制度と同じ `change…()` に足す（ゲーム開始時の `init` で適用するので、既存のセーブの指導者・文明に足しても反映されない）。
- `iFoodKept`（成長後に食料を % 残す。全都市）は `CvCity::getMaxFoodKeptPercent` にそのつど志向の合計を足す（建物の分と合わせて 100 まで）。
- 説明文は社会制度の `TXT_KEY_CIVIC_LARGEST_CITIES_HAPPINESS`・`TXT_KEY_CIVIC_UNIT_HAPPINESS`・`TXT_KEY_CIVIC_NO_POP_UNHEALTHY`・`TXT_KEY_CIVIC_NO_BUILDING_UNHEALTHY`、建物の `TXT_KEY_BUILDING_STORES_FOOD`。
- Python は `CvTraitInfo` の同名の getter

## `TraitInfo/FreeBonuses`、`BonusProductionModifiers`、`CommerceHappinesses`、`CommerceHealths`

- ファイル: `CIV4TraitInfos.xml`
- 建物のタグの志向版（2026-11-19）。
- `FreeBonuses`（`<FreeBonus>` が `BonusType` + `iNumBonuses`）: その資源を N 個持つ。
- 輸入した資源と同じく首都の交易網に数える（`CvPlot::updatePlotGroupBonus` の首都の部分）ので、遷都しても新しい首都に移り、交易にも出せる。
- 志向が変わると `updateTraitBonusPlots` が首都の交易網を足し引きし直す。
- `BonusProductionModifiers`（`<BonusProductionModifier>` が `BonusType` + `iProductionModifier` + 省略可の `bWorldWondersOnly`）: その資源を使える都市の生産 %。
- `bWorldWondersOnly` なしならユニット・建物・プロジェクトのすべて、ありなら世界遺産だけ（建物の「大理石で遺産の生産」のような使い方）。
- 生産の内訳に「○○を使えるため（志向）」の行（`TXT_KEY_MISC_HELP_PROD_TRAIT_BONUS`）。
- `CommerceHappinesses`（`<iCommerce>` の並び。建物と同じく配分 100% あたりの幸福。10 なら配分 10% につき +1）: `CvCity::getCommerceHappinessPer` に志向の合計を足すので、幸福の内訳は BtS の行に合算。
- `CommerceHealths`: その衛生版（新規。`CvCity::getTraitCommerceHealth`、衛生の内訳に「商業の配分（志向）により」の行 `TXT_KEY_MISC_GOOD_HEALTH_FROM_TRAIT_COMMERCE` / `TXT_KEY_MISC_HEALTH_FROM_TRAIT_COMMERCE`。都市画面の配分の欄にも衛生を出す（`CvMainInterface.py`、`CyPlayer.getTraitCommerceHealth(eCommerce)`）。なお文化の配分の欄の幸福は BtS 本来のもの（`CIV4CommerceInfo` の文化の `iInitialHappiness` 10）。どれもそのつど計算（`FreeBonuses` は交易網の数）でセーブは変わらない。資源は志向より後に読み込まれるので `readPass3()` で番号に変える。説明文は建物の `TXT_KEY_BUILDING_PROVIDES`・`TXT_KEY_BUILDING_PER_LEVEL`（衛生も同じキーにアイコンを渡す）、`TXT_KEY_TRAIT_BONUS_PRODUCTION` / `TXT_KEY_TRAIT_BONUS_WONDER_PRODUCTION`。Python は `CvTraitInfo.getFreeBonus(i)`・`getBonusProductionModifier(i)`・`getBonusWonderProductionModifier(i)`・`getCommerceHappiness(i)`・`getCommerceHealth(i)`

## `TraitInfo/bUnsettleGreatPeople`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-23。
- その志向を持つプレイヤーは、都市に定住した偉人をユニットに戻せる。
- 都市画面の右下の定住専門家のアイコン（`WIDGET_FREE_CITIZEN`。BtS ではクリックしても何も起きなかった）をクリックすると確認の画面（`BUTTONPOPUP_PYTHON`。Python は呼ばない。文 `TXT_KEY_POPUP_UNSETTLE_GREAT_PERSON`）が出て、「はい」で無料の専門家が 1 人減り、その都市に偉人ユニットが出る（`CvCity::unsettleGreatPerson`）。
- ユニットは偉人の誘拐と同じ `CvPlayer::getGreatPersonUnit`（文明の固有ユニット）。
- 出たユニットはそのターンから動け、技術・黄金時代・別の都市への定住など何にでも使える（制限なし）。
- 戻せる数は `getAddedFreeSpecialistCount`（建物の無料の専門家を除いた数。イベントなどで得た無料の専門家も含む）。
- 結果は `ModNetMessage`（`MOD_NET_MESSAGE_UNSETTLE_GREAT_PERSON` = -20261125。`CvNetModNetMessage::Execute` が DLL で処理し、Python には渡さない）で全員のコンピューターに送るので、マルチプレイでも同期する。
- 戻せるアイコンのマウスオーバーに `TXT_KEY_UNSETTLE_GREAT_PERSON_CLICK`。
- AI は使わない。
- 説明文 `TXT_KEY_TRAIT_UNSETTLE_GREAT_PEOPLE`。
- Python: `CvTraitInfo.isUnsettleGreatPeople()`、`CyPlayer.isUnsettleGreatPeople()`、`CyCity.canUnsettleGreatPerson(eSpecialist)`・`unsettleGreatPerson(eSpecialist)`（後者は全員のコンピューターで呼ぶこと）

## `TraitInfo/UnsettleSpecialists`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-24。
- 書式は `<UnsettleSpecialists><SpecialistType>SPECIALIST_...</SpecialistType></UnsettleSpecialists>`。
- `bUnsettleGreatPeople` と同じく定住した偉人をユニットに戻せるが、書いた専門家として定住した偉人だけ（`CvPlayer::canUnsettleSpecialist`）。
- `bUnsettleGreatPeople` はすべての偉人で、どちらかがあれば戻せる。
- 戻す操作・同期・ユニットの決め方は `bUnsettleGreatPeople` と同じ。
- 世界ユニット（`iMaxGlobalInstances`）でも戻せる。BtS の上限は「作った数」で数え、減らないので、定住しても作り直せず、戻しても数が増えるだけ。
- 説明文 `TXT_KEY_TRAIT_UNSETTLE_SPECIALISTS`（「ユニットに戻せる定住偉人（都市画面でクリック）: 検証用学派A（定住）, …」。`bUnsettleGreatPeople` があるときは出さない）。
- Python: `CvTraitInfo.isUnsettleSpecialist(iSpecialist)`、`CyPlayer.canUnsettleSpecialist(eSpecialist)`

## `TraitInfo/GreatPeopleHurry`（`iGoldPerPoint`・`UnitClassType`）

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-26。
- 書式は `<GreatPeopleHurry><iGoldPerPoint>3</iGoldPerPoint><UnitClassType>UNITCLASS_ARTIST</UnitClassType></GreatPeopleHurry>`（`UnitClassType` は省略可）。
- 都市画面の偉人のプログレスバー（`CvMainInterface.py` の `GreatPeopleBar`。`WIDGET_HELP_GREAT_PEOPLE`。BtS ではクリックしても何も起きなかった）をクリックすると、確認の画面（`BUTTONPOPUP_PYTHON`。Python は呼ばない。文 `TXT_KEY_POPUP_HURRY_GREAT_PEOPLE`）が出る。
- 「はい」で、足りない偉人ポイント（閾値 − 今のポイント）を金で買い、**その場で偉人が生まれる**（`CvCity::hurryGreatPeople`）。
- 値段 = 足りないポイント × `iGoldPerPoint`。必要なポイントは偉人が生まれるたびに増えるので、値段も上がる。ゲーム速度は閾値に入っている。
- `UnitClassType` を書くと、買ったポイントはすべてそのクラスの文明のユニット（`getCivilizationUnits`。固有の偉人に対応）に入り、必ずその偉人が生まれる。ほかの種類の溜まっていたポイントは、ふだんの誕生と同じく消える。
- 省略すると（または文明にそのクラスのユニットがないと）、今の種類ごとのポイントの割合で配り（端数は最も多い種類へ）、BtS と同じ抽選（`CvCity::chooseGreatPeopleUnit`。`doGreatPeople` から切り出した）で決まる。
- 誕生は `CvCity::createGreatPeopleFromProgress`（閾値ぶんを引き、種類ごとのポイントを 0 に戻して `createGreatPeople`。`doGreatPeople` と共通）。
- 使えないとき（`CvCity::getGreatPeopleHurryDenial`）:
  - 種類ごとのポイントの合計が 0（割合を計算できない）: `TXT_KEY_GREAT_PEOPLE_HURRY_DENIAL_NO_POINTS`
  - 金が足りない: `TXT_KEY_GREAT_PEOPLE_HURRY_DENIAL_GOLD`
- 買うとバーが 0 になるので、次のターンの初めにポイントが入るまで使えない（1 都市につき 1 ターンに 1 回）。セーブには何も足していない。
- 暴動中の都市でも買える（BtS の誕生は暴動中に止まる）。
- 偉人バーのマウスオーバーに、値段の行 `TXT_KEY_GREAT_PEOPLE_HURRY_CLICK` か使えない理由を足す。志向を持たない文明では何も出ず、クリックしても何も起きない。
- 買うとイベントログに `TXT_KEY_GREAT_PEOPLE_HURRY_BOUGHT`（都市・金・ポイント）。
- 結果は `ModNetMessage`（`MOD_NET_MESSAGE_HURRY_GREAT_PEOPLE` = -20261129。`CvNetModNetMessage::Execute` が DLL で処理）で全員のコンピューターに送るので、マルチプレイでも同期する。
- `iGoldPerPoint` は志向の合計（0 以下なら使えない）。種類は `iGoldPerPoint` が正でクラスを書いた最初の志向のもの（`CvPlayer::getGreatPeopleHurryUnit`）。
- AI は使わない。
- 説明文 `TXT_KEY_TRAIT_GREAT_PEOPLE_HURRY`（1 ポイントあたりの金と、クラスのリンクか `TXT_KEY_GREAT_PEOPLE_HURRY_BY_SHARES`）。
- Python:
  - `CvTraitInfo.getGreatPeopleHurryGold()`・`getGreatPeopleHurryUnitClass()`
  - `CyPlayer.getGreatPeopleHurryGold()`・`getGreatPeopleHurryUnit()`（-1 は割合）
  - `CyCity.getTotalGreatPeopleUnitProgress()`・`getGreatPeopleHurryPoints()`・`getGreatPeopleHurryCost()`・`canHurryGreatPeople()`・`hurryGreatPeople()`（最後は全員のコンピューターで呼ぶこと）

## `TraitInfo/GoldenAgeUnitClasses`、`iGoldenAgeUnitsChange`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-26（ペルシャの案）。
- 偉人を消費して起こす黄金期の条件。
- どちらも省略可。
- `GoldenAgeUnitClasses`（`UnitClassType` の並び）: その志向を持つプレイヤーの、このクラスのユニットも偉人と同じく黄金期を起こせる（`CvUnit::isGoldenAge` が真になる。行動ボタン・必要な人数の判定・一緒に消費する偉人の選び方は BtS のまま）。
- 違う種類を揃える判定はユニットの型で数えるので、大将軍（`UNIT_GREAT_GENERAL`）は大科学者などと別の 1 種類になる（大将軍 2 人では 2 種類にならない）。
- 志向を持つプレイヤーのユニットだけで、どれかの志向が持っていれば有効（`hasTraitFlag`）。
- AI はこのタグを考えない（大将軍を黄金期に使うとは限らない）。
- `iGoldenAgeUnitsChange`: 黄金期に必要な偉人の数（`CvPlayer::unitsRequiredForGoldenAge`。BtS は `BASE_GOLDEN_AGE_UNITS` 1 + 起こした回数 × `GOLDEN_AGE_UNITS_MULTIPLIER` 1）に志向の合計を足す。最低 1 人。
- -1 なら 1・1・2・3… 人。
- 必要な人数は毎回計算するので、既存のセーブでも効く。
- 偉人を消費しない黄金期（建物・イベント・`startGoldenAge`）は回数に数えない（BtS どおり）。
- 説明文 `TXT_KEY_TRAIT_GOLDEN_AGE_UNIT_CLASSES`（クラス名をリンクで並べる）・`TXT_KEY_TRAIT_GOLDEN_AGE_UNITS_CHANGE`。
- Python: `CvTraitInfo.getNumGoldenAgeUnitClasses()`・`getGoldenAgeUnitClass(i)`・`isGoldenAgeUnitClass(iUnitClass)`・`getGoldenAgeUnitsChange()`

## `TraitInfo/NearbyCityCommerce`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-26（パカル2世の案）。
- 子要素: `iRange`（タイルの範囲）、`CommerceChanges`（`<iCommerce>` は金・研究・文化・諜報の順）、`bDoubleInCapital`。
- 自分の各都市に、`iRange` タイル以内にある自分のほかの都市 1 つにつき、商業 +N。
- 距離は `plotDistance`（都市圏と同じ形。範囲 3 なら 37 タイル）。
- 別の大陸・海の向こうの都市も数える。自分自身は数えない。
- `bDoubleInCapital` が 1 なら、首都ではその志向の分を 2 倍にする。
- 複数の志向が持つときは、志向ごとにその範囲で数え、首都の 2 倍も志向ごとにかけてから足す（`sumTraitValues` ではなく志向のループ）。
- 保存しない。商業の計算のたびに数え直す（`CvPlayer::getNearbyCityCommerce`。`CvCity::getBaseCommerceRateTimes100` で足す）。
- 都市が増減したら持ち主の全都市の商業を計算し直す（`CvCity::init` と `CvCity::kill` の `CvPlayer::updateNearbyCityCommerce`。占領・譲渡は kill と init なので両方の持ち主）。
- 遷都は `setCapitalCity` が新旧の首都の商業を計算し直す。志向の変化は `updateTraitDependents`。
- 既存のセーブでも効く。
- 都市の商業の内訳の行は「近隣の都市（志向）」（`TXT_KEY_MISC_HELP_NEARBY_CITY_COMMERCE`）。
- 説明文 `TXT_KEY_TRAIT_NEARBY_CITY_COMMERCE`（首都 2 倍なら `_CAPITAL`）。XML の値だけを使う。
- Python: `CvTraitInfo.getNearbyCityRange()`・`getNearbyCityCommerce(i)`・`isNearbyCityDoubleInCapital()`
