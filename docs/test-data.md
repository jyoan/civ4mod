# 検証用データ（文明・指導者・志向）

社会制度・建物・技術・施設・ユニット・昇進・呪文などは `docs/test-data-other.md`。1 つの見出しが 1 つの検証用の項目（見出しは Type と名前、`- 複製元: ` は複製した既存の Info）。新しい項目は該当するファイルの末尾に足す。

## `CIVILIZATION_TEST`（検証用文明）

- 複製元: 日本
- プレイ可・**AI は選ばない**（`bAIPlayable=0`）。
- 色 `PLAYERCOLOR_GOLD_AND_BLACK`。
- 固有ユニット・固有建物なし。
- 文明の志向 `TRAIT_TEST`、首都に `BUILDING_TEST` が無料、開始ユニットに `UNIT_TEST_A`。
- 初期の社会制度は他の文明と同じ（専制政治など）で、`CIVIC_TEST` は革命で採用する（無政府 1 ターン）。
- 開始位置の好み: 沿岸 +200%（`iStartBiasCoastal`）

## `LEADER_TEST`（検証用指導者）

- 複製元: 徳川
- **指導者の志向なし**（検証の邪魔をしないため）

## `LEADER_TEST_RAZER`（検証用指導者（破壊））

- 複製元: チンギス・ハン
- 指導者の志向なし。
- `iRazeCityProb` 1000（チンギス・ハンは 75。BtS の最大）。
- AI が占領した都市を破壊するかの判定（`CvPlayerAI::AI_conquerCity`）で、近さや国教による減点を打ち消して必ず破壊させる。
- ほかの性格はチンギス・ハンのまま（好戦的）。
- `CIVILIZATION_TEST` の指導者に入れてあるが、AI に検証用志向を持たせないように、ゲームオプション「指導者制限なし」（`GAMEOPTION_LEAD_ANY_CIV`）で普通の文明（モンゴルなど）の指導者として AI に使わせる。
- 検証用志向の征服の文化（`Conquest/iCulturePercent`）を AI が持つと、破壊した都市で文化が最も多いのが AI になり、パルチザンが起きないため。
- AI が都市の破壊を考えるのは、ゲーム開始から 21 ターン目以降で、AI の都市が 5 つ以上、破壊する都市と同じ大陸に AI の都市があり、聖地でも世界遺産のある都市でもないときだけ。
- 検証用文明でプレイするときは、指導者に `LEADER_TEST` を選ぶ

## `CIVILIZATION_TEST_AI`（検証用 AI 文明）

- 複製元: アステカ
- 外交の確認で AI に割り当てる文明。
- プレイ可・**AI は選ばない**（`bAIPlayable=0`。カスタムゲームで AI の枠に指定する）。
- 色 `PLAYERCOLOR_RED_AND_GOLD`。
- 固有ユニット・固有建物なし。
- 文明の志向 `TRAIT_TEST_AI`、指導者は `LEADER_TEST_DEMANDER` だけ。
- 開始ユニットに弓兵 3 体（要求の条件「相手の防衛力 < 自分の軍事力」を満たしやすくする）。
- 初期技術・社会制度・都市名はアステカと同じ

## `LEADER_TEST_DEMANDER`（検証用指導者（要求））

- 複製元: モンテスマ
- 指導者の志向なし。
- **ほぼ毎ターン人間に貢ぎ物（資金・技術・地図・資源）を要求する**: `CONTACT_DEMAND_TRIBUTE` の `iContactRand` 1（モンテスマ 25）。
- 援助の依頼・援助の申し出・改宗の要求も `iContactRand` 1（`CONTACT_ASK_FOR_HELP`・`CONTACT_GIVE_HELP`・`CONTACT_RELIGION_PRESSURE`）。
- 改宗の要求は、AI に国教があり、その宗教がこちらの都市にあり、こちらが国教を変えられる（無政府でない・変更の待ち時間がない・前の国教と違う）ときだけ。
- すべての `iContactDelay` を 1（AI_doCounter がターンの初めに 1 減らすので、0 と同じく毎ターン接触できる）。
- `iBaseAttitude` -12（モンテスマ -1）で、どの難易度でも怒りから始まる（難易度の補正は開拓者 +2 〜 貴族以上 -1。怒りは -10 以下）。
- 記憶: 態度を下げるもの（拒否・宣戦など）は `iMemoryAttitudePercent` を 2 倍にして忘れない（`iMemoryRand` 0）、態度を上げるもの（受け入れ・援助など）は次のターンに 1 つ忘れる（`iMemoryRand` 1。`MEMORY_LIBERATED_CITIES` は減衰を追加）。
- 態度に関係しない記憶（`MEMORY_MADE_DEMAND_RECENT` など）はモンテスマのまま。
- 奇襲・宣戦しない（`iDemandRebukedSneakProb`・`iDemandRebukedWarProb` 0、`NoWarAttitudeProbs` をすべての態度で 100）。
- 取引を態度で断らない（`*RefuseAttitudeThreshold` 15 個を `NONE`）。
- 要求の条件（`CvPlayerAI::AI_doDiplo`）: 相手が人間・宣戦できる（**平和条約中は不可**）・相手のチームの防衛力 < 自分の軍事力・自分の相手への態度が**相手の指導者の** `DemandTributeAttitudeThreshold` 以下（`LEADER_TEST` は警戒）。
- 資金は「相手の資金 - 50」と相手の `AI_goldTarget()` の小さい方が 50 を超えるとき（`AI_goldTarget` は **40 ターン目より前で相手の都市が 3 つ以下なら 0** なので、序盤は資金を要求しない。その後も都市数×3 + 人口/3 + 経過ターン/2 にゲーム速度を掛けた程度。要求される側の値なので指導者の XML では変えられない）、地図は AI が見ていなくて相手が見ているタイルの価値（陸 5・水 1 の合計 ÷10）が 100 を超えるとき（陸なら 200 タイル以上。固定値）、技術は 2 チーム以上が知っていて自分が持たないもの、地図は価値が 100 を超えるとき、資源は相手が 2 つ以上持つもの。
- **要求を受け入れると平和条約（`PEACE_TREATY_LENGTH` 10 ターン）になり、その間は要求しない**（`DIPLOEVENT_ACCEPT_DEMAND` の `forcePeace`。XML の値では変えられない）。
- 断れば次のターンにまた要求する。
- 同じターンにほかの用件で接触したターンは要求しない（判定の順は 改宗 → 社会制度 → 参戦 → 禁輸 → 援助の申し出 → 援助の依頼 → 要求（資金 → 地図 → 技術 → 資源）。改宗の要求の条件を満たすと毎ターン改宗の要求になる。援助の依頼も要求より先なので、こちらの資産が AI の半分を超え、AI に渡せる技術があると、要求の代わりに援助の依頼が来る）

## `LEADER_TEST_SEA`（検証用指導者（海））

- 複製元: 徳川
- 検証用指導者の複製で、指導者の志向 `TRAIT_TEST_SEA`。
- `CIVILIZATION_TEST` の指導者に入れてある。
- 人間で選ぶと海で開始する。
- AI の追加ユニット（神で 9 体・ガレオン船 3 隻）は、カスタムゲームで AI の枠に検証用文明・この指導者を指定して確かめる

## `TRAIT_TEST`（検証用志向）

- 志向のタグをすべて持つ:
  - 労働者速度 +50%
  - 社会制度風の数値 9 個と配列 4 個（新ユニット経験値 +2、軍事生産 +10% など）
  - 白兵戦に経験値 +2・生産 +25%・アップグレード割引 50%・領土外アップグレード（敵国でも可）
  - 昇進の組（白兵戦 → `PROMOTION_TEST`、海洋 → 航海術Ⅰ、弓兵 → `PROMOTION_TEST_COMBAT`、偵察 → `PROMOTION_TEST_CHOOSE_GOODY`、騎乗 → `PROMOTION_TEST_BUILDING`、兵科のあるユニットすべての陸上・海洋（`bFreePromotionAllUnitCombats` と `FreePromotionDomains`。2026-11-28 に兵科を並べた書き方から替えた） → `PROMOTION_TEST_ENCIRCLE`、兵科のない陸上ユニット → 森林Ⅱ、作業船（ユニットクラス） → 航海術Ⅰ）
  - タイルの産出（砂漠・ツンドラ・雪原に生産 +1、丘陵に食料 +1、山岳に生産 +2、氷河に食料 +1・商業 +1、森に商業 +1、農場に生産 +1、鉱山に商業 +1、川沿いに食料 +1）
  - 都市圏の半径 +1（文化レベル未熟以上の都市）
  - 召喚したユニットが消えるまで +1 ターン
  - 新しい都市の人口 +1・新しい都市に国教・占領した都市の反乱期間 -50%
  - 施設を作っても森・ジャングルが残る
  - 自国の領土の森・ジャングルを道路として扱う `FeatureRoutes` は書いていない（交易網の確認の邪魔になるため 2026-11-19 に外した。確かめるときは一時的に森・ジャングル → `ROUTE_ROAD` を書く）
  - 黄金時代の期間 +50%・黄金時代の偉人ポイント +50%
  - 首都にある建物の生産 +25%
  - 略奪と都市の征服で得る資金 +200%
  - 石材・馬・鉄のタイルに生産 +1
  - 新しい都市は文化レベル未熟から
  - 城・大図書館・僧院が陳腐化しない（`NoObsolescence/BuildingClasses` / `NoObsolescence/SpecialBuildings`。`NoObsolescence/bAllBuildings` は書いていない。確かめるときは一時的に `1` にする）。
- Python フック（説明は `Help` の `TXT_KEY_TRAIT_TEST_HELP`）:
  - ゲーム開始時に資金 +100（開始時代が 1 つ後になるごとに +50。ゲーム速度で変わる: `scaleBySpeed(..., 'Train')`。普通 100、エピック 150）
  - 5 ターンごとに首都に戦士
  - 建設した都市に城壁と 3 ターンの検証用志向（期間）と、陸路 6 タイル以内なら首都への道路（`buildRouteToCapital`。道を敷いたタイル数をイベントログに出す。経路がなければ -1）
  - 獲得した都市に兵舎
  - 都市を占領されるとその近くにパルチザン（人口 + 2）体（`PyOnCityLost` の `createPartisans`。占領される前の人口と文化レベル）
  - 穀物庫か検証用建物を生産で完成した都市の人口 +1（`PyOnBuildingBuilt` の `isBuildingClassIn`）
  - 宗教を創始すると聖都にその宗教の宣教師 +2 体（`PyOnReligionFounded` の `createMissionaries`。BtS の 1 体と合わせて 3 体）。
- 都市数の上限（`iMaxCities` / `iMaxCitiesPercent`）は書いていない（ほかの確認で都市を増やせなくなるため。2026-11-03 に確認済み）。
- 確かめるときは一時的に `<iMaxCities>1</iMaxCities>`・`<iMaxCitiesPercent>40</iMaxCitiesPercent>` を書く（建設できる都市は「1 + マップの目標都市数の 40%」まで。デュエル・小 2、やや小・標準・大・特大 3）。
- ユニットの生産コスト -20%・建造物の生産コスト -50%（例: 普通の速度で戦士 15 → 12、兵舎 50 → 25）。
- 条件つきの志向（`ConditionalFreeTraits`）: 戦時は攻撃志向、平時は哲学志向、仏教が国教なら宗教志向、国教なしなら創造志向、古典～中世なら拡張志向、平時に検証用社会制度を採用していれば組織志向、敵ユニットを累計 2 体倒すとカリスマ志向、都市を累計 1 個占領すると帝国志向、好きな社会制度（検証用指導者は重商主義）を採用中は金融志向。
- 検証用社会制度を採用中は検証用志向（蛮族）。
- 自国の領土では他国のユニットが略奪できない（`bNoPillageInBorders`）。
- 都市の最低距離 -2（隣接して建てられる。`iMinCityRangeChange`）、他国の文化圏にも都市を建てられる（`bFoundInForeignTerritory`）。
- 自国の文化圏は地球温暖化の被害を受けない（`bNoGlobalWarmingInBorders`）。
- 宣教師の布教と幹部の企業の進出が必ず成功する（`iReligionSpreadChange`・`iCorporationSpreadChange` 100）。
- 黄金期の間、陸上ユニットの移動力 +1・艦船の移動力 +2（`iGoldenAgeLandMovesChange`・`iGoldenAgeSeaMovesChange`）。
- 艦船が水に接していない要塞や都市にも入れる（`bInlandCanals`）。
- 丘陵のタイルが領土になると道路を敷く（`TerritoryRoutes`。2026-11-28）。`bInlandCanals` は 2026-11-28 から、入れる他国の領土の要塞にも入れる。
- 毛皮と象牙が陳腐化しない（`NoObsolescence/Bonuses`。鯨は本体どおり）。
- 開始位置の好み: 川沿い +50%、都市圏の砂漠 1 タイルごとに +40%、森とジャングル 1 タイルごとに +20%、馬 1 つごとに +100%。
- `CityFoundedUnits` は書いていない（確認済み。ほかの確認の邪魔になるため 2026-11-23 に外した。確かめるときは一時的に、ガレー船 1 隻（艦船なので沿岸の都市だけ）と戦士 2 体（`bCoastalOnly`）を書く）。
- 奴隷制を採用できない（`ForbiddenCivics`）。
- 国庫の利子 3%（最大 25）。
- 湖のタイルに生産 +1（`LakePlotYieldChanges`）。
- 平和の不満: 毎ターン 5%・戦争中は毎ターン -10%・上限 20%（`PeaceAnger` の `iPerTurn` 50、`iDecayPerTurn` 100、`iMax` 200）。
- 怒っている市民 1 人につき研究 +1（`AngryCommerceChanges`）、不衛生 1 につき食料 +1（`UnhealthyYieldChanges`）。
- 文化圏の広がり +1（`iCultureRangeChange`）。
- 固有ユニット: 戦士→検証用戦士、大将軍→検証用大将軍、大科学者→検証用大科学者、スパイ→検証用スパイ。
- 固有建造物: 穀物庫→検証用穀物庫（`UniqueUnits`・`UniqueBuildings`）。
- 占領した都市: 他の文明の文化の 50% を自分の文化にする（`Conquest/iCulturePercent`）、建物が残る確率 +100%（`Conquest/iBuildingSurviveModifier`）、人口が減らない（`Conquest/bNoPopulationLoss`）、育っていない都市も自動で破壊されない（`Conquest/bNoAutoRaze`）、征服した相手の文明の固有ユニット・固有建造物を学ぶ（`Conquest/bLearnUniqueUnits`・`Conquest/bLearnUniqueBuildings`。戦士と穀物庫の種類は検証用志向自身の固有があるので学ばない）。
- 国教の広まった都市の幸福 +1・衛生 +1（`iStateReligionHappiness`・`iStateReligionHealth`）、大技術者の偉人ポイント +100%（`GreatPeopleClassRateModifiers`）、自国の領土で交戦中の敵の陸上ユニットの移動コスト +1（`iEnemyMoveCostInBorders`）、都市を征服した相手との戦闘力 +25%（`iRevengeCombatPercent`）、僧院の生産 +100%（`SpecialBuildingProductionModifiers`）、その技術を持つ既知の文明 1 つにつき研究費 -10%（-75% まで。`KnownTechCost`）、都市を征服すると全ユニットが全回復（`Conquest/bHeal`）。
- 都市の文化による防御と建造物の防御を合計（`bStackCityDefense`）。
- 他の文明が建造中の世界遺産と戦争計画が指導者のツールチップとアナウンスに出る（`bRevealWonderProduction`・`bRevealWarPlans`）。
- AI の態度の隠れた補正が見える（`bRevealHiddenAttitude`）。
- 文化圏の施設が市民を配置しなくても成長する（`bImprovementUpgradeUnworked`）。
- 外国の都市は自国の文化による 1 回目の反乱で寝返り（`CultureFlip/bInstantFlip`）、その反乱の確率が 2 倍（`CultureFlip/iRevoltChance` 100）。
- 地形の特徴: 伐採の生産力 森 +10・ジャングル +30（`FeatureProductionChanges`）、領土内で森・ジャングルが広がる確率 隣接 1 つにつき +2%（`FeatureGrowthChanges` 200）、衛生 ジャングル +25（0 になる）・氾濫原 +40（0 になる）（`FeatureHealthChanges`）。
- 専門家: 科学者 +1 食料（`SpecialistYieldChanges`）・+2 金、大預言者（定住）+3 文化（`SpecialistCommerceChanges`）。
- 地域: 新世界に建てた都市の人口 +2（`iNewWorldCityPopulation`）、海外に建てた都市の人口 +1（`iOverseasCityPopulation`）、海外の都市の維持費 -50%（`iOverseasMaintenanceModifier`）、遠く離れた都市との交易路の収入 +100%（`iDistantTradeRouteModifier`）。
- 自国の領土の交戦中の敵ユニットは毎ターン HP 10% を失う（HP 50% まで。`EnemyAttrition` の `iPercent` 10・`iLimit` 50）。
- 社会制度・宗教の変更による無政府期間が終わると、その長さの 200% の黄金期（`iAnarchyGoldenAgePercent`。黄金期の期間 +50% もかかるので、無政府 2 ターンなら 4 × 1.5 = 6 ターン）。
- 技術の研究コスト -40%（`iTechCostModifier`）、ほかの文明からの態度 +3（`iAttitudeChange`）。
- 採鉱を発見すると勤労志向（`ConditionalFreeTraits` の `PrereqTech`）。
- 自国の領土の道路の移動コスト -50%（`iRouteMovementModifierInBorders`）、占領した都市に国教と、建設の条件を満たす寺院・大聖堂（修道院は書いていない。`Conquest/bStateReligion`・`Conquest/ReligionBuildings`）、カースト制度・組織化された宗教・奴隷解放を技術なしで採用できる（`NoTechCivics`。奴隷解放はパルチザンの確認用）。
- 国家遺産の上限は 1 都市に 4、1 つの都市でチャレンジでは無制限（`iMaxNationalWondersPerCity` 4・`iMaxNationalWondersPerCityForOCC` -1）。
- 人間のプレイヤーでも AI の属国になれる（`bHumanCanBeVassal`）。
- `ConditionalFreeTraits` の 2026-11-14 に足した条件は、下の検証用条件の志向 10 個で確かめる。
- 新しい都市の人口 +1 と合わせて、建てた都市の人口は本国 2、海外の旧世界 3、新世界 5。
- 遺産は奴隷制なしで人口による緊急生産ができ、遺産の緊急生産に追加の費用がかからない（`Hurrys` の `bWondersOnly`・`bIgnoreCostModifier`）。
- 都市の宗教 1 つにつき幸福 +1・文化 +1、防衛時の戦闘力 +10%、敵の厭戦感情 +50%、企業を設立すると本社にその企業の幹部 3 体（`PyOnCorporationFounded`）、都市の世界遺産 1 つにつき文化 +3・持っている世界遺産 1 つにつき全都市の研究 +1、タイルの川の辺 1 本につき商業 +1・3 本で食料 +1（`RiverEdgeYieldPercents` 食料 34・商業 100）。
- 2026-11-18 の分:
  - アップグレードしても経験値がすべて残る（`iMaxExperienceAfterUpgrade` -1）
  - 自国の領土の外での戦闘の経験値 +1（`iExpOutsideBordersChange`）
  - 条件を満たす都市は必ず国王感謝祭（`iWeLoveTheKingChance` 100）
  - 全都市で技術者の枠 +1・聖職者は無制限（`SpecialistSlotChanges`。カースト制が無制限にする科学者・商人・芸術家と重ならないように）
  - ユニットは普通選挙なしで資金による緊急生産ができ、その費用 -25%
  - 資金で緊急生産したユニットは経験値 +3（`Hurrys` の `bUnitsOnly`・`iCostModifier`・`iExperience`）
  - 銅の鉱山に商業 +3・トウモロコシの農場に食料 +1（`ImprovedBonusYieldChanges`）
  - 余剰の幸福 1 につき金 +1（`HappyCommercePercents` 100）
  - 余剰の衛生 2 につき食料 +1（`HealthyYieldPercents` 50）。
- 2026-11-19 の分:
  - 石油・ウランが技術なしで見える（`RevealBonuses`）
  - アルファベット・通貨なしで研究・富を選べる（`NoTechProcesses`）
  - 徴兵したユニットの経験値が半分にならない（`Conscription` の `bNoExperiencePenalty`）
  - ゲーム開始時に車輪・陶器（`FreeTechs`）
  - 全都市の生産 +1（`YieldChanges`）
  - 都市の宗教 1 つにつき衛生 +1・食料 +1（`iHealthPerReligion`・`YieldPerReligion`）
  - 都市圏の森 1 つにつき幸福 +1（`FeatureHappinessChanges`）
  - 都市の世界遺産 1 つにつき生産 +2・持っている世界遺産 1 つにつき全都市の食料 +1（`WorldWonderYieldChanges`・`GlobalWorldWonderYieldChanges`）
  - 全専門家の生産 +1（`SpecialistExtraYields`）
  - 国教以外の宗教 1 つにつき幸福 +1（`iNonStateReligionHappiness`）
  - 穀物庫（検証用穀物庫にも）の幸福 +1・兵舎の衛生 +1（`BuildingHappinessChanges`・`BuildingHealthChanges`）
  - トウモロコシで幸福 +1・石材で衛生 +1（`BonusHappinessChanges`・`BonusHealthChanges`）
  - 国教の都市の偉人ポイント +25%・ユニットの生産 +25%・建物の生産 +25%・ユニットの経験値 +2（`iStateReligionGreatPeopleRateModifier`・`iStateReligionUnitProductionModifier`・`iStateReligionBuildingProductionModifier`・`iStateReligionFreeExperience`）
  - 沿岸の都市の交易路 +1・交易路の収入 +25%・外国との交易路の収入 +50%・都市の防御 +25%・砲撃による防御の減少 -50%・諜報への防御 +50%（`iCoastalTradeRoutes`・`iTradeRouteModifier`・`iForeignTradeRouteModifier`・`iAllCityDefense`・`iBombardDefense`・`iEspionageDefense`）
  - 最大の都市の幸福 +1・都市の軍事ユニット 1 つにつき幸福 +1・成長後に食料を 25% 残す（`iLargestCityHappiness`・`iHappyPerMilitaryUnit`・`iFoodKept`。`bNoUnhealthyPopulation`・`bBuildingOnlyHealthy` は検証用志向（蛮族））
  - 社会制度なしで毎ターン 1 体（マップの大きさの補正あり）を徴兵できる（`Conscription` の `iMaxConscript`）。
- 2026-11-23 の分（`Conscription`）:
  - 徴兵で失う人口の計算のコスト -50%（`iPopulationModifier`。機関銃兵 125 → 1 人）
  - 必要な人口 -3（`iMinCityPopulationChange`。人口 2 + 失う人口から）
  - 必要な文化 -10%（`iMinCulturePercentChange`。0% になり、占領直後の都市でも徴兵できる）
  - 1 回の不満 -2 人（`iPopAngerChange`。3 → 1 人）
  - 不満の期間 -50%（`iAngerLengthPercent`。普通の速度で 10 → 5 ターン）
  - 都市でのユニットの回復 +10%（`iHealRateChange`）
  - 艦船の経験値 +2・生産 +25%（`DomainFreeExperiences`・`DomainProductionModifiers`。`bMilitaryFoodProduction` は検証用志向（蛮族））
  - 無料で維持できるユニット +3・軍事ユニット +2・総人口の 10% ずつ（`iBaseFreeUnits`・`iBaseFreeMilitaryUnits`・`iFreeUnitsPopulationPercent`・`iFreeMilitaryUnitsPopulationPercent`）
  - ユニット 1 体の維持費 -1・軍事ユニット 1 体の維持費 +1（`iGoldPerUnit`・`iGoldPerMilitaryUnit`）
  - 企業の維持費 -50%（`iCorporationMaintenanceModifier`）
  - 都市の維持費 -20%（`iMaintenanceModifier`）
  - ワイン 1 つ（`FreeBonuses`）
  - 銅を使える都市の生産 +10%・石材を使える都市の世界遺産の生産 +50%（`BonusProductionModifiers`。石材は `bWorldWondersOnly`）
  - 金の配分 10% につき幸福 +1・文化の配分 10% につき衛生 +1（`CommerceHappinesses`・`CommerceHealths`）
  - 山岳が交易網をつなぐ（`TerrainTrades`。2026-11-19 に追加。通行不能でも交易網を止めない。砂漠は交易網の確認の邪魔になるので同じ日に外した）
  - 時代が 1 つ進むごとにツンドラの商業 +1（`TerrainYieldChangesPerEra`）
  - 自国の領土の山岳にユニットが入れる（`PassableTerrainsInBorders`。ダメージなし）
  - 森にいるユニットの回復 +20%（`FeatureHealChanges`）
  - 時代が 1 つ進むごとに湖の商業 +1（`LakePlotYieldChangesPerEra`）
  - 首都の都市圏の砂漠 1 タイルにつき首都の食料 +0.5・商業 +1（`CapitalTerrainYieldPercents` 食料 50・商業 100）。
- 2026-11-21 の分: 戦争で首都を占領すると相手の基本志向を 1 つ、相手を滅ぼすと基本志向をすべて得る（`bGainTraitOnCapitalCapture`・`bGainTraitsOnElimination`）。
- 2026-11-22 の分: AI の指導者が親密になると、その指導者の基本志向を 1 つランダムに得る（`GainTraitsOnFriendly` の `bAll` 0。すべてのモードを確かめるときは一時的に `1` にする。ほかの文明からの態度 +3 があるので親密になりやすい）。
- 検証用世襲統治を採用中は検証用志向（狂気）。
- 2026-11-22 の分（諜報任務の `PrereqOrTraits`・`DisableTraits`）: 水源への毒混入と不満を煽るが、値を 8 → 12 にした検証用の任務（`ESPIONAGEMISSION_TEST_CITY_POISON_WATER`・`ESPIONAGEMISSION_TEST_CITY_UNHAPPINESS`。`PrereqOrTraits` が検証用志向）に置き換わる（元の任務は `DisableTraits` が検証用志向）。
- 生産の種類の `PrereqOrTraits`: 技術なしで生産「諜報（検証用）」（`PROCESS_TEST_ESPIONAGE`。ハンマーを諜報ポイントに）を選べる。
- 2026-11-22 の分（社会制度の枠）: 法律の社会制度を同時に 2 つ採用できる（`CivicOptionExtraSlots` の `CIVICOPTION_LEGAL` 1）。
- 探検の報酬（`ExplorationRewards`）: 水タイル 10 枚ごとに金 +1（10）、本国の陸地 2 枚ごとに首都の文化 +1（50）と 4 枚ごとに金 +1（25。同じ種類に 2 行）、海外の旧世界の陸地 1 枚ごとに諸報 +1（100）、海外の新世界の陸地 1 枚ごとに研究 +2.5（250）。
- 2026-11-23 の分（フック）: 技術を最初に得ると資金 +25、技術を 3 つ得るごとに首都に斥候（`PyOnTechAcquired`・`countEvery`）。
- 偉人が生まれると検証用志向（期間）を 5 ターン、生まれた都市の文化 +100（`PyOnGreatPersonBorn`）。
- 黄金時代が始まると資金 +100、終わると検証用志向（期間）を 3 ターン（`PyOnGoldenAgeStart` / `End`）。
- 宣戦すると資金 +50、宣戦されると首都と人口 3 以上の都市に徴兵ユニット（`PyOnWarDeclared`）。
- 講和すると資金 +30（`PyOnPeaceMade`）。
- 新しい時代に入ると首都に大技術者（`PyOnEraChanged`）。
- 2026-11-23 の分（フック 2 回目）: 他の文明と出会うと資金 +20（`PyOnFirstContact`）。
- 都市がプロジェクトを完成すると資金 +100（`PyOnProjectBuilt`）。
- 属国を得ると資金 +100、降伏による属国なら +200（`PyOnVassalGained`）。
- 自国の都市に宗教が広まると資金 +10（`PyOnReligionSpread`）。
- 社会制度を変えると検証用志向（期間）を 2 ターン（`PyOnCivicChanged`）。
- 世界一周を 1 番目に達成すると黄金期、2 番目以降なら船の移動力 +1（`PyOnCircumnavigated`。順位をイベントログに出す）。
- 2026-11-23 に足したフック 12 個は、効果に加えてイベントログに「[検証] フック名: 引数」を出す（`showMessage` と `TXT_KEY_CLAUDE_TEST_HOOK_LOG`。例: 技術の名前・bFirst・カウンター、相手のチーム名、都市と宗教、前と後の社会制度）。
- ランダムイベントのタグ `bIgnoreEvents`・`iEventChanceChange`・`bNoFirstEventDelay` は書いていない（確認済み。ほかの確認の邪魔になるため 2026-11-23 に外した。確かめるときは一時的に `1`・`100`・`1` を書く。開始直後から毎ターンイベントが来て、画面に「このイベントを無視する」が出る）。
- 2026-11-24 の分（`Deals`）: 平時でも AI から首都以外の都市を買える（`bBuyCities`）、取引で得る都市の値段 -50%（`iCityValueModifier`）、AI が出す金の上限 +100%（`iGoldLimitModifier`）、こちらが出す金の AI から見た価値 +50%（`iGoldValueModifier`。AI が求める金は 2/3）
- 2026-11-26 の分（`LendLease`）: 自国の都市のユニットに行動ボタン「レンドリース」（`bLendLease`）、生産コストの 50% を研究（`iResearchPercent`）・25% を金（`iGoldPercent`）、送るたびに相手の取引の態度 +1（`iAttitudeChange`。上限は省略して 4）。
- 2026-11-26 の分（`AttitudeCombatPercents`）: 自分への態度が激怒している AI の文明 1 つにつき全ユニットの戦闘力 +20%、いらだっている +10%、警戒している +5%（開始直後から確かめられるように）、上限 +60%。
- 2026-11-26 の分（ペルシャの案）: 大将軍も黄金期を起こせる（`GoldenAgeUnitClasses`）、黄金期に必要な偉人 -1（`iGoldenAgeUnitsChange`。1・1・2 人）、インフレ率 -50%（`iInflationModifier`）。
- 都市を占領すると通常の半分の黄金期（`PyOnCityAcquired` の `startGoldenAge(pPlayer, 50)`。蛮族の都市でも起きる）、最後の都市を占領した文明が滅亡すると通常の黄金期（`PyOnPlayerEliminated`）。どちらもイベントログに「[検証] フック名: 都市名か文明名, golden age turns N」。
- 2026-11-26 の分（`NearbyCityCommerce`）: 3 タイル以内にある自分のほかの都市 1 つにつき研究 +1・文化 +1、首都では 2 倍（`iRange` 3・`bDoubleInCapital` 1）。
- 2026-11-26 の分（`GreatPeopleHurry`）: 偉人バーのクリックで足りない偉人ポイントを 1 ポイントあたり 3 ゴールドで購入し、その場で偉大な芸術家が生まれる（`iGoldPerPoint` 3・`UnitClassType` `UNITCLASS_ARTIST`）。種類を指定しない版は `TRAIT_TEST_AI`。
- 2026-11-26 の分（`bClaimLastReligion`）: 他の文明が最後から 2 番目の宗教を創始すると、最後の宗教の技術を得て創始する。創始すると聖地に大預言者を出し、イベントログに「[検証] PyOnLastReligionClaimed: 都市名, 宗教名」（`PyOnLastReligionClaimed`）。
- 確かめ方: 中世の時代で始めると、古代と古典時代の技術の宗教 5 つは 5 ターン経過後にまとめて創始され（`CvGame::doHolyCity`）、キリスト教（神学 500）とイスラム教（神授王権 1200）が残る。自分では両方を研究しない。

## `TRAIT_TEST_TEMPORARY`（検証用志向（期間））

- 幸福 +1。
- 得ると資金 +10、失うと資金 +20（`PyOnGained`・`PyOnLost`）。
- 検証用志向の `PyOnCityFounded` で、都市を建設するたびに 3 ターン得る（`gainTraitForTurns`。期間中に建設すると 3 ターンに戻る）

## `TRAIT_TEST_BARBARIAN`（検証用志向（蛮族））

- 複製元: 海洋志向
- `bBarbarianAlly` 1、`bBarbarianVision` 1、`bNoUnhealthyPopulation` 1、`bBuildingOnlyHealthy` 1、`bMilitaryFoodProduction` 1、労働の社会制度を同時に 2 つ採用できる（`CivicOptionExtraSlots` の `CIVICOPTION_LABOR` 1。検証用社会制度をやめると失うので、枠を失ったときの確認用）。
- 検証用志向の `ConditionalFreeTraits` で、検証用社会制度を採用している間だけ付く（蛮族や不衛生を使うほかの確認に影響しないように）

## `TRAIT_TEST_COND_*`（検証用条件（…）、10 個）

- 効果なし（`_ANARCHY` だけ、全ユニットの戦闘力 +25%（`iCombatPercent`）。無政府期間中だけ付くので、付け外しで今いるユニットの戦闘力が変わることを確かめる）。
- 検証用志向の `ConditionalFreeTraits` で、条件を満たす間だけ付く。
- 外交顧問（F4）で自分の指導者アイコンにカーソルを置くと出る志向の一覧で、どの条件を満たしているかがわかる: `_GOLDEN_AGE`（`bGoldenAge`）、`_ANARCHY`（`bAnarchy`）、`_NORMAL`（`bNoGoldenAge` + `bNoAnarchy`）、`_CITIES`（`iMinCities` 3 + `iMaxCities` 4）、`_BONUS`（`PrereqBonus` 馬）、`_BUILDING`（`PrereqBuildingClass` 兵舎）、`_VASSAL`（`bVassal`）、`_MASTER`（`bHasVassal`）、`_RANK`（`iMinScoreRank` 2）、`_PYTHON`（`PyCondition` `pPlayer.getGold() >= 200`、説明は `PyConditionText` の `TXT_KEY_CONDITION_TEST_GOLD`「資金が200以上」）。
- 略称はすべて `TXT_KEY_TRAIT_TEST_COND_SHORT`（TC）

## `TRAIT_TEST_INSANE`（検証用志向（狂気））

- 3 ターンごと（ゲーム速度で変わる）に基本志向を 3 個ランダムに得る（`RandomBasicTraits` の `iTurns` 3・`iCount` 3）。
- 検証用志向の `ConditionalFreeTraits` で、検証用世襲統治（`CIVIC_TEST_HEREDITARY_RULE`）を採用している間だけ付く（ランダムな志向がほかの確認の邪魔をしないように）

## `TRAIT_TEST_AI`（検証用志向（AI））

- 検証用 AI 文明の文明の志向。
- ゲーム開始時に `TECH_TEST_DIPLOMACY` を得る（`FreeTechs`）。
- 都市は文化で寝返らない（反乱は起きる。`CultureFlip/bNoFlip`）。
- 反乱も止める確認は一時的に `CultureFlip/bNoRevoltAndNoFlip` に替える
- 偉人バーのクリックで足りない偉人ポイントを 1 ポイントあたり 2 ゴールドで購入し、都市の偉人ポイントの割合で選ばれる偉人がその場で生まれる（`GreatPeopleHurry/iGoldPerPoint` 2、`UnitClassType` なし。2026-11-26）。確かめるときは検証用 AI 文明（指導者は検証用指導者（要求）。人間が操作するので要求は来ない）でプレイする

## `TRAIT_TEST_SEA`（検証用志向（海））

- 海で開始し、初期ユニットがガレオン船に乗っている（`StartBias/SeaStart` の `ShipUnitClass` `UNITCLASS_GALLEON`）。
- 検証用指導者（海）の志向。
- 乗れない船を確かめるときは一時的に `UNITCLASS_CARAVEL`（斥候だけ乗り、開拓者は海上に残る）や `NONE` にする

## `TRAIT_TEST_FOUNDING_VASSAL`（検証用志向（属国））・`LEADER_TEST_VASSAL`（検証用指導者（属国））

- 複製元: 検証用志向（海）・検証用指導者（海）
- 最初の都市を建てると、5 タイル以内にケルト（指導者はガンジー）が AI の初期ユニット（貴族・古代なら開拓者と戦士か斥候）を連れて参加し、永久同盟でチームに入る。
- AI 補正は貴族（`FoundingVassal` の `CIVILIZATION_CELT`・`LEADER_GANDHI`・`HANDICAP_NOBLE`・`iRange` 5）。
- 指導者は `CIVILIZATION_TEST` の指導者に入れてある。
- ケルトやガンジーがすでにゲームにいると自動で選ぶ。
- 自動の選択を確かめるときは一時的に `NONE` にする

## `TRAIT_TEST_CALENDAR`（検証用志向（暦））

- 検証用文明と検証用 AI 文明の文明の志向（2 つ目）。
- 暦による偉人: `PyOnBeginTurn` に `chooseGreatPeopleByCalendar(pPlayer, eTrait, 'TECH_MYSTICISM', 394, 2012, 'TRAIT_TEST_LONG_COUNT')`（神秘主義の後、394 年の周期が終わるたびに偉人を 1 人選んで首都に出す。2026-11-23 に外し、2026-11-26 に `szTechTrait` の確認のため戻した）。
- `bLongCountCalendar` は子の `TRAIT_TEST_LONG_COUNT`（検証用志向（長期暦））に移した（2026-11-26）。神秘主義を得た次のターンの初めにその志向を得て、右上の日付が長期暦になる。
- `UnsettleSpecialists` に検証用学派A・B（定住）（都市画面でクリックするとユニットに戻る。検証用定住偉人などほかの偉人は戻せない）。2026-11-24 に `bUnsettleGreatPeople` 1 から替えた（`bUnsettleGreatPeople` を確かめるときは一時的に戻す）。
- `StackPromotions`: 検証用学派Bと同じタイルにいる白兵戦・騎乗ユニットに検証用随行（`bStackUnitsToo` 1 で学派B 自身にも付く。2026-11-26）、検証用学派Aと同じタイルにいる兵科のあるユニットすべてに検証用随行

## `TRAIT_TEST_TREATY`（検証用志向（条約））・`PROJECT_TEST_TREATY`（検証用プロジェクト（条約））

- 複製元: — / インターネット
- 2026-11-24。
- 検証用文明と検証用 AI 文明の文明の志向（3 つ目）。
- プロジェクトは筆記か乗馬で作れ（`PrereqOrTechs`）、この志向が必要（`PrereqOrTraits`）、コスト 30、チームに 1 回。
- 完成すると優先地域を選ぶ（`chooseTreatyZone`）。
- AI は最初の都市で自動で選ぶ（`PyOnCityFounded` の `pPlayer.isHuman() or chooseTreatyZone(...)`）。
- 毎ターン、優先地域の他文明の都市 1 つにつき金 +1（イベントログに「検証用プロジェクト（条約）より[金]N を取得。」）。
- 他文明が優先地域に都市を建てると、建てた側が 50 ゴールド（ゲーム速度で変わる）・技術 1 つ・その都市に検証用建造物（`BUILDINGCLASS_TEST`）から選ぶ（`offerTreatyTerms`。イベントログに「[検証] PyOnOtherCityFounded: 都市名, offerTreatyTerms True/False」）。
- 人間が建てた側になる確認は、カスタムゲームで AI の枠に検証用 AI 文明を入れ、その優先地域に都市を建てる

## `TRAIT_TEST_LONG_COUNT`（検証用志向（長期暦））

- `bLongCountCalendar` 1 だけ。検証用志向（暦）の `chooseGreatPeopleByCalendar` が、神秘主義を得た後に `szTechTrait` で与える（2026-11-26）。

## `TRAIT_TEST_GREAT_LEAP`（検証用志向（大躍進））

- 検証用社会制度（`CIVIC_TEST`）を採用している間だけ持つ（`FreeTraits`）。
- 工房を固定する（`LockedImprovements`）。
- 施設が変わるたびに「[検証] PyOnImprovementChanged: (x, y) 古い施設 -> 新しい施設, gold 金, hammers 生産」を出す（`PyOnImprovementChanged`）。
- 工房に置き換えたときの金: 大商人の交易任務（交易の利益 5）の 4 分の 1 を町 100%・村 40%・集落 15%・小屋 5% で（通常の速度で 375・150・56・18）。
- 農地を工房に置き換えたときの生産: `scaleBySpeed(30, 'Train')`（通常の速度で 30）を、そのタイルの都市に（`gainImprovementProduction`。都市画面の生産の内訳に「施設の転換により」の行）
