# 検証用データ（社会制度・建物・技術・施設・ユニット・昇進・呪文など）

文明・指導者・志向は `docs/test-data.md`。


## `CIVIC_TEST`（検証用社会制度）

- 複製元: 専制政治
- 政治の区分。
- 前提技術なし・無政府期間 0・維持費なし・AI 評価 -10000。
- 白兵戦に経験値 +1・生産 +10%・アップグレード割引 25%、弓兵に領土外アップグレード（敵国の領土を除く）。
- 前提の志向 `TRAIT_TEST`。
- 採用している間、金融志向を得る（`FreeTraits`。検証用志向に金融志向はない）。
- 怒っている市民 1 人につき商業 +1（`AngryYieldChanges`）、不衛生 1 につき文化 +1（`UnhealthyCommerceChanges`）。
- 採用すると首都に労働者（`PyOnAdopt`）、やめると資金 +20（`PyOnRevoke`）
- 採用している間、検証用志向（大躍進）も得る（`FreeTraits`）。
- 採用している間、決議とスパイによる社会制度の強制を受けない（`bNoForcedCivics`）

## `CIVIC_TEST_DESPOTISM`・`CIVIC_TEST_HEREDITARY_RULE`（検証用専制政治・検証用世襲統治）

- 複製元: 専制政治・世襲統治
- 前提の志向 `TRAIT_TEST`、`ReplacedCivic` が元の社会制度。
- 健康 +1 のほかは元と同じ。
- 専制政治の方は技術なしで初期の社会制度の置換を、世襲統治の方は技術（君主政治）のある置換を確かめる

## `CIVIC_TEST_NO_FORCED`（検証用社会制度（強制なし））

- 複製元: 地方分権
- 前提の志向 `TRAIT_TEST_AI`（検証用 AI 文明）、`ReplacedCivic` が地方分権。
- 検証用 AI 文明は最初からこれを採用する（初期の社会制度の置換）。
- `bNoForcedCivics` のほかは地方分権と同じ。
- 人間のスパイが検証用 AI 文明に体制変革を使えないことを確かめる

## `BUILDING_TEST` / `BUILDINGCLASS_TEST`（検証用建物）

- 複製元: 兵舎
- 前提技術なし・元の効果（攻撃志向の生産 2 倍、陸上ユニット経験値 +3）は削除・AI 評価 -10000。
- 騎乗に領土外アップグレード。
- 前提の志向 `TRAIT_TEST`。
- 5 ターンごとに都市の文化 +10（`PyOnBeginTurn`: `isEveryTurns(5) and addCulture(pCity, 10)`）

## `BUILDING_TEST_RADIUS` / `BUILDINGCLASS_TEST_RADIUS`（検証用建物（都市圏））

- 複製元: 検証用建物
- `iPlotRadius` 3（文化レベル未熟以上で）。
- 前提の志向なし（どの文明も建てられる）・費用 20。
- 検証用文明は志向で半径 3 になるので、建物の効果は他の文明（日本など）で確かめる

## `BUILDING_TEST_CITY` / `BUILDINGCLASS_TEST_CITY`（検証用建物（新しい都市））

- 複製元: 検証用建物（都市圏）
- 都市圏の半径の代わりに、新しい都市の人口 +2・新しい都市に国教・占領した都市の反乱期間 +100%。
- 前提の志向なし・費用 20。
- 新しい都市は文化レベル発展途上から。
- 志向と合わせると、検証用文明の新しい都市は人口 4（1 + 志向 1 + 建物 2）、反乱期間は +50%（-50 + 100）、文化レベルは発展途上（志向の未熟より高い方）。
- 陳腐化していないものをどこかの都市に持つ間、博愛志向を得る（`FreeTraits`）。
- 世界遺産の数に比例する効果: この都市の世界遺産 1 つにつき商業 +4・金 +1、所有する世界遺産 1 つにつき生産 +1・文化 +1

## `BUILDING_TEST_GREAT_PEOPLE` / `BUILDINGCLASS_TEST_GREAT_PEOPLE`（検証用建物（偉人））

- 複製元: 検証用建物（新しい都市）
- 新しい都市の効果の代わりに、前提「定住した偉大な商人 1 人 と 偉大な芸術家 1 人」（`PrereqAndSettledSpecialists`）と「定住した偉大な科学者 1 人 または 偉大な技術者 2 人」（`PrereqOrSettledSpecialists`）。
- 効果はない。
- 前提の志向なし・費用 20。
- 偉人はワールドビルダーで置いて定住させる

## `BUILDING_TEST_PREREQ` / `BUILDINGCLASS_TEST_PREREQ`（検証用建物（前提））

- 複製元: 検証用建物（偉人）
- 偉人の前提の代わりに、前提「兵舎 または 穀物庫」（`PrereqOrBuildingClasses`）、「検証用建物（都市圏）がない」（`PrereqNotInCityBuildingClasses`）、人口 3 以上（`iPrereqPopulation`）、文化レベル未熟以上（`PrereqCultureLevel`）。
- 効果はない。
- 前提の志向なし・費用 20。
- どの文明も兵舎と検証用建物（都市圏）を技術なしで建てられる

## `BUILDING_TEST_PREREQ_PLOT` / `BUILDINGCLASS_TEST_PREREQ_PLOT`（検証用建物（都市圏の前提））

- 複製元: 検証用建物（前提）
- 前提の代わりに、都市圏に「丘陵 または 山岳」（`PrereqOrTerrains`）、「森 または ジャングル」（`PrereqOrFeatures`）、「鉄 または 銅」（`PrereqRawVicinityBonuses`）、「開発済みの鉄 または 銅」（`PrereqVicinityBonuses`）、「農場 または 小屋」（`PrereqOrImprovements`）。
- はじめの 3 つを満たすまで一覧に出ず、残り 2 つを満たすまで灰色。
- 効果はない。
- 前提の志向なし・費用 20。
- タイルと資源はワールドビルダーで置く

## `BUILDING_TEST_PREREQ_CIVIC` / `BUILDINGCLASS_TEST_PREREQ_CIVIC`（検証用建物（社会制度の前提））

- 複製元: 検証用建物（前提）
- 前提の代わりに、「原始部族社会 と 精霊崇拝」（`PrereqAndCivics`。初期の社会制度なので最初は満たす）と「世襲統治 または 検証用社会制度」（`PrereqOrCivics`）。
- 効果はない。
- 前提の志向なし・費用 20。
- 検証用文明なら検証用社会制度を技術なしで採用できる

## `BUILDING_TEST_GROWTH` / `BUILDINGCLASS_TEST_GROWTH`（検証用建物（成長））

- 複製元: 検証用建物（前提）
- 前提なし（どの文明も最初から建てられる）・費用 20。
- 技術で伸びる効果: `TECH_POTTERY` で金 +2・研究 +1、`TECH_MINING` で生産 +2、`TECH_MYSTICISM` で幸福 +2、`TECH_MASONRY` で不幸 +1（幸福 -1）、`TECH_HUNTING` で衛生 +2、`TECH_THE_WHEEL` で不衛生 +1（衛生 -1）、`TECH_WRITING` で科学者の枠 +1。
- 完成すると資金 +30（`PyOnBuilt`: `gainGold(pPlayer, 30)`）

## `BUILDING_TEST_GROWTH_BONUS` / `BUILDINGCLASS_TEST_GROWTH_BONUS`（検証用建物（資源で成長））

- 複製元: 検証用建物（成長）
- 前提なし・費用 20。
- 銅を使えると金 +2（`BonusCommerceChanges`）、鉄を使えると生産 +1（`BonusYieldChanges`）、都市圏に鉄が開発済みなら食料 +2（`VicinityBonusYieldChanges`）。
- 鉄の鉱山を道路でつながないと、都市圏の分だけが効く

## `BUILDING_TEST_GROWTH_POP` / `BUILDINGCLASS_TEST_GROWTH_POP`（検証用建物（人口で成長））

- 複製元: 検証用建物（成長）
- 前提なし・費用 20。
- 人口 2 につき幸福 +1（`iHappinessPercentPerPopulation` 50）、人口 4 につき不衛生 +1（`iHealthPercentPerPopulation` -25）

## `BUILDING_TEST_GROWTH_GLOBAL` / `BUILDINGCLASS_TEST_GROWTH_GLOBAL`（検証用建物（全都市の商業））

- 複製元: 検証用建物（成長）
- 前提なし・費用 20。
- 持つ間、全都市の兵舎 1 つにつき金 +1、検証用建物（都市圏）1 つにつき研究 +2（`GlobalBuildingExtraCommerces`）。
- `TECH_WRITING` で陳腐化する（陳腐化で効果が消えることの確認用）

## `BUILDING_TEST_MISERY` / `BUILDINGCLASS_TEST_MISERY`（検証用建物（怒りの恩恵））

- 複製元: 検証用建物（全都市の商業）
- 前提なし・費用 20。
- 怒っている市民 1 人につき生産 +1（`AngryYieldChanges`）、不衛生 1 につき金 +2（`UnhealthyCommerceChanges`）

## `BUILDING_TEST_GRANARY`（検証用穀物庫）

- 複製元: 穀物庫
- クラスは穀物庫のまま。
- 費用 30。
- 検証用志向の固有建造物

## `TECH_TEST`（検証用技術）

- 複製元: 神秘主義
- 技術ツリーの 1 列目の空き（`iGridX` 1・`iGridY` 9）。
- 前提なし・費用 10・古代。
- 交換できない・小屋から出ない（`bTrade` / `bGoodyTech` 0）・AI 評価 -10000。
- チームが持つ間、検証用志向を得る（`FreeTraits`）。
- 日本などで研究すると検証用志向の効果がすべて付く（検証用文明は元から持つので変わらない）。
- 得ると資金 +20、最初に得たチームなら +50（`PyOnDiscovered`: `gainGold(pPlayer, 20 + 30 * bFirst)`）

## `TECH_TEST_DIPLOMACY`（検証用技術（外交））

- 複製元: 検証用技術
- 研究できない（`bDisable`）・交換できない・小屋から出ない。
- 地図・技術・資金・国境開放・防衛協定・永久同盟・属国の取引を解禁（`bMapTrading` など 7 個）。
- 技術ツリーの `iGridX` 2・`iGridY` 15。
- 検証用文明（`FreeTechs`）と検証用 AI 文明（`TRAIT_TEST_AI` の `FreeTechs`）が開始時に持つ。
- 取引の解禁はどちらかのチームが持てばよいが、属国は宗主国の側が持つ必要がある（`CvPlayer::canTradeItem`）。
- 永久同盟はゲームオプション「永久同盟」が必要

## `IMPROVEMENT_TEST_TERRACE`（検証用施設（棚田））

- 複製元: 農場
- 隣接する山岳 1 つにつき食料 +1・生産 +1（`AdjacentPeakYieldChanges`）。
- 隣接する棚田 1 つにつき商業 +1、隣接するコテージ（集落・村・町も）1 つにつき食料 +1（`AdjacentImprovementYieldChanges`）、隣接する森 1 つにつき生産 +1（`AdjacentFeatureYieldChanges`）。
- 作業は `BUILD_TEST_TERRACE`

## `IMPROVEMENT_TEST_FARM`（検証用施設（強化農場））

- 複製元: 農場
- 灌漑で食料 +2（農場は +1）。
- ほかは農場と同じ。
- `ReplacedImprovement` が農場なので、検証用志向の「農場に生産 +1」を受ける

## `IMPROVEMENT_TEST_TUNNEL`（検証用施設（トンネル））

- 複製元: 検証用施設（棚田）
- 山岳だけに作れる（`TerrainMakesValids`）。
- `bMakesPassable` 1、`iPassageMoveCost` 1。
- 産出なし。
- 作業は `BUILD_TEST_TUNNEL`

## `IMPROVEMENT_TEST_POLDER`（検証用施設（ポルダー））

- 複製元: 検証用施設（トンネル）
- 湖と沿岸（`bWater` 1、`TerrainMakesValids` が `TERRAIN_COAST`）。
- `bMakesPassable` 1。
- 食料 +2・生産 +1。
- 見た目は漁船。
- 作業は `BUILD_TEST_POLDER`

## `IMPROVEMENT_TEST_CIV`（検証用施設（文明））

- 複製元: 検証用施設（棚田）
- 効果は棚田と同じ（隣接の産出は書いていない）。
- 同じ施設と隣接できない（`iUniqueRange` 1）、1 つの都市圏に 2 つまで（`iMaxPerCity`）、自国の領土に 3 つまで（`iMaxPerPlayer`）。
- 作業は `BUILD_TEST_CIV`

## `IMPROVEMENT_TEST_FORT`（検証用砦）

- 複製元: 砦
- 効果は砦と同じ。
- 2 タイル以内の自チームのユニットの回復 +20%・防御 +25%（`iRange` 2、`iHealRateChange`・`iRangeDefenseModifier`）、持ち主の視界 +2（`iPlotVisibilityChange`。このタイルから 3 タイル先まで）。
- 2 タイル以内の各タイルに持ち主の文化 +10/ターン、そこまで国境が広がる（`iCulture`・`iCultureRange`）。
- `ReplacedImprovement` が砦。
- 作業は `BUILD_TEST_CIV_FORT`

## `IMPROVEMENT_TEST_LANDMARK`・`IMPROVEMENT_TEST_WONDER`・`IMPROVEMENT_TEST_PEAK_WONDER`（検証用名所・検証用自然遺産・検証用名峰）

- 複製元: 検証用施設（トンネル）
- どれも `iUniqueChance` 100、`bPermanent` 1。
- 名所は草原と平原で食料 +2・商業 +3（砦の見た目）。
- 自然遺産は草原と平原で `bImpassable`、生産 +2・商業 +4（風車の見た目）。
- 名峰は山岳で商業 +6（水車の見た目）

## `BONUS_TEST`（検証用資源）

- 複製元: 銀
- 前提の志向 `TRAIT_TEST`。
- 平地の草原と平原。
- 技術なし。
- `iPlacementOrder` 7、`iTilesPer` 16、`bNormalize` 0。
- 検証用施設（棚田）で幸福 +1 の資源として得られ、商業 +2。
- マナ資源の効果（累積 `bModifierPerBonus`）: 持つ都市の生産 +10%・研究 +10%、偉人ポイント +10%、維持費 -10%、作ったユニットに戦闘力Ⅰ（`FreePromotion`）、首都が持つと自国の領土での回復 +10%・敵の回復 -10%・施設で資源が見つかる確率 +100%。
- 幸福 +1 も持つ数だけ重なる

## `BONUS_TEST_WANDER`（検証用資源（移動））

- 複製元: 馬（グラフィック）
- 前提の志向 `TRAIT_TEST`。技術なし（最初から見える）。
- 平地の草原・平原・ツンドラ。
- `iPlacementOrder` 7、`iTilesPer` 32。商業 +1。
- 毎ターン、3 タイル以内に移動する（`Wander` の `iTurns` 1、`iRange` 3）。
- 検証用志向を持つ文明の文化圏に入るか、そのユニットが同じタイルにいると止まり、馬に変わる（`SettleBonus`・`bStopTerritory`・`bStopUnit`）

## `BUILD_TEST_FARM`（検証用施設（強化農場）の建設）

- 複製元: 農場の建設
- 前提の志向 `TRAIT_TEST`、`ReplacedBuild` が農場の建設（置換のパターン）。
- 検証用志向を持つと農場の建設の代わりに出る。
- 前提技術は農業のまま

## `BUILD_TEST_TERRACE`（検証用施設（棚田）の建設）

- 複製元: 農場の建設
- 前提の志向 `TRAIT_TEST`、`UnitsFromBuild` が農場の建設（新規のパターン。労働者の `Builds` は書き換えていない）。
- 前提技術なし、ホットキーなし

## `BUILD_TEST_TUNNEL`（検証用施設（トンネル）の建設）

- 複製元: 検証用施設（棚田）の建設
- 前提の志向 `TRAIT_TEST`、`UnitsFromBuild` が農場の建設（前提技術なし）

## `BUILD_TEST_POLDER`（検証用施設（ポルダー）の建設）

- 複製元: 検証用施設（トンネル）の建設
- 前提の志向 `TRAIT_TEST`、`UnitsFromBuild` が農場の建設（前提技術なし）

## `BUILD_TEST_CIV`（検証用施設（文明）の建設）

- 複製元: 検証用施設（棚田）の建設
- 前提の志向なし。
- `CIVILIZATION_TEST` の `UniqueBuilds` にだけ書いてある（文明から設定する新規のパターン）。
- `UnitsFromBuild` が農場の建設（前提技術なし）

## `BUILD_TEST_CIV_FORT`（検証用砦の建設）

- 複製元: 砦の建設
- 前提の志向なし。
- `CIVILIZATION_TEST` の `UniqueBuilds` にだけ書いてある（文明から設定する置換のパターン）。
- `ReplacedBuild` が砦の建設。
- 作業時間 300（砦は 1000）、ホットキーなし。
- 前提技術は数学のまま

## `BUILD_TEST_CANAL_NORTH`・`_EAST`・`_SOUTH`・`_WEST`（検証用運河（北の辺）など）

- 新規。前提の志向 `TRAIT_TEST`、`UnitsFromBuild` が道路の建設（労働者ならどれでも）。
- `RiverEdge` がそれぞれの方向、`RiverFeature` が氾濫原、`iMaxPlotRiverEdges` 3。
- 前提技術なし、作業時間 100（エジプト用は工学・1000 の予定）、ホットキーなし。ボタンは農場の仮のもの

## `EVENTTRIGGER_TEST_TRAIT` / `EVENT_TEST_TRAIT`（検証用志向の固有イベント）

- 複製元: 森林火災
- 前提の志向 `TRAIT_TEST`（`PrereqOrTraits`）。
- すべてのゲームで有効、`iWeight` 0 で止めてある（発生しない。確認済みで、ほかの確認の邪魔になるため 2026-11-23 に止めた。確かめるときは -1 に戻すと毎ターン必ず判定）、1 回だけ、都市もタイルも選ばない。
- 選ぶと資金 +10。
- 検証用文明は最初のターンに起きる。
- 日本などは検証用技術を得た次のターンに起きる

## `UNIT_TEST_A` / `UNIT_TEST_B`（検証用ユニット A・B）

- 複製元: 戦士・斧兵
- 白兵戦。
- 前提技術・資源なし。
- A は B にだけアップグレードできる（1 ターン目からアップグレード関連を試せる）。
- AI 評価 -10000。
- B だけ前提の志向 `TRAIT_TEST` または `TRAIT_AGGRESSIVE`（「どれか 1 つ」の確認用。攻撃志向の徳川の日本でも作れる）。
- B を作れる文明では A は生産候補に出ない（BtS の仕様: アップグレード先がすべて作れるユニットは候補から外れる。`CvCity::canTrain` → `allUpgradesAvailable`）。
- A のアップグレードは検証用文明の開始ユニットで試す。
- A は検証用資源 1 つにつき戦闘力 +1（`BonusAffinities`）。
- A は死ぬと首都に斥候が出る（`PyOnDeath`）、B は生まれると戦闘術Ⅰを得る（`PyOnCreated`）。
- A を B にアップグレードしても、斥候は出ず、B に戦闘術Ⅰは付かない（同じユニットが続くとみなす）
- A は `iUpgradeDiscount` 100（B へのアップグレードが無料）
- A は生まれると経験値 +30 と、大将軍が合流したユニットの扱い（`PyOnCreated` の `gainExperience`・`keepExperienceOnUpgrade`）。B にアップグレードしても経験値は 30 のまま（10 に切り捨てられない）

## `UNIT_TEST_WARRIOR`・`UNIT_TEST_GENERAL`・`UNIT_TEST_SCIENTIST`（検証用戦士・大将軍・大科学者）

- 複製元: 戦士・大将軍・大科学者
- クラスは元のまま。
- 戦士は戦闘力 4、大将軍は `iLeaderExperience` 40、大科学者は `iBaseDiscover` 3000、定住すると `SPECIALIST_TEST`（検証用定住偉人）になる。
- 検証用志向の固有ユニット

## `UNIT_TEST_SPY`（検証用スパイ）

- 複製元: スパイ
- 無印 Civ4 の旧スパイの行動をすべて有効にした（`bSabotage`・`bDestroy`・`bStealPlans`・`bInvestigate` を 1。`bCounterSpy` は元から 1）。
- 前提技術なし（スパイはアルファベット）、生産コスト 10（スパイは 40）。
- `iStationedAttitudeChange` 3（ほかの文明の都市にいる間、その AI の態度 +3）。検証用呪文（和平）を使える。
- 行動の費用の金は `GlobalDefines.xml` の共通の値（破壊工作 `BASE_SPY_SABOTAGE_COST` 200、生産の破壊 `BASE_SPY_DESTROY_COST` 50 から、計画を盗む `BASE_SPY_STEAL_PLANS_COST` 100 から）。
- 検証用志向の固有ユニット

## `SPECIALIST_TEST`（検証用定住偉人）

- 複製元: 定住した大科学者
- 1 人につき都市の幸福 +2・不衛生 +1（`iHappiness` 2・`iHealth` -1）。
- ほかは定住した大科学者と同じ。
- 検証用大科学者が都市に加わるとこれになる

## `SPECIALIST_TEST_SCHOOL_A`・`SPECIALIST_TEST_SCHOOL_B`（検証用学派A・B（定住））

- 複製元: 定住した大預言者・定住した大技術者（テクスチャだけ）
- 2026-11-24。諸子百家の検証用。
- A: 食料 +20%・生産 +25%（`YieldModifiers`）、維持費 -50%、諜報への防御 +50%、不満なし、偉人ポイント -100%、都市圏の森 1 つにつき幸福 +1、研究 +25%（`CommerceModifiers`）、無償の科学者 1 人（`FreeSpecialistCounts`）、無政府状態でも止まらない、排他。
- B: 大将軍の偉人ポイント +3（既存の `GreatPeopleUnitClass` と `iGreatPeopleRateChange`）、新しいユニットに経験値 +4（既存の `iExperience`）、幸福 +1、都市防御 +50%、砲撃への耐性 50%、この都市の専門家 1 人につき研究 +2、都市圏での戦闘の大将軍ポイント +100%、排他。
- A と B は同じ都市に定住できない。

## `UNIT_TEST_SCHOOL_A`・`UNIT_TEST_SCHOOL_B` / `UNITCLASS_TEST_SCHOOL_A`・`_B`（検証用学派A・B）

- 複製元: 検証用大科学者
- 2026-11-24。
- 世界ユニット（`iMaxGlobalInstances` 1）、生産コスト 30、前提の志向 `TRAIT_TEST`（検証用文明だけが作れる）。前提技術なし。
- 黄金時代・技術・建物の行動はない。定住するとそれぞれ `SPECIALIST_TEST_SCHOOL_A` / `_B` になる。
- 検証用志向（暦）の `UnsettleSpecialists` で、定住したあとユニットに戻せる

## `PROMOTION_TEST_STACK`（検証用随行）

- 複製元: 検証用状態Ⅱ
- 2026-11-24。状態の昇進（`UnitCombats` は空）、戦闘力 +25%、移動力 +1・敵の道路を使える・移動コスト -1（2026-11-26。スタックで動くときの移動力の確認用）。
- 検証用志向（暦）の `StackPromotions` で、検証用学派Bと同じタイルにいる白兵戦・騎乗ユニット（と学派B 自身。`bStackUnitsToo`）と、検証用学派Aと同じタイルにいる兵科のあるユニットに付く

## `UNIT_TEST_WORKER`（検証用労働者）

- 複製元: 労働者（2026-11-28）
- 検証用文明の `UNITCLASS_WORKER` を置き換える（固有ユニット）。検証用文明は開始時に 2 体持つ。
- 偉人の任務: 生産の加速 20 + 1×人口、交易任務 20 + 5×交易の利益、傑作 文化 30。
- `iGreatWorkOccupationTurns` 1（傑作で反乱の期間を 1 ターン短くするだけ）。
- `FreePromotions` に `PROMOTION_TEST_FORCED_LABOR`。
- ペディアは `TXT_KEY_CLAUDE_TEST_PEDIA`

## `UNIT_TEST_PREREQ` / `UNITCLASS_TEST_PREREQ`（検証用ユニット（前提））

- 複製元: 検証用ユニット A
- アップグレードなし。
- 前提に人口 3 以上（`iPrereqPopulation`）、文化レベル未熟以上（`PrereqCultureLevel`）。
- 満たすまで生産の一覧に灰色で出る。
- 前提技術・資源・志向なし。
- AI 評価 -10000

## `UNIT_TEST_PREREQ_PLOT` / `UNITCLASS_TEST_PREREQ_PLOT`（検証用ユニット（都市圏の前提））

- 複製元: 検証用ユニット A
- アップグレードなし。
- 前提に都市圏の「丘陵 または 山岳」（`PrereqOrTerrains`）、「森 または ジャングル」（`PrereqOrFeatures`）、「鉄 または 銅」（`PrereqRawVicinityBonuses`）、「開発済みの鉄 または 銅」（`PrereqVicinityBonuses`）、「農場 または 小屋」（`PrereqOrImprovements`）。
- はじめの 3 つを満たすまで一覧に出ず、残り 2 つを満たすまで灰色。
- タイルと資源はワールドビルダーで置く

## `PROMOTION_TEST`（検証用昇進）

- 複製元: 戦闘術Ⅰ
- 戦闘力 +10%。
- `UnitCombats` が空なので通常のレベルアップでは誰も選べず、昇進の組でだけ付く。
- 勝利すると `PROMOTION_TEST_STATE1` を得る。
- 生き残った戦闘相手に `PROMOTION_TEST_POISON` を与える。
- 毎ターン経験値 +1（10 まで）。
- 撤退率 +100%（攻撃で負けそうになると必ず撤退するので、相手が生き残って毒を受ける）。
- 勝利すると資金 +5、50% で捕獲。
- 自国の領土で戦闘力 +15%。
- 検証用資源 1 つにつき戦闘力 +1（`BonusAffinities`。白兵戦の A は種類の分と合わせて +2）。
- 海外で戦闘力 +25%（`iCombatPercentOverseas`）。
- 国教が違う文明との戦闘で戦闘力 +20%（`iOtherReligionCombatPercent`）、自国の国教が広まった都市への攻撃 +30%（`iStateReligionCityAttack`）。
- 自国が黄金期のとき、黄金期でない文明との戦闘で戦闘力 +20%（`iGoldenAgeAdvantageCombatPercent`。2026-11-26）。
- 2026-11-28 の分: 丘陵から攻撃すると +50%（`AttackFromTerrains` の `TERRAIN_HILL`）、森から攻撃すると +25%（`AttackFromFeatures`）。森の丘陵からなら +75%。検証用資源 1 つにつき戦闘力 +10%、最大 +20%（`BonusCombatPercents`・`iBonusCombatMaxPercent`）。相手との厭戦感情が生むはずの不満の割合 1% につき、その相手との戦闘力 +1%、最大 +50%（`iWarWearinessCombatPercent`・`iWarWearinessCombatMaxPercent`）。軍事力が自分より上の文明との戦闘力 +20%（`iStrongerEnemyCombatPercent`）。
- 勝つたびに、倒したユニットの基本の戦闘力 × 2 の金（`iCombatGoldPerStrength` 200。標準の速度。2026-11-28）。
- 国境を開いていない他国の領土にも入れる（`bRivalTerritory`）

## `PROMOTION_TEST_STATE1` / `STATE2`（検証用状態Ⅰ・Ⅱ）

- 複製元: 検証用昇進
- 状態の昇進（`UnitCombats` は空）。
- Ⅰ: 戦闘力 +20%、毎ターン 50% で消える、勝利すると 50% でⅡを得る・資金 +3・捕獲 50%（検証用昇進と合わせて資金 +8・捕獲 100%）。
- Ⅱ: 戦闘力 +10%、戦闘すると消える。
- ボタンは戦闘術Ⅱ・Ⅲ

## `PROMOTION_TEST_POISON`（検証用毒）

- 複製元: 検証用状態Ⅱ
- 状態の昇進。
- 戦闘力 -20%、完全に回復すると消える、生き残った戦闘相手に 50% でうつる（毒を受けた敵と戦うと自分も受けることがある）。
- ボタンは戦闘術Ⅳ

## `PROMOTION_TEST_COMBAT`（検証用昇進（戦闘結果））

- 複製元: 検証用状態Ⅱ
- 第 4 段階の戦闘結果の昇進（`UnitCombats` は空。検証用志向の昇進の組で弓兵ユニットに付く）。
- 勝利すると 25 回復、50% で奴隷化（労働者。弓兵を倒すと斥候、戦士は捕らえない）、相手の国庫の 10%（最大 30）を奪う、100% で相手の昇進を 1 つ得る。
- 戦闘力の補正はなし。
- ボタンはコマンドー

## `PROMOTION_TEST_PASSAGE`（検証用昇進（通行不可タイル））

- 複製元: 検証用昇進（戦闘結果）
- 状態の昇進（`UnitCombats` は空。ワールドビルダーで付ける）。
- 山岳に入れて最大 HP の 34% のダメージ（3 回で死ぬ）、氷河に入れてダメージなし（`ImpassablePlotPassages`）。
- 戦闘の効果はない。
- 川を渡るたびに最大 HP の 20% のダメージ（`iRiverCrossingDamagePercent`）

## `PROMOTION_TEST_ICEBREAKER`（検証用昇進（砕氷船））

- 複製元: 検証用昇進（通行不可タイル）
- 状態の昇進（ワールドビルダーで船に付ける）。
- 氷河に入れ、移動コスト +1、入ると氷河が消える（`iExtraMoveCost`・`bRemoveFeature`）。
- ダメージなし

## `PROMOTION_TEST_NO_PENALTY`（検証用昇進（傷の影響なし））

- 複製元: 検証用昇進（砕氷船）
- 状態の昇進（`UnitCombats` は空。ワールドビルダーで付ける）。
- `bNoDamagePenalty` 1。
- ほかの効果はない

## `PROMOTION_TEST_CHOOSE_GOODY`（検証用昇進（部族集落））

- 複製元: 検証用昇進（傷の影響なし）
- `bChooseGoody` 1。
- 検証用志向の昇進の組で偵察ユニット（斥候・探検家）に付く

## `PROMOTION_TEST_FEATURE`（検証用昇進（地形の特徴））

- 複製元: 検証用昇進（傷の影響なし）
- 状態の昇進（ワールドビルダーで付ける）。
- `FeatureSpreads`: いるタイルに森 100%、周囲 1 マス（いるタイルを除く）に死の灰 25%（`bForce`）

## `PROMOTION_TEST_HIDDEN`（検証用昇進（国籍秘匿））

- 複製元: 検証用昇進（傷の影響なし）
- 状態の昇進（ワールドビルダーで付ける）。
- `bHiddenNationality` 1。
- Python フックのサンプル: 戦闘に勝つと資金 +10（`PyOnCombatWin`）、負けると持ち主に資金 +5（`PyOnCombatLoss`）、森にいると毎ターン 10% 回復（`PyOnBeginTurn`: `isOnFeature(pUnit, 'FEATURE_FOREST') and healUnit(pUnit, 10)`）

## `PROMOTION_TEST_RIVER`（検証用昇進（川））

- 複製元: 検証用昇進（部族集落）
- `bRiverNavigation` 1。
- `bRiverAttack` 1（2026-11-28）: 川の陸のタイルにいる陸のユニットを攻撃・捕獲できる。
- 検証用志向の昇進の組で、作業船・ガレー船・三段櫂船に付く。
- ボタンは航海術Ⅰ

## `PROMOTION_TEST_RAIDER`（検証用昇進（略奪者））

- 複製元: 検証用昇進（部族集落）
- `bFreeEmbark` 1、`bPillageNoMoveCost` 1、`iPillageHealPercent` 20、`iPillageExperience` 2（2026-11-28 から、自分のチームの領土の略奪では得ない）。
- 検証用志向の昇進の組で、陸上ユニット（弓兵・白兵戦・騎乗・偵察・銃火器・攻城・装甲と、兵科のない陸上ユニット）に付く。
- ボタンは水陸両用

## `PROMOTION_TEST_BUILDING`（検証用昇進（建造物））

- 複製元: 検証用昇進（略奪者）
- `BuildingCombatPercents`: 兵舎 1 つにつき +5%、国教の寺院 1 つにつき +3%（`SPECIALBUILDING_TEMPLE`・`bStateReligion`）、僧院（全宗教）1 つにつき +2%。
- 合計の上限 +20%（`iBuildingCombatMaxPercent`）。
- 検証用志向の昇進の組で騎乗ユニットに付く。
- 検証用志向は僧院を陳腐化させない（科学的手法の後も僧院の分が残る。陳腐化で外れることは、検証用昇進をワールドビルダーで他の文明のユニットに付けて確かめる）。
- ボタンは都市守備隊Ⅰ

## `PROMOTION_TEST_ENCIRCLE`（検証用昇進（包囲））

- 複製元: 検証用昇進（建造物）
- `Encirclement`: 横 1 体につき +5%（`iSidePercent`）、背後 1 体につき +10%（`iRearPercent`）、1 タイル 3 体まで（`iMaxUnitsPerPlot`）。上限の確認用に、シャカの予定（1・2・10）より大きく、体数は少なくしてある。
- 現在・かつての宗主国との戦闘力 +25%（`iFormerMasterCombatPercent`。2026-11-28）。
- 検証用志向の昇進の組で、陸上と海洋のユニット（全兵科と海洋）に付く。
- ボタンは側面攻撃Ⅰ

## `PROMOTION_TEST_FORCED_LABOR`（検証用昇進（強制労働））

- 複製元: 検証用昇進（包囲）（2026-11-28）
- `bForcedLabor` 1。ほかの効果なし。
- 検証用労働者の `FreePromotions` で付く。
- ボタンは側面攻撃Ⅰ（複製元のまま）

## 検証用呪文（`SPELL_TEST_*`）は無効

- 2026-11-26 に `CIV4SpellInfos.xml` で項目をすべてコメントアウトした（確認済み。行動ボタンがほかの確認の邪魔になるため）。使うときはコメントの印を外し、キャッシュを消す。
- 以下の各項目の中身はそのまま残してある。
- `SPELL_TEST_FORCE_CIVICS`（2026-11-26）だけはコメントの外にあり、有効。

## `SPELL_TEST_FORCE_CIVICS`（検証用呪文（社会制度の強制））

- 検証用スパイ（`UNIT_TEST_SPY`）が使える。
- 言論の自由と環境保護主義を、国連の決議と同じように強制する。強制中なら解除する（`spellToggleForcedCivics`。`CyGame.changeForceCivicCount`）。
- 強制されると、`bNoForcedCivics` の社会制度を採用していないプレイヤーは、技術がなくてもすぐにその社会制度になる。
- ボタンは言論の自由

## `SPELL_TEST_GOLD`（検証用呪文（資金））

- 検証用文明の白兵戦ユニットが、自国の領土で資金 +25（`spellGainGold(pCaster, 25)`。`PyRequirement` なし）。
- 使えないときも無効のボタンで出る（`bDisplayWhenDisabled`）。
- 実行済みになる。
- ボタンは「富」

## `SPELL_TEST_PROMOTION`（検証用呪文（昇進））

- 検証用昇進を持つユニットが、都市で、検証用状態Ⅰを持っていなければ得る（`reqNotHasPromotion` / `spellAddPromotion`）。
- 使えないときはボタンが出ない。
- 実行済みになる。
- ボタンは戦闘術Ⅱ

## `SPELL_TEST_CONVERT`（検証用呪文（変身））

- ユニット A が、資金 10 で検証用状態Ⅱを得てからユニット B に変わる（状態Ⅱ・経験値・実行済みは B に引き継ぐ。B はそのターン動けない）。
- 演出 `EFFECT_SETTLERSMOKE`、音 `AS2D_GOODY_WARRIOR`。
- 使えないときも無効のボタンで出る。
- 実行済みになる

## `SPELL_TEST_CITY`（検証用呪文（都市））

- 検証用建物のある都市（検証用文明の首都）で、オベリスクを建て、仏教を広め、人口 +1、資金 +50（`iCost` -50）。
- 実行したユニットは消える。
- オベリスクがあると使えない（ボタンも出ない）。
- 音 `AS2D_GOODY_GOLD`

## `SPELL_TEST_FOREST`（検証用呪文（森））

- 検証用建物を持つ文明が、自国の領土の水に隣接したタイルで、資金 5 で森を作り、検証用毒を外す（どちらかが効けば使える）。
- 使えないときも無効のボタンで出る。
- 実行済みになる

## `SPELL_TEST_FORT`（検証用呪文（砦））

- 検証用文明の、検証用昇進を持たないユニット（開拓者など）が、検証用昇進を持つ自国のユニットと同じタイルにいるとき、森かジャングルのタイルに砦を作る。
- 使えないときも無効のボタンで出る（同じタイルの条件を満たさないとボタンは出ない）。
- 実行済みになる

## `SPELL_TEST_REFRESH`（検証用呪文（再実行））

- 検証用文明のユニットが、砦のタイルで、資金 20 で実行済みを取り消す（`bIgnoreHasCasted`・`bRemoveHasCasted`）。
- 実行済みでないときは無効のボタン（砦のタイルでないとボタンは出ない）

## `SPELL_TEST_BLESS`（検証用呪文（祝福））

- 検証用文明のユニットが、1 タイル以内の自国（と同じチーム）のユニットに検証用状態Ⅱを与え、検証用毒を外す（`bImmuneEnemy`。兵科のないユニットには状態Ⅱは付かない）。
- 演出 `EFFECT_PROMOTION_UNIT`。
- 使えないときも無効のボタンで出る。
- 実行済みになる

## `SPELL_TEST_FIREBALL`（検証用呪文（火球））

- 検証用文明のユニットが、周囲 1 タイル（自分のタイルを除く）の交戦中の相手（動物・蛮族）に 40 前後のダメージ（`bImmuneTeam`。上限なしなので倒すこともある）。
- 演出 `EFFECT_ARTILLERY_SHELL_EXPLODE`。
- 使えないときも無効のボタンで出る。
- 実行済みになる

## `SPELL_TEST_SNARE`（検証用呪文（足止め））

- 検証用文明のユニットが、周囲 2 タイル（自分のタイルを除く）の交戦中の相手を 2 ターン動けなくし、10 前後のダメージ（25 まで）を与える（`bImmuneTeam`・`bImmuneFlying`）。
- 使えないときも無効のボタンで出る。
- 実行済みになる

## `SPELL_TEST_SUMMON`（検証用呪文（召喚））

- 検証用文明の、検証用昇進を持つユニットが、戦士を 2 体召喚する（2 ターンで消える: 既定 1 + 検証用志向 1）。
- 戦士は術者の昇進をすべて得る（`bCopyCastersPromotions`）。
- 召喚した戦士は実行できない（`bCasterNoDuration`）。
- 25% で失敗し、術者が 20 のダメージ（`spellDamageCaster`）。
- 演出 `EFFECT_SETTLERSMOKE`。
- 使えないときも無効のボタンで出る。
- 実行済みになる

## `SPELL_TEST_PERMANENT`（検証用呪文（永続召喚））

- 検証用文明の、検証用昇進を持つユニットが、資金 10 で、次のターンの初めに探検家を召喚する（`iDelay` 1。消えない。検証用状態Ⅱが付く。召喚したターンは動けない）。
- 検証用昇進を持つ自国のユニットの数まで。
- 使えないときも無効のボタンで出る。
- 実行済みになる

## `SPELL_TEST_WORLD`（検証用呪文（世界））

- 検証用文明のユニットが、資金 +100（世界呪文。確認のポップアップ。使うとボタンが出なくなる）。
- 音 `AS2D_GOODY_GOLD`。
- 実行済みになる

## `SPELL_TEST_TRAIT`（検証用呪文（志向））

- 検証用昇進を持つユニット（どの文明でも。日本のユニットにはワールドビルダーで検証用昇進を付ける）が、持ち主に検証用志向を与える。
- 与えてあれば取り上げる（`spellToggleFreeTrait(pCaster, 'TRAIT_TEST')` → `CyPlayer.changeFreeTraitCount`）。
- 実行済みにならないので、同じターンに何度でも切り替えられる。
- AI は使わない。
- 検証用文明は検証用志向を元から持つので、使っても効果は変わらない（二重に効かないことの確認）。
- ボタンは「研究」

検証用呪文は、検証用呪文（砦）以外すべて `bAllowAI`（AI も使う。確かめるには AI に検証用文明を割り当てる）。Python の効果しかない資金・昇進と、敵がいないと評価が 0 の永続召喚は `iAIWeight` を持つ（資金 100・昇進 50・永続召喚 100）。砦は `bAllowAI` がない（評価は正なので、AI が使わないことで `bAllowAI` を確かめる）。再実行は評価が負（資金 20）なので AI は使わない

検証用文明が `CIVIC_TEST` を採用したあと、白兵戦ユニット（都市で生産）で期待される値: 生産直後の経験値 5（志向 +2、白兵戦 +2、社会制度 +1。その後は毎ターン +1 で 10 まで増える）、ユニット A → B のアップグレード割引 75%（志向 50% + 社会制度 25%）、`PROMOTION_TEST` が付く

ゲームルールの確認: `GlobalDefinesAlt.xml` の `RIVER_CROSSING_MOVE_COST` はコミット時は 0（BtS と同じ）。確かめるときだけ一時的に 1 にする

## `SPELL_TEST_PEACE`（検証用呪文（和平））

- 2026-11-24。検証用スパイだけ。
- 戦争中の文明の都市にいるときに使える（`reqInWarEnemyCity`）。その文明と和平し、術者は消える（`spellForcePeaceWithCityOwner`）
