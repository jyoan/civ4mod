# 志向のタグ（領土・地形）

地形・地形の特徴・都市・都市圏・国境・移動・蛮族・資源の発見。

## `TraitInfo/iWorkerSpeedModifier`

- ファイル: `CIV4TraitInfos.xml`
- 労働者の作業速度 %（文明全体に 1 回だけ適用）

## `TraitInfo/FeatureProductionChanges`、`FeatureGrowthChanges`、`FeatureHealthChanges`

- ファイル: `CIV4TraitInfos.xml`
- 三国志 Mod（The History of Three Kingdoms）の山越・南蛮の志向と同じ名前・同じ書式。
- どれも `<FeatureProductionChange>` / `<FeatureGrowthChange>` / `<FeatureHealthChange>` を何個でも書け、各項目は `FeatureType` と数値（`iProductionChange` / `iGrowthChange` / `iHealthPercent`）。
- 志向の合計（`CvPlayer::getFeatureProductionChange` など）。
- `FeatureProductionChanges`: その特徴を労働者が取り除いたとき（伐採）の生産力に足す（`CvPlot::getFeatureProduction`。都市からの距離による減少の後、`iFeatureProductionModifier`・ゲーム速度・都市の人口の補正の前。生産力を受け取る都市の持ち主の志向で判定）。
- BtS で生産力の出ないジャングルの伐採でも生産力が出る。
- `FeatureGrowthChanges`: 地形の特徴が広がる確率（BtS の `iGrowth` と同じ 1/10000 単位。隣接する同じ特徴のタイル 1 つにつき足す。`CvPlot::doFeature`）に足す。
- そのタイルの持ち主の志向で判定するので、**領土内だけ**。
- BtS の森は 8、ジャングルは 16。
- BtS で広がらない特徴（氾濫原など。`canHaveFeature` を満たす場所だけ）も広がるようになる。
- UP のゲーム速度の補正もかかる。
- `FeatureHealthChanges`: 都市圏のその特徴の衛生（BtS の `iHealthPercent`、1/100 単位。森 +50、ジャングル -25、氾濫原 -40、死の灰 -50）に足す（`CvCity::updateFeatureHealth`。都市の持ち主の志向）。
- 足した結果が正なら衛生、負なら不衛生として数える（ジャングルに +25 で 0、+50 で森と同じ +0.25）。
- 志向が変わったときは全都市で計算し直す（`updateTraitDependents`）。
- そのつど判定するのでセーブデータは変わらない。
- 説明文は `TXT_KEY_TRAIT_FEATURE_PRODUCTION_CHANGE`（「ジャングルの伐採で得る生産力+30」）・`TXT_KEY_TRAIT_FEATURE_GROWTH_CHANGE`（確率は % に直して小数で出す）・`TXT_KEY_TRAIT_FEATURE_HEALTH_CHANGE`（衛生も小数で出す）。
- Python は `CvTraitInfo` / `CyPlayer` の `getFeatureProductionChange(iFeature)`・`getFeatureGrowthChange(iFeature)`・`getFeatureHealthChange(iFeature)`

## `TraitInfo/bNoPillageInBorders`

- ファイル: `CIV4TraitInfos.xml`
- その志向を持つプレイヤーの領土では、他のチームのユニット（蛮族を含む）が略奪できない（ex 氏の CivGoldEmpires の防衛志向「領内略奪不可」）。
- 施設も道路も対象。
- 誰の志向かはタイルの持ち主で決まる（`CvUnit::canPillage`。AI の略奪もここを通る）。
- 自国のユニットは今までどおり。
- そのつど判定（`CvPlayer::isNoPillageInBorders()`）。
- 説明文は `TXT_KEY_TRAIT_NO_PILLAGE_IN_BORDERS`（「自国の領土では他国のユニットが略奪できない」）

## `TraitInfo/bNoGlobalWarmingInBorders`

- ファイル: `CIV4TraitInfos.xml`
- その志向を持つプレイヤーの領土のタイルでは、地球温暖化（`CvGame::doGlobalWarming`）が何も起こさない（地形の特徴の消失、`GLOBAL_WARMING_TERRAIN`（砂漠）への変化、施設の破壊のどれも）。
- 温暖化はランダムに選んだ陸のタイルに起こる仕組みで、選ばれたのが領土のタイルならその回は何も起きず、ほかのタイルにも回らない（他国の被害は増えない）。
- 乱数の消費は本体と同じ。
- 核の放射性降下物は対象外。
- 説明文 `TXT_KEY_TRAIT_NO_GLOBAL_WARMING_IN_BORDERS`。
- Python は `CyPlayer.isNoGlobalWarmingInBorders()`、`CvTraitInfo.isNoGlobalWarmingInBorders()`

## `TraitInfo/bInlandCanals`

- ファイル: `CIV4TraitInfos.xml`
- そのプレイヤーの艦船は、水に接していない要塞（`bActsAsCity`）や都市にも入れる。
- 本体では、船が陸のタイルに入れるのは「友好的な都市として扱われ（要塞は自分のチームか、相互通行・属国の相手の領土。敵のユニットがいない）、周囲 8 マスに水がある」タイルだけで、陸の幅が 3 マス以上だと要塞で運河を作れない。
- このタグは後半の条件だけを外す（`CvUnit::canMoveInto` の `DOMAIN_SEA`）。
- 2026-11-28 から、自分のチームの領土の外の要塞にも入れる（メフメド2世）:
  - どこの領土でもない要塞（BtS の `CvPlot::isFriendlyCity` は持ち主のいないタイルでは偽なので、本体では入れない）
  - 他国の領土の要塞（その領土に入れるとき。相互通行か戦争中）
  - どちらも都市でなく、敵のユニットがいないとき（`CvUnit::canEnterForeignCanalFort`）。攻撃はしない
  - 海に面した要塞が上陸地点と見なされないよう、`CvSelectionGroup::isAmphibPlot` でも同じ判定を使う
- 以前この項目に書いていた「どこの領土でもない場所」は誤りだった（本体の判定では入れなかった）
- 前半の条件と、船を作れる都市（水に接した都市）は本体のまま。
- 船の持ち主の志向で判定する。
- 説明文 `TXT_KEY_TRAIT_INLAND_CANALS`。
- Python は `CyPlayer.isInlandCanals()`

## `TraitInfo/bBarbarianAlly`

- ファイル: `CIV4TraitInfos.xml`
- 蛮族と平和（FFH2 と同じ名前。FFH2 の `TRAIT_BARBARIAN`）。
- チームの生きているプレイヤーの誰かがこの志向を持つと有効（`CvTeam::isBarbarianAlly`。その都度判定するのでセーブしない）。
- ゲーム開始時は蛮族が宣戦しない（`CvGame::initDiplomacy`。FFH2 はここで小文明も宣戦しないが、この Mod は蛮族だけ）。
- ゲームの途中で志向を得ると蛮族と和平し、失うと蛮族が宣戦する（`CvTeam::updateBarbarianAlly`。FFH2 は開始時だけ）。
- 蛮族と平和なチームと蛮族は、お互いの領土に入れる（`canEnterTerritory`。FFH2 と同じ。ほかのチームは蛮族と戦争中なので影響しない）。
- 動物も蛮族なので攻撃してこない（動物は本体どおり持ち主のある土地に入らない）。
- 平和なので蛮族の都市やユニットを攻撃できない。
- ゲームの途中で生まれた植民地などの新しいプレイヤーは本体の扱いのまま。
- 説明文 `TXT_KEY_TRAIT_BARBARIAN_ALLY`。
- Python は `CyTeam.isBarbarianAlly()`

## `TraitInfo/bBarbarianVision`

- ファイル: `CIV4TraitInfos.xml`
- 蛮族の視界を共有する（2026-11-19）。
- 蛮族のユニット（動物も）と都市が見ているタイルが見える（見えないユニットは見えない）。
- `bBarbarianAlly` と同じく、チームの生きているプレイヤーの誰かが持てばチーム全体に効く（`CvTeam::isBarbarianVision`）。
- BtS の「視界を盗む」仕組み（無印のスパイの「計画を盗む」の効果。`CvTeam::setStolenVisibilityTimer`。相手の視界の変化にも追従し、タイマーはチームのセーブにある）を使い、蛮族チームへのタイマーを 2 以上に保つ（`CvTeam::updateBarbarianVision`。志向を得た・失ったときにすぐ反映（`changeFreeTraitCount`）、ゲーム開始時（`PyOnGameStart` の前）、チームの `doTurn` でタイマーが減った直後に補充）。
- 志向を失うと 0 にするので、スパイが蛮族の都市から計画を盗んだ分の視界も消える。
- プレイヤーが死んで志向がなくなった場合などは 2 ターン以内に切れる。
- 説明文 `TXT_KEY_TRAIT_BARBARIAN_VISION`。
- Python は `CvTraitInfo.isBarbarianVision()`、`CyTeam.isBarbarianVision()`

## `TraitInfo/PassableTerrainsInBorders`、`PassableFeaturesInBorders`

- ファイル: `CIV4TraitInfos.xml`
- 自国の領土の通行不能なタイルにユニットが入れる（2026-11-19。カパック・ニャンの案）。
- 地形（`TerrainType`）・地形の特徴（`FeatureType`）の一覧。
- `TERRAIN_PEAK` は山岳のタイル。
- 判定は `CvPlot::isImpassable` と同じ順（山岳 → 地形の特徴 → 地形。`CvPlayer::isTraitPassableInBorders`）。
- そのチームの領土のタイルだけで、**ダメージも追加の移動コストもない**（昇進の `ImpassablePlotPassages` の `iDamagePercent` 0 と同じ扱い。`CvUnit::getImpassablePlotPassageDamage` / `getImpassablePlotPassageMoveCost` で 0 を返す）。
- 陸上ユニットすべて（兵科のない労働者なども）。
- 昇進の `ImpassablePlotPassages`（ハンニバル）は領土の外でも入れるがダメージを受ける。
- 説明文 `TXT_KEY_TRAIT_PASSABLE_IN_BORDERS`。
- Python は `CvTraitInfo.isPassableTerrainInBorders(i)`・`isPassableFeatureInBorders(i)`

## `TraitInfo/FeatureHealChanges`

- ファイル: `CIV4TraitInfos.xml`
- その地形の特徴にいるユニットの回復 +%（2026-11-19。ドルイドの森の案）。
- `<FeatureHealChange>` が `FeatureType` + `iHealChange`。
- どの領土でも、毎ターンの回復量（`CvUnit::healRate`）に足す。
- 説明文 `TXT_KEY_TRAIT_FEATURE_HEAL`（「森にいるユニットの回復 +20%」）。
- Python は `CvTraitInfo.getFeatureHealChange(iFeature)`

## `TraitInfo/iNewWorldCityPopulation`、`iOverseasCityPopulation`、`iOverseasMaintenanceModifier`、`iDistantTradeRouteModifier`

- ファイル: `CIV4TraitInfos.xml`
- 地域区分（「タグのない機能」の節）を使う効果。
- `iNewWorldCityPopulation`: 新世界に建てた都市の人口 +N。
- `iOverseasCityPopulation`: 海外（首都と別の地域）に建てた都市の人口 +N。
- どちらも建設したときだけ（`CvPlayer::found`。`iNewCityPopulation` と合計する）。
- 最初の都市は首都なので海外にならない。
- `iOverseasMaintenanceModifier`: 海外の都市の維持費 %（距離・都市数・植民地・企業の維持費すべてにかかる）。
- `CvCity::getMaintenanceModifier` に足すので、維持費の計算・都市画面の説明・財務顧問がすべて含む。
- 都市画面の維持費の説明には内訳の 1 行（`TXT_KEY_MISC_OVERSEAS_MAINT_MODIFIER`）も出す。
- 遷都すると海外かどうかが変わり、BtS の遷都の処理が維持費を計算し直す。
- BtS の植民地の維持費（首都と別の陸塊 `CvArea`）はそのまま。
- `iDistantTradeRouteModifier`: 遠く離れた都市との交易路の収入 %。
- 交易路の 2 つの都市の距離が、マップの最大距離の `REGION_DISTANT_PERCENT`% 以上のとき（`CvCity::isDistantTradeRoute`。首都は関係ない。`CvCity::totalTradeModifier`。交易路の説明にも 1 行）。
- 自国の都市にも外国の都市にも効く。
- 志向が変わったときは `updateTraitDependents` が維持費と交易路を計算し直す。
- Python: `CyPlayer.get〜`

## `TraitInfo/ExplorationRewards`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-22。
- 探検の報酬。
- `<ExplorationReward>` に `ExplorePlotType`・`CommerceType`・`iPercent` を書き、複数並べられる。
- ターン中にチームに初めて明らかになったタイルを種類ごとに数え（`CvPlot::setRevealed` → `CvPlayer::onPlotExplored`。チームの全員が数える）、プレイヤーのターンの終わり（`doTurn` の `doGold` の前。`doExplorationRewards`）に、枚数 × `iPercent` ÷ 100 を与える。
- 種類と商業の組ごとに全志向の `iPercent` を合計し（`getExplorationRewardPercentFor`）、1 未満の端数は次のターンに持ち越す（`m_aiExplorationRemainder`）。
- `iPercent` が 0 以下の行は無視。
- **種類**（`CvPlayer::getExplorePlotType`）: `EXPLORE_PLOT_WATER`（水タイルすべて。外洋・沿岸・湖）、`EXPLORE_PLOT_HOME`（本国の陸地。`!isOverseas`）、`EXPLORE_PLOT_OVERSEAS_OLD_WORLD`（海外で新世界でない陸地）、`EXPLORE_PLOT_OVERSEAS_NEW_WORLD`（海外で新世界の陸地）。
- 本国が優先するので、首都が新世界にあればその地域は本国（SeaStart の文明で起こりやすい）。
- 種類は明らかになった時点で決める。
- **報酬**: 金は国庫、研究は研究中の技術（なければ `changeOverflowResearch`。使うときに研究の補正が掛かるので、補正を割って入れる）、文化は首都（首都がなければ失う）、諸報は `doEspionagePoints` と同じく相手ごとの比重で分ける（`getEspionageSpending` に配る総量の引数を足した）。
- 人間には「探検の報酬: +3[金]」のメッセージを 1 行出す。
- **数えないもの**: ゲーム開始前（`isFinalInitialized` が false。開始時の視界）、他のチームから（`eFromTeam` あり。地図の取引・永久同盟の合流・植民地の独立）、地形だけ（`bTerrainOnly`。技術の `bMapVisible`、Advanced Start で買う視界）、Advanced Start 中のプレイヤー（視界を売り戻せる）。
- 部族集落の地図・国境の拡大は数える（国境の拡大は `doTurn` 中なので次のターンの分）。
- カウンタは志向に関係なく全プレイヤーが持ち、セーブする（`CvPlayer` の `uiFlag=17`）。
- **説明文**: 行ごとに「ターン中に明らかにした水タイル（海・湖）10枚ごとに+1[金]」（`iPercent` が 100 の約数なら「N 枚ごとに +1」、それ以外は「1 枚ごとに +2.5」）、最後に注釈 1 行（数えないもの・持ち越し・概念「地域区分」へのリンク）。
- `parseTraitExplorationRewards`。
- Python: `CvTraitInfo.getNumExplorationRewards` / `getExplorationRewardPlotType` / `getExplorationRewardCommerceType` / `getExplorationRewardPercent` / `getExplorationRewardPercentFor`、`CyPlayer.getExploredPlotCount`

## `TraitInfo/iRouteMovementModifierInBorders`、`Conquest/bStateReligion`、`Conquest/ReligionBuildings`、`NoTechCivics`

- ファイル: `CIV4TraitInfos.xml`
- RFC の固有能力から。
- `iRouteMovementModifierInBorders`: 自国のチームの領土の 2 タイルの間の道路・鉄道の移動コスト %（`CvPlot::movementCost`。-50 で 2 倍の速さ: BtS の道路は移動力 1 で 2 タイル、-50 なら 4 タイル。鉄道の 0 コストの移動はそのまま。経路探索も同じ。RFC のローマ）。
- `bStateReligion`（2026-11-23 に `<Conquest>` の子に移し、名前を `bConquestStateReligion`・`ConquestReligionBuildings` から変えた（getter と Python の名前は元のまま）。）: 占領した都市に国教が広まる。
- `ReligionBuildings`: 占領した都市に建てる国教の建物を、特殊建造物（`SpecialBuildingType`。`SPECIALBUILDING_TEMPLE`・`_MONASTERY`・`_CATHEDRAL`）で何個でも書く。
- 国教でその特殊建造物の建物（文明の固有建物ならその版）を、**建設の条件を満たすときだけ**建てる（`CvCity::canConstruct` の費用以外の判定: 技術、都市の宗教、大聖堂に要る寺院の数など。ここで建てた寺院も数えるように 2 回まわす）。
- どちらも征服のときだけ（`CvPlayer::acquireCity`。RFC のアラビア）。
- `NoTechCivics`: 技術なしで採用できる社会制度（`CivicType` を何個でも。`CvPlayer::canDoCivics`。社会制度の説明にも志向の名前。RFC のエジプト）

## `TraitInfo/MaintainFeatures`

- ファイル: `CIV4TraitInfos.xml`
- FFH2 の文明の同名タグと同じ書式（`<MaintainFeature>` が `FeatureType` + `bMaintain`）。
- その志向を持つプレイヤーの領土では、施設を作る作業をしても、その地形の特徴（森・ジャングルなど）が消えず、施設の下に残る（`CvPlot::isBuildKeepsFeature`。誰の志向かはタイルの持ち主で決まる。FFH2 と同じ）。
- 誰の領土でもないタイルと、施設のない作業（伐採だけ）では今までどおり消える。
- 残すときは、特徴を取り除く時間を足さず（FFH2 は足す）、伐採の生産は入らず、取り除くための技術（青銅器など）も要らない（FFH2 と同じ）。
- 産出は特徴の分も足され、衛生（森 +0.5・ジャングル -0.25）も残る。
- 作業ボタンの説明は「森は残ります」（`TXT_KEY_ACTION_KEEP_FEATURE`）になり、産出の予測も特徴を残した値。
- アドバンスドスタートで施設を置いても特徴は消えない。
- そのつど判定（`CvPlayer::isMaintainFeatures`）。
- AI の作業の判断は変えていない。
- 説明文は `TXT_KEY_TRAIT_MAINTAIN_FEATURES`（「施設を作っても残る: 森, ジャングル」）

## `TraitInfo/FeatureRoutes`

- ファイル: `CIV4TraitInfos.xml`
- `<FeatureRoute>`（`FeatureType` + `RouteType`）を何個でも書ける。
- その志向を持つプレイヤーの領土では、その地形の特徴のタイルを、道路がなくても `RouteType` の道として扱う（Civ5 のイロコイの「領内の森・ジャングルを道路として扱う」）。
- 移動: 同じチームのユニット（労働者・開拓者を含む）だけが道のコストで移動できる（`CvPlot::getUnitRouteType`。本物の道路もあれば良い方（`iValue`）。川を渡るときは本物の道路と同じく橋梁建設が必要。本物の道路と違い、他チームのユニットは使えない）。
- 交易網: 同じチームの都市のつながりと資源のつながりに使える（`CvPlot::isNetworkRoute`。`isTradeNetworkConnected` と `isBonusNetwork`。森の中の資源も道路なしで交易網に入る）。
- 特徴が変わると交易網を計算し直す（`setFeatureType`。領土が変わるときは BtS がもともと計算し直す）。
- 地図に道路は描かれず、道路で増える産出（鉄道 + 鉱山など）は本物の道路だけ。
- AI の作業の判断は変えていない（経路探索は移動コストを使う）。
- 道の種類は志向より後に読み込まれるので、`readPass3()` で番号に変える。
- そのつど判定（`CvPlayer::getFeatureRouteType`）。
- 説明文は `TXT_KEY_TRAIT_FEATURE_ROUTES`（「自国の領土で道路として扱う（移動と交易網）: 森, ジャングル」）

## `TraitInfo/RevealBonuses`

- ファイル: `CIV4TraitInfos.xml`
- 資源の種類（`BonusType`）を何個でも書ける。
- その志向を持つプレイヤーのチームは、資源の `TechReveal` の技術なしでその資源が見える（ランダムイベントの資源の発見 `BonusRevealed` と同じ扱い。`CvPlot::getBonusType(eTeam)` で `CvTeam::isTraitRevealedBonus` を見る）。
- タイルの産出（資源の産出、施設の資源の産出）を得られ、鉱山などの施設が資源を発見することもある（`CvPlot::doImprovement`）。
- 資源を使う（交易網につなぐ）には、これまでどおり `TechCityTrade` の技術が要る。
- 志向を持つ間だけ効き、志向を失えば見えなくなる（イベントの発見と違い、チームに記録しない）。
- 志向が変わると `updateTraitBonusPlots` が資源のタイルの交易網・産出・表示を更新する。
- 後で `TechReveal` の技術を得ても「資源を発見」のメッセージは出さない。
- 資源は志向より後に読み込まれるので `readPass3()` で番号に変える。
- 説明文 `TXT_KEY_TRAIT_REVEAL_BONUSES`（「技術なしで見える資源: 石油, ウラン」。リンク）。
- Python は `CyPlayer.isTraitRevealBonus(eBonus)`、`CvTraitInfo.isRevealBonus(iBonus)`

## `TraitInfo/TerrainTrades`、`CapitalTerrainYieldPercents`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-19。
- `TerrainTrades`: 技術の同名のタグと同じ書式（`<TerrainTrade>` が `TerrainType` + `bTerrainTrade`）。
- その地形がチームの交易網をつなぐ（川・海と同じく、道路がなくても資源と交易路がつながる）。
- `TERRAIN_HILL` / `TERRAIN_PEAK` は丘陵・山岳のタイルを指す（`TerrainYieldChanges` と同じ。2026-11-19 に追加。それまでは本当の地形でだけ判定していたので効かなかった。`CvPlot::isNetworkTerrain`）。
- `TERRAIN_PEAK` があると、山岳は通行不能でも交易網を止めない（`isTradeNetworkImpassable`）。
- `CvPlayer::processTrait` で技術と同じ `CvTeam::changeTerrainTradeCount` に足すので、交易網はその場で計算し直される（ゲーム開始時の `init` で適用するので、既存のセーブの指導者・文明に足しても反映されない。永久同盟でチームが合わさると、合わさった側の志向の分は数え直さない）。
- `CapitalTerrainYieldPercents`: `TerrainYieldChanges` と同じ書式（`<CapitalTerrainYieldPercent>` が `TerrainType` + `<Yields>`）で、値は 1/100。
- 首都の都市圏にあるその地形のタイル（自国の領土だけ。都市のタイルも数える）1 つにつき、首都の産出に足す（合計してから切り捨て）。
- 地形のタイルそのものの産出は変わらない（マリの案）。
- `CvCity::getTraitCapitalTerrainYield` を都市の動的な産出に含めるので、更新はターンの初め・志向の変化・遷都（宮殿の付け外し）のとき。
- 産出の内訳に「首都の都市圏の地形（志向）」の行（`TXT_KEY_MISC_HELP_YIELD_TRAIT_CAPITAL_TERRAIN`）。
- 説明文 `TXT_KEY_TRAIT_TERRAIN_TRADE`（「交易網をつなぐ地形: 砂漠」）、`TXT_KEY_TRAIT_CAPITAL_TERRAIN_YIELD`（「首都の都市圏の砂漠 1 タイルにつき、首都で+0.50[食料], +1[商業]」）。
- Python は `CvTraitInfo.isTerrainTrade(iTerrain)`・`getCapitalTerrainYieldPercent(iTerrain, iYield)`

## `TraitInfo/StartBias/SeaStart`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-23 に `<StartBias>` の子に移した（名前は同じ）。
- `<SeaStart>` に `ShipUnitClass`（必須。`NONE` で船なし）を書く。
- 書けば有効（ブーリアンの代わり）。
- Civ6 のマオリ。
- 海で開始し、初期ユニット（難易度による AI の追加ユニットを含む）が `ShipUnitClass` の船に乗っている。
- 船は文明のユニット（固有ユニットに置き換わる）。
- 艦船でないユニットや文明に無いクラスは船なしと同じ。
- **開始位置**: 陸の開始位置を BtS どおりに割り当てて調整した後（`normalizeStartingPlots` の後。チームの仲間を近づける入れ替えで海の開始位置が他の文明に移らないように）、海のタイルに移す（`CvPlayer::moveStartingPlotToSea`。`CvGame::setInitialItems` から）。
- 候補は湖でない（`MIN_WATER_SIZE_FOR_OCEAN`）・通行可能（氷なし）・船が入れる・ユニットのいない・他の開始位置でないタイル。
- まず `TERRAIN_OCEAN`（外洋）、無ければ `TERRAIN_COAST`（近海）から、`getSorenRandNum` で選ぶ（`findSeaStartingPlot`）。
- 条件は 陸から 2〜5 タイル（近海は 1〜5）で他の開始位置から `startingPlotRange()` 以上 → 1〜8 タイルでその半分以上 → どこでも、の順に緩める。
- 外洋も近海も選べなければ陸で普通に開始する。
- 最初に割り当てた陸の開始位置は空き地になり、地域の区分では誰も開始していない新世界になりうる。
- 海の開始位置は地域に属さない。
- 開始時のカメラは海の開始位置を映す。
- **開始位置のバイアス**: このプレイヤーは文明・志向のバイアスを使わない（`buildStartBias`）。
- ただし地図に海（湖でない水域）がなければ陸で開始するのでバイアスも効く。
- **船**: 陸上ユニットを全員開始タイルに置き（人間の探索ユニットなどを周りに散らさない）、乗れるユニット（`iCargo`・`SpecialCargo`・`DomainCargo`）が乗り切るだけ船を作って乗せる（`loadSeaStartUnits`）。
- 乗れないユニット（キャラベル船に開拓者など）は船に乗らずに海上に残る（ワールドビルダーで置いたのと同じ。陸に隣接していれば上陸できる。宣戦や国境の変化などで `CvPlot::verifyUnitValidPlot` がそのタイルを調べると、最寄りの陸に移される）。
- 乗れるユニットが 1 体もないときも船は 1 隻作る。
- 船の UnitAI は、陸上ユニットを積めるなら `UNITAI_SETTLER_SEA`。
- **AI**: 都市が 1 つもない間、開拓者を乗せた船はすぐに都市の候補地へ運ぶ（`CvUnitAI::AI_settlerSeaMove`。BtS は満載か、防衛ユニットと労働者も乗るまで待つので、都市を建てなかった）。
- 候補地を知らなければ、未探索のタイルと見えている陸に近い水のタイルへ 1 ターン分だけ進み、毎ターン候補地を探し直す（`AI_seaStartExplore`。BtS の `AI_explore` は何ターンもかかる移動を命じるので、見えた陸を素通りする）。
- 候補地は都市を建てる価値が `AI_getMinFoundValue` を超える見えているタイル（都市のない大陸では沿岸だけ）で、価値はターンのはじめに計算し直す。
- **制覇勝利**: まだ最初の都市を建てていない海で開始したプレイヤーは、都市を持つチームと同じく制覇勝利を妨げる（`CvGame::testVictory`。BtS は都市を持つチームがほかにいなければ成立するので、AI が上陸する前に人間が都市を建てると勝利になった）。
- 乗ったユニットは移動力が残るので 1 ターン目に降ろせる。
- **効かない場合**: 上級スタート（`GAMEOPTION_ADVANCED_START`）、開始位置を決めてあるシナリオ（WB マップでプレイヤーを含むもの）。
- 指導者・文明の志向だけが対象（`RandomBasicTraits`・`ConditionalFreeTraits` で得る志向は初期ユニットの後に付くので効かない）。
- 複数の志向が持つときは番号の小さい志向の船。
- 説明文: `TXT_KEY_TRAIT_SEA_START`（船なしは `TXT_KEY_TRAIT_SEA_START_NO_SHIP`）。
- Python: `CvTraitInfo.isSeaStart()`・`getSeaStartUnitClass()`

## `TraitInfo/bImprovementUpgradeUnworked`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-23。
- その志向を持つプレイヤーの文化圏（タイルの所有者がそのプレイヤー）の施設は、市民を配置していなくても毎ターン成長する（小屋 → 集落 → 村 → 町など、`ImprovementUpgrade` のある施設）。
- 都市圏の外のタイルも含む。
- 速度は BtS と同じ `CvPlayer::getImprovementUpgradeRate`（`iImprovementUpgradeRateModifier` も効く）。
- 配置されていないタイルは `CvPlot::doTurn` で進め、配置されているタイルは BtS どおり `CvCity::doTurn` で進める（その都市が暴動中なら進まない）。
- 説明文 `TXT_KEY_TRAIT_IMPROVEMENT_UPGRADE_UNWORKED`。
- Python: `CyPlayer.isImprovementUpgradeUnworked()`、`CvTraitInfo.isImprovementUpgradeUnworked()`

## `TraitInfo/TerritoryRoutes`

- ファイル: `CIV4TraitInfos.xml`
- `<TerritoryRoute>`（`TerrainType` + `RouteType`）を何個でも書ける（2026-11-28。ワイナ・カパックのインカ道）。
- その地形のタイルがそのプレイヤーの領土になったとき（都市の建設・文化圏の拡大・征服・取引）、1 回だけその道を敷く（`CvPlot::setOwner`）。
- 技術は要らない。よりよい道（`iValue`）がすでにあればそのまま。略奪された道は直さない。
- 丘陵は `TERRAIN_HILL` と下の地形の両方、山岳は `TERRAIN_PEAK` として数える（`CvPlayer::getTerritoryRouteType`）。
- 道の種類は志向より後に読み込まれるので、`readPass3()` で番号に変える。
- 説明文 `TXT_KEY_TRAIT_TERRITORY_ROUTES`（「領土になったタイルに道路を敷く: 丘陵」）。
- Python は `CvTraitInfo.getTerritoryRouteType(iTerrain)`
