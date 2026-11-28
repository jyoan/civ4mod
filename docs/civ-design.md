# 実在の文明のデザイン（civ-design ブランチ）

`master` の機能を使って、実在の文明を XML でデザインする。機能の追加は `master` に入れてからマージする（`CLAUDE.md` の git の節）。

## Mod：better_trait_mod

このブランチは、既存の BtS 文明を強化する Mod「better_trait_mod」として配置する（2026-11-14 から）。

- 配置先は `<BTS>\Mods\better_trait_mod\`。`build.bat` の `MODNAME` をこのブランチだけ `better_trait_mod` にし、設定ファイル `better_trait_mod.ini` を置いた（中身は `claude_sample.ini` と同じ）。`master` は今までどおり `claude_sample` に配置する
- 起動: `& "<BTS>\Civ4BeyondSword.exe" "mod= mods\better_trait_mod"`
- ブランチを切り替えたら、配置し直してキャッシュを消す（`CLAUDE.md`）
- `master` から入る検証用データ（検証用文明など）も含まれる。配布するときに外す

### 作るもの

1. BtS の 11 志向そのものの強化（全指導者に効く）
2. 文明固有の志向（文明の `Traits`）
3. 指導者固有の志向（指導者の志向に 3 つ目として足す）
4. （見送り）文明固有の施設（文明の `UniqueBuilds`。必要ならほかの Mod からグラフィックを持ってくる）
5. （見送り）文明固有のランダムイベント（文明志向を `PrereqOrTraits` に書く。時代の違うものを 1 文明につき 2 つ以上）

2026-11-15 に方針を変え、4 と 5 は見送って、1〜3 の志向だけで先に完成させる。

数文明で型を作ってから広げる。試作は日本・エジプト・モンゴル・オランダ。遊ぶたびに違いが出るよう、効果の重複を避ける（下の「使った効果」と照らし合わせる）。作った能力はあとで入れ替えることもある。

### 11 志向の強化

BtS の効果はそのままにして足す。

| 志向 | 足した効果 | タグ |
|---|---|---|
| 攻撃 | 略奪で得る資金 +100%、都市を征服したときの資金 +100% | `iPillageGoldModifier` 100、`Conquest/iGoldModifier` 100 |
| 防衛 | 自国の領土では敵が略奪できない、弓兵のアップグレード費 -50% | `bNoPillageInBorders`、`UnitCombatUpgradeDiscounts` 弓兵 50 |
| 拡張 | 新しい都市の人口 +1、労働者の作業速度 +25% | `iNewCityPopulation` 1、`iWorkerSpeedModifier` 25 |
| 金融 | 国庫の利子 毎ターン 1%（上限なし）、交易路の収入 +25% | `GoldInterest` の `iPercent` 1、`TradeYieldModifiers` 商業 25 |
| 組織 | 距離による維持費 -25%、都市数による維持費 -25% | `iDistanceMaintenanceModifier` -25、`iNumCitiesMaintenanceModifier` -25 |
| 哲学 | 専門家 1 人につき研究 +1 | `SpecialistExtraCommerces` 研究 1 |
| 宗教 | 宣教師の布教の成功率 +25% | `iReligionSpreadChange` 25 |
| 創造 | 新しい都市が文化レベル「未熟」から始まる | `NewCityCultureLevel` `CULTURELEVEL_FLEDGLING` |
| 勤労 | 1 都市の国家遺産の上限 3、森林の伐採で得る生産 +10 | `iMaxNationalWondersPerCity` 3、`FeatureProductionChanges` 森林 10 |
| カリスマ | ほかの文明（AI）からの態度 +2、厭戦感情 -25% | `iAttitudeChange` 2、`iWarWearinessModifier` -25 |
| 帝国主義 | 占領した都市の反乱期間 -50%、海外の都市の維持費 -50% | `iOccupationTimeModifier` -50、`iOverseasMaintenanceModifier` -50 |

### 使った効果（重複を避けるための一覧）

文明志向・指導者志向・固有施設を決めるときは、ここにある効果を使わない。使ったら書き足す。同じタグでも、対象（地形・地形の特徴・資源・配る昇進の中身・条件など）が違えば重複としない。数値だけが違うものは重複とする（2026-11-15 に決定）。

| 効果（タグ） | 使ったところ |
|---|---|
| `iPillageGoldModifier`、`Conquest/iGoldModifier` | 攻撃志向 |
| `bNoPillageInBorders`、`UnitCombatUpgradeDiscounts` | 防衛志向 |
| `iNewCityPopulation`、`iWorkerSpeedModifier` | 拡張志向 |
| `GoldInterest`、`TradeYieldModifiers` | 金融志向 |
| `iDistanceMaintenanceModifier`、`iNumCitiesMaintenanceModifier` | 組織志向 |
| `SpecialistExtraCommerces` | 哲学志向 |
| `iReligionSpreadChange` | 宗教志向 |
| `NewCityCultureLevel` | 創造志向 |
| `iMaxNationalWondersPerCity`、`FeatureProductionChanges` | 勤労志向 |
| `iAttitudeChange`、`iWarWearinessModifier` | カリスマ志向 |
| `iOccupationTimeModifier`、`iOverseasMaintenanceModifier` | 帝国主義志向 |
| `FreePromotionSets` + `bNoDamagePenalty` の昇進 | 日本の武士道志向 |
| `bInlandCanals`（2026-11-28 から中立地帯・他国の領土の要塞も） | メフメド２世 |
| `LakePlotYieldChanges`、`LakePlotYieldChangesPerEra` | アステカ |
| `iEnslavementChance` の昇進 | モンテスマ |
| `iAnarchyGoldenAgePercent`、`iCombatPercent` | フランス |
| `bRiverNavigation`・`bFreeEmbark` の昇進 | ヴァイキング |
| `bPillageNoMoveCost`・`iPillageHealPercent`・`iPillageExperience` の昇進 | ラグナル |
| `Hurrys`（`bWondersOnly`・`bIgnoreCostModifier`） | 始皇帝 |
| `FeatureRoutes`・`MaintainFeatures` の森 | アメリカ先住民 |
| `FeatureRoutes`・`MaintainFeatures`・`FeatureHealthChanges`・`FeatureYieldChanges` のジャングル | クメール |
| 昇進の `TerrainDoubleMoves`・`TerrainDefenses` の砂漠、`StartBias/Terrains` の砂漠、石油の `RevealBonuses`・`StartBias/BonusBiases`・`BonusYieldChanges`、オアシスの `FeatureYieldChanges` | アラブ |
| `bClaimLastReligion`、`PyOnLastReligionClaimed` | アラブ（2026-11-27 に文明の志向を最後の預言者に替えた。上の行のベドウィンは XML に残るが誰も持たない） |
| `iHappinessPerReligion`、`CommercePerReligion` | インド |
| `TerrainYieldChanges`・`StartBias/Terrains` の丘陵・山岳、`TerrainYieldChangesPerEra` の山岳 | インカ |
| `iCorporationSpreadChange`、幹部への `bRivalTerritory` の昇進、`PyOnCorporationFounded` | オランダ |
| `PrereqBonus`（鉄・石炭・石油）+ `YieldModifiers` の生産 | ドイツ（2026-11-28 に `PrereqTech` から替えた） |
| `FeatureYieldChanges`・`FeatureHealthChanges`・`StartBias/Features` の氾濫原 | エジプト |
| `iCityRadiusChange`（文化レベルの条件なし）、`TerrainYieldChanges` のツンドラ・雪原 | ロシア（2026-11-28 に `iCultureRangeChange` から替えた） |
| `ImprovementYieldChanges` の牧草地、`UnitCombatFreeExperiences` の騎乗、`StartBias/BonusBiases` の馬 | モンゴル |
| `iCapitalBuildingProductionModifier` | アウグストゥス |
| `iOverseasCityPopulation`、`iNewWorldCityPopulation` | スペイン |
| `ExplorationRewards` の海外の旧世界・新世界、金・研究、艦船への歩哨の昇進 | ポルトガル |
| `Conquest/bStateReligion`、`Conquest/ReligionBuildings`、`Conquest/iBuildingSurviveModifier`、`Conquest/bNoPopulationLoss` | サラディン |
| `FeatureGrowthChanges` の森、陸の非戦闘ユニットへのウッドマンⅡ（`FreePromotionSets` の `bFreePromotionNoUnitCombat`） | シッティング・ブル |
| 外洋艦への目印の昇進と、`bHiddenNationality` の昇進を付け外しする呪文（`PromotionPrereq1`・`AddPromotionType1`・`RemovePromotionType1`・`PyResult`） | エリザベス |
| `bAtWar` + `YieldModifiers` の生産 | チャーチル |
| 昇進の `ImpassablePlotPassages` の山岳・`iEnemyHealChange`・`AttackFromTerrains` の山岳 | ハンニバル |
| `PyOnCityAcquired`・`PyOnPlayerEliminated`（`startGoldenAge`。占領で半分・滅亡で全部の黄金期） | 大キュロス |
| `UnitCombatProductionModifiers` の銃火器・装甲 | ビスマルク |
| `UpgradeAnywhereUnitCombats` の白兵戦・銃火器 | 誰も持たない（2026-11-28 にフリードリッヒから外した） |
| 全戦闘ユニットへの `iStrongerEnemyCombatPercent` の昇進 | フリードリッヒ |
| 騎乗ユニットへの `BonusCombatPercents`（馬）の昇進 | モンゴル |
| 昇進の `iOtherReligionCombatPercent`・`iStateReligionCityAttack` | イザベル |
| `NoObsolescence/BuildingClasses` の城壁・城 | ギルガメッシュ |
| `bNewCityStateReligion` | ザラ・ヤコブ |
| `bBarbarianAlly`、`PrereqTech`（騎乗）+ `bBarbarianVision` | チンギス・ハーン |
| `EnemyAttrition` | オラニエ公ヴィレム1世 |
| `iEnemyWarWearinessModifier` | ガンジー（`iDefenseCombatPercent` は 2026-11-28 に外した） |
| `NoObsolescence/BuildingClasses` の世界遺産 | ラムセス２世 |
| `PrereqTech` + `iFreeSpecialist` | リンカーン |
| `iImprovementUpgradeRateModifier`、`bImprovementUpgradeUnworked` | アメリカ |
| `iDistantTradeRouteModifier` | ハトシェプスト |
| `iCityRadiusChange` | スーリヤヴァルマン2世 |
| `iGoldenAgeLandMovesChange`、`bGoldenAge` + `iCombatPercent`、`GoldenAgeUnitClasses`（大将軍）、`iGoldenAgeUnitsChange`、昇進の `iGoldenAgeAdvantageCombatPercent` | ペルシャ |
| `WorldWonderCommerceChanges` の文化 | ルイ14世 |
| `RiverEdgeYieldPercents` | シュメール |
| 偵察ユニットへの `bChooseGoody`・歩哨の昇進、首都の建設時の斥候（`PyOnCityFounded`） | ジョアン2世 |
| `Hurrys` の資金（制限なし）、大商人の `UniqueUnits` | マンサ・ムサ |
| 征服した都市の徴兵ユニット、国教以外の宗教の数だけ追加（`PyOnCityAcquired` の `createConscripts`・`countCityReligions`） | オスマン |
| `SpecialistYieldChanges`（技術者・聖職者）、`SpecialistCommerceChanges`（科学者・商人・芸術家・スパイ）と定住した偉人 | ギリシャ |
| `Conquest/bLearnUniqueUnits`、`Conquest/bLearnUniqueBuildings` | アレクサンドロス |
| `iRouteMovementModifierInBorders` | ローマ |
| 昇進の `iCombatPercentOverseas`、艦船への `iMovesChange` の昇進と戦闘術Ⅰ、`UnitCombatProductionModifiers` の海洋 | イギリス |
| 都市のタイルを丘陵にする（`PyOnCityFounded` の `makeCityPlotHills`） | エチオピア |
| `Conscription` の一式（`iMaxConscript`・`iPopulationModifier`・`iMinCityPopulationChange`・`iMinCulturePercentChange`・`iPopAngerChange`・`iAngerLengthPercent`・`bNoExperiencePenalty`） | ズールー |
| `GlobalWorldWonderCommerceChanges` の研究 | ヴィクトリア |
| `iGoldenAgeModifier`、`iGoldenAgeGreatPeopleRateModifier`、`PyOnGreatPersonBorn` の `extendGoldenAge`、`bGoldenAge` + `CommerceModifiers` の文化 | ペリクレス |
| 昇進の `iCombatGoldStealPercent`・`iCombatGoldPerStrength`（2026-11-28 に `iGoldFromCombat` から替えた） | ブレヌス |
| `NoObsolescence/SpecialBuildings` の僧院、`PyOnReligionFounded`、`PyOnBuildingBuilt` の僧院で宣教師、`SpecialBuildingProductionModifiers` の大聖堂 | ユスティニアヌス1世 |
| 昇進の `BuildingCombatPercents`（国教の僧院・寺院・大聖堂・聖地） | ビザンチン |
| `iStateReligionHappiness`、`iStateReligionHealth` | アショーカ |
| `GreatPeopleClassRateModifiers` の大技術者、`CultureFlip/bNoRevoltAndNoFlip` | スレイマン |
| `iEnemyMoveCostInBorders` | オラニエ公ヴィレム1世 |
| `iRevengeCombatPercent`、`iWarWearinessModifier` -100、全戦闘ユニットへの `iWarWearinessCombatPercent` の昇進 | ブーディカ |
| `KnownTechCost` | ピョートル |
| `Conquest/bHeal` | ユリウス・カエサル |
| `CapitalTerrainYieldPercents`、`TerrainTrades` の砂漠 | マリ |
| `Hurrys` の資金（ユニットだけ、`iExperience`） | カルタゴ |
| 昇進の `FeatureDoubleMoves`・`FeatureDefenses` の森、`FeatureHappinessChanges`・`StartBias/Features`・`FeatureHealChanges` の森 | ケルト |
| `TerrainTrades` の丘陵・山岳、`PassableTerrainsInBorders` の山岳、陸の非戦闘ユニットへのゲリラⅡ | ワイナ・カパック |
| `SpecialistSlotChanges`（全専門家）、`ConditionalFreeTraits` の `PrereqTech`（教育） | 朝鮮 |
| `Conquest/iCulturePercent`、`bStackCityDefense` | 王建 |
| `PyOnCityLost`（`createPartisans`） | ド・ゴール |
| `bAtWar` + `PyOnGained`（`createCityConscripts`）、陸の戦闘ユニットへの `bRiver` の昇進 | ワシントン |
| `bAtWar` + `PyOnLost` で期間つきの志向（`CommerceModifiers` の文化、`iHappiness`） | 徳川家康 |
| `HealthyCommercePercents`（研究）、`PyOnBuildingBuilt`（`isBuildingClassIn`） | バビロニア |
| `CivicOptionExtraSlots`（法律） | ハンムラビ |
| `chooseGreatPeopleByCalendar`（`PyOnBeginTurn`。暦の周期ごとに選んだ偉人。`szTechTrait` で子の志向）、`bLongCountCalendar` | マヤ |
| `LendLease`（レンドリース） | ルーズヴェルト |
| 昇進の `Encirclement`（包囲と支援） | シャカ |
| `AttitudeCombatPercents`（AI の態度ごとの戦闘力）、`StackPromotions` の大将軍（`bStackUnitsToo`） | ナポレオン |
| `bGainTraitOnCapitalCapture`、`bGainTraitsOnElimination`、`iTradeRoutes` | クビライ |
| `Deals`（`bBuyCities`・`iGoldValueModifier`）、`iInflationModifier` | ダレイオス1世 |
| `NearbyCityCommerce`（研究・文化、首都 2 倍）、`iMinCityRangeChange`、`HealthyCommercePercents`（金） | パカル2世 |
| `CultureFlip`（`bInstantFlip`・`iRevoltChance`）、`GreatPeopleHurry`（偉大な芸術家） | エカチェリーナ2世 |
| 固有の社会制度（国有化の置換、`bNoForcedCivics`、`FreeTraits`）、`LockedImprovements`（工房）、`PyOnImprovementChanged`（小屋系・農地 → 工房） | 毛沢東 |
| 固有の大将軍（`UniqueUnits`、`UniqueNames` 12 人、`UnitClassUpgrades` で陸上の全軍事ユニットへ、`FreePromotions` で大将軍の昇進）、`PyOnCreated` の `gainExperience`・`keepExperienceOnUpgrade`、昇進の `iFreeXPPerTurn`・`iFreeXPCap` | シャルルマーニュ |

### 新しい志向で決まったこと（2026-11-15）

| 対象 | 内容 | タグ | 状態（志向） |
|---|---|---|---|
| メフメド２世 | 艦隊の山越え: 艦船が、水に面していない要塞・都市にも入れる。どこの領土でもない要塞と、入れる他国の領土（相互通行か戦争中）の要塞にも入れる（2026-11-28） | `bInlandCanals` | 実装済み `TRAIT_OVERLAND_FLEET` |
| アステカ（文明） | テノチティトラン: 湖のタイルの産出が増える。時代が 1 つ進むごとに湖の商業 +1（2026-11-19 に追加） | `LakePlotYieldChanges`、`LakePlotYieldChangesPerEra` | 実装済み `TRAIT_CHINAMPA` |
| モンテスマ | 花の戦争: 倒した敵を捕虜（労働者）にする | 昇進の `iEnslavementChance` | 実装済み `TRAIT_FLOWER_WAR` |
| フランス（文明） | 革命: 無政府期間が明けると黄金期。無政府期間中は全ユニットの戦闘力が上がる | `iAnarchyGoldenAgePercent`、`iCombatPercent` を持つ志向を `ConditionalFreeTraits` の `bAnarchy` で与える | 実装済み `TRAIT_REVOLUTION` |
| ヴァイキング（文明） | 作業船・ガレー船・三段櫂船・キャラベル船・ガレオン船・私掠船が川を進める（2026-11-28 から、川の陸のタイルにいる陸のユニットを攻撃・捕獲できる）。乗船・下船で移動力を余分に使わない | `FreePromotionSets` で、`bRiverNavigation`・`bRiverAttack` の昇進を作業船・ガレー船・三段櫂船・キャラベル船・ガレオン船・私掠船に、`bFreeEmbark` の昇進を陸上ユニットに配る | 実装済み `TRAIT_LONGSHIP` |
| ラグナル | 略奪で移動力を使わない。略奪するとユニットが回復する。自分のチームの領土の外で施設を略奪すると経験値 +3（2026-11-28） | `FreePromotionSets` で、`bPillageNoMoveCost`・`iPillageHealPercent`・`iPillageExperience` の昇進を陸上の戦闘ユニットに配る | 実装済み `TRAIT_RAIDING` |
| 始皇帝 | 奴隷制なしで、遺産（世界遺産・国家遺産）を人口で緊急生産できる。遺産の人口による緊急生産に追加の費用がかからない（生産 0 のときの +50% と人口を使ったときの不満は残る。2026-11-19 に `Hurrys` へ書き換えたので、追加の費用の無視は人口による緊急生産だけになった。以前は資金による緊急生産にも効いていた） | `Hurrys`（`HURRY_POPULATION`、`bWondersOnly`、`bIgnoreCostModifier`） | 実装済み `TRAIT_CORVEE` |
| アメリカ先住民（文明） | 森に特化: 自国の領土の森を道路として扱う（移動と交易網）。施設を作っても森が残る | `FeatureRoutes`（森 → `ROUTE_ROAD`）、`MaintainFeatures`（森） | 実装済み `TRAIT_FOREST_PEOPLE` |
| クメール（文明） | ジャングルに特化: 自国の領土のジャングルを道路として扱う。ジャングルの不衛生がなくなる。ジャングルの食料 +2（BtS の -1 と合わせて +1）。施設を作ってもジャングルが残る | `FeatureRoutes`（ジャングル → `ROUTE_ROAD`）、`FeatureHealthChanges`（ジャングル +25。BtS の -25 と合わせて 0）、`FeatureYieldChanges`（ジャングル 食料 +2）、`MaintainFeatures`（ジャングル） | 実装済み、ゲームで確認済み `TRAIT_JUNGLE_PEOPLE` |
| サラディン | 共存: 占領した都市に国教が広まり寺院が建つ、建物が残りやすい（+50%）。占領しても人口が減らない（2026-11-28） | `Conquest/bStateReligion`、`Conquest/ReligionBuildings`（寺院）、`Conquest/iBuildingSurviveModifier`、`Conquest/bNoPopulationLoss` | 実装済み `TRAIT_COEXISTENCE` |
| アラブ（文明） | ベドウィン: 騎乗ユニットが砂漠で移動 2 倍・防御 +25%。砂漠のバイアス。2026-11-19 に追加: 石油がゲーム開始時から見え、石油のバイアス（1 つにつき +50%）、石油のタイルの商業 +2、オアシスのタイルの生産 +1・商業 +1（マリの内向きの砂漠経済と分けて、資源とオアシスに寄せた。数値は仮） | 昇進の `TerrainDoubleMoves`・`TerrainDefenses`、`StartBias/Terrains`、`RevealBonuses`・`StartBias/BonusBiases`・`BonusYieldChanges`（石油）、`FeatureYieldChanges`（オアシス） | 実装済み `TRAIT_BEDOUIN`（2026-11-27 に下の行の最後の預言者に替えた。XML に残るが誰も持たない） |
| アラブ（文明） | 最後の預言者（短縮名「預言者」。2026-11-27 にベドウィンから替えた）: 他の文明が残り 2 つの宗教の一方を創始すると、最後の宗教の技術を無償で得て創始する。もう一方の技術も 1 ターン以内に第三の文明が終えそうなら、研究費の高い方を先に奪う。創始した聖地に大預言者 | `bClaimLastReligion`、`PyOnLastReligionClaimed` の `createUnits`（大預言者。偉人の誕生に数えない）。Help に大預言者を手で書く | 実装済み `TRAIT_LAST_PROPHET` |
| インド（文明） | 多宗教の受容: 都市の宗教 1 つにつき幸福 +1・文化 +1 | `iHappinessPerReligion`、`CommercePerReligion` | 実装済み `TRAIT_MULTI_RELIGION` |
| インカ（文明） | アンデスの民: 丘陵 食料 +1、山岳 食料 +2・生産 +1、時代が 1 つ進むごとに山岳の商業 +1（山岳も働かせられる）。丘陵・山岳のバイアス（2026-11-19 に山岳を食料 +1・生産 +1 から変更） | `TerrainYieldChanges`、`TerrainYieldChangesPerEra`、`StartBias/Terrains` | 実装済み `TRAIT_ANDES` |
| ワイナ・カパック | カパック・ニャン: 丘陵と山岳が交易網をつなぐ（山岳は通行不能でも交易網を止めない）。自国の領土の山岳にユニットが入れる（ダメージなし。ハンニバルは領土の外でも入れるがダメージを受ける）（2026-11-19）。陸の非戦闘ユニットにゲリラⅡ。丘陵のタイルが領土になると道路を敷く（技術不要、1 回だけ）（2026-11-28） | `TerrainTrades`（`TERRAIN_HILL`・`TERRAIN_PEAK`）、`PassableTerrainsInBorders`（`TERRAIN_PEAK`）、`FreePromotionSets`（`PROMOTION_GUERILLA2`）、`TerritoryRoutes`（`TERRAIN_HILL` → `ROUTE_ROAD`） | 実装済み `TRAIT_QHAPAQ_NAN` |
| ケルト（文明） | ドルイドの森: 全陸上ユニットが森で移動 2 倍・防御 +25%、森にいるユニットの回復 +20%、都市圏の森 1 つにつき幸福 +1、森のバイアス（2026-11-19。数値は仮。丘陵は固有ユニット・固有建造物に任せる） | `FreePromotionSets`（`PROMOTION_DRUID_FOREST`: `FeatureDoubleMoves`・`FeatureDefenses`）、`FeatureHappinessChanges`、`StartBias/Features`、`FeatureHealChanges` | 実装済み `TRAIT_DRUID_FOREST` |
| オランダ（文明） | 東インド会社: 幹部の成功率 +50%、幹部が国境を閉じた国にも入れる、企業を設立すると本社に幹部 3 体 | `iCorporationSpreadChange`、昇進の `bRivalTerritory`、`PyOnCorporationFounded` | 実装済み `TRAIT_VOC` |
| ドイツ（文明） | 産業化: 鉄があれば全都市の生産 +5%、石炭 +10%、石油 +10%（2026-11-28 に「産業主義で +10%」から替えた。`TRAIT_INDUSTRIAL_POWER` は誰も持たない） | `ConditionalFreeTraits` の `PrereqBonus` で `TRAIT_INDUSTRY_IRON`・`_COAL`・`_OIL`（`YieldModifiers`） | 実装済み、ゲームで確認済み `TRAIT_INDUSTRIALIZATION` |
| エジプト（文明） | ナイルの賜物: 氾濫原 生産 +1、不衛生なし。氾濫原のバイアス | `FeatureYieldChanges`、`FeatureHealthChanges`、`StartBias/Features` | 実装済み `TRAIT_NILE` |
| ロシア（文明） | 広大な大地: 都市圏の半径 +1（3。文化レベルの条件なし）、ツンドラ・雪原の食料 +1（2026-11-28 に文化圏 +1 から替えた。アンコールの効果を移した） | `iCityRadiusChange`、`TerrainYieldChanges` | 実装済み、ゲームで確認済み `TRAIT_VAST_LAND` |
| モンゴル（文明） | 騎馬の民: 牧草地 食料 +1、騎乗ユニットの経験値 +2。馬のバイアス。騎乗ユニットに昇進「駿馬」: 馬 1 つにつき戦闘力 +10%（輸入した馬も数える。上限 +50%）（2026-11-28） | `ImprovementYieldChanges`、`UnitCombatFreeExperiences`、`StartBias/BonusBiases`、`FreePromotionSets`（`PROMOTION_FINE_HORSES`。`BonusCombatPercents`・`iBonusCombatMaxPercent`） | 実装済み `TRAIT_HORSE_PEOPLE` |
| ローマ（文明） | アッピア街道: 自国の領土の道路・鉄道の移動コスト -50%（道路で 2 倍進める）。都市を建設すると、陸路 6 タイル以内で首都とつながるなら首都までの道路が最初から敷かれる（技術は不要。Civ6 のローマの能力から。2026-11-23） | `iRouteMovementModifierInBorders`、`PyOnCityFounded` の `buildRouteToCapital(pCity, 'ROUTE_ROAD', 6)` | 実装済み `TRAIT_APPIAN_WAY` |
| アウグストゥス | パクス・ロマーナ: 首都にある建物を他の都市で建てるとき生産 +50%（2026-11-17 にローマの文明志向から移した） | `iCapitalBuildingProductionModifier` | 実装済み `TRAIT_PAX_ROMANA` |
| ポルトガル（文明） | 大航海時代: そのターンに初めて見えた海外の陸地 1 タイルにつき、旧世界なら金 +1・研究 +1、新世界なら金 +2・研究 +2（ターンの終わりにまとめて。端数は持ち越し）。艦船に歩哨（2026-11-22 に作り直した。歩哨のほかの前の内容はスペインへ移した。地図によって弱く、外洋に出られるのも遅いので数値は大きめ。数値は仮） | `ExplorationRewards`（`EXPLORE_PLOT_OVERSEAS_OLD_WORLD` 金・研究 100、`EXPLORE_PLOT_OVERSEAS_NEW_WORLD` 金・研究 200）、`FreePromotionSets`（`PROMOTION_SENTRY`） | 実装済み `TRAIT_AGE_OF_DISCOVERY` |
| スペイン（文明） | 新世界探検: 海外に建てた都市の人口 +1、新世界に建てた都市の人口 +1（重なる）。そのうち沿岸の都市には灯台、新世界なら港も。最初に世界一周を達成すると黄金時代（2026-11-23 に追加。2 番目以降は何もない）（2026-11-22 にポルトガルの前の志向「大航海時代」を移して改名。艦船の歩哨はポルトガルに残した） | `iOverseasCityPopulation`、`iNewWorldCityPopulation`、`PyOnCityFounded`、`PyOnCircumnavigated`（`iOrder == 1 and startGoldenAge(pPlayer)`） | 実装済み `TRAIT_NEW_WORLD_EXPLORATION` |
| シッティング・ブル | 森を育む: 領土の森が広がりやすい。陸の非戦闘ユニットにウッドマンⅡ（2026-11-28） | `FeatureGrowthChanges`、`FreePromotionSets`（`PROMOTION_WOODSMAN2`） | 実装済み `TRAIT_FOREST_KEEPER` |
| エリザベス | 私掠免状: 外洋に出られる艦（キャラベル船以降。私掠船を除く）が、行動「私掠免状を交付」「私掠免状を返上」で国籍を隠す・戻す（どちらも移動力をすべて使い、1 ターンに 1 回）（2026-11-28 に「フリゲートが国籍を隠す」から替えた） | `FreePromotionSets` で目印の昇進 `PROMOTION_BLUE_WATER`（外洋艦隊）を 12 の艦船のクラスに配る。呪文 `SPELL_ISSUE_LETTERS_OF_MARQUE`（条件は外洋艦隊と `reqNotHasPromotion`）・`SPELL_RETURN_LETTERS_OF_MARQUE` が `PROMOTION_LETTERS_OF_MARQUE`（`bHiddenNationality`）を付け外しし、`PyResult` で `finishMoves`。`Help` | 実装済み、ゲームで確認済み `TRAIT_LETTERS_OF_MARQUE` |
| チャーチル | ネバーサレンダー（2026-11-28 に「最も輝ける時」から改名。短縮名「不屈」。効果の変更は段階 3）: 戦時は全都市の生産 +15% | `bAtWar` で `TRAIT_FINEST_HOUR_WAR`（`YieldModifiers`） | 実装済み `TRAIT_FINEST_HOUR` |
| ハンニバル | アルプス越え: 陸上ユニットが山岳に入れる（山岳でターンを迎えると 20% のダメージ）。敵の領土での回復 +10。山岳のタイルから攻撃すると戦闘力 +50%（奇襲。2026-11-28） | 昇進の `ImpassablePlotPassages`・`iEnemyHealChange`・`AttackFromTerrains`（`TERRAIN_PEAK`） | 実装済み `TRAIT_ALPS` |
| 大キュロス | 四方世界の王（短縮名「四」。2026-11-26 に寛容王（都市を占領しても人口が減らない。サラディンと印象が重なる）から作り直した）: 都市（蛮族を除く）を占領すると、通常の半分の期間の黄金期。最後の都市を占領して文明を滅ぼすと、通常の期間の黄金期（最後の都市では両方起きる）。黄金期の最中なら延び、次に必要な偉人の数は増えない | `PyOnCityAcquired`・`PyOnPlayerEliminated` の `startGoldenAge`（`getGoldenAgeLength` なのでゲーム速度などの補正が乗る）。Help に手で書く | 実装済み `TRAIT_FOUR_QUARTERS` |
| ビスマルク | 鉄と血: 銃火器・装甲ユニットの生産 +25% | `UnitCombatProductionModifiers` | 実装済み `TRAIT_BLOOD_AND_IRON` |
| フリードリッヒ | 軍隊を持つ国家: 白兵戦・銃火器ユニットが戦地でアップグレードできる（2026-11-28 に外した。データは残し、誰も持たない） | `UpgradeAnywhereUnitCombats` | 実装済み `TRAIT_ARMY_STATE` |
| フリードリッヒ | ブランデンブルクの奇跡（短縮名「奇跡」。2026-11-28）: 軍事力が自分より上の文明との戦闘力 +20% | `FreePromotionSets`（`bFreePromotionAllUnitCombats`）で昇進 `PROMOTION_BRANDENBURG_MIRACLE`（`iStrongerEnemyCombatPercent`） | 実装済み、ゲームで確認済み `TRAIT_BRANDENBURG_MIRACLE` |
| イザベル | レコンキスタ: 異教徒との戦闘力 +25%、国教の広まった都市への攻撃 +25% | 昇進の `iOtherReligionCombatPercent`・`iStateReligionCityAttack` | 実装済み `TRAIT_RECONQUISTA` |
| ギルガメッシュ | ウルクの城壁: 城壁と城が陳腐化しない | `NoObsolescence/BuildingClasses` | 実装済み `TRAIT_WALLS_OF_URUK` |
| ザラ・ヤコブ | 信仰の王: 新しい都市に国教が広まる | `bNewCityStateReligion` | 実装済み `TRAIT_KING_OF_FAITH` |
| チンギス・ハーン | 草原の統一: 蛮族と平和。騎乗を得ると蛮族の視界を共有する（蛮族のユニット・動物・都市が見ているタイルが見える。2026-11-19 に追加。視界は強力なので騎乗を条件にした） | `bBarbarianAlly`、`ConditionalFreeTraits` の `PrereqTech`（`TECH_HORSEBACK_RIDING`）で `TRAIT_STEPPE_UNITY_VISION`（`bBarbarianVision`） | 実装済み `TRAIT_STEPPE_UNITY` |
| スターリン | チェカ: 最初から生産「諜報」（ハンマーを諜報ポイントに）を選べる。固有ユニット「チェキスト」（スパイ。移動力 2、コスト 30）。諜報ポイント +25%。諜報任務「要人の殺害」「要人の誘拐」「体制転覆」（相手が採用できる社会制度を何でも押し付ける）「国教転覆」（自分の国教でない宗教か国教なしを押し付ける）を使える（ほかの文明は使えない。後の 2 つは 2026-11-23 に追加）。水源への毒混入・不満を煽るの値が 8 → 12 | 生産の `PrereqOrTraits`（`PROCESS_ESPIONAGE`）、`UniqueUnits`（`UNIT_CHEKIST`）、`CommerceModifiers`、諜報任務の `PrereqOrTraits`・`DisableTraits`（`ESPIONAGEMISSION_CHEKA_CITY_POISON_WATER`・`_UNHAPPINESS`） | 実装済み `TRAIT_CHEKA`（2026-11-22。焦土 `TRAIT_SCORCHED_EARTH` は同日に削除し、`EnemyAttrition` はヴィレムへ移した） |
| ガンジー | 非暴力不服従: 敵の厭戦感情 +100%（2026-11-28 に防衛時の戦闘力 +10% を外し、+50% から上げた。非難は段階 3） | `iEnemyWarWearinessModifier` | 実装済み `TRAIT_SATYAGRAHA` |
| ラムセス２世 | 我が名はオジマンディアス（2026-11-28 に「永遠の記念碑」から改名。短縮名「王」）: すべての世界遺産が陳腐化しない（BtS で陳腐化する 13 個を並べた） | `NoObsolescence/BuildingClasses` | 実装済み `TRAIT_ETERNAL_MONUMENTS` |
| リンカーン | 奴隷解放: 自由主義を得ると全都市に無償の専門家 +1 | `PrereqTech` で `TRAIT_EMANCIPATION_ACT`（`iFreeSpecialist`） | 実装済み、ゲームで確認済み `TRAIT_EMANCIPATION` |
| アメリカ（文明） | ホームステッド法: 小屋などの成長 +50%。文化圏の施設は市民を配置しなくても成長する（2026-11-23 に追加） | `iImprovementUpgradeRateModifier`、`bImprovementUpgradeUnworked` | 実装済み、ゲームで確認済み `TRAIT_HOMESTEAD` |
| ハトシェプスト | プント遠征: 遠く離れた都市との交易路 +50% | `iDistantTradeRouteModifier` | 実装済み、ゲームで確認済み `TRAIT_PUNT` |
| スーリヤヴァルマン2世 | アンコール: 文化レベル「洗練」以上の都市は都市圏の半径 +1 | `iCityRadiusChange` + `CityRadiusCultureLevel` | 実装済み、ゲームで確認済み `TRAIT_ANGKOR` |
| ペルシャ（文明） | 黄金の帝国（2026-11-26 に作り直した。黄金期を起こしやすく、強くする）: 黄金期は陸上ユニットの移動力 +1、全ユニットの戦闘力 +10%（+15% から下げた）。大将軍でも黄金期を起こせる（偉人とは別の種類）。偉人で起こす黄金期に必要な人数 -1（1・1・2 人…）。自国の黄金期に、黄金期でない文明との戦闘で戦闘力 +20%（戦闘ユニット） | `iGoldenAgeLandMovesChange`、`bGoldenAge` で `TRAIT_GOLDEN_EMPIRE_AGE`（`iCombatPercent` 10）、`GoldenAgeUnitClasses`、`iGoldenAgeUnitsChange` -1、`FreePromotionSets` で `PROMOTION_GOLDEN_EMPIRE`（`iGoldenAgeAdvantageCombatPercent` 20） | 実装済み `TRAIT_GOLDEN_EMPIRE` |
| ダレイオス1世 | ダレイコス金貨（短縮名「貨」。2026-11-26。金で領土を得る）: 平時でも AI から都市を金で買える。AI はダレイオスの出す金を 1.25 倍に評価する（都市の値段は 8 割）。インフレ率 -50% | `Deals/bBuyCities`、`Deals/iGoldValueModifier` 25、`iInflationModifier` -50 | 実装済み `TRAIT_DARIC` |
| ルイ14世 | 太陽王: 世界遺産のある都市は、遺産 1 つにつき文化 +3 | `WorldWonderCommerceChanges` | 実装済み、ゲームで確認済み `TRAIT_SUN_KING` |
| ジョアン2世 | 探検王: 偵察ユニットが部族集落の報酬を選べ、歩哨の昇進も持つ。首都を建設すると斥候 1 体。固有プロジェクト「トルデシリャス条約」（2026-11-24。光学か紙。コスト 300・数値は仮）: 完成時の首都を通る線で地図を東西に分け、選んだ側（AI は陸の多い側）が優先地域。優先地域の他文明の都市 1 つにつき毎ターン金 +1。他文明が優先地域に都市を建てると、建てた側が 25 ×（建てた側の時代 + 1）× 都市の人口（ゲーム速度で変わる）の金を払う・技術を 1 つ渡す・その都市に「教皇の非難」（`BUILDING_TORDESILLAS_VIOLATION`。不満 +3、建設不可、占領されても残る、核で消えない。2026-11-24 に「トルデシリャス条約違反」から改名。条約が教皇の勅書を下敷きにしていたため）が建つ、から選ぶ | `FreePromotionSets`（`bChooseGoody` の昇進 `PROMOTION_EXPLORER_KING`、`PROMOTION_SENTRY`）、`PyOnCityFounded`、`PROJECT_TORDESILLAS`（`PrereqOrTechs`・`PrereqOrTraits`）、`PyOnProjectBuilt` の `chooseTreatyZone`・`PyOnBeginTurn` の `treatyZoneTurn`・`PyOnOtherCityFounded` の `offerTreatyTerms`、`BUILDING_TORDESILLAS_VIOLATION` | 実装済み `TRAIT_EXPLORER_KING` |
| マンサ・ムサ | 黄金の巡礼: 普通選挙なしで資金による緊急生産ができる（2026-11-19 に「普通選挙を最初から採用できる」から変更）。大商人の代わりにワンガラ商人（移動力 3、交易任務の金 1.5 倍） | `Hurrys`（`HURRY_GOLD`）、`UniqueUnits`（`UNIT_WANGARA_TRADER`） | 実装済み `TRAIT_GOLDEN_PILGRIMAGE` |
| オスマン（文明） | デヴシルメ: 都市を征服すると、その都市で徴兵できるユニットが 1 体と、その都市の国教以外の宗教 1 つにつき 1 体加わる（国教がなければすべての宗教を数える。2026-11-28） | `PyOnCityAcquired`（`createConscripts`・`countCityReligions`） | 実装済み `TRAIT_DEVSHIRME` |
| ギリシャ（文明） | アゴラ: 専門家 1 人につき、科学者 研究 +1、商人 金 +1、技術者 生産 +1、芸術家 文化 +1、スパイ 諜報 +1、聖職者 商業 +1。定住した偉人はそれぞれ 2 倍 | `SpecialistYieldChanges`、`SpecialistCommerceChanges` | 実装済み `TRAIT_AGORA` |
| カルタゴ・イギリス・ヴァイキング・日本・ポルトガル・オランダ・スペイン（文明） | 開始位置のバイアス: 沿岸 +100%（2026-11-28 にヴァイキング・日本・ポルトガル・オランダ・スペインを足した） | 文明の `iStartBiasCoastal` | 実装済み、ゲームで確認済み |
| シュメール（文明） | 両河の間: タイルの川の辺 1 本につき商業 +1、3 本で食料 +1。開始位置のバイアス 川沿い +100%（文明に直接） | `RiverEdgeYieldPercents`（商業 100・食料 34）、文明の `iStartBiasRiver` | 実装済み `TRAIT_MESOPOTAMIA`（バイアスはゲームで確認済み） |
| アレクサンドロス | オリエンタリズム: 都市を征服すると、奪った相手の文明の固有ユニット・固有建造物を学ぶ（自分の固有がない種類だけ。同じ種類は後に学んだものに替わる）。既存の建物も建て替え（立地の条件は見ない）、作りかけのハンマーも引き継ぐ。今いるユニットはそのまま。ギリシャのファランクス・オデオンは替わらない | `Conquest/bLearnUniqueUnits`、`Conquest/bLearnUniqueBuildings` | 実装済み、ゲームで確認済み `TRAIT_ORIENTALISM` |
| イギリス（文明） | 日の沈まぬ帝国: 海外（首都と別の地域）で戦うユニットの戦闘力 +15%。艦船の移動力 +1。艦船の生産 +25%、艦船に戦闘術Ⅰ（2026-11-28。作業船は兵科がないので対象外） | 昇進の `iCombatPercentOverseas`（全戦闘ユニット）、BtS の昇進の `iMovesChange`（海洋）、`UnitCombatProductionModifiers`（海洋）、`FreePromotionSets`（`PROMOTION_COMBAT1`） | 実装済み、ゲームで確認済み `TRAIT_SUN_NEVER_SETS` |
| エチオピア（文明） | 高地の王国: 建設した都市のタイルが平原の丘陵になる（2026-11-19 にアドワの戦い（自国の領土の経験値 +50%・戦闘力 +15%）から作り直した。名前は仮） | `PyOnCityFounded` の `makeCityPlotHills` | 実装済み `TRAIT_HIGHLAND_KINGDOM` |
| ズールー（文明） | アマブト（年齢連隊）: 社会制度なしで毎ターン 2 体まで徴兵できる（マップの大きさで補正。デュエル・タイニーでも 1 体）。失う人口の計算 -50%（歩兵・機械化歩兵も 1 人）、必要な人口 -2（徴兵の分と合わせて 4）、必要な自国の文化 -10%（0%）、不満 -1 人（2 人）、不満の期間 -25%、徴兵したユニットも都市の経験値を全部得る（イカンダの +3 で昇進 1 つ）。2026-11-26 に `iFreeExperience` +2 から作り直した | `Conscription` の一式 | 実装済み、ゲームで確認済み `TRAIT_AMABUTHO` |
| ヴィクトリア | 万国博覧会: 持っている世界遺産 1 つにつき、全都市の研究 +1 | `GlobalWorldWonderCommerceChanges` | 実装済み、ゲームで確認済み `TRAIT_GREAT_EXHIBITION` |
| ペリクレス | ペリクレス時代: 黄金時代の期間 +50%、黄金時代の偉人ポイント +100%。黄金時代中に偉人（大将軍を除く）が生まれると黄金時代が 2 ターン延びる（ゲームの速度で調整。1 回の黄金時代で通常の長さまで）。黄金時代中は文化 +25%（2026-11-28） | `iGoldenAgeModifier`、`iGoldenAgeGreatPeopleRateModifier`、`PyOnGreatPersonBorn`（`extendGoldenAge`）・`PyOnGoldenAgeStart`（カウンターを 0 に）、`bGoldenAge` で `TRAIT_AGE_OF_PERICLES_GOLDEN`（`CommerceModifiers`）、`Help` | 実装済み、ゲームで確認済み `TRAIT_AGE_OF_PERICLES` |
| ブレヌス | 敗者に災いあれ: 戦闘に勝つたびに、負けた側の国庫の 5% を奪い、倒したユニットの基本の戦闘力 × 2 の金を得る（生産の速度で調整。2026-11-28 に金 +5 から替えた） | 昇進の `iCombatGoldStealPercent`（上限なし）・`iCombatGoldPerStrength` 200（全戦闘ユニット） | 実装済み、ゲームで確認済み `TRAIT_VAE_VICTIS` |
| ユスティニアヌス1世 | ハギア・ソフィア: 僧院が陳腐化しない。宗教を創始すると、聖都にその宗教の宣教師 +2 体（BtS の 1 体と合わせて 3 体。2026-11-22 に追加）。僧院を生産（購入を含む）で建てると、その都市にその宗教の宣教師が 1 体（上限を無視）。大聖堂の生産 +100%（2026-11-28） | `NoObsolescence/SpecialBuildings`（`SPECIALBUILDING_MONASTERY`）、`PyOnReligionFounded`（`createMissionaries(pPlayer, eReligion, 2, pCity)`）、`PyOnBuildingBuilt`（`isSpecialBuildingIn`・`createMissionaries`）、`SpecialBuildingProductionModifiers`（大聖堂）、`Help` | 実装済み `TRAIT_HAGIA_SOPHIA`（僧院はゲームで確認済み） |
| ビザンチン（文明） | 位階（Civ6 の位階から）: 航空以外の全戦闘ユニットが、国教の僧院 1 つにつき戦闘力 +2%、寺院 +3%、大聖堂 +5%、聖地 +10%（合計の上限 +50%）。陳腐化した僧院は数えないが、ユスティニアヌス1世なら僧院が陳腐化しないので残る | `FreePromotionSets`（`PROMOTION_RANKS`。昇進の `BuildingCombatPercents` の `bStateReligion`、聖地は 7 宗教の建物クラスを 1 つずつ、`iBuildingCombatMaxPercent` 50） | 実装済み `TRAIT_RANKS` |
| アショーカ | ダルマの勅令: 国教の広まった都市の幸福 +1・衛生 +1 | `iStateReligionHappiness`、`iStateReligionHealth` | 実装済み、ゲームで確認済み `TRAIT_DHAMMA` |
| スレイマン | 立法者（2026-11-28 に「大建築家スィナン」から改名。短縮名「立法」）: 大技術者の偉人ポイント +100%。自国の都市が文化で反乱・寝返りを起こさない（2026-11-28。他国の文化による不満なしは段階 2） | `GreatPeopleClassRateModifiers`（大技術者）、`CultureFlip/bNoRevoltAndNoFlip` | 実装済み、ゲームで確認済み `TRAIT_SINAN` |
| オラニエ公ヴィレム1世 | 洪水線: 自国の領土では、交戦中の敵の陸上ユニットの移動コスト +1。自国の領土にいる交戦中の敵が毎ターン最大 HP の 10% を失う（50% まで） | `iEnemyMoveCostInBorders`、`EnemyAttrition` | 実装済み `TRAIT_RELIEF_OF_LEIDEN`（移動コストはゲームで確認済み。2026-11-22 に「ライデンの解放」から改名し、損耗をスターリンから移した） |
| ブーディカ | 復讐の女王: 自分の都市を一度でも征服した文明との戦闘力 +25%。厭戦感情による不満がない。その相手との厭戦感情が生むはずの不満の割合 1% につき戦闘力 +1%（上限 +50%）（2026-11-28） | `iRevengeCombatPercent`、`iWarWearinessModifier` -100、`FreePromotionSets`（`bFreePromotionAllUnitCombats`）で昇進 `PROMOTION_REVENGE_QUEEN`（`iWarWearinessCombatPercent` 100・`iWarWearinessCombatMaxPercent` 50） | 実装済み、ゲームで確認済み `TRAIT_REVENGE_QUEEN` |
| ピョートル | 大使節団: その技術を持つ既知の文明 1 つにつき研究費 -10%（-75% まで） | `KnownTechCost` | 実装済み、ゲームで確認済み `TRAIT_GRAND_EMBASSY` |
| ユリウス・カエサル | 来た、見た、勝った: 都市を征服すると全ユニットが全回復 | `Conquest/bHeal` | 実装済み、ゲームで確認済み `TRAIT_VENI_VIDI_VICI` |
| マリ（文明） | サハラ交易: 首都の都市圏の砂漠 1 タイルにつき、首都の食料 +0.5・商業 +1（砂漠のタイルそのものは変わらない）。砂漠が交易網をつなぐ。砂漠のバイアス | `CapitalTerrainYieldPercents`（砂漠 食料 50・商業 100）、`TerrainTrades`（砂漠）、`StartBias/Terrains`（砂漠 10） | 実装済み `TRAIT_TRANS_SAHARAN` |
| カルタゴ（文明） | 傭兵: 普通選挙なしで、資金でユニットを緊急生産できる。資金で緊急生産したユニットは経験値 +2（普通選挙で緊急生産した場合も）（2026-11-19） | `Hurrys`（`HURRY_GOLD`、`bUnitsOnly`、`iExperience` 2） | 実装済み `TRAIT_MERCENARIES` |
| ド・ゴール | レジスタンス: 都市を占領される（破壊される）と、その近くにパルチザンが（人口 + 2）体現れる。首都で徴兵できるユニットで、都市の文化レベルに応じて戦闘術Ⅰ～Ⅵを持つ（2026-11-19。人口と文化レベルは占領される前のもの） | `PyOnCityLost` で `createPartisans` | 実装済み `TRAIT_RESISTANCE` |
| ワシントン | ミニットマン: 戦争が始まると（交戦相手がいない状態から）、首都と人口 3 以上の都市に、その都市で徴兵できるユニットが 1 体ずつ現れる（2026-11-19。2026-11-28 に人口 5 → 3）。陸の戦闘ユニットに昇進「デラウェア渡河」（渡河の攻撃の -25% を打ち消す。2026-11-28） | `bAtWar` で `TRAIT_MINUTEMEN_WAR`、その `PyOnGained` で `createCityConscripts`。`FreePromotionSets` で `bRiver` だけの昇進 `PROMOTION_DELAWARE` | 実装済み `TRAIT_MINUTEMEN` |
| 徳川家康 | 天下泰平: 戦争が終わると（交戦相手がいなくなると）、20 ターン（ゲーム速度で変わる）泰平志向（全都市の文化 +50%、幸福 +1）を得る（2026-11-19。数値は仮） | `bAtWar` で `TRAIT_PAX_TOKUGAWA_WAR`、その `PyOnLost` で `gainTraitForTurns`（`TRAIT_PAX_TOKUGAWA_PEACE`） | 実装済み `TRAIT_PAX_TOKUGAWA` |
| 朝鮮（文明） | 両班: ゲーム開始時から全都市で全専門家（聖職者・芸術家・科学者・商人・技術者・スパイ）の枠 +1。教育を得ると全専門家の枠が無制限になる（2026-11-21。名前は仮） | `SpecialistSlotChanges`（各 1）、`PrereqTech`（`TECH_EDUCATION`）で `TRAIT_MUNBAN`（文班。`SpecialistSlotChanges` 各 -1） | 実装済み、ゲームで確認済み `TRAIT_YANGBAN` |
| 王建 | 後三国統一: 征服した都市のほかの文明の文化の 50% を自分の文化に移す。都市の文化による防御と建造物の防御を合計する（2026-11-21。新羅の帰順・甄萱の亡命を受け入れた統一。防衛志向・火車の守りと合わせる） | `Conquest/iCulturePercent` 50、`bStackCityDefense` | 実装済み、ゲームで確認済み `TRAIT_LATER_THREE_KINGDOMS` |
| バビロニア（文明） | 空中庭園: 余剰の衛生 2 につき、その都市の研究 +1。穀物庫・水道橋・雑貨商・病院・リサイクル施設を生産で建てるたびに、その都市の人口 +1（2026-11-22） | `HealthyCommercePercents`（研究 50）、`PyOnBuildingBuilt` で `isBuildingClassIn` と `pCity.changePopulation(1)` | 実装済み、ゲームで確認済み `TRAIT_HANGING_GARDENS` |
| ハンムラビ | 法典: 法律の社会制度を同時にさらに 1 つ採用できる | `CivicOptionExtraSlots`（`CIVICOPTION_LEGAL` 1） | 実装済み、ゲームで確認済み `TRAIT_LAW_CODE` |
| 神聖ローマ（文明） | 帝国諸侯: 最初の都市を建てると、5 タイル以内にドイツ（指導者はフリードリッヒ）が AI の初期ユニットを連れて参加し、永久同盟で同じチームになる。参加した AI の AI 補正は、どの難易度でも開拓者（最弱。生産・成長のコスト 160% など）（2026-11-23。強いので補正を最弱にした） | `FoundingVassal`（`CIVILIZATION_GERMANY`・`LEADER_FREDERICK`・`HANDICAP_SETTLER`・`iRange` 5） | 実装済み `TRAIT_IMPERIAL_ESTATES` |
| マヤ（文明） | 長期暦: 神学を得た後、マヤ長期暦の周期（394 年。西暦 830・1224・1618・2012 年）が終わるたびに、偉人を 1 人選んで首都に出す（選んだ偉人は、全員を選ぶまで選べない。全員を選ぶとまた全員を選べる）。神学を得た次のターンから、右上の日付が長期暦になり、マウスを重ねると西暦が出る（Civ5 のマヤ。2026-11-23。2026-11-26 に、暦も偉人と同じ神学から切り替わるようにした。それまではゲーム開始時から長期暦だった） | `PyOnBeginTurn` の `chooseGreatPeopleByCalendar(pPlayer, eTrait, 'TECH_THEOLOGY', 394, 2012, 'TRAIT_LONG_COUNT_CALENDAR')`、子の `TRAIT_LONG_COUNT_CALENDAR`（マヤ暦志向、短縮名「紀」）の `bLongCountCalendar`。技術名は Python の 1 か所だけに書く | 実装済み `TRAIT_LONG_COUNT` |
| ルーズヴェルト | 民主主義の兵器廠（短縮名「廠」）: 自国の都市にいるユニットを、行動ボタン「レンドリース」で平和な文明の首都に送る（積み荷も一緒、UnitAI は既定に戻る）。送ったユニットの生産コストの 50% を研究、25% を金として得る。送るたびに相手の態度（取引）+1、+4 まで（2026-11-26。数値は仮。財政難にさせる抜け道は防がない） | `LendLease`（`iResearchPercent` 50・`iGoldPercent` 25・`iAttitudeChange` 1） | 実装済み `TRAIT_ARSENAL_OF_DEMOCRACY` |
| シャカ | 牛の角の陣（短縮名「角」）: 陸上の戦闘ユニットが、攻撃するときは相手の、守るときは自分の周囲にいる自軍の陸上ユニット（戦闘力 1 以上）1 体につき戦闘力 +1%、攻撃側の反対側の 3 タイル（背後）では +2%。1 タイル 10 体まで（横 4 タイルで最大 +40%、背後 3 タイルで +60%、計 +100%）。攻撃側のタイルは数えない。都市の攻撃・防御にも効く（2026-11-26。Civ6 の側面攻撃・支援から） | `FreePromotionSets`（`PROMOTION_BULL_HORNS`。昇進の `Encirclement` 1・2・10。陸上の 8 兵科と `DOMAIN_LAND`） | 実装済み、ゲームで確認済み `TRAIT_BULL_HORNS` |
| ナポレオン | 大陸軍（短縮名「陸」）: 対仏大同盟 — 自分への態度が「激怒している」AI の文明 1 つにつき全ユニットの戦闘力 +20%、「いらだっている」1 つにつき +10%（上限 +60%。人間・自分のチームは数えない。交戦中かどうかは別に数えない）。大陸軍 — ユニットのままの大将軍と同じタイルにいる白兵戦・騎乗・攻城・銃兵ユニットと、その大将軍自身に昇進「大陸軍」（移動力 +1）（2026-11-26。数値は仮。カリスマ志向の態度 +1 と向きが逆なのは演出として残す） | `AttitudeCombatPercents`、`StackPromotions`（`UNITCLASS_GREAT_GENERAL` → `PROMOTION_GRANDE_ARMEE`、`bStackUnitsToo`） | 実装済み `TRAIT_GRANDE_ARMEE` |
| クビライ | 大元ウルス（短縮名「元」）: 戦争で相手の首都を占領すると、相手の基本志向のうち持っていないものから 1 つをランダムに得る。相手を滅ぼすと、持っていない基本志向をすべて得る。ジャムチ（駅伝制）— 全都市の交易路 +1（2026-11-26。設計は 2026-11-25。渡るのは基本志向 11 個だけで、文明志向・指導者固有の志向はクセが強いので渡さない。`GainTraitsOnFriendly` は別の文明のために残す） | `bGainTraitOnCapitalCapture`、`bGainTraitsOnElimination`、`iTradeRoutes` 1 | 実装済み `TRAIT_YUAN_ULUS` |
| パカル2世 | 密集する都市国家（短縮名「邦」）: 都市どうしの最低距離 -1（間に 1 マスで建てられる）。余剰の衛生 2 につき、その都市の金 +1（2026-11-26 に 1 につき +1 から下げた。都市数の維持費を払い、狭い都市圏で余る拡張志向の衛生 +2 を生かす）。3 タイル以内にある自分のほかの都市 1 つにつき、その都市の研究 +1・文化 +1、首都では 2 倍（都市を詰めて建てる動機。2026-11-26。設計は 2026-11-25。数値は仮。バビロニアの `HealthyCommercePercents` は研究なので重複としない） | `iMinCityRangeChange` -1、`HealthyCommercePercents`（金 50）、`NearbyCityCommerce`（`iRange` 3・研究 1・文化 1・`bDoubleInCapital`） | 実装済み `TRAIT_CITY_STATES` |
| エカチェリーナ2世 | 啓蒙専制（短縮名「啓」）: ポーランド分割 — 文化で上回った他国の都市は反乱が起きやすく（+100%）、1 回目の反乱で寝返る。エルミタージュ — 都市画面の偉人のプログレスバーをクリックすると、足りない偉人ポイントを 1 ポイントあたり 3 ゴールドで買い、その場で偉大な芸術家が生まれる（1 都市につき 1 ターンに 1 回）。金で芸術家を買い、国境の都市で傑作を使って隣の都市を寝返らせる（スターリンは諜報、ピョートルは技術と分ける。2026-11-26。設計は 2026-11-25。数値は仮） | `CultureFlip`（`bInstantFlip` 1・`iRevoltChance` 100）、`GreatPeopleHurry`（`iGoldPerPoint` 3・`UNITCLASS_ARTIST`） | 実装済み `TRAIT_ENLIGHTENED_ABSOLUTISM` |
| 毛沢東 | 大躍進（短縮名「進」）: 国有化の代わりに固有の社会制度「大躍進」を採用できる。国有化の効果に加えて工房の生産 +1、決議とスパイによる社会制度の強制を受けない。採用している間は人民公社志向（短縮名「公」）を持つ: 小屋・集落・村・町を工房に替えると金（町で大商人の交易任務の平均の 4 分の 1。交易の利益 5 として `(iBaseTrade + iTradeMultiplier × 5) × 速度 / 4`、通常の速度で 375。村 40%・集落 15%・小屋 5%）、農地を工房に替えるとそのタイルの都市に生産（30 × `iTrainPercent`）。工房はほかの施設に変えられず、自分で略奪もできない（敵の略奪は BtS どおり）。都市の将来を売って、いまの工業を買う。共産主義の前は効果がない（承知の上。設計はユーザー、2026-11-25。2026-11-26） | 固有の社会制度 `CIVIC_GREAT_LEAP`（`ReplacedCivic` 国有化、`PrereqOrTraits`、`bNoForcedCivics`、`FreeTraits`）、`LockedImprovements`、`PyOnImprovementChanged` | 実装済み、ゲームで確認済み `TRAIT_GREAT_LEAP`・`TRAIT_PEOPLES_COMMUNE` |
| シャルルマーニュ | 十二聖騎士（短縮名「聖騎士」）: 大将軍の代わりに固有の大将軍「聖騎士」が生まれる。『ローランの歌』『狂えるオルランド』の 12 人（ローラン、オリヴィエ、テュルパン、オジエ、ネーム、ルノー、アストルフォ、ブラダマンテ、ルッジェーロ、マルフィーザ、ブランディマルテ、ドゥドーネ）の名前が付き、13 人目からは名前なし。経験値 30・大将軍の昇進・武勇（毎ターン経験値 +1、100 まで）を持ち、陸上の軍事ユニットに無料でアップグレードできる（名前・経験値・昇進は残り、大将軍の能力は失う）。両方の作品にいる人物はフランス語の名前にした（2026-11-26） | `UniqueUnits`、`UnitClassUpgrades`、`FreePromotions`（大将軍の昇進は BtS で `iUpgradeDiscount` 100 なので無料）、`PyOnCreated`（`gainExperience`・`keepExperienceOnUpgrade`。大将軍が合流したユニットの扱いにして、アップグレードで経験値が 10 に切り捨てられないようにした）、昇進の `iFreeXPPerTurn`・`iFreeXPCap` | 実装済み `TRAIT_TWELVE_PALADINS` |

- 2026-11-15 に XML に入れた。文明志向は文明の `Traits`、指導者志向は指導者の `Traits` の 3 つ目。テキストは `CIV4GameText_ClaudeCivs.xml`。昇進は `PROMOTION_FLOWER_WAR`（花の戦争）・`PROMOTION_LONGSHIP`（ロングシップ）・`PROMOTION_SEA_RAIDER`（上陸の心得）・`PROMOTION_RAIDING`（略奪行）。フランスの無政府期間中の志向は `TRAIT_REVOLUTIONARY_FERVOR`（革命の熱狂）
- 2 回目（2026-11-15）に入れた 34 件の仮の数値: 砂漠の移動 2 倍・防御 +25%、インカ・アラブ・エジプトのバイアス 1 タイルにつき +10%、馬のバイアス +50%、産業化 +10%、首都の建物 +50%、幹部 +50%、サラディンの建物 +50%、森の広がり +40、戦時の生産 +15%、山岳のダメージ 20%、銃火器・装甲 +25%、異教徒 +25%、損耗 10%（50% まで）、防衛 +10%、敵の厭戦感情 +50%
- 3 回目（2026-11-15）の仮の数値: 小屋の成長 +50%、遠い交易路 +50%、アンコールは文化レベル「洗練」から、黄金時代の戦闘力 +15%、沿岸・川のバイアス +100%
- 4 回目（2026-11-15）の仮の数値: ワンガラ商人の移動力 3（大商人は 2）、交易任務の金 1.5 倍（基本 750・倍率 300。大商人は 500・200）
- ギリシャの前の案（都市の間隔 `iMinCityRangeChange`）は空いた。ペリクレスの前の案（芸術家 文化 +2）はギリシャと数値の違いしかなくなったので、別の案にする（黄金時代の案など）
- カルタゴ（2026-11-19 に傭兵）・スペイン（2026-11-22 に新世界探検）とユスティニアヌス1世（サラディンと同じタグになった）は別の案を考える
- 数値はユーザーが決めていないものを仮に入れた（変えてよい）: 湖のタイル 食料 +1・生産 +1、花の戦争 25% で労働者、革命 黄金期は無政府期間の 200%・無政府期間中の戦闘力 +25%、略奪 1 回で HP 25% 回復、開始位置のバイアス 森・ジャングル 1 タイルにつき +10%
- これからは、確定したものをその都度 XML に入れる（2026-11-15 にユーザーの指示）
- 2026-11-17: ローマの文明志向をアッピア街道に替え、パクス・ロマーナはアウグストゥスに移した（ユーザーの管理票のメモから）。仮の数値: 道路の移動コスト -50%。移す先の指導者はユーザーが指定していないので、パクス・ロマーナを始めたアウグストゥスにした
- 5 回目（2026-11-17）の仮の数値: エチオピアの経験値 +50%、イギリスの海外の戦闘力 +15%、ズールーの経験値 +2、ヴィクトリアの研究 +1、ペリクレスの黄金時代 +50%・偉人ポイント +100%、ブレヌスの金 +5（エチオピアの領土内 +15% と艦船の移動力 +1 はユーザーの指定）。新しいタグの分: アショーカの幸福・衛生 +1、スレイマン +100%、ヴィレム +1、ブーディカ +25% は提案の値。ピョートルの -10%・-75% はユーザーの指定。ユスティニアヌス1世の寺院・大聖堂の生産 +100% は宗教志向（BtS）と同じ効果なので入れず、代わりの案を待つ。黄金時代の期間はゲーム開始時に 1 回だけ適用する
- 始皇帝の最初の案「長城線」（万里の長城が陳腐化しない）は取り下げた。BtS の万里の長城はもともと陳腐化しない（`ObsoleteTech` が `NONE`）
- 2026-11-19: マリの文明志向（ユーザーの管理票の記入「都市圏内の砂漠タイル 1 つにつき、首都タイルの食料と商業にボーナス」から）。仮の数値: 砂漠 1 タイルにつき食料 +0.5・商業 +1、砂漠のバイアス 1 タイルにつき +10%（アラブと同じ）。砂漠を交易網にするのはユーザーの指示
- 2026-11-22: バビロニアの文明志向（ユーザーの指定）。余剰の衛生は UB の庭園（衛生 +2）・世界遺産の空中庭園と合う。建物で人口を得る効果のために、master に志向のフック `PyOnBuildingBuilt` を足した
- 2026-11-22: ハンムラビの指導者志向「法典」（ユーザーの指定）。このために master に社会制度の枠（`CivicOptionExtraSlots`）を足した。略称の「法」はアショーカのダルマ志向が使っているので「典」
- 2026-11-23: 神聖ローマの文明志向「帝国諸侯」（ユーザーの指定。master に `FoundingVassal` を足した）。ドイツとフリードリッヒの志向は、バランスのため後半に効くものにしたほうがよいかもしれない（ユーザーの余談。ドイツの産業化は後半向け、フリードリッヒの軍隊を持つ国家は火薬以降で効く）

## リリースに向けたリメイク（2026-11-27〜28 に決定、未実装）

civfanatics での公開（`master` を枠組み、この Mod をその使用例として出す）に向けて、文明と指導者の志向を見直した。ユーザーの管理票 `docs/better_trait_mod_civs_v5.xlsx` の赤字をもとに、1 文明ずつ決めた。

- 進め方: まず理想の設計で志向を確定し、AI への対応はそのあとの段階で行う（2026-11-27 にユーザーが決定）
- 2026-11-27〜28 のセッションは設計だけ。実装は次以降のセッションで行う
- 管理票の「状態」の欄は古い。11-24〜27 に実装したもの（ペルシャの文明、ズールー・シャカ、中国・毛沢東、アラブの文明）は解決済み
- 上の「使った効果」と「新しい志向で決まったこと」の表は、実装したときに書き換える
- 外した志向（チナンパ `TRAIT_CHINAMPA`、ホームステッド法 `TRAIT_HOMESTEAD`、軍隊を持つ国家 `TRAIT_ARMY_STATE`）は、データを消さずに誰も持たない状態で残す（ベドウィンと同じ）。アンコール `TRAIT_ANGKOR` の効果はロシアの文明の志向に移す

### 実装の順番

新しいタグとフックは `master` で作ってから `civ-design` にマージし、文明の XML はそのあと `civ-design` で書く（`CLAUDE.md` の git の節）。`master` にいるときは `git show civ-design:docs/civ-design.md` でこの節を読む。

#### 段階 1: XML・Python だけ（DLL の変更なし）

共通関数が要るものは `master` の `CvHookUtils.py` に足す。

2026-11-28: 19 件すべてを実装し、ゲームで確認した（共通関数 `createConscripts`・`countCityReligions`・`isSpecialBuildingIn`・`extendGoldenAge` は `master`）。

- シッティング・ブル: 非戦闘ユニットにウッドマンⅡ
- イギリス（文明）: 艦船の生産 +25%、海洋ユニットに戦闘術Ⅰ
- エリザベス: 私掠免状の付け外しの呪文
- ガンディー: 防衛 +10% を外す、敵の厭戦感情 50 → 100（非難は段階 3）
- ラムセス2世: 改名だけ
- ドイツ（文明）: 資源ごとの生産
- フリードリッヒ: 軍隊を持つ国家を外す（新しい志向は段階 2）
- ロシア（文明）: 文化圏 +1 を外し、都市圏の半径 +1 とツンドラ・雪原の食料 +1
- ヴァイキング（文明）: キャラベル船・ガレオン船・私掠船も川を進める
- オスマン（文明）: デヴシルメの宗教の数
- スレイマン: 反乱・寝返りなし、改名（他国の文化による不満は段階 2）
- サラディン: 占領しても人口が減らない
- ワイナ・カパック: 非戦闘ユニットにゲリラⅡ（道路は段階 2）
- ハンニバル: 敵の領土での回復 +10（奇襲は段階 2）
- ペリクレス: 黄金時代の延長、黄金時代の文化 +25%
- ブレヌス: 国庫から奪う 5%（戦利品は段階 2）
- ブーディカ: 厭戦感情 -100%（戦闘力は段階 2）
- ユスティニアヌス1世: 僧院で宣教師、大聖堂の生産 +100%
- ワシントン: ミニットマンの人口 5 → 3、渡河の昇進（宗主国への戦闘力は段階 3）

#### 段階 2: 小さな DLL のタグ（`master`）

計算の 1 か所に入るもの。近いものをまとめて作る。

2026-11-28: 戦闘力の 4 件を実装し、ゲームで確認した（master `2dd37b9`）。戦闘の結果と移動の 5 件（戦利品・略奪の経験値・川の攻撃・運河・道）も実装し、確認した（master `f055185`）。厭戦感情と軍事力は志向のタグではなく昇進のタグにし、志向が `bFreePromotionAllUnitCombats`（全戦闘ユニット）で配る形にした。

- 戦闘力（`CvUnit::maxCombatStr` の周り）
  - 攻撃する側のいる地形・地形の特徴による攻撃ボーナス（昇進。ハンニバル）
  - 厭戦感情の値に比例した戦闘力（志向。ブーディカ）
  - 軍事力が自分より高い文明への戦闘力（志向。フリードリッヒ）
  - 資源 1 つにつき戦闘力、上限あり（昇進。モンゴル）
- 戦闘の結果
  - 倒したユニットの戦闘力に比例した金（昇進。ブレヌス）
  - 他国の領土での略奪で経験値（昇進。ラグナル）
  - 川の陸のタイルにいる船が陸のユニットを攻撃・捕獲できる（昇進 `bRiverAttack`。ヴァイキング。2026-11-28 に段階 1 の確認で判明し、まとまり B で実装）
- 都市の宗教の数（インド）: 宗教 1 つにつき商業 %、作ったユニットの経験値、偉人率。僧院なしで宣教師
- 都市圏の地形・地形の特徴の数による建物の生産 %（指定した建物クラスか分類。エチオピアの宗教建築、クメールの世界遺産。数は Python にも公開）
- 世界遺産の完成時（`CvCity::popOrder`）: 偉人ポイント × N ターン（ヴィクトリア）、ハンマーの ○% を文化に（ルイ14世）
- 他国との交易路 +%（ハトシェプスト）
- 他国の文化による不満なし（スレイマン）
- 領土になった地形に道路（ワイナ・カパック）
- `bInlandCanals` を文化圏の外の要塞にも（メフメド2世）
- ほかの文明が宗教を創始すると首都に広まる（ザラ・ヤコブ）
- 企業の `PrereqOrTraits`（オランダ）

#### 段階 3: 状態を保存する仕組み（`master`。セーブデータが変わる）

- 戦争を宣言した側の記録と、`ConditionalFreeTraits` の条件「自分に宣戦した相手と交戦中」（チャーチル）。ガンディーの非難にも使う
- チームごとのかつての宗主国の記録と、現在・かつての宗主国への戦闘力（ワシントン）
- `FoundingVassal` の拡張: AI 補正を省略可能に（省略時はゲームのほかの AI と同じ）、候補の文明からランダムに選ぶ、その指導者に基本志向を 1 つランダムに与える（神聖ローマ。AI 補正の省略はアメリカの宗主国でも使う）

#### 段階 4: 大きな仕組み

1 つずつ設計を詰めてから作る。

- アステカ: 労働者の小さな偉人化と強制労働
- アメリカ: 宗主国のダミー文明と独立戦争の黄金期
- エジプト: 運河の掘削（川を作る作業）
- モンゴル: 名馬（移動する固有の資源）
- オランダ: 東インド会社（海上の重役、沿岸だけの本社、GameFont のアイコン）
- 神聖ローマ: シャルルマーニュのシナリオの諸侯の文明（段階 3 の `FoundingVassal` の拡張を使う）

#### そのあと

- 昇進の XML で、使わない項目（値 0 や NONE）も書かないといけないのを、省略できるようにする（Schema の `minOccurs="0"` と読み込みの既定値。2026-11-28 にユーザーの要望）
- 既存の志向の戦闘のタグ（`iRevengeCombatPercent`・`AttitudeCombatPercents`・`iCombatPercent`・`iDefenseCombatPercent` など）を、昇進のタグにして志向が `FreePromotionSets` で配る形に移す（2026-11-28 に方針を決定。兵科・領域で絞れ、ほかの Info からも配れる）。兵科を並べて全戦闘ユニットに配っている志向（ブレヌス・ハンニバルなど）も `bFreePromotionAllUnitCombats` に書き換える
- AI への対応。判断コードが要るもの: アステカの生贄と強制労働、アメリカの独立、インドのほかの宗教の宣教師、オランダの海上の重役
- ゲームバランス、英語版（`CYBERFRONT` なしの DLL、英語版をもとにした Python）、配布の準備（検証用データを外す）

### 文明ごとの決定

#### アステカ

文明（労働者の生贄）。チナンパは外す。

- 労働者を小さな偉人として使う: 大技術者の生産の加速、大芸術家の傑作（文化）、大商人の交易任務（金）
- BtS の偉人の式をそのまま使い、基礎値は控えめにする（`iBaseHurry` + `iHurryMultiplier` × 人口、`iGreatWorkCulture`、`iBaseTrade` + `iTradeMultiplier` × 交易の利益。どれもゲームの速度で調整）
- どれも都市の中でだけ使える。得るものは労働者のコスト（ハンマー 60、食料で作れる）の半分くらいにして、作って生贄にするループにならないようにする。特別な捕虜ユニットは作らない
- 金（奴隷売買）: 大商人と同じく外国の都市だけ。労働者は `bRivalTerritory` 0 なので相互通行が要る（新しいコードは不要）
- 文化: BtS の傑作は反乱の期間を 0 にするが、労働者の分は 1 ターン短くするだけ（新しいコード）
- 強制労働（常に有効）: 地形改善（森の伐採も）がすぐに終わり、本来かかったターン数だけ労働者が動けなくなる（BtS の移動できないタイマー）。スタックなら、できればターン数を均等に割る（同じグループの労働者）。都市には施設を作れないので、生贄とぶつからない
- 作り方の案（ユーザーは未決定）: アステカの固有の労働者（`UNITCLASS_WORKER` の置き換え）に偉人の値を XML で持たせる。奴隷化と捕獲は文明のそのクラスのユニットを作るので、モンテスマの花の戦争（25% で労働者）ともかみ合う。新しいタグは 1 ターンの短縮と強制労働だけになる
- AI: 偉人の任務は偉人の UnitAI でしか使わないので、AI の段階で判断コードが要る

#### アメリカ

文明（植民地からの独立）。ホームステッド法は外す。

- 最初の都市を建てると、遠く（いちばん遠い開始位置。できれば別の大陸）に固有のダミー文明が現れ、アメリカはその属国になる。ダミー文明はイギリスではなく、選べず、ランダムの AI にもならない
- 取引の属国（`TRADE_VASSAL`）にして、解消のボタンが出るようにする（外交顧問の取引の一覧。属国は `PEACE_TREATY_LENGTH` のあと解消でき、宗主国はできない）。人間が属国になるには志向の `bHumanCanBeVassal`。属国なしのオプションのときや、空きの枠がないときは何もしない
- BtS がすでにすること: 宗主国は技術をくれる。AI の宗主国は属国が取引できる資源を要求し（`CvPlayerAI::AI_doDiplo`）、人間の属国が断ると宗主国に宣戦する。足すのは、要求の間隔を空ける待ち時間（アメリカの志向のタグ）だけで、基本のルールは変えない
- どの形で属国を抜けても（解消でも拒否でも）宗主国と戦争になる。属国が解消したら宗主国が宣戦する
- 黄金期は独立戦争に勝ったときだけ: 有利な条件で講和 = 通常の期間、宗主国の都市を占領 = 2 倍、講和なしで宗主国を滅ぼす = 3 倍。「有利な条件」の判定は実装のときに決める
- 宗主国の AI 補正は XML で指定でき、なければほかの AI と同じ

ワシントン:

- ミニットマンの人口の条件 5 → 3
- 現在とかつての宗主国への戦闘力ボーナス（かつての宗主国はチームごとに保存する汎用の仕組み）
- 渡河の攻撃: BtS の昇進の `bRiver` だけを持つ昇進を配る（`RIVER_ATTACK_MODIFIER` -25% を打ち消す。新しいコードなし）

AI: AI のアメリカはいつも要求に応じ、戦争なしで属国を抜けるので、強くなったら拒否して戦争を始めるルールが要る。

#### アメリカ先住民

- 文明の森の民: そのまま
- シッティング・ブル: 森を育むはそのまま。`FreePromotionSets`（`bFreePromotionNoUnitCombat` と `FreePromotionDomains` の `DOMAIN_LAND`）で、非戦闘ユニット（労働者・開拓者・宣教師・重役・偉人・スパイ）に `PROMOTION_WOODSMAN2` を配る

#### イギリス

- 文明の日の沈まぬ帝国: 海外 +15% と艦船の移動力 +1 はそのまま。艦船の生産 +25%（`UnitCombatProductionModifiers` の `UNITCOMBAT_NAVAL`）と、海洋ユニットに無償の戦闘術Ⅰ（`FreePromotionSets`）を足す
- エリザベス: 私掠免状を「`bHiddenNationality` を持つ昇進」にし、外洋に出られる全艦船が呪文で付け外しする
  - 切り替えると移動力をすべて使う（呪文の `PyResult` で `finishMoves`）
  - 「外洋に出られる」の条件: 外洋の艦船のクラスに `FreePromotionSets` で目印の昇進を配り、呪文の `PromotionPrereq` にする
  - 国籍を隠した船の輸送や、港への出入りは実装のときに確かめる
- チャーチル: 名前を「ネバーサレンダー」に変える
  - 自分に宣戦した相手と交戦中なら、生産 +15%。宣戦した側の記録が要る（防衛協定と属国は宣戦して参戦するので注意）
  - 航空ユニットには BtS の既存の昇進（迎撃Ⅰなど）を `FreePromotionSets` で配る
  - 未決定: いつも配るか、条件を満たす戦争中に作ったユニットだけか
- ヴィクトリア: 万国博覧会（世界遺産 1 つにつき研究 +1）に足す
  - 世界遺産を建てると、その都市にその遺産の偉人ポイント × N ターン分を与える（XML で N = 10。ストーンヘンジは大預言者 2 → 20）
  - 都市の偉人の進み具合と、ユニットの種類ごとの進み具合の両方に足す
  - BtS の世界遺産はすべて偉人ポイントを持つ（2。聖地と企業の本社は 1）
  - 既定: 補正なしの値、ゲームの速度の偉人の % で調整、征服で得た遺産は数えない
  - 未決定: 聖地と本社を数えるか

#### インド

3 つとも都市の宗教の数で効く。数値は目安。

- 文明の多宗教の受容: 宗教 1 つにつき幸福 +1・文化 +1 はそのまま。足す:
  - 宗教 1 つにつき研究 +5% くらい（商業の %。新しいタグ）
  - 僧院なしで宣教師を作れる（都市にあるどの宗教でも）
- アショーカ: ダルマの勅令を「都市の宗教 1 つにつき、その都市で作ったユニットに経験値 +1」に替える（新しいタグ）
- ガンディー:
  - 防衛 +10%（チャーチルと被る）を外す
  - 都市の宗教 1 つにつき偉人率 +10%（新しいタグ）
  - 敵の厭戦感情 +100%（今は 50）
  - 非難: ガンディーに宣戦した文明への、ほかの文明の態度が下がる（チャーチルの宣戦の記録を使う）
- AI: AI のインドがほかの宗教の宣教師を使うようにする

#### エジプト

- 文明のナイルの賜物: 今の効果はそのまま。労働者が運河を掘る
  - `BuildInfo` の作業にする（施設ではないので農地と両立する）。自国の文化圏だけ
  - 新しい川の辺は、端（角）が既存の川の辺か湖に接していること。源流を伸ばす、支流、湖から、をまとめて扱う。海岸からは掘れない
  - 辺の両側のタイルが自国の領土であること
  - 方向は 4 つの作業（労働者のタイルの北・東・南・西の辺）で選び、できるものだけ出す。流れは合流する川・湖に向かう
  - 新しい辺に接する、自国の文化圏の平地の砂漠は氾濫原になる
  - 技術は工学、作業時間は要塞くらい（`BUILD_FORT` の `iTime` 1000）。川は強いので厳しめにする
  - `CvPlot::setNOfRiver` / `setWOfRiver` とエンジンの `RebuildRiverPlotTile` で作れる見込み（BtS のワールドビルダーはゲーム中に川を描ける）
- ハトシェプスト: 遠い交易路 +50%（`iDistantTradeRouteModifier`）を、他国との交易路 +50% に替える（新しい志向のタグ。建物の `iForeignTradeRouteModifier` と同じ名前）。古いタグは残す
- ラムセス2世: 改名だけ（永遠の記念碑 → 我が名はオジマンディアス）

#### エチオピア

指導者は 1 人なので、文明と指導者はいつも組になる。

- 文明の高地の王国: 都市のタイルの丘陵はそのまま。岩窟教会を足す: 都市圏の丘陵 1 つにつき宗教建築の生産 +10%（都市のタイルは数えない）
  - 「丘陵の都市でボーナス」は、すべての都市が丘陵になるので不採用。丘陵の食料 +1 はインカが使っている
  - タグは汎用にする: 「都市圏の地形・地形の特徴 1 つにつき、指定した建物クラスか分類（宗教建築など）の生産 +%」。生産の計算は頻繁に呼ばれ、BtS の `getBuildingCostMod` の Python のコールバックは全体に効いて既定で無効なので、DLL にする（生産の内訳にも出る）。数は Python にも公開する
- ザラ・ヤコブ: 「ほかの文明が宗教を創始すると、エチオピアの首都にその宗教が広まる」を足す。さかのぼらない。`PyOnReligionFounded` は創始した側にしか呼ばれないので、DLL のタグか、ほかの文明の創始で呼ぶフックが要る

#### オランダ

- 文明の東インド会社: 固有の企業「東インド会社」をオランダだけが設立できる（企業の `PrereqOrTraits`。新しいタグ）
  - 設立は大商人、技術は経済学（最初に得た文明に大商人）
  - 暦の資源（バナナ・染料・香・絹・香辛料・砂糖）をすべて消費し、資源 1 つにつき金 +1・文化 +1。砂糖はスタンダード・エタノールと競合する
  - 維持費で破産しないようにする（都市ごとの維持費を金より小さく）
  - 重役は海上ユニットなので、沿岸の都市にしか広まらない。本社の建物は `bWater`（沿岸だけ）
  - アイコン: GameFont.tga に 2 つ（企業と本社）足す。8 つ目の企業を足しても、ほかのフォントの記号はずれない（`CvGameTextMgr` が行を詰めて割り当てる）。絵はユーザーが描くか、フリーのものを探す
  - 今の 3 つの効果（重役の成功率 +50%、ライバルの領土に入れる昇進、設立で重役 3 体）はそのまま
- AI: 海上の重役の AI コードが要る

#### ドイツ

- 文明: 産業化を、資源による生産に替える。鉄 +5%、石炭 +10%、石油 +10%（それぞれ `ConditionalFreeTraits` の既存の条件 `PrereqBonus` で、`YieldModifiers` を持つ子の志向を与える。XML だけ）
- ビスマルク: そのまま
- フリードリッヒ: 新しい志向「ブランデンブルクの奇跡」（七年戦争）: 軍事力が自分より高い文明との戦闘力 +20%（BtS の軍事力の値。ナポレオンは態度なので被らない）。今の軍隊を持つ国家（`UpgradeAnywhereUnitCombats`）は外して残す（ほかの指導者に回すかもしれない）

#### フランス

- 文明の革命: そのまま（無政府期間の戦闘力 +25% は仮。バランスの段階で見る）
- ナポレオン、ド・ゴール: そのまま
- ルイ14世: 遺産 1 つにつき文化 +3 はそのまま。世界遺産が完成した瞬間、そのハンマーの N% をその都市の文化に足す
  - 都市の生産コストを使うので、文化レベルの閾値と同じくゲームの速度に合う
  - 征服した遺産は数えない。聖地・本社はコスト 0
  - ヴィクトリアと同じきっかけで、効果は別

#### モンゴル

- 文明の騎馬の民: 牧草地の食料 +1、騎乗ユニットの経験値 +2、馬のバイアスはそのまま。移動する固有の資源「名馬」を足す
  - 既存の `BonusInfo/PrereqOrTraits`（モンゴルだけが見えて使える。モンゴルがゲームにいるときだけ置かれる）。数は BtS の配置の設定で 2〜4
  - 新しい汎用の `BonusInfo` のタグ（DLL。同期した乱数）: N ターンごとに、同じ地域の M タイル以内のランダムな置ける場所（`CvPlot::canHaveBonus`、その資源の地形。雪原には行かない）へ移る。陸か海かを明示するフラグ。使えるプレイヤーの領土に入ったときに変わる資源（馬）
  - 止まるのはモンゴルの文化圏の中だけ（モンゴルの国境が広がって覆ったときも）。ほかの文化圏は通り抜ける
- 戦闘: 新しい昇進のタグ「資源 1 つにつき戦闘力 +X%（上限あり）」を、`FreePromotionSets` で騎乗ユニットに配る。輸入した馬も数える（交易画面の数、`getNumAvailableBonuses`）
- 指導者: そのまま

#### ロシア

- 文明の広大な大地: 文化圏 +1（`iCultureRangeChange`）を外し、次にする
  - クメールのアンコールの効果（都市圏の半径 +1 で 3 に。`iCityRadiusChange` と `CityRadiusCultureLevel`）。文化レベル「洗練」の条件は残す前提（ユーザーは未確認）
  - ツンドラ・雪原の食料 +1（`TerrainYieldChanges`）
- エカチェリーナ2世、スターリン、ピョートル: そのまま

#### ヴァイキング

- 文明のロングシップ: 川を進める艦船に、キャラベル船・ガレオン船・私掠船を足す（固有ユニットもクラスで含む）。フリゲート・戦列艦は入れない
  - 私掠船で川沿いの労働者を襲うのがユーザーの狙い。川の陸のタイルにいる船が陸のユニットを攻撃・捕獲できるか、国籍を隠した船との関係は実装のときに確かめる
- ラグナル: 新しい昇進のタグ: 他国の領土で略奪すると経験値 +3（戦争の行為としての略奪。BtS は自分の施設も略奪できるので、自国の領土では何も得ない）

#### 神聖ローマ

- 文明の帝国諸侯: `FoundingVassal` で参加するのを、ドイツ・フリードリッヒから、毎回ランダムな諸侯の文明に替える
  - 諸侯は公式シナリオ「Charlemagne」から。絵は BtS 本体の Assets にある（`Art/Leaderheads/Charlemagne_Scenario/*.dds` の 2D の静止画、`Art/Interface/Buttons/Charlemagne_Atlas.dds`）ので、パスで参照してコピーしない。readme にクレジットを書く
  - 候補: アウストラシア（シナリオの指導者がシャルルマーニュ本人）以外の公国すべて（ネウストリア・キルペリク1世、アキテーヌ・アリエノール、ブルグント・グンドバド、ランゴバルド・アルボイン、ザクセン、バイエルン）
  - 諸侯の文明: 選べず、ランダムの AI にもならない。文明の志向なし。指導者は毎回、BtS の基本志向から 1 つだけランダムに得る
  - AI 補正は省略可能にする（省略時はほかの AI と同じ）
- シャルルマーニュ: 実装済み

#### オスマン

- 文明のデヴシルメ: 征服した都市にある、オスマンの国教以外の宗教 1 つにつき、徴兵できるユニットを 1 体追加（今の 1 体に足す）。国教がないときはすべての宗教を数える（Claude の既定）。Python のフックだけ
- スレイマン: 大建築家スィナンを「立法者（カーヌーニー）」に改名し、次の 3 つにする。国教以外の宗教で得をする案は、インドと被るので不採用
  1. 自国の都市が文化で反乱・寝返りを起こさない（既存の `CultureFlip` の `bNoRevoltAndNoFlip`）
  2. 他国の文化による不満がない（新しいタグ。BtS の `CvCity::getCulturePercentAnger` は、ほかの生きているチームすべての文化を数え、戦争中の相手の分を `AT_WAR_CULTURE_ANGER_MODIFIER` で増やす）
  3. 大技術者の偉人ポイント +100% は残す
  - 占領した都市の反乱期間 -50% は、基本志向の帝国主義志向にあるので使わない
- メフメド2世: 艦隊の山越え（`bInlandCanals`）を文化圏の外でも効くようにする
  - 今は BtS どおり、要塞が都市として扱われるのは自国（と属国）の領土か、どこの領土でもない場所だけ（`CvPlot::isCity`・`isFriendlyCity`）
  - 他国の領土の要塞にも、船がそのタイルに入れるとき（相互通行、または戦争中）は入れるようにする
  - 敵のユニットのいる要塞と、敵の領土の要塞に入った船への攻撃は、実装のときに確かめる

#### アラブ

- 文明の最後の預言者: 実装済み
- サラディン: 共存に「都市を占領しても人口が減らない」を足す（`Conquest/bNoPopulationLoss`。今は検証用の志向だけが使っている）

#### インカ

- 文明のアンデスの民: そのまま
- ワイナ・カパック: カパック・ニャンの今の効果（丘陵と山岳が交易網をつなぐ、自国の領土の山岳に入れる）に足す
  1. 非戦闘ユニットに `PROMOTION_GUERILLA2`（シッティング・ブルと同じ配り方。効くのは丘陵の移動 2 倍だけ）
  2. 丘陵のタイルがインカの領土になったとき（都市の建設・文化圏の拡大・征服）、道路を敷く。技術は不要（インカは車輪なしでインカ道を作った）。略奪された道路は直さない（領土になったときの 1 回だけ）。新しい汎用の DLL のタグ（地形と道路の種類。`CvPlot::setOwner`。丘陵は `TerrainTrades` と同じく `TERRAIN_HILL` で書く）

#### カルタゴ

- 文明の傭兵: そのまま
- ハンニバル: アルプス越えの昇進に足す（山岳のダメージ 20% は、バランスの段階まで今のまま）
  1. 奇襲: 山岳のタイルから攻撃すると戦闘力 +50%（新しい汎用の昇進のタグ「攻撃する側のいる地形・地形の特徴ごとの攻撃ボーナス」。戦闘力の計算に入れ、AI の勝率にも入るようにする）
  2. 現地調達: 敵の領土での回復 +10（BtS の昇進の `iEnemyHealChange`）

#### ギリシャ

- 文明のアゴラ、アレクサンドロス: そのまま
- ペリクレス: 黄金時代の期間 +50% と、黄金時代の偉人ポイント +100% はそのまま。足す:
  1. 黄金時代のあいだに偉人が生まれると、黄金時代が 2 ターン延びる（ゲームの速度で調整）。大将軍は数えない。1 回の黄金時代で延びる合計は、その黄金時代の長さまで（最大 2 倍）。`PyOnGreatPersonBorn` と `PyOnGoldenAgeStart`（数え直し）で書く
  2. 黄金時代中、文化 +25%（`ConditionalFreeTraits` の `bGoldenAge`）
  - ペルシャは黄金時代を始めやすくし、ペリクレスは長く続ける

#### ケルト

- 文明のドルイドの森: そのまま
- ブレヌス: 敗者に災いあれの昇進の、固定の金 +5（`iGoldFromCombat`）を替える
  1. 戦利品: 倒したユニットの基本の戦闘力 × 2 の金（ゲームの速度で調整。新しい昇進のタグ。蛮族も対象）
  2. 身代金: 勝つたびに、負けた側の国庫の 5% を奪う（既存の `iCombatGoldStealPercent`。今は検証用の昇進だけが使っている。上限の `iCombatGoldStealMax` は固定値で時代についてこないので使わない）
- ブーディカ: 復讐の女王 +25%（`iRevengeCombatPercent`）はそのまま。足す:
  1. 厭戦感情による不満がない（既存の `iWarWearinessModifier` -100）
  2. その相手との厭戦感情の値に比例して、その相手への戦闘力 +%（上限 +50%）。値は `CvTeam::getWarWeariness`（不満が 0 でも数え続ける。戦争中は少しずつ減り、和平で 99% 減る）。新しい志向のタグで、`iRevengeCombatPercent` と同じく戦闘力の計算に入れる。式は実装のときに決める（例: 不満にするはずだった割合 → 戦闘力の %）。相手の `iEnemyWarWearinessModifier`（ガンディーの +100%）は入れない

#### ビザンチン

- 文明の位階: そのまま
- ユスティニアヌス1世: ハギア・ソフィアの今の効果（僧院が陳腐化しない、創始で宣教師 +2）に足す
  1. 僧院を生産（購入を含む）で建てると、その都市にその僧院の宗教の宣教師が 1 体出る（`PyOnBuildingBuilt` と `createMissionaries`。征服や無料の建物では出ない）。宣教師の上限（宗教ごとに 3 体）は無視する（創始の 2 体と同じ）
  2. 大聖堂の生産 +100%（既存の `SpecialBuildingProductionModifiers`。今は検証用の志向だけが使っている）。寺院は基本志向の宗教志向と被るので入れない。僧院は宣教師が出るので入れない

#### クメール

- 文明の密林の民: そのまま
- スーリヤヴァルマン2世: アンコール（ロシアへ移す）の代わりに、新しい志向「密林の大寺院」（名前は仮）: 都市圏のジャングル 1 つにつき、世界遺産の生産 +3%（半径 2 で最大 +60%）
  - クメールの施設の下に残るジャングル（`MaintainFeatures`）も数える
  - エチオピアの汎用のタグに「世界遺産」の分類を足して使う
  - 案B（国教を変えても無政府状態にならず、旧国教の都市に新しい国教が広まる）は不採用

#### 未決定の小さな点

実装のときに決めればよい。

- アステカ: 固有の労働者で作るか
- アメリカ: 「有利な条件で講和」の判定
- チャーチル: 航空ユニットの昇進をいつも配るか、条件を満たす戦争中に作ったユニットだけか
- ヴィクトリア: 聖地と企業の本社も対象にするか
- ロシア: 都市圏の半径 +1 に「洗練」の条件を残すか

## 文明

| 文明 | 追加した要素 | 内容 |
|---|---|---|
| `CIVILIZATION_JAPAN`（日本） | 文明の `Traits` に `TRAIT_BUSHIDO` | Civ5 の文明能力「武士道」。指導者（徳川家康）の志向は元のまま |
| `CIVILIZATION_CHINA`（中国） | 文明の `Traits` に `TRAIT_HUNDRED_SCHOOLS` | 諸子百家（2026-11-24）。指導者の志向は元のまま |

## 志向と昇進

| 項目 | 内容 |
|---|---|
| `TRAIT_BUSHIDO`（武士道志向、短縮名「武」） | `FreePromotionSets` で、航空以外の全戦闘ユニット（偵察・弓兵・騎乗・白兵戦・攻城・銃・装甲・ヘリコプター・海洋）に `PROMOTION_BUSHIDO` を配る |
| `PROMOTION_BUSHIDO`（武士道） | `bNoDamagePenalty` 1。`UnitCombats` は空でレベルアップでは選べない。アイコンはコマンドーの流用 |
| `TRAIT_HUNDRED_SCHOOLS`（諸子百家志向、短縮名「百」） | 7 人の世界ユニット（生産 60、`PrereqOrTraits` でこの志向だけ）を作れる。張儀以外の 6 人は専用の専門家として定住し（`bExclusiveSettle` で 1 都市 1 人）、`UnsettleSpecialists` でユニットに戻せる。`StackPromotions` で孫武と同じタイルに孫子の兵法、墨子と同じタイルに墨守 |
| `UNIT_CONFUCIUS`・`SPECIALIST_CONFUCIUS`（孔子（儒家）） | 法律。幸福 +2、この都市の専門家 1 人につき金 +1・研究 +1（`ExtraSpecialistCommerces`） |
| `UNIT_HAN_FEIZI`・`SPECIALIST_HAN_FEIZI`（韓非子（法家）） | 君主政治。生産 +25%、維持費 -50%、諜報への防御 +50%、不満なし、偉人ポイント -100% |
| `UNIT_SUN_WU`・`SPECIALIST_SUN_WU`（孫武（兵家）） | 鉄器。新しいユニットに経験値 +4、大将軍の偉人ポイント +3、都市圏での戦闘の大将軍ポイント +100% |
| `UNIT_MOZI`・`SPECIALIST_MOZI`（墨子（墨家）） | 数学。都市防御 +50%、砲撃への耐性 50% |
| `UNIT_LAOZI`・`SPECIALIST_LAOZI`（老子（道家）） | 哲学。食料 +20%、都市圏の森 1 つにつき幸福 +1、無政府状態でも止まらない |
| `UNIT_ZOU_YAN`・`SPECIALIST_ZOU_YAN`（鄒衍（陰陽家）） | 天文学。天文暦法: 研究 +25%（`CommerceModifiers`）、無償の科学者 1 人（`FreeSpecialistCounts`。偉人ではない） |
| `UNIT_ZHANG_YI`（張儀（縦横家）） | 筆記。スパイの複製（見えない、諜報任務）。`bInvestigate`（都市の様子）、`iStationedAttitudeChange` 3。定住しない |
| `PROMOTION_ART_OF_WAR`（孫子の兵法） | 戦闘力 +10%、撤退率 +20%。状態の昇進 |
| `PROMOTION_MOHIST_DEFENSE`（墨守） | 都市防御 +25%。状態の昇進 |
| `TRAIT_BULL_HORNS`（牛の角の陣志向、短縮名「角」） | シャカの 3 つ目の志向。`FreePromotionSets` で、陸上の戦闘ユニット（弓兵・白兵戦・騎乗・偵察・銃・攻城・装甲・ヘリコプター。`DOMAIN_LAND` に絞る）に `PROMOTION_BULL_HORNS` |
| `PROMOTION_BULL_HORNS`（牛の角の陣） | `Encirclement`: 横 1 体につき +1%、背後 +2%、1 タイル 10 体まで。`UnitCombats` は空でレベルアップでは選べない。アイコンは側面攻撃Ⅱの流用 |
| `TRAIT_GRANDE_ARMEE`（大陸軍志向、短縮名「陸」） | ナポレオンの 3 つ目の志向。`AttitudeCombatPercents`（激怒 +20%・いらだち +10%・上限 60）と `StackPromotions`（大将軍と同じタイルの白兵戦・騎乗・攻城・銃兵に `PROMOTION_GRANDE_ARMEE`、`bStackUnitsToo` で大将軍自身にも） |
| `PROMOTION_GRANDE_ARMEE`（大陸軍） | 移動力 +1。`UnitCombats` は空でレベルアップでは選べない。アイコンは機動力Ⅰの流用 |
| `TRAIT_YUAN_ULUS`（大元ウルス志向、短縮名「元」） | クビライの 3 つ目の志向。`iTradeRoutes` 1、`bGainTraitOnCapitalCapture` 1、`bGainTraitsOnElimination` 1。Help は書かない（3 つともタグの説明文が出る） |
| `TRAIT_FOUR_QUARTERS`（四方世界の王志向、短縮名「四」） | 大キュロスの 3 つ目の志向（`TRAIT_TOLERANCE` の置き換え。番号は同じ）。`PyOnCityAcquired`（`bConquest` で蛮族以外なら `startGoldenAge(pPlayer, 50)`）、`PyOnPlayerEliminated`（`startGoldenAge(pPlayer)`）。Help `TXT_KEY_TRAIT_FOUR_QUARTERS_HELP` |
| `TRAIT_DARIC`（ダレイコス金貨志向、短縮名「貨」） | ダレイオス1世の 3 つ目の志向。`Deals`（`bBuyCities` 1、`iGoldValueModifier` 25）、`iInflationModifier` -50 |
| `PROMOTION_GOLDEN_EMPIRE`（黄金の帝国） | `iGoldenAgeAdvantageCombatPercent` 20。黄金の帝国志向の `FreePromotionSets` で、航空以外の全戦闘ユニット（偵察・弓兵・騎乗・白兵戦・攻城・銃・装甲・ヘリコプター・海洋）に付く。`UnitCombats` は空でレベルアップでは選べない。アイコンは戦闘Ⅰの流用 |
| `SPELL_LIANHENG`（連衡） | 張儀だけ。戦争中の文明の都市で、その文明と和平し、張儀は消える |
| `TRAIT_GREAT_LEAP`（大躍進志向、短縮名「進」） | 毛沢東の 3 つ目の志向。中身はなく、`CIVIC_GREAT_LEAP` の `PrereqOrTraits` の対象（説明文は固有の社会制度の行が出る） |
| `CIVIC_GREAT_LEAP`（大躍進） | 国有化の複製（共産主義、`ReplacedCivic` 国有化）。工房の産出を食料 +1・生産 +1 に（国有化は食料 +1）。`bNoForcedCivics` 1、`FreeTraits` で人民公社志向 |
| `TRAIT_PEOPLES_COMMUNE`（人民公社志向、短縮名「公」） | `LockedImprovements`（工房）、`PyOnImprovementChanged`（金・生産とメッセージ `TXT_KEY_PEOPLES_COMMUNE_GOLD`・`_PRODUCTION`。生産は `gainImprovementProduction` で、都市画面の内訳に「施設の転換により」の行）。Help `TXT_KEY_TRAIT_PEOPLES_COMMUNE_HELP` |
| `TRAIT_TWELVE_PALADINS`（十二聖騎士志向、短縮名「聖騎士」） | シャルルマーニュの 3 つ目の志向。`UniqueUnits` で大将軍を聖騎士に置き換える。`Help` に聖騎士の経験値・アップグレードを書いた |
| `UNIT_PALADIN`（聖騎士） | 大将軍の複製（合流・定住・陸軍士官学校、グラフィックとボタンも同じ）。`UniqueNames` 12 人（`TXT_KEY_PALADIN_*`）、`UnitClassUpgrades` は陸上で戦闘力と兵科を持つ 36 クラス（偵察・白兵戦・銃・弓兵・騎乗・装甲・ヘリコプター・攻城）、`FreePromotions` 大将軍・武勇、`PyOnCreated` `(gainExperience(pUnit, 30), keepExperienceOnUpgrade(pUnit))` |
| `PROMOTION_VALOR`（武勇） | `iFreeXPPerTurn` 1、`iFreeXPCap` 100。`UnitCombats` は空でレベルアップでは選べない。アイコンは統率の流用 |
| `TRAIT_LAST_PROPHET`（最後の預言者志向、短縮名「預言者」） | アラブの文明の志向（`TRAIT_BEDOUIN` の置き換え。ベドウィンの志向は XML に残し、誰も持たない）。`bClaimLastReligion` 1、`PyOnLastReligionClaimed` で聖地に大預言者 1 人 |

## 確認の記録

- 2026-11-07: 日本の武士道をゲーム内で確認（ユーザーの確認）
- 2026-11-23: ローマのアッピア街道に、建設した都市から首都への道路を足した（ユーザーの指定。Civ6 のローマの能力。Civ6 の交易路の射程 15 タイルは強すぎるので 6 タイルにした）
