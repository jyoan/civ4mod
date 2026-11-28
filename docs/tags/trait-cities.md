# 志向のタグ（都市・文化圏）

2026-11-28 に `docs/tags/trait-territory.md` から分けた。

## `TraitInfo/iCityRadiusChange`

- ファイル: `CIV4TraitInfos.xml`
- 都市圏の半径 +○（既定 2、最大 3）。
- FFH2 の `bSprawling` から都市圏の部分だけを取り出したもの（都市数の上限は `iMaxCities` / `iMaxCitiesPercent`）。
- 仕組みは `docs/city-radius.md`。
- 説明文は `TXT_KEY_TRAIT_CITY_RADIUS_CHANGE`（「都市圏の半径+1（最大3）」）。
- `CityRadiusCultureLevel`（文化レベル）を書くと、その文化レベル以上の都市だけに効く（説明文 `TXT_KEY_TRAIT_CITY_RADIUS_CHANGE_CULTURE`）

## `TraitInfo/iMaxCities`、`iMaxCitiesPercent`

- ファイル: `CIV4TraitInfos.xml`
- 建設できる都市の数の上限（FFH2 の `bSprawling` の都市数の部分）。
- 上限は「`iMaxCities` + マップサイズの `iTargetNumCities`（`CIV4WorldInfo.xml`。デュエル・小 4、やや小・標準 5、大・特大 6）の `iMaxCitiesPercent`%」（切り捨て、最低 1）。
- どちらも 0 なら上限なし。
- 複数の志向が上限を持つときは最も小さいもの（`CvPlayer::getMaxCities()`、そのつど判定。`CvTraitInfo::getMaxCitiesLimit`）。
- 都市の数（征服した都市も含む）が上限に達すると、BtS の一都市チャレンジと同じ場所で止める: 都市の建設（`canFound`。AI の候補地にもならない）、開拓者（`bFound` のユニット）の生産（`canTrain`。生産の一覧から消える）、小屋の開拓者（`canReceiveGoody`）、開始時の開拓者（`addFreeUnit`。都市と開拓者の合計が上限まで）。
- 征服・取引・文化の寝返りでは上限を超えて都市を得てよく、超えている間は建設できない。
- 上限に達する前に作った開拓者は残る（建設はできない）。
- 説明文は `TXT_KEY_TRAIT_MAX_CITIES`（「建設できる都市は 3 都市まで」）、`iMaxCitiesPercent` があれば `TXT_KEY_TRAIT_MAX_CITIES_MAP`（「…（マップの大きさで変わる）」）で、数はそのゲームのマップサイズで計算する。
- Python は `CyPlayer.getMaxCities()`（-1 で上限なし）/ `isMaxCitiesReached()`

## `TraitInfo/iMinCityRangeChange`・`bFoundInForeignTerritory`

- ファイル: `CIV4TraitInfos.xml`
- 都市を建てる条件（`CvPlayer::canFound`）。
- `iMinCityRangeChange`: 都市どうしの最低距離 `MIN_CITY_RANGE`（`GlobalDefines.xml`、本体は 2 で、間に 2 マス必要）に足す。
- 志向の合計で、0 未満にはならない（`CvPlayer::getMinCityRange`）。
- -1 で間に 1 マス、-2 で隣接も可。
- 正の値で厳しくもできる。
- 範囲内に都市があると建てられないこと（同じ大陸の都市だけ数える）は本体と同じ。
- 建設のボタンの説明（「○マス以内に都市がある」）も志向の値を使う。
- `bFoundInForeignTerritory`: 他国の文化圏にも都市を建てられる（平時も。ユニットが入れる領土なら。平時は開放国境が必要）。
- 都市のタイルは自分の領土になり、周りのタイルは本体どおり文化の強い方のもの。
- 文化による離反も本体どおり起こりうる。
- AI はこれらを狙って都市を建てるわけではない。
- 説明文: `TXT_KEY_TRAIT_MIN_CITY_RANGE`（その志向の値だけで計算した距離）・`_ZERO`、`TXT_KEY_TRAIT_FOUND_IN_FOREIGN_TERRITORY`。
- Python は `CyPlayer.getMinCityRange()` / `isFoundInForeignTerritory()`、`CvTraitInfo.getMinCityRangeChange()` / `isFoundInForeignTerritory()`

## `TraitInfo/iCultureRangeChange`

- ファイル: `CIV4TraitInfos.xml`
- 全都市の文化圏（国境）の広がり +○（マイナスも可）。
- BtS では文化レベルの番号がそのまま広がりで、これに足す。
- ただし文化を持つ都市は隣のタイルを失わない（文化レベル 1 以上なら広がり 1 以上）。
- プラスのときは、文化がまだない都市（建設した直後）も文化レベル 1 として数え、建設したときに広がりの内側のタイル（隣のタイルより外）に隣のタイルと同じ初期の文化（`FREE_CITY_ADJACENT_CULTURE`）を置くので、建設したその場で国境が広がる（2026-11-22。それまでは次のターンに都市が文化を得てから広がった。`CvCity::init`・`updateCultureRange`）。
- 文化レベルの閾値や、都市圏の半径（`iCityRadiusChange`）は変えない。
- 上の文化レベルの範囲を先取りするので、タイルへの文化の加算（`CITY_FREE_CULTURE_GROWTH_FACTOR` 分）も増える。
- 他のプレイヤーの文化がそのタイルへ広がる範囲は変えない（持ち主の分だけ）。
- 範囲は `CvCity::updateCultureRange()` で更新し（文化レベルの変化、志向の変化、`doTurn`）、セーブに入る（`CvCity` の `uiFlag` 5）。
- 範囲が広がったタイルは、次のターンに文化が加わってから領土になる。
- Python は `CyCity.getCultureRange()`・`CyPlayer.getCultureRangeChange()`。
- 説明文は `TXT_KEY_TRAIT_CULTURE_RANGE_CHANGE`

## `TraitInfo/iNewCityPopulation`、`bNewCityStateReligion`、`iOccupationTimeModifier`（`BuildingInfo` にも同名タグ）

- ファイル: `CIV4TraitInfos.xml`、`CIV4BuildingInfos.xml`
- 新しい都市と占領した都市（ex 氏の CivGoldEmpires の農耕志向・伝道志向・法隆寺・大衆志向・桃源郷を参考にした）。
- `iNewCityPopulation`: 建設した都市（開拓者などで建てたもの。占領した都市は含まない）の人口 +○（`CvPlayer::found`。人口は 1 未満にしない。アドバンスドスタートでは、この分の人口は売れない）。
- `bNewCityStateReligion`: 建設した都市に国教が広まる（国教があれば。メッセージあり）。
- `iOccupationTimeModifier`: 占領した都市の反乱期間 ○%（征服した側の値。BtS の反乱期間「(3 + 人口の 50%) × (100 − 自チームの文化の割合)%」に掛ける。`CvPlayer::acquireCity`。-100 以下で反乱なし）。
- 占領した都市がすでに反乱中だった場合（奪い合いの最中など）、引き継いだ残りのターンにも同じ % を掛ける（三国志 Mod の仁徳の「反乱なし」を -100 で再現できるように。奪還（`bRecapture`）では BtS と同じく引き継がない。取引・文化による転向ではそのまま引き継ぐ）。
- 有効になるのは「その志向を持つ」か「その建物（陳腐化していないもの）をどこかの都市に持つ」とき。
- 建物は種類ごとに 1 回分（何都市にあっても同じ）。
- 値は足し算（国教はどれか 1 つで有効）。
- `CvPlayer::getNewCityPopulation()` / `isNewCityStateReligion()` / `getOccupationTimeModifier()` がそのつど合計するので、セーブデータは変わらず、既存のセーブにも効く。
- 説明文は `TXT_KEY_NEW_CITY_POPULATION`（「新しい都市の人口+1」）・`TXT_KEY_NEW_CITY_STATE_RELIGION`・`TXT_KEY_OCCUPATION_TIME_MODIFIER`（「占領した都市の反乱期間-50%」）で、志向と建物で共通

## `TraitInfo/NewCityCultureLevel`（`BuildingInfo` にも同名タグ）

- ファイル: `CIV4TraitInfos.xml`、`CIV4BuildingInfos.xml`
- Civ5 のショショーニ。
- 建設した都市（占領した都市は含まない）の文化を、その文化レベルの閾値（ゲーム速度ごと）まで増やす（`CvPlayer::found`。すでに多ければそのまま）。
- 国境もその文化レベルの範囲に広がり、文化レベルの条件のある都市圏の広がり（`CityRadiusCultureLevel` など）も最初から効く。
- 有効になるのは志向と、建物（陳腐化していないものをどこかの都市に持つ）で、その中で最も高い文化レベル（`CvPlayer::getNewCityCultureLevel()`。そのつど判定）。
- 志向の分は `readPass3()` で番号に変える。
- 説明文は `TXT_KEY_NEW_CITY_CULTURE_LEVEL`（「新しい都市は文化レベル未熟から始まる」）で、志向と建物で共通

## `TraitInfo/CityFoundedUnits`

- ファイル: `CIV4TraitInfos.xml`
- 都市を建設するたびにユニットを得る。
- `<CityFoundedUnit>` を何個でも書ける。
- 各項目は `UnitClassType`（そのプレイヤーの文明のユニット。志向の固有ユニットも含む `getCivilizationUnit`）、`iNumUnits`（省略時 1）、`bCoastalOnly`（沿岸の都市のときだけ）。
- 艦船は `bCoastalOnly` がなくても沿岸の都市のときだけ（内陸の都市では動けないため）。
- 沿岸は海（`MIN_WATER_SIZE_FOR_OCEAN` 以上の水域）に面していること。
- 最初の都市も含む。
- 占領や取引で得た都市には付かない。
- `PyOnCityFounded` の後に、建設した都市に作る（`CvPlayer::found`）。
- 説明文 `TXT_KEY_TRAIT_CITY_FOUNDED_UNIT`（艦船と `bCoastalOnly` は `_COASTAL` を添える。ユニット名は既定のユニット）

## `TraitInfo/CultureFlip`（`bNoFlip`・`bNoRevoltAndNoFlip`・`bInstantFlip`・`iRevoltChance`）

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-23。
- 同じ日に `<CultureFlip>` の子に移し、名前を `bNoCultureFlip`・`bNoRevoltAndNoCultureFlip`・`bInstantCultureFlip`・`iCultureRevoltChance` から変えた（getter と Python の名前は元のまま）。
- 文化による反乱と寝返り（`CvPlot::doCulture`）。
- BtS の流れ: 文化で上回られた（`calculateCulturalOwner` が別チーム）占拠中でない都市で、毎ターン `getRevoltTestProbability`（`REVOLT_TEST_PROB` × (100 - ユニットの反乱防止)%）の確率で判定し、`rand(cultureStrength) > cultureGarrison` なら反乱（守備ユニットの HP 半分）。
- 寝返りの条件は「都市の寝返りなし」オプションが無効・相手が元の持ち主でない（「征服後の寝返り」で無視）・相手の文化による反乱が `NUM_WARNING_REVOLTS`（1）回以上で、満たさなければ反乱回数 +1 と占拠。
- **`bNoFlip`**（都市の持ち主の志向）: 寝返りの条件を満たさなくする。
- 反乱・占拠・ダメージは起きる。
- **`bNoRevoltAndNoFlip`**（都市の持ち主）: 反乱の確率を 0 にする（`CvCity::getRevoltTestProbability`）ので反乱も寝返りも起きない。
- `CvPlayer::isNoCultureFlip` はこちらでも真。
- **`bInstantFlip`**（文化で上回っている側）: 反乱回数の条件だけを外し、1 回目の反乱で寝返る。
- オプションと元の持ち主の条件は残る。
- **`iRevoltChance`**（文化で上回っている側、合計）: `REVOLT_TEST_PROB` を (100 + 値)% にする（ユニットの反乱防止の前。0 未満にはならない）。
- 都市画面の文化のマウスオーバーの「反乱の確率」にも反映（`getRevoltTestProbability` に文化の持ち主を渡す）。
- 蛮族の都市は BtS どおり常に寝返る。
- 説明文 `TXT_KEY_TRAIT_NO_CULTURE_FLIP`・`_NO_REVOLT_AND_NO_CULTURE_FLIP`（両方あればこちらだけ）・`_INSTANT_CULTURE_FLIP`・`_CULTURE_REVOLT_CHANCE`。
- Python: `CvTraitInfo.isNoCultureFlip`・`isNoRevoltAndNoCultureFlip`・`isInstantCultureFlip`・`getCultureRevoltChance`、`CyPlayer` の同名の関数

## `TraitInfo/CityPlotBuildingProductionModifiers`

- ファイル: `CIV4TraitInfos.xml`
- 都市圏の地形・地形の特徴 1 つにつき、対象の建物の生産 %（2026-11-28。エチオピアの宗教建築、クメールの世界遺産）。
- `<CityPlotBuildingProductionModifier>` を何個でも書ける。各項目は:
  - タイルの条件: `TerrainType`・`FeatureType`（どちらかか両方。両方なら両方に当てはまるタイル）
  - 対象: `BuildingClassType`・`SpecialBuildingType`・`bReligiousBuildings`（宗教の建物すべて。寺院・僧院・大聖堂など）・`bWorldWonders`（世界遺産すべて）のどれか
  - `iProductionModifier`
- 数えるタイルは、都市圏のうち自国の領土のもの。都市のタイルは数えない。働いているかは問わない（`CvCity::countCityPlots`）。
- 丘陵は `TERRAIN_HILL` と下の地形の両方、山岳は `TERRAIN_PEAK` として数える。施設の下に残る地形の特徴（`MaintainFeatures`）も数える。
- `CvCity::getProductionModifier(BuildingTypes)` に足す。その都度数えるので、タイルが変わるとすぐに変わる。
- 生産の内訳に `TXT_KEY_MISC_HELP_PROD_CITY_PLOTS`、志向の説明文に `TXT_KEY_TRAIT_CITY_PLOT_BUILDING_PRODUCTION`。
- 建物クラスと特別な建物は志向より後に読み込まれるので、`readPass3()` で番号に変える。
- Python は `CyCity.countCityPlots(eTerrain, eFeature)`・`getCityPlotBuildingProductionModifier(eBuilding)`、`CvTraitInfo` の `getCityPlotBuildingProduction...(i)`

## `TraitInfo/iWonderGreatPeopleTurns`、`iWonderProductionCulturePercent`

- ファイル: `CIV4TraitInfos.xml`
- 世界遺産を生産で完成させたとき（`CvCity::popOrder` の `doWonderCompletedTraits`。2026-11-28。ヴィクトリア・ルイ14世）:
  - `iWonderGreatPeopleTurns`: その遺産の偉人ポイント（`iGreatPeopleRateChange`）× N ターン分を、その都市の偉人ポイントと、その偉人の種類の分の両方に足す。補正なし、ゲームの速度の `iGreatPeoplePercent` をかける
  - `iWonderProductionCulturePercent`: その遺産の生産コスト（`getProductionNeeded`。ゲームの速度に合う）の N% を、その都市の文化に足す
- 征服で得た遺産、大預言者が建てる聖地、設立で置かれる企業の本社は、生産で完成しないので対象外。
- どちらもイベントログにメッセージ（`TXT_KEY_MISC_WONDER_GREAT_PEOPLE`。偉人の種類も出す。`TXT_KEY_MISC_WONDER_CULTURE`）。
- 説明文 `TXT_KEY_TRAIT_WONDER_GREAT_PEOPLE_TURNS`・`TXT_KEY_TRAIT_WONDER_PRODUCTION_CULTURE`。
- Python は `CvTraitInfo` の同名の getter
