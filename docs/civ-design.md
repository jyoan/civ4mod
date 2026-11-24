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
| `bInlandCanals` | メフメド２世 |
| `LakePlotYieldChanges`、`LakePlotYieldChangesPerEra` | アステカ |
| `iEnslavementChance` の昇進 | モンテスマ |
| `iAnarchyGoldenAgePercent`、`iCombatPercent` | フランス |
| `bRiverNavigation`・`bFreeEmbark` の昇進 | ヴァイキング |
| `bPillageNoMoveCost`・`iPillageHealPercent` の昇進 | ラグナル |
| `Hurrys`（`bWondersOnly`・`bIgnoreCostModifier`） | 始皇帝 |
| `FeatureRoutes`・`MaintainFeatures` の森 | アメリカ先住民 |
| `FeatureRoutes`・`MaintainFeatures`・`FeatureHealthChanges`・`FeatureYieldChanges` のジャングル | クメール |
| 昇進の `TerrainDoubleMoves`・`TerrainDefenses` の砂漠、`StartBias/Terrains` の砂漠、石油の `RevealBonuses`・`StartBias/BonusBiases`・`BonusYieldChanges`、オアシスの `FeatureYieldChanges` | アラブ |
| `iHappinessPerReligion`、`CommercePerReligion` | インド |
| `TerrainYieldChanges`・`StartBias/Terrains` の丘陵・山岳、`TerrainYieldChangesPerEra` の山岳 | インカ |
| `iCorporationSpreadChange`、幹部への `bRivalTerritory` の昇進、`PyOnCorporationFounded` | オランダ |
| `PrereqTech` + `YieldModifiers` の生産 | ドイツ |
| `FeatureYieldChanges`・`FeatureHealthChanges`・`StartBias/Features` の氾濫原 | エジプト |
| `iCultureRangeChange` | ロシア |
| `ImprovementYieldChanges` の牧草地、`UnitCombatFreeExperiences` の騎乗、`StartBias/BonusBiases` の馬 | モンゴル |
| `iCapitalBuildingProductionModifier` | アウグストゥス |
| `iOverseasCityPopulation`、`iNewWorldCityPopulation` | スペイン |
| `ExplorationRewards` の海外の旧世界・新世界、金・研究、艦船への歩哨の昇進 | ポルトガル |
| `Conquest/bStateReligion`、`Conquest/ReligionBuildings`、`Conquest/iBuildingSurviveModifier` | サラディン |
| `FeatureGrowthChanges` の森 | シッティング・ブル |
| フリゲートへの `bHiddenNationality` の昇進 | エリザベス |
| `bAtWar` + `YieldModifiers` の生産 | チャーチル |
| 昇進の `ImpassablePlotPassages` の山岳 | ハンニバル |
| `Conquest/bNoPopulationLoss` | 大キュロス |
| `UnitCombatProductionModifiers` の銃火器・装甲 | ビスマルク |
| `UpgradeAnywhereUnitCombats` の白兵戦・銃火器 | フリードリッヒ |
| 昇進の `iOtherReligionCombatPercent`・`iStateReligionCityAttack` | イザベル |
| `NoObsolescence/BuildingClasses` の城壁・城 | ギルガメッシュ |
| `bNewCityStateReligion` | ザラ・ヤコブ |
| `bBarbarianAlly`、`PrereqTech`（騎乗）+ `bBarbarianVision` | チンギス・ハーン |
| `EnemyAttrition` | オラニエ公ヴィレム1世 |
| `iDefenseCombatPercent`、`iEnemyWarWearinessModifier` | ガンジー |
| `NoObsolescence/BuildingClasses` の世界遺産 | ラムセス２世 |
| `PrereqTech` + `iFreeSpecialist` | リンカーン |
| `iImprovementUpgradeRateModifier`、`bImprovementUpgradeUnworked` | アメリカ |
| `iDistantTradeRouteModifier` | ハトシェプスト |
| `iCityRadiusChange` | スーリヤヴァルマン2世 |
| `iGoldenAgeLandMovesChange`、`bGoldenAge` + `iCombatPercent` | ペルシャ |
| `WorldWonderCommerceChanges` の文化 | ルイ14世 |
| `RiverEdgeYieldPercents` | シュメール |
| 偵察ユニットへの `bChooseGoody`・歩哨の昇進、首都の建設時の斥候（`PyOnCityFounded`） | ジョアン2世 |
| `Hurrys` の資金（制限なし）、大商人の `UniqueUnits` | マンサ・ムサ |
| 征服した都市の徴兵ユニット（`PyOnCityAcquired`） | オスマン |
| `SpecialistYieldChanges`（技術者・聖職者）、`SpecialistCommerceChanges`（科学者・商人・芸術家・スパイ）と定住した偉人 | ギリシャ |
| `Conquest/bLearnUniqueUnits`、`Conquest/bLearnUniqueBuildings` | アレクサンドロス |
| `iRouteMovementModifierInBorders` | ローマ |
| 昇進の `iCombatPercentOverseas`、艦船への `iMovesChange` の昇進 | イギリス |
| 都市のタイルを丘陵にする（`PyOnCityFounded` の `makeCityPlotHills`） | エチオピア |
| `iFreeExperience` | ズールー |
| `GlobalWorldWonderCommerceChanges` の研究 | ヴィクトリア |
| `iGoldenAgeModifier`、`iGoldenAgeGreatPeopleRateModifier` | ペリクレス |
| 昇進の `iGoldFromCombat` | ブレヌス |
| `NoObsolescence/SpecialBuildings` の僧院、`PyOnReligionFounded` | ユスティニアヌス1世 |
| 昇進の `BuildingCombatPercents`（国教の僧院・寺院・大聖堂・聖地） | ビザンチン |
| `iStateReligionHappiness`、`iStateReligionHealth` | アショーカ |
| `GreatPeopleClassRateModifiers` の大技術者 | スレイマン |
| `iEnemyMoveCostInBorders` | オラニエ公ヴィレム1世 |
| `iRevengeCombatPercent` | ブーディカ |
| `KnownTechCost` | ピョートル |
| `Conquest/bHeal` | ユリウス・カエサル |
| `CapitalTerrainYieldPercents`、`TerrainTrades` の砂漠 | マリ |
| `Hurrys` の資金（ユニットだけ、`iExperience`） | カルタゴ |
| 昇進の `FeatureDoubleMoves`・`FeatureDefenses` の森、`FeatureHappinessChanges`・`StartBias/Features`・`FeatureHealChanges` の森 | ケルト |
| `TerrainTrades` の丘陵・山岳、`PassableTerrainsInBorders` の山岳 | ワイナ・カパック |
| `SpecialistSlotChanges`（全専門家）、`ConditionalFreeTraits` の `PrereqTech`（教育） | 朝鮮 |
| `Conquest/iCulturePercent`、`bStackCityDefense` | 王建 |
| `PyOnCityLost`（`createPartisans`） | ド・ゴール |
| `bAtWar` + `PyOnGained`（`createCityConscripts`） | ワシントン |
| `bAtWar` + `PyOnLost` で期間つきの志向（`CommerceModifiers` の文化、`iHappiness`） | 徳川家康 |
| `HealthyCommercePercents`（研究）、`PyOnBuildingBuilt`（`isBuildingClassIn`） | バビロニア |
| `CivicOptionExtraSlots`（法律） | ハンムラビ |
| `chooseGreatPeopleByCalendar`（`PyOnBeginTurn`。暦の周期ごとに選んだ偉人）、`bLongCountCalendar` | マヤ |

### 新しい志向で決まったこと（2026-11-15）

| 対象 | 内容 | タグ | 状態（志向） |
|---|---|---|---|
| メフメド２世 | 艦隊の山越え: 艦船が、水に面していない要塞・都市にも入れる | `bInlandCanals` | 実装済み `TRAIT_OVERLAND_FLEET` |
| アステカ（文明） | テノチティトラン: 湖のタイルの産出が増える。時代が 1 つ進むごとに湖の商業 +1（2026-11-19 に追加） | `LakePlotYieldChanges`、`LakePlotYieldChangesPerEra` | 実装済み `TRAIT_CHINAMPA` |
| モンテスマ | 花の戦争: 倒した敵を捕虜（労働者）にする | 昇進の `iEnslavementChance` | 実装済み `TRAIT_FLOWER_WAR` |
| フランス（文明） | 革命: 無政府期間が明けると黄金期。無政府期間中は全ユニットの戦闘力が上がる | `iAnarchyGoldenAgePercent`、`iCombatPercent` を持つ志向を `ConditionalFreeTraits` の `bAnarchy` で与える | 実装済み `TRAIT_REVOLUTION` |
| ヴァイキング（文明） | 古典時代までの艦船が川を進める。乗船・下船で移動力を余分に使わない | `FreePromotionSets` で、`bRiverNavigation` の昇進を作業船・ガレー船・三段櫂船に、`bFreeEmbark` の昇進を陸上ユニットに配る | 実装済み `TRAIT_LONGSHIP` |
| ラグナル | 略奪で移動力を使わない。略奪するとユニットが回復する | `FreePromotionSets` で、`bPillageNoMoveCost`・`iPillageHealPercent` の昇進を陸上の戦闘ユニットに配る | 実装済み `TRAIT_RAIDING` |
| 始皇帝 | 奴隷制なしで、遺産（世界遺産・国家遺産）を人口で緊急生産できる。遺産の人口による緊急生産に追加の費用がかからない（生産 0 のときの +50% と人口を使ったときの不満は残る。2026-11-19 に `Hurrys` へ書き換えたので、追加の費用の無視は人口による緊急生産だけになった。以前は資金による緊急生産にも効いていた） | `Hurrys`（`HURRY_POPULATION`、`bWondersOnly`、`bIgnoreCostModifier`） | 実装済み `TRAIT_CORVEE` |
| アメリカ先住民（文明） | 森に特化: 自国の領土の森を道路として扱う（移動と交易網）。施設を作っても森が残る | `FeatureRoutes`（森 → `ROUTE_ROAD`）、`MaintainFeatures`（森） | 実装済み `TRAIT_FOREST_PEOPLE` |
| クメール（文明） | ジャングルに特化: 自国の領土のジャングルを道路として扱う。ジャングルの不衛生がなくなる。ジャングルの食料 +2（BtS の -1 と合わせて +1）。施設を作ってもジャングルが残る | `FeatureRoutes`（ジャングル → `ROUTE_ROAD`）、`FeatureHealthChanges`（ジャングル +25。BtS の -25 と合わせて 0）、`FeatureYieldChanges`（ジャングル 食料 +2）、`MaintainFeatures`（ジャングル） | 実装済み、ゲームで確認済み `TRAIT_JUNGLE_PEOPLE` |
| サラディン | 共存: 占領した都市に国教が広まり寺院が建つ、建物が残りやすい（+50%） | `Conquest/bStateReligion`、`Conquest/ReligionBuildings`（寺院）、`Conquest/iBuildingSurviveModifier` | 実装済み `TRAIT_COEXISTENCE` |
| アラブ（文明） | ベドウィン: 騎乗ユニットが砂漠で移動 2 倍・防御 +25%。砂漠のバイアス。2026-11-19 に追加: 石油がゲーム開始時から見え、石油のバイアス（1 つにつき +50%）、石油のタイルの商業 +2、オアシスのタイルの生産 +1・商業 +1（マリの内向きの砂漠経済と分けて、資源とオアシスに寄せた。数値は仮） | 昇進の `TerrainDoubleMoves`・`TerrainDefenses`、`StartBias/Terrains`、`RevealBonuses`・`StartBias/BonusBiases`・`BonusYieldChanges`（石油）、`FeatureYieldChanges`（オアシス） | 実装済み `TRAIT_BEDOUIN` |
| インド（文明） | 多宗教の受容: 都市の宗教 1 つにつき幸福 +1・文化 +1 | `iHappinessPerReligion`、`CommercePerReligion` | 実装済み `TRAIT_MULTI_RELIGION` |
| インカ（文明） | アンデスの民: 丘陵 食料 +1、山岳 食料 +2・生産 +1、時代が 1 つ進むごとに山岳の商業 +1（山岳も働かせられる）。丘陵・山岳のバイアス（2026-11-19 に山岳を食料 +1・生産 +1 から変更） | `TerrainYieldChanges`、`TerrainYieldChangesPerEra`、`StartBias/Terrains` | 実装済み `TRAIT_ANDES` |
| ワイナ・カパック | カパック・ニャン: 丘陵と山岳が交易網をつなぐ（山岳は通行不能でも交易網を止めない）。自国の領土の山岳にユニットが入れる（ダメージなし。ハンニバルは領土の外でも入れるがダメージを受ける）（2026-11-19） | `TerrainTrades`（`TERRAIN_HILL`・`TERRAIN_PEAK`）、`PassableTerrainsInBorders`（`TERRAIN_PEAK`） | 実装済み `TRAIT_QHAPAQ_NAN` |
| ケルト（文明） | ドルイドの森: 全陸上ユニットが森で移動 2 倍・防御 +25%、森にいるユニットの回復 +20%、都市圏の森 1 つにつき幸福 +1、森のバイアス（2026-11-19。数値は仮。丘陵は固有ユニット・固有建造物に任せる） | `FreePromotionSets`（`PROMOTION_DRUID_FOREST`: `FeatureDoubleMoves`・`FeatureDefenses`）、`FeatureHappinessChanges`、`StartBias/Features`、`FeatureHealChanges` | 実装済み `TRAIT_DRUID_FOREST` |
| オランダ（文明） | 東インド会社: 幹部の成功率 +50%、幹部が国境を閉じた国にも入れる、企業を設立すると本社に幹部 3 体 | `iCorporationSpreadChange`、昇進の `bRivalTerritory`、`PyOnCorporationFounded` | 実装済み `TRAIT_VOC` |
| ドイツ（文明） | 産業化: 産業主義を得ると全都市の生産 +10% | `ConditionalFreeTraits` の `PrereqTech` で `TRAIT_INDUSTRIAL_POWER`（`YieldModifiers`） | 実装済み `TRAIT_INDUSTRIALIZATION` |
| エジプト（文明） | ナイルの賜物: 氾濫原 生産 +1、不衛生なし。氾濫原のバイアス | `FeatureYieldChanges`、`FeatureHealthChanges`、`StartBias/Features` | 実装済み `TRAIT_NILE` |
| ロシア（文明） | 広大な大地: 文化圏 +1 | `iCultureRangeChange` | 実装済み `TRAIT_VAST_LAND` |
| モンゴル（文明） | 騎馬の民: 牧草地 食料 +1、騎乗ユニットの経験値 +2。馬のバイアス | `ImprovementYieldChanges`、`UnitCombatFreeExperiences`、`StartBias/BonusBiases` | 実装済み `TRAIT_HORSE_PEOPLE` |
| ローマ（文明） | アッピア街道: 自国の領土の道路・鉄道の移動コスト -50%（道路で 2 倍進める）。都市を建設すると、陸路 6 タイル以内で首都とつながるなら首都までの道路が最初から敷かれる（技術は不要。Civ6 のローマの能力から。2026-11-23） | `iRouteMovementModifierInBorders`、`PyOnCityFounded` の `buildRouteToCapital(pCity, 'ROUTE_ROAD', 6)` | 実装済み `TRAIT_APPIAN_WAY` |
| アウグストゥス | パクス・ロマーナ: 首都にある建物を他の都市で建てるとき生産 +50%（2026-11-17 にローマの文明志向から移した） | `iCapitalBuildingProductionModifier` | 実装済み `TRAIT_PAX_ROMANA` |
| ポルトガル（文明） | 大航海時代: そのターンに初めて見えた海外の陸地 1 タイルにつき、旧世界なら金 +1・研究 +1、新世界なら金 +2・研究 +2（ターンの終わりにまとめて。端数は持ち越し）。艦船に歩哨（2026-11-22 に作り直した。歩哨のほかの前の内容はスペインへ移した。地図によって弱く、外洋に出られるのも遅いので数値は大きめ。数値は仮） | `ExplorationRewards`（`EXPLORE_PLOT_OVERSEAS_OLD_WORLD` 金・研究 100、`EXPLORE_PLOT_OVERSEAS_NEW_WORLD` 金・研究 200）、`FreePromotionSets`（`PROMOTION_SENTRY`） | 実装済み `TRAIT_AGE_OF_DISCOVERY` |
| スペイン（文明） | 新世界探検: 海外に建てた都市の人口 +1、新世界に建てた都市の人口 +1（重なる）。そのうち沿岸の都市には灯台、新世界なら港も。最初に世界一周を達成すると黄金時代（2026-11-23 に追加。2 番目以降は何もない）（2026-11-22 にポルトガルの前の志向「大航海時代」を移して改名。艦船の歩哨はポルトガルに残した） | `iOverseasCityPopulation`、`iNewWorldCityPopulation`、`PyOnCityFounded`、`PyOnCircumnavigated`（`iOrder == 1 and startGoldenAge(pPlayer)`） | 実装済み `TRAIT_NEW_WORLD_EXPLORATION` |
| シッティング・ブル | 森を育む: 領土の森が広がりやすい | `FeatureGrowthChanges` | 実装済み `TRAIT_FOREST_KEEPER` |
| エリザベス | 私掠免状: フリゲートが国籍を隠す | 昇進の `bHiddenNationality` | 実装済み `TRAIT_LETTERS_OF_MARQUE` |
| チャーチル | 最も輝ける時: 戦時は全都市の生産 +15% | `bAtWar` で `TRAIT_FINEST_HOUR_WAR`（`YieldModifiers`） | 実装済み `TRAIT_FINEST_HOUR` |
| ハンニバル | アルプス越え: 陸上ユニットが山岳に入れる（山岳でターンを迎えると 20% のダメージ） | 昇進の `ImpassablePlotPassages` | 実装済み `TRAIT_ALPS` |
| 大キュロス | 寛容王: 都市を占領しても人口が減らない | `Conquest/bNoPopulationLoss` | 実装済み `TRAIT_TOLERANCE` |
| ビスマルク | 鉄と血: 銃火器・装甲ユニットの生産 +25% | `UnitCombatProductionModifiers` | 実装済み `TRAIT_BLOOD_AND_IRON` |
| フリードリッヒ | 軍隊を持つ国家: 白兵戦・銃火器ユニットが戦地でアップグレードできる | `UpgradeAnywhereUnitCombats` | 実装済み `TRAIT_ARMY_STATE` |
| イザベル | レコンキスタ: 異教徒との戦闘力 +25%、国教の広まった都市への攻撃 +25% | 昇進の `iOtherReligionCombatPercent`・`iStateReligionCityAttack` | 実装済み `TRAIT_RECONQUISTA` |
| ギルガメッシュ | ウルクの城壁: 城壁と城が陳腐化しない | `NoObsolescence/BuildingClasses` | 実装済み `TRAIT_WALLS_OF_URUK` |
| ザラ・ヤコブ | 信仰の王: 新しい都市に国教が広まる | `bNewCityStateReligion` | 実装済み `TRAIT_KING_OF_FAITH` |
| チンギス・ハーン | 草原の統一: 蛮族と平和。騎乗を得ると蛮族の視界を共有する（蛮族のユニット・動物・都市が見ているタイルが見える。2026-11-19 に追加。視界は強力なので騎乗を条件にした） | `bBarbarianAlly`、`ConditionalFreeTraits` の `PrereqTech`（`TECH_HORSEBACK_RIDING`）で `TRAIT_STEPPE_UNITY_VISION`（`bBarbarianVision`） | 実装済み `TRAIT_STEPPE_UNITY` |
| スターリン | チェカ: 最初から生産「諜報」（ハンマーを諜報ポイントに）を選べる。固有ユニット「チェキスト」（スパイ。移動力 2、コスト 30）。諜報ポイント +25%。諜報任務「要人の殺害」「要人の誘拐」「体制転覆」（相手が採用できる社会制度を何でも押し付ける）「国教転覆」（自分の国教でない宗教か国教なしを押し付ける）を使える（ほかの文明は使えない。後の 2 つは 2026-11-23 に追加）。水源への毒混入・不満を煽るの値が 8 → 12 | 生産の `PrereqOrTraits`（`PROCESS_ESPIONAGE`）、`UniqueUnits`（`UNIT_CHEKIST`）、`CommerceModifiers`、諜報任務の `PrereqOrTraits`・`DisableTraits`（`ESPIONAGEMISSION_CHEKA_CITY_POISON_WATER`・`_UNHAPPINESS`） | 実装済み `TRAIT_CHEKA`（2026-11-22。焦土 `TRAIT_SCORCHED_EARTH` は同日に削除し、`EnemyAttrition` はヴィレムへ移した） |
| ガンジー | 非暴力不服従: 防衛時の戦闘力 +10%、敵の厭戦感情 +50% | `iDefenseCombatPercent`、`iEnemyWarWearinessModifier` | 実装済み `TRAIT_SATYAGRAHA` |
| ラムセス２世 | 永遠の記念碑: すべての世界遺産が陳腐化しない（BtS で陳腐化する 13 個を並べた） | `NoObsolescence/BuildingClasses` | 実装済み `TRAIT_ETERNAL_MONUMENTS` |
| リンカーン | 奴隷解放: 自由主義を得ると全都市に無償の専門家 +1 | `PrereqTech` で `TRAIT_EMANCIPATION_ACT`（`iFreeSpecialist`） | 実装済み、ゲームで確認済み `TRAIT_EMANCIPATION` |
| アメリカ（文明） | ホームステッド法: 小屋などの成長 +50%。文化圏の施設は市民を配置しなくても成長する（2026-11-23 に追加） | `iImprovementUpgradeRateModifier`、`bImprovementUpgradeUnworked` | 実装済み、ゲームで確認済み `TRAIT_HOMESTEAD` |
| ハトシェプスト | プント遠征: 遠く離れた都市との交易路 +50% | `iDistantTradeRouteModifier` | 実装済み、ゲームで確認済み `TRAIT_PUNT` |
| スーリヤヴァルマン2世 | アンコール: 文化レベル「洗練」以上の都市は都市圏の半径 +1 | `iCityRadiusChange` + `CityRadiusCultureLevel` | 実装済み、ゲームで確認済み `TRAIT_ANGKOR` |
| ペルシャ（文明） | 黄金の帝国: 黄金時代は陸上ユニットの移動力 +1、全ユニットの戦闘力 +15% | `iGoldenAgeLandMovesChange`、`bGoldenAge` で `TRAIT_GOLDEN_EMPIRE_AGE`（`iCombatPercent`） | 実装済み、ゲームで確認済み `TRAIT_GOLDEN_EMPIRE` |
| ルイ14世 | 太陽王: 世界遺産のある都市は、遺産 1 つにつき文化 +3 | `WorldWonderCommerceChanges` | 実装済み、ゲームで確認済み `TRAIT_SUN_KING` |
| ジョアン2世 | 探検王: 偵察ユニットが部族集落の報酬を選べ、歩哨の昇進も持つ。首都を建設すると斥候 1 体。固有プロジェクト「トルデシリャス条約」（2026-11-24。光学か紙。コスト 300・数値は仮）: 完成時の首都を通る線で地図を東西に分け、選んだ側（AI は陸の多い側）が優先地域。優先地域の他文明の都市 1 つにつき毎ターン金 +1。他文明が優先地域に都市を建てると、建てた側が 25 ×（建てた側の時代 + 1）× 都市の人口（ゲーム速度で変わる）の金を払う・技術を 1 つ渡す・その都市に「トルデシリャス条約違反」（不満 +3、建設不可、占領されても残る、核で消えない）が建つ、から選ぶ | `FreePromotionSets`（`bChooseGoody` の昇進 `PROMOTION_EXPLORER_KING`、`PROMOTION_SENTRY`）、`PyOnCityFounded`、`PROJECT_TORDESILLAS`（`PrereqOrTechs`・`PrereqOrTraits`）、`PyOnProjectBuilt` の `chooseTreatyZone`・`PyOnBeginTurn` の `treatyZoneTurn`・`PyOnOtherCityFounded` の `offerTreatyTerms`、`BUILDING_TORDESILLAS_VIOLATION` | 実装済み `TRAIT_EXPLORER_KING` |
| マンサ・ムサ | 黄金の巡礼: 普通選挙なしで資金による緊急生産ができる（2026-11-19 に「普通選挙を最初から採用できる」から変更）。大商人の代わりにワンガラ商人（移動力 3、交易任務の金 1.5 倍） | `Hurrys`（`HURRY_GOLD`）、`UniqueUnits`（`UNIT_WANGARA_TRADER`） | 実装済み `TRAIT_GOLDEN_PILGRIMAGE` |
| オスマン（文明） | デヴシルメ: 都市を征服すると、その都市で徴兵できるユニットが 1 体加わる | `PyOnCityAcquired`（`CyCity.getConscriptUnit`） | 実装済み `TRAIT_DEVSHIRME` |
| ギリシャ（文明） | アゴラ: 専門家 1 人につき、科学者 研究 +1、商人 金 +1、技術者 生産 +1、芸術家 文化 +1、スパイ 諜報 +1、聖職者 商業 +1。定住した偉人はそれぞれ 2 倍 | `SpecialistYieldChanges`、`SpecialistCommerceChanges` | 実装済み `TRAIT_AGORA` |
| カルタゴ・イギリス（文明） | 開始位置のバイアス: 沿岸 +100%（イギリスの文明の志向は未定。カルタゴは 2026-11-19 に傭兵志向） | 文明の `iStartBiasCoastal` | 実装済み、ゲームで確認済み |
| シュメール（文明） | 両河の間: タイルの川の辺 1 本につき商業 +1、3 本で食料 +1。開始位置のバイアス 川沿い +100%（文明に直接） | `RiverEdgeYieldPercents`（商業 100・食料 34）、文明の `iStartBiasRiver` | 実装済み `TRAIT_MESOPOTAMIA`（バイアスはゲームで確認済み） |
| アレクサンドロス | オリエンタリズム: 都市を征服すると、奪った相手の文明の固有ユニット・固有建造物を学ぶ（自分の固有がない種類だけ。同じ種類は後に学んだものに替わる）。既存の建物も建て替え（立地の条件は見ない）、作りかけのハンマーも引き継ぐ。今いるユニットはそのまま。ギリシャのファランクス・オデオンは替わらない | `Conquest/bLearnUniqueUnits`、`Conquest/bLearnUniqueBuildings` | 実装済み、ゲームで確認済み `TRAIT_ORIENTALISM` |
| イギリス（文明） | 日の沈まぬ帝国: 海外（首都と別の地域）で戦うユニットの戦闘力 +15%。艦船の移動力 +1 | 昇進の `iCombatPercentOverseas`（全戦闘ユニット）、BtS の昇進の `iMovesChange`（海洋） | 実装済み、ゲームで確認済み `TRAIT_SUN_NEVER_SETS` |
| エチオピア（文明） | 高地の王国: 建設した都市のタイルが平原の丘陵になる（2026-11-19 にアドワの戦い（自国の領土の経験値 +50%・戦闘力 +15%）から作り直した。名前は仮） | `PyOnCityFounded` の `makeCityPlotHills` | 実装済み `TRAIT_HIGHLAND_KINGDOM` |
| ズールー（文明） | アマブト（年齢連隊）: 都市で生産したユニットの経験値 +2 | `iFreeExperience` | 実装済み、ゲームで確認済み `TRAIT_AMABUTHO` |
| ヴィクトリア | 万国博覧会: 持っている世界遺産 1 つにつき、全都市の研究 +1 | `GlobalWorldWonderCommerceChanges` | 実装済み、ゲームで確認済み `TRAIT_GREAT_EXHIBITION` |
| ペリクレス | ペリクレス時代: 黄金時代の期間 +50%、黄金時代の偉人ポイント +100% | `iGoldenAgeModifier`、`iGoldenAgeGreatPeopleRateModifier` | 実装済み、ゲームで確認済み `TRAIT_AGE_OF_PERICLES` |
| ブレヌス | 敗者に災いあれ: 戦闘に勝つたびに金 +5 | 昇進の `iGoldFromCombat`（全戦闘ユニット） | 実装済み、ゲームで確認済み `TRAIT_VAE_VICTIS` |
| ユスティニアヌス1世 | ハギア・ソフィア: 僧院が陳腐化しない。宗教を創始すると、聖都にその宗教の宣教師 +2 体（BtS の 1 体と合わせて 3 体。2026-11-22 に追加）（寺院・大聖堂の生産 +100% は master の新しいタグを待つ） | `NoObsolescence/SpecialBuildings`（`SPECIALBUILDING_MONASTERY`）、`PyOnReligionFounded`（`createMissionaries(pPlayer, eReligion, 2, pCity)`）、`Help` | 実装済み `TRAIT_HAGIA_SOPHIA`（僧院はゲームで確認済み） |
| ビザンチン（文明） | 位階（Civ6 の位階から）: 航空以外の全戦闘ユニットが、国教の僧院 1 つにつき戦闘力 +2%、寺院 +3%、大聖堂 +5%、聖地 +10%（合計の上限 +50%）。陳腐化した僧院は数えないが、ユスティニアヌス1世なら僧院が陳腐化しないので残る | `FreePromotionSets`（`PROMOTION_RANKS`。昇進の `BuildingCombatPercents` の `bStateReligion`、聖地は 7 宗教の建物クラスを 1 つずつ、`iBuildingCombatMaxPercent` 50） | 実装済み `TRAIT_RANKS` |
| アショーカ | ダルマの勅令: 国教の広まった都市の幸福 +1・衛生 +1 | `iStateReligionHappiness`、`iStateReligionHealth` | 実装済み、ゲームで確認済み `TRAIT_DHAMMA` |
| スレイマン | 大建築家スィナン: 大技術者の偉人ポイント +100% | `GreatPeopleClassRateModifiers`（大技術者） | 実装済み、ゲームで確認済み `TRAIT_SINAN` |
| オラニエ公ヴィレム1世 | 洪水線: 自国の領土では、交戦中の敵の陸上ユニットの移動コスト +1。自国の領土にいる交戦中の敵が毎ターン最大 HP の 10% を失う（50% まで） | `iEnemyMoveCostInBorders`、`EnemyAttrition` | 実装済み `TRAIT_RELIEF_OF_LEIDEN`（移動コストはゲームで確認済み。2026-11-22 に「ライデンの解放」から改名し、損耗をスターリンから移した） |
| ブーディカ | 復讐の女王: 自分の都市を一度でも征服した文明との戦闘力 +25% | `iRevengeCombatPercent` | 実装済み、ゲームで確認済み `TRAIT_REVENGE_QUEEN` |
| ピョートル | 大使節団: その技術を持つ既知の文明 1 つにつき研究費 -10%（-75% まで） | `KnownTechCost` | 実装済み、ゲームで確認済み `TRAIT_GRAND_EMBASSY` |
| ユリウス・カエサル | 来た、見た、勝った: 都市を征服すると全ユニットが全回復 | `Conquest/bHeal` | 実装済み、ゲームで確認済み `TRAIT_VENI_VIDI_VICI` |
| マリ（文明） | サハラ交易: 首都の都市圏の砂漠 1 タイルにつき、首都の食料 +0.5・商業 +1（砂漠のタイルそのものは変わらない）。砂漠が交易網をつなぐ。砂漠のバイアス | `CapitalTerrainYieldPercents`（砂漠 食料 50・商業 100）、`TerrainTrades`（砂漠）、`StartBias/Terrains`（砂漠 10） | 実装済み `TRAIT_TRANS_SAHARAN` |
| カルタゴ（文明） | 傭兵: 普通選挙なしで、資金でユニットを緊急生産できる。資金で緊急生産したユニットは経験値 +2（普通選挙で緊急生産した場合も）（2026-11-19） | `Hurrys`（`HURRY_GOLD`、`bUnitsOnly`、`iExperience` 2） | 実装済み `TRAIT_MERCENARIES` |
| ド・ゴール | レジスタンス: 都市を占領される（破壊される）と、その近くにパルチザンが（人口 + 2）体現れる。首都で徴兵できるユニットで、都市の文化レベルに応じて戦闘術Ⅰ～Ⅵを持つ（2026-11-19。人口と文化レベルは占領される前のもの） | `PyOnCityLost` で `createPartisans` | 実装済み `TRAIT_RESISTANCE` |
| ワシントン | ミニットマン: 戦争が始まると（交戦相手がいない状態から）、首都と人口 5 以上の都市に、その都市で徴兵できるユニットが 1 体ずつ現れる（2026-11-19。人口 5 は仮） | `bAtWar` で `TRAIT_MINUTEMEN_WAR`、その `PyOnGained` で `createCityConscripts` | 実装済み `TRAIT_MINUTEMEN` |
| 徳川家康 | 天下泰平: 戦争が終わると（交戦相手がいなくなると）、20 ターン（ゲーム速度で変わる）泰平志向（全都市の文化 +50%、幸福 +1）を得る（2026-11-19。数値は仮） | `bAtWar` で `TRAIT_PAX_TOKUGAWA_WAR`、その `PyOnLost` で `gainTraitForTurns`（`TRAIT_PAX_TOKUGAWA_PEACE`） | 実装済み `TRAIT_PAX_TOKUGAWA` |
| 朝鮮（文明） | 両班: ゲーム開始時から全都市で全専門家（聖職者・芸術家・科学者・商人・技術者・スパイ）の枠 +1。教育を得ると全専門家の枠が無制限になる（2026-11-21。名前は仮） | `SpecialistSlotChanges`（各 1）、`PrereqTech`（`TECH_EDUCATION`）で `TRAIT_MUNBAN`（文班。`SpecialistSlotChanges` 各 -1） | 実装済み、ゲームで確認済み `TRAIT_YANGBAN` |
| 王建 | 後三国統一: 征服した都市のほかの文明の文化の 50% を自分の文化に移す。都市の文化による防御と建造物の防御を合計する（2026-11-21。新羅の帰順・甄萱の亡命を受け入れた統一。防衛志向・火車の守りと合わせる） | `Conquest/iCulturePercent` 50、`bStackCityDefense` | 実装済み、ゲームで確認済み `TRAIT_LATER_THREE_KINGDOMS` |
| バビロニア（文明） | 空中庭園: 余剰の衛生 2 につき、その都市の研究 +1。穀物庫・水道橋・雑貨商・病院・リサイクル施設を生産で建てるたびに、その都市の人口 +1（2026-11-22） | `HealthyCommercePercents`（研究 50）、`PyOnBuildingBuilt` で `isBuildingClassIn` と `pCity.changePopulation(1)` | 実装済み、ゲームで確認済み `TRAIT_HANGING_GARDENS` |
| ハンムラビ | 法典: 法律の社会制度を同時にさらに 1 つ採用できる | `CivicOptionExtraSlots`（`CIVICOPTION_LEGAL` 1） | 実装済み、ゲームで確認済み `TRAIT_LAW_CODE` |
| 神聖ローマ（文明） | 帝国諸侯: 最初の都市を建てると、5 タイル以内にドイツ（指導者はフリードリッヒ）が AI の初期ユニットを連れて参加し、永久同盟で同じチームになる。参加した AI の AI 補正は、どの難易度でも開拓者（最弱。生産・成長のコスト 160% など）（2026-11-23。強いので補正を最弱にした） | `FoundingVassal`（`CIVILIZATION_GERMANY`・`LEADER_FREDERICK`・`HANDICAP_SETTLER`・`iRange` 5） | 実装済み `TRAIT_IMPERIAL_ESTATES` |
| マヤ（文明） | 長期暦: 神学を得た後、マヤ長期暦の周期（394 年。西暦 830・1224・1618・2012 年）が終わるたびに、偉人を 1 人選んで首都に出す（選んだ偉人は、全員を選ぶまで選べない。全員を選ぶとまた全員を選べる）。右上の日付が長期暦になり、マウスを重ねると西暦が出る（Civ5 のマヤ。2026-11-23） | `PyOnBeginTurn` の `chooseGreatPeopleByCalendar(pPlayer, eTrait, 'TECH_THEOLOGY', 394, 2012)`、`bLongCountCalendar` | 実装済み `TRAIT_LONG_COUNT` |

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
- 2026-11-23: ローマのアッピア街道に、建設した都市から首都への道路を足した（ユーザーの指定。Civ6 のローマの能力。Civ6 の交易路の射程 15 タイルは強すぎるので 6 タイルにした）
