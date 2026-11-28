# 志向のタグ（Python フック・条件）

志向の Python フック（`Py...`）と `CvHookUtils.py` の共通関数、志向の条件（`ConditionalFreeTraits`）。


## `TraitInfo/PyOnGameStart`、`PyOnBeginTurn`、`PyOnCityFounded`、`PyOnCityAcquired`、`PyOnCorporationFounded`、`PyOnCityLost`、`PyOnBuildingBuilt`、`PyOnReligionFounded`、`PyOnGained`、`PyOnLost`

- ファイル: `CIV4TraitInfos.xml`
- 志向の Python フック。
- Python の式を書き、`EntryPoints\CvTraitInterface.py` が評価する（呪文の `PyResult` と同じ方式）。
- その志向を持つプレイヤーごとに、志向の番号順に呼ぶ（`CvPlayer::doTraitPythonHook`）。
- `FreeTraits` で社会制度・建物・技術から与えた志向でも呼ばれる。
- 式で使える名前: `pPlayer`（CyPlayer）・`eTrait`、`PyOnCityFounded` / `PyOnCityAcquired` は `pCity`（CyCity）、`PyOnCityAcquired` はさらに `iOldOwner`・`bConquest`・`bTrade`（1 か 0）。
- 呼ぶ時: `PyOnGameStart` はゲーム開始時に 1 回（`CvGame::setInitialItems` で開始時の資金とユニットを配った後。シナリオは開始時の配布がないので呼ばない。開始時に持つ志向（初期の社会制度・技術の `FreeTraits` を含む）だけ）、`PyOnBeginTurn` はそのプレイヤーのターンの初め（`CvPlayer::doTurn` の `onBeginPlayerTurn` の直後）、`PyOnCityFounded` は都市を建設した後（`CvPlayer::found` の `onCityBuilt` の直後）、`PyOnCityAcquired` は都市を獲得した後（征服・取引・文化。`CvPlayer::acquireCity` の `onCityAcquired` の直後。征服した都市はその後で破壊されることがある）。
- `PyOnCorporationFounded`（2026-11-15 に追加）はそのプレイヤーが企業を設立した後（`CvGame::makeCorporationFounded`。本社の都市が初めて企業を持ったとき。ワールドビルダーとゲーム開始前は呼ばない）で、名前は `pPlayer`・`eTrait`・`pCity`（本社）・`eCorporation`。
- `PyOnCityLost`（2026-11-19 に追加）はそのプレイヤーが都市を失った後（征服・取引・文化。`CvPlayer::acquireCity` で新しい持ち主の `PyOnCityAcquired` の直後に、元の持ち主について呼ぶ。征服した都市を破壊するかはその後で決まるので、**破壊された都市でも呼ばれる**（都市が破壊されるのは占領された直後だけ）。元の持ち主が生きているときだけ）で、名前は `pPlayer`・`eTrait`・`pCity`（もう新しい持ち主の都市）・`iNewOwner`・`bConquest`・`bTrade`・`iPopulation`・`iCultureLevel`（失う前の人口と、元の持ち主にとっての文化レベル。征服で人口が 1 減る前の値）。
- 共通関数 `createPartisans(pPlayer, pCity, iNum, iCultureLevel)`（2026-11-19）: 首都で今徴兵できるユニット（`CyCity.getConscriptUnit`。徴兵の値が最大のもの）を iNum 体、都市の隣のタイルにランダムに置く（`isPartisanPlot`: 陸で、通行不能でも都市でもなく、そのプレイヤーと交戦中のユニットがいない。隣に 1 つもなければ 2 タイル先）。
- 文化レベルの段階まで戦闘術を与える（貧弱で戦闘術Ⅰ … 伝説でⅠ～Ⅵ。`PARTISAN_PROMOTIONS`）。
- 本人にメッセージ `TXT_KEY_MISC_PARTISANS_CREATED`。
- 首都・徴兵できるユニット・置けるタイルのどれかがなければ何もしない。
- 例: `bConquest and createPartisans(pPlayer, pCity, 2 + iPopulation, iCultureLevel)`（Python 2.4 は条件式がないので `and` で書く）。
- `PyOnBuildingBuilt`（2026-11-22 に追加。バビロニアの案）はそのプレイヤーの都市が生産で建物を完成した後（`CvCity::popOrder` で、建物の `PyOnBuilt` の直後、`onBuildingBuilt` の前。建物の `PyOnBuilt` と同じく、無料の建物・イベント・Python で置いた建物では呼ばない）で、名前は `pPlayer`・`eTrait`・`pCity`・`eBuilding`。
- 共通関数 `isBuildingClassIn(eBuilding, ['BUILDINGCLASS_GRANARY', ...])`（その建物の種類が一覧のどれか。固有建物も種類で判定する）。
- 例: `isBuildingClassIn(eBuilding, ['BUILDINGCLASS_GRANARY']) and pCity.changePopulation(1)`。
- `PyOnReligionFounded`（2026-11-22 に追加。ユスティニアヌス1世の案）はそのプレイヤーが宗教を創始した後（`CvPlayer::foundReligion` で聖都を決め、宗教の無料のユニット（BtS の宣教師 1 体）を配った後。技術で創始したとき、宗教を選ぶオプションで選んだとき、開始時代が遅いゲームで聖都のない宗教が割り当てられたとき（`CvGame::doHolyCity`。無料のユニットはない）も呼ぶ。ワールドビルダーとゲーム開始前は呼ばない）で、名前は `pPlayer`・`eTrait`・`pCity`（聖都）・`eReligion`。
- 共通関数 `createMissionaries(pPlayer, eReligion, iNum, pCity)`: その宗教を広める文明のユニット（宣教師）を都市に iNum 体作る（`createExecutives` の宗教版）。
- 例: `createMissionaries(pPlayer, eReligion, 2, pCity)`。
- `PyOnGained` / `PyOnLost`（2026-11-19 に追加。Mod Helper の考え方）は、その志向をゲーム中に得た・失ったとき（`CvPlayer::changeFreeTraitCount` で持つかどうかが変わったとき。条件つきの志向、社会制度・建物・技術の `FreeTraits`、期間つきの志向、Python）に、その志向について呼ぶ（`CvPlayer::doTraitGainedPythonHook`）。
- 名前は `pPlayer`・`eTrait`。
- ゲーム開始の時点で持っている志向（開始時に条件で得たものも。`isFinalInitialized` の前）では呼ばない。
- 指導者・文明の志向と同じ志向をほかから得ても、持つかどうかは変わらないので呼ばない。
- **期間つきの志向**（2026-11-19。DynTraits の考え方）: `CvPlayer::setTraitTurns(eTrait, iTurns)` でその志向を iTurns ターン持つ（0 で終わる）。
- 残りターンが 1 以上の間、`getFreeTraitCount` に 1 つ数えるので、ほかからも得ている志向は期間が終わっても残る。
- そのプレイヤーのターンの初め（`doTurn` の `updateConditionalTraits` の直後、`PyOnBeginTurn` の前）に 1 ずつ減る（`doTraitTurns`）。
- 得た・終わったとき本人にメッセージ（`TXT_KEY_MISC_TEMPORARY_TRAIT_GAINED` / `_ENDED`。持つかどうかが変わったときだけ）。
- ゲーム中の指導者の説明（`parsePlayerLeaderTraits`）では志向の名前の後に「（残り N ターン）」（`TXT_KEY_TRAIT_TURNS_LEFT`。exe が使う `parseTraits` は引数を変えられないので `parseTraitsWithTurns` を足した）。
- セーブに保存（`CvPlayer::m_paiTraitTurns`、`uiFlag=12`。古いセーブは期間つきの志向なし）。
- Python は `CyPlayer.getTraitTurns(eTrait)` / `setTraitTurns(eTrait, iTurns)`、共通関数 `gainTraitForTurns(pPlayer, 'TRAIT_X', iTurns)`（すでに期間つきなら長い方。重ねない。ゲーム速度で変えるなら `scaleBySpeed(iTurns, 'GoldenAge')`）。
- 永続的に与えるのは `gainTrait(pPlayer, 'TRAIT_X')`（`CyPlayer.changeFreeTraitCount(eTrait, 1)`。すでに持っていれば何もしない。与えたら `True`。2026-11-26）。
- 共通関数 `makeCityPlotHills(pCity, 'TERRAIN_PLAINS')`（2026-11-19。エチオピアの案）: 都市のタイルをその地形の丘陵にする（`PyOnCityFounded` から。`CyPlot.setPlotType`・`setTerrainType`。残せない地形の特徴は消す）。
- 共通関数 `buildRouteToCapital(pCity, 'ROUTE_ROAD', iMaxDistance)`（2026-11-23。Civ6 のローマの案）: 首都から陸路 iMaxDistance タイル以内でつながる都市なら、首都までその道を敷く（`PyOnCityFounded` から。技術は問わない）。
- 経路は同じ陸地の、通行不能でなく、持ち主がいないか自チームのタイルだけを通る、iMaxDistance タイル以内の経路のうち、労働者の経路のようにコストが最小のもの（Python のダイクストラ法。状態は (x, y, タイル数)。`isImprovementPassable` は見ない）。
- コストは `CvHookUtils.py` の定数 `ROUTE_COST_*`: 同等以上の道があるタイル 1、ないタイルは移動コスト（地形または地形の特徴 + 丘陵）× 10、川を渡ると +30、斜めに（川の角で）渡るとさらに +30（2026-11-23。最初はタイル数が最小の経路にしていたが、川沿いで川を何度もまたぐ道になり見た目が不自然だった）。
- 同等以上の道（`CvRouteInfo` の `iValue`。鉄道は道路で置き換えない）があるタイルと都市のタイルはそのまま。
- 道を敷いたタイル数を返す（経路がなければ -1）。
- 例: `buildRouteToCapital(pCity, 'ROUTE_ROAD', 15)`。
- 共通関数 `createCityConscripts(pPlayer, iMinPopulation)`（2026-11-19）: 首都と人口 iMinPopulation 以上の都市に、その都市で今徴兵できるユニットを 1 体ずつ出し、本人に数のメッセージ（`TXT_KEY_MISC_CITY_CONSCRIPTS_CREATED`）。
- 共通関数 `createConscripts(pPlayer, pCity, iNum)`（2026-11-28。オスマンのデヴシルメ）: その都市で今徴兵できるユニットを都市に iNum 体出す（メッセージなし）。出した数を返す（徴兵できるユニットがなければ 0）。
- 共通関数 `countCityReligions(pCity, eExceptReligion)`（2026-11-28）: 都市にある宗教の数。`eExceptReligion` は数えない（省略か -1 ならすべて数える。`pPlayer.getStateReligion()` を渡すと、国教がないときはすべて数える）。
- 例: `bConquest and createConscripts(pPlayer, pCity, 1 + countCityReligions(pCity, pPlayer.getStateReligion()))`。
- 共通関数 `isSpecialBuildingIn(eBuilding, ['SPECIALBUILDING_MONASTERY'])`（2026-11-28。ユスティニアヌス1世）: その建物の特別な分類が一覧のどれか（すべての宗教の僧院など）。宗教は `gc.getBuildingInfo(eBuilding).getReligionType()`。
- 共通関数 `extendGoldenAge(pPlayer, eTrait, iTurns)`（2026-11-28。ペリクレス）: 黄金時代のあいだなら iTurns ターン延ばす。1 回の黄金時代で延ばす合計は通常の黄金時代の長さ（`CyPlayer.getGoldenAgeLength`）まで。延ばした合計は志向のカウンターに入れるので、`<PyOnGoldenAgeStart>pPlayer.setTraitCounter(eTrait, 0)</PyOnGoldenAgeStart>` で 0 に戻す。延ばしたら本人にメッセージ（`TXT_KEY_MISC_GOLDEN_AGE_EXTENDED`。延びたターン数。都市の偉人はプレイヤーのターンの終わりの、黄金時代の残りを 1 減らす前に生まれるので、画面の残りターンは減らずに同じ数のままになる）。延ばしたターン数を返す。
- 1 つのフックで 2 つのことをするには、タプルで書く（`(addBuilding(pCity, 'BUILDINGCLASS_WALLS'), gainTraitForTurns(pPlayer, 'TRAIT_X', 3))`）。
- どれも `CvEventManager` のイベントと同じく全マシンで呼ばれるので、乱数は `getSorenRandNum`。
- `CvTraitInterface.py` の共通関数: 値 `scaleBySpeed(iValue, 'Train')`（ゲーム速度の `iTrainPercent` などで掛ける）・`startEra()`・`isEveryTurns(iTurns)`・`capitalPlot(pPlayer)`、効果 `gainGold`・`createUnits(pPlayer, 'UNITCLASS_X', iNum, pPlot)`（文明のユニット）・`addBuilding(pCity, 'BUILDINGCLASS_X')`（文明の建物）。
- 2026-11-15 に、都市の判定 `isCoastalCity(pCity)`（海に面する）・`isOverseasCity(pCity)`・`isNewWorldCity(pCity)` と、`createExecutives(pPlayer, eCorporation, iNum, pCity)`（その企業を広める文明のユニット）を足した。
- 効果の説明は志向の `Help` に書く（志向の説明文に出る。BtS の志向のテキストと同じく `[NEWLINE][SPACE][SPACE][ICON_BULLET]` で始める）。
- キャッシュなし（`CvTraitInfo` はストリーム版の `read` / `write` を持たない）。
- 共通の関数は 2026-11-14 に `CvHookUtils.py` へ移した（`CvTraitInterface.py`・`CvHookInterface.py`・`CvSpellInterface.py` が import する）。
- **2026-11-23 に追加したフック**（どれもゲーム開始前とワールドビルダーでは呼ばない）: `PyOnTechAcquired` はチームが技術を得た後（`CvTeam::setHasTech` で技術の `PyOnDiscovered` と同じ場所。研究・取引・遺跡・諜報・Python と経路を問わない。開始時の技術では呼ばない）で、名前は `pPlayer`・`eTrait`・`eTech`・`bFirst`（そのチームが最初に得た技術なら 1）。
- `PyOnGreatPersonBorn` は偉人が生まれた後（`CvPlayer::createGreatPeople` の `onGreatPersonBorn` の後。戦闘で生まれた偉大な将軍のように都市がないときも呼ぶ）で、名前は `pPlayer`・`eTrait`・`pUnit`（CyUnit）・`pCity`（都市がなければ `pCity.isNone()` が真）。
- `PyOnGoldenAgeStart` / `PyOnGoldenAgeEnd` は黄金時代が始まった・終わったとき（`CvPlayer::changeGoldenAgeTurns` で黄金時代かどうかが変わったとき。延長では呼ばない）で、名前は `pPlayer`・`eTrait`。
- `PyOnWarDeclared` は戦争が始まった後（`CvTeam::declareWar` の `changeWar` の後。`CvTeam::doWarPeaceTraitPythonHooks`）に両方のチームのプレイヤーについて呼び、名前は `pPlayer`・`eTrait`・`iOtherTeam`・`bAggressor`（宣戦した側なら 1。防衛協定や属国で参戦したチームも宣戦した側になる）。
- `PyOnPeaceMade` は講和した後（`CvTeam::makePeace`）に両方のチームのプレイヤーについて呼び、名前は `pPlayer`・`eTrait`・`iOtherTeam`。
- `PyOnEraChanged` は新しい時代に入った後（`CvPlayer::setCurrentEra`。後の時代の技術を得ると時代を飛ばすことがある。開始時の時代では呼ばない）で、名前は `pPlayer`・`eTrait`・`eEra`・`iOldEra`。
- DLL では、引数が整数だけのフックは `CvPlayer::doTraitPythonHookInts`、共通部分は `callTraitPythonHook`（ユニット・都市・整数の並び）。
- **志向のカウンター**（2026-11-23）: プレイヤーごと・志向ごとの整数（`CvPlayer::m_paiTraitCounter`、セーブに保存、`uiFlag=19`。古いセーブは 0）。
- DLL は使わず、フックの式で数えるためのもの。
- 志向を失っても残る。
- 同じ志向のフックは 1 つのカウンターを共有する。
- Python は `CyPlayer.getTraitCounter(eTrait)` / `setTraitCounter(eTrait, iValue)` / `changeTraitCounter(eTrait, iChange)`、共通関数 `addTraitCounter(pPlayer, eTrait, iChange)`（足して新しい値を返す。`eTrait` は志向の名前の文字列でもよい）・`countEvery(pPlayer, eTrait, iEvery)`（1 足して iEvery 回目に真を返し、0 に戻す）。
- 共通関数 `isUnitClassIn(pUnit, ['UNITCLASS_GREAT_GENERAL'])`・`isTechIn(eTech, ['TECH_WRITING'])` も足した。
- **2026-11-23 に追加したフック（2 回目）**（どれもゲーム開始前とワールドビルダーでは呼ばない）: `PyOnFirstContact` はそのプレイヤーのチームがほかのチームと初めて出会った後（`CvTeam::makeHasMet` の `firstContact` の後。ユニットでも連絡先の取引でも。蛮族は除く）で、名前は `pPlayer`・`eTrait`・`iOtherTeam`。
- `PyOnProjectBuilt` はそのプレイヤーの都市がプロジェクトを完成した後（`CvCity::popOrder` で `changeProjectCount` の後）で、名前は `pPlayer`・`eTrait`・`pCity`・`eProject`。
- `PyOnVassalGained` はほかのチームがそのプレイヤーのチームの属国になった後（`CvTeam::setVassal` の `vassalState` の後）で、名前は `pPlayer`・`eTrait`・`iVassalTeam`・`bCapitulated`（降伏による属国なら 1）。
- `PyOnReligionSpread` はそのプレイヤーの都市に宗教が広まった後（`CvCity::setHasReligion` の `religionSpread` の後。自然な伝播・宣教師・聖都・イベントなど、告知するときだけ。都市の持ち主が変わって引き継ぐ宗教では呼ばない）で、名前は `pPlayer`・`eTrait`・`pCity`・`eReligion`。
- `PyOnCivicChanged` はそのプレイヤーが社会制度を変えた後（`CvPlayer::setCivics` で社会制度の `PyOnAdopt` の後。変わった枠ごとに 1 回）で、名前は `pPlayer`・`eTrait`・`eCivic`・`iOldCivic`（`CivicOptionExtraSlots` の空いた枠は -1）。
- `PyOnCircumnavigated`（2026-11-23 に追加）はそのプレイヤーのチームが世界一周を達成した後（`CvTeam::testCircumnavigated`。1 番目は船の移動力のボーナスを得た後。ゲーム開始時の達成では呼ばない）で、名前は `pPlayer`・`eTrait`・`iOrder`（1 番目なら 1、2 番目なら 2…）。
- このため世界一周の判定を**チームごと**にしたが、ゲームのルールは元のまま: 移動力 +`CIRCUMNAVIGATE_FREE_MOVES` と通知は 1 番目のチームだけで、2 番目以降のチームは志向のフックの効果だけを得る（通知もない）。
- 順位は `CvTeam::m_iCircumnavigatedOrder`（セーブする。`CvTeam` の `uiFlag` 1。Python は `CyTeam.getCircumnavigatedOrder()`）。
- 共通関数 `gainCircumnavigationMoves(pPlayer)`: そのプレイヤーのチームが 1 番目と同じ移動力のボーナスと通知（`TXT_KEY_MISC_YOU_CIRC_GLOBE`）を得る。
- 例: `iOrder &gt; 1 and gainCircumnavigationMoves(pPlayer)`（XML では `&gt;`）。
- プレイヤーごとに呼ばれるので、この志向を持つ 2 人のチームでは 2 回得る。
- 地図の条件は `CvGame::isCircumnavigationMap()` に分け、`circumnavigationAvailable()`（AI が狙うかの判定）は「まだ誰も達成していない」のまま。
- 変更前のセーブで達成済みのゲームでは、誰が 1 番目か分からないので 2 番目以降は起きない。
- 共通関数 `startGoldenAge(pPlayer)`: 通常の長さの黄金期を始める（進行中なら延びる。偉人は消費しない。長さの % は `PyOnPlayerEliminated` の項）。
- 例: `iOrder == 1 and startGoldenAge(pPlayer)`。
- 共通関数 `isCivicIn(eCivic, ['CIVIC_REPRESENTATION'])` も足した。
- 共通関数 `showMessage(pPlayer, 'TXT_KEY_X', (引数, ...))`（そのプレイヤーのイベントログにテキストを出す。引数はテキストの %s1, %s2...）・`infoDescription('Tech', eTech)`（Info の名前。-1 なら '-'）・`cityName(pCity)`（都市がなければ '-'）も足した

## `TraitInfo/ConditionalFreeTraits`

- ファイル: `CIV4TraitInfos.xml`
- `<ConditionalFreeTrait>` を何個でも書ける。
- 各項目は `TraitType`（与える志向）と条件（どれも省略可。書いたものすべてを満たすとき）:
  - `bAtWar`（蛮族・小文明以外のどこかと交戦中。チーム単位）
  - `bAtPeace`（交戦していない）
  - `bDefensiveWar`（自分のチームに宣戦してきた、蛮族・小文明以外のチームと交戦中。宣戦した側の記録 `CvTeam::isAtDefensiveWar` は `docs/tags/trait-diplomacy.md` の `iDenounceAttitudeChange`。2026-11-28、チャーチル用）
  - `StateReligion`（その宗教が国教）
  - `bNoStateReligion`（国教なし）
  - `EraMin` / `EraMax`（そのプレイヤーの今の時代が範囲内）
  - `PrereqCivic`（その社会制度を採用中）
  - `bFavoriteCivic`（指導者の `FavoriteCivic` を採用中。`ReplacedCivic` で置き換えた制度も採用中として扱う。AI の性格ではなくプレイヤーの指導者で引くので、性格ランダムのオプションでも変わらない。制度をやめると外れる。説明文には、読んでいるプレイヤーがその志向を持っていれば、その指導者の制度名も出す。社会制度の画面では、その制度の名前を黄色にし、説明欄に `TXT_KEY_CIVIC_FAVORITE_GIVES_TRAIT` を出す（`CvPlayer::getFavoriteCivicTraits`・`isFavoriteCivicWithTraits`。同じ項目のほかの条件は見ない））
  - `iMinUnitsKilled`（ゲーム全体で倒した敵ユニットの累計が○体以上。蛮族・動物も数える。倒した側のプレイヤーに数え、輸送中のユニットも 1 体ずつ数える。捕獲は数えない。`CvUnit::kill`）
  - `iMinCitiesConquered`（占領した都市の累計が○個以上。`acquireCity` の `bConquest` で数えるので、奪い返しも破壊した都市も数え、文化による転向と取引は数えない。解放で元の持ち主に戻った場合は受け取った側に数える）。
- 累計は減らないので、この 2 つの条件だけの志向は一度得ると失わない（成長する志向）。
- 累計は `CvPlayer::m_iNumUnitsKilled` / `m_iNumCitiesConquered`（セーブに保存、`CvPlayer` の `uiFlag=7`。古いセーブは 0 から数える）。
- 条件を満たした瞬間に志向を与え、本人にだけメッセージ `TXT_KEY_MISC_TRAIT_GAINED_BY_RECORD` を出す。
- この志向を持つ間、条件を満たす項目の志向を与える（ex 氏の CivGoldEmpires の双頭志向・転生志向・五賢帝を XML で書くためのもの）。
- 与えた数は `CvPlayer::m_paiConditionalTraitCount` に持ち（セーブに保存、`CvPlayer` の `uiFlag=4`）、`changeFreeTraitCount` で差分だけ足し引きする（`CvPlayer::updateConditionalTraits`）。
- 計算し直すのは、交戦状態の変化（`CvTeam::setAtWar`。宣戦・和平）、戦績の増加（`changeNumUnitsKilled`・`changeNumCitiesConquered`）、国教の変更（`setLastStateReligion`）、時代の変化（`setCurrentEra`）、社会制度の変更（`setCivics`。国教の有無もここで変わる）、`ConditionalFreeTraits` を持つ志向を得た・失ったとき、ゲーム開始時（`PyOnGameStart` の前）、ターンの初め（`PyOnBeginTurn` の前。無政府期間などほかの経路の変化もここで反映）。
- 与えた志向がさらに `ConditionalFreeTraits` を持っていてもよい（変化がなくなるまで繰り返す）。
- ただし輪（A が B を、B が A を与える。ほかの志向を経由する場合と、自分自身を与える場合も）にすると、一度得た志向が互いを与え合い、条件が外れても残る（`tools\check_refs.ps1` が警告する）。
- 既存のセーブでは、次のターンの初めに与える。
- 説明文は `TXT_KEY_TRAIT_CONDITIONAL_FREE_TRAIT`（「攻撃志向を得る（戦時）」「組織志向を得る（平時、検証用社会制度を採用）」。志向・宗教・社会制度はリンク）で、条件は `TXT_KEY_CONDITION_*`。
- 戦績の条件は、読んでいるプレイヤーがその志向を持っていれば現在の数も出す。
- `PrereqCivic` の項目は社会制度の説明にも出す（`TXT_KEY_CIVIC_CONDITIONAL_FREE_TRAIT` / `_IF`。「検証用志向があれば検証用志向（蛮族）を得る（平時）」。置き換えた制度の説明にも出す。ゲーム中は操作中のプレイヤーの志向の分だけ、ペディアでは全部）。
- 条件の文は `getConditionalFreeTraitConditions` で作る。
- 読み込みは、この志向より後の志向と社会制度を参照するので種類を文字列のまま持ち、`CvTraitInfo::readPass3()` で番号に変える（このため `readPass3()` を呼ぶ位置を社会制度の読み込みの後に移した）。
- Python は `CyPlayer.getConditionalTraitCount(eTrait)` / `updateConditionalTraits()` / `getNumUnitsKilled()` / `getNumCitiesConquered()`、`CvTraitInfo.getNumConditionalFreeTraits()` / `getConditionalFreeTraitType(i)`。
- `PrereqTech`（チームがその技術を持っている。技術を得たとき `CvTeam::setHasTech` で更新。RFC のドイツ「産業主義の後」のような、技術で目覚める志向）。
- `bGoldenAge` / `bNoGoldenAge`（黄金時代中 / 黄金時代でない。`changeGoldenAgeTurns` で始まり・終わりに更新）、`bAnarchy` / `bNoAnarchy`（無政府状態中 / 無政府状態でない。`changeAnarchyTurns`）、`iMinCities` / `iMaxCities`（都市数が○以上 / ○以下。0 は制限なし。都市の建設（`found`。`PyOnCityFounded`・`CityFoundedUnits` の前）、取得（`acquireCity`。新旧の持ち主。`PyOnCityAcquired` の前）、破壊（`raze`）で更新）、`PrereqBonus`（その資源を使える。首都の交易網の資源で、交易画面と同じ `getNumAvailableBonuses`。ターンの初めにだけ更新するので、反映は最大 1 ターン遅れる）、`PrereqBuildingClass`（その建物クラスの建物をどこかの都市に持つ。陳腐化していても数える `getBuildingClassCount`。生産で完成したとき（`CvCity::popOrder`）と都市の取得・破壊で更新し、無料の建物・イベント・Python で置いた建物はターンの初めに反映）、`bVassal` / `bHasVassal`（チームが属国である / 属国を持つ。`CvTeam::setVassal` で両方のチームのプレイヤーを更新。人間のプレイヤーが属国になるには志向の `bHumanCanBeVassal` が要る）、`iMinScoreRank`（スコアの順位が○位以下。1 が首位。順位は `CvGame::updateScore` のものなので、ターンの初めにだけ更新）。
- 説明文の条件は `TXT_KEY_CONDITION_GOLDEN_AGE` など（都市数は `_CITIES_RANGE` / `_MIN_CITIES` / `_MAX_CITIES`）。
- 都市数とスコアの順位は、読んでいるプレイヤーがその志向を持っていれば現在の値も出す。
- 建物は建物クラスの既定の建物の名前（リンク）。
- 資源と建物クラスは `readPass3()` で番号に変える（1 項目につき 7 個の種類を文字列で持つ）。
- 志向そのものが条件を変える組み合わせ（例: 都市数で得た志向が都市数を変える）は付け外しを繰り返しうるが、判定するのは上の時点とターンの初めだけなので、止まらなくなることはない。
- `PyCondition`（Python の式。ほかの条件をすべて満たすときに最後に評価し、真なら満たす。`CvHookInterface.py` の `isConditionTrue` が `pPlayer`・`eTrait`（与える志向）で `eval` する。ゲームを変えてはいけない。判定し直すのは上の時点とターンの初めなので、資金や人口の条件は次のターンに反映）と`PyConditionText`（説明文に出す条件のテキストキー。省略すると `TXT_KEY_CONDITION_PYTHON`「特別な条件」）

## `chooseGreatPeopleByCalendar(pPlayer, eTrait, szTech, iCycleYears, iCycleEndYear, szTechTrait)`（`PyOnBeginTurn` に書く共通関数）

- ファイル: `CvHookUtils.py`
- Civ5 のマヤの長期暦（2026-11-23）。
- チームが `szTech` を持っていれば（`None` なら最初から）、暦の周期が終わるたびに偉人を 1 人選び、首都に出す。
- 周期は `iCycleYears` 年で、`iCycleEndYear` 年に 1 つ終わり、その前後も `iCycleYears` 年ごとに終わる（`calendarCyclesEnded`。フックはゲームのターンが進む前に呼ばれるので、次のターンの年 `getTurnYear(iTurn + 1)` と今のターンの年をはさむ区切りを数える。暦はゲーム速度ごとに年の進み方が違うので、速度の補正は要らない）。
- 選べるのは、既定のユニットが偉人として都市に定住できる（`GreatPeoples` を持つ）ユニットクラス（`greatPeopleUnitClasses`。BtS では預言者・芸術家・科学者・商人・技術者・大将軍・大スパイの 7 種）のうち、その文明にユニットがあるもの。
- 文明の固有ユニットで出す。
- 一度選んだものは、全員を選ぶまで選べず、全員を選ぶとまた全員を選べる（`greatPersonChoices`）。
- 人間は選択の画面（`BUTTONPOPUP_PYTHON`。Esc では閉じられない。ボタンはユニットの名前とアイコン。ボタンの関数 `chooseGreatPersonCallback` は、exe が `EntryPoints` のモジュールからしか呼べない（`Can't find module CvHookUtils` になった）ので、`setPythonModule("CvTraitInterface")` で呼ぶ。`CvTraitInterface` は `CvHookUtils` を `import *` している）で選び、選んだ結果は `CyMessageControl().sendModNetMessage`（`MOD_NET_MESSAGE_CHOOSE_GREAT_PERSON` = -20261123。DLL の bChooseGoody の -20261110 と重ならない）から `CvEventManager.onModNetMessage` → `receiveModNetMessage` で全員のコンピューターに届き、そこでユニットを出す（マルチプレイでずれない）。
- AI は `getSorenRandNum` で選ぶ。
- 状態は志向のカウンターに持つ（下位 24 ビットが選んだ偉人、上位が未選択の数。`greatPersonChoiceState`）。
- 首都がないときや、選ぶ前にロードして画面が消えたとき（選択の画面はセーブされない）は未選択のまま残り、次のターンの初めにもう一度画面を出す。
- 未選択が 2 つ以上なら、1 つ選ぶたびに次の画面を出す。
- 出したときに本人にメッセージ `TXT_KEY_MISC_CALENDAR_GREAT_PERSON`（首都の位置。偉人の誕生の音）。
- 画面の文 `TXT_KEY_POPUP_CHOOSE_GREAT_PERSON`。
- 説明文は自動で出ないので、志向の `Help` に書く。
- 省略できる `szTechTrait`（2026-11-26）: チームが `szTech` を持っていれば、その志向を永続的に与える（`gainTrait`）。`bLongCountCalendar` の子の志向を書けば、偉人と同じ技術で暦が切り替わる（技術名を 1 か所に書くため。`ConditionalFreeTraits` の `PrereqTech` を使わない）。
- 与えるのはフックが呼ばれるターンの初めなので、技術を得た次のターンの初めになる。
- 周期が終わったターンは周期の数を返す（式で `and` につなげられる）

- `PyOnOtherReligionFounded`（2026-11-28）は `docs/tags/trait-religion.md`。

## `TraitInfo/PyOnOtherCityFounded`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-24（トルデシリャス条約の案）。
- **ほかのプレイヤーが**都市を建設した後（`CvPlayer::found` で、建てたプレイヤーの `PyOnCityFounded` の直後）に、生きているほかの全プレイヤー（蛮族も含む）について呼ぶ。
- 名前は `pPlayer`（志向の持ち主）・`eTrait`・`pCity`（建てたプレイヤーの都市。建てた人は `pCity.getOwner()`）。
- DLL は `CvPlayer::doTraitPythonHook(TRAIT_PYHOOK_OTHER_CITY_FOUNDED, pCity)`

## 優先地域（`chooseTreatyZone`・`treatyZoneTurn`・`offerTreatyTerms`）

- ファイル: `CvHookUtils.py`
- 2026-11-24（トルデシリャス条約の案）。
- **地域**: `chooseTreatyZone(pPlayer, eTrait)`（`PyOnProjectBuilt` などに 1 回）で、その時点の首都の x を線として固定し、地図の西半分か東半分を優先地域に選ぶ。
- 人間はポップアップ（`TXT_KEY_POPUP_CHOOSE_TREATY_ZONE`。ModNetMessage `-20261124` で全員に送る）、AI は陸のタイルが多い側（同数なら西）。
- 選ぶと全員にメッセージ（`TXT_KEY_MISC_TREATY_ZONE_CHOSEN`）。
- 首都が移っても線は動かない。
- 東西につながった地図では、線の列と地球の反対側の列は両方に入り、残りを半分ずつ（`isInTreatyZone`）。
- つながっていない地図では線の位置で分ける（均等にならない）。
- 状態は志向のカウンター（側 + 4 × 線の x。0 = 未設定、3 = 選択待ち）。
- 選択待ちのままロードしたら、`treatyZoneTurn` が次のターンにもう一度出す。
- **収入**: `treatyZoneTurn(pPlayer, eTrait, iGoldPerCity, szSource)`（`PyOnBeginTurn`）で、優先地域にあるほかのプレイヤー（蛮族を除く）の都市 1 つにつき金 iGoldPerCity。
- 金を得たターンは本人に「<szSource の名前>より[金]N を取得。」（`TXT_KEY_MISC_TREATY_ZONE_INCOME`。szSource は `'PROJECT_TORDESILLAS'` のような種類の名前で、接頭辞でプロジェクト・建物・志向・社会制度・技術を見分ける。省略すると志向の名前）。
- いつ誰が建てた都市でも、占領・譲渡で持ち主が変わった都市でも、そのときの状態で数える。
- 都市の数を返す。
- **条件**: `offerTreatyTerms(pPlayer, eTrait, pCity, iGold, 'BUILDINGCLASS_X')`（`PyOnOtherCityFounded`）で、ほかのプレイヤー（蛮族・同じチームを除く）が優先地域に都市を建てたら、建てた側が 1 つ選ぶ: iGold ゴールドを払う（それだけの資金があるときだけ）、建てた側のチームが持ち志向の持ち主のチームが持たない技術をランダムに 1 つ渡す（今研究できるものがあればその中から）、その都市にその種類の建物（既定の建物）が建つ。
- 人間はポップアップ（`TXT_KEY_POPUP_TREATY_TERMS`。ModNetMessage `-20261127`）、AI はランダム。
- 結果は両方にメッセージ（`TXT_KEY_MISC_TREATY_TERMS_*`）。
- ポップアップに答える前にロードすると、条件はなくなる（もう一度は出ない）。
- **色分け**: 開拓者など都市を建てられるユニットを選んでいる間、優先地域の見えている陸のタイルを地域の持ち主のメインの色で塗る（`updateTreatyZonePlots`。`CvGameUtils.updateColoredPlots` から。DLL が消す `PLOT_LANDSCAPE_LAYER_BASE` に描くので選択を外すと消える）。
- 対象は `PyOnBeginTurn` に `treatyZoneTurn` を書いた志向

## `TraitInfo/bLongCountCalendar`

- ファイル: `CIV4TraitInfos.xml`
- Civ5 のマヤ（2026-11-23）。
- そのプレイヤーの画面の右上の日付を、年の代わりにマヤの長期暦（`長期暦 13.0.0.0.0`。バクトゥン.カトゥン.トゥン.ウィナル.キン）で出す（`CvGameTextMgr::setInterfaceTime` → `setLongCountStr`。exe と `CvMainInterface.py` の `getInterfaceTimeStr` が使う）。
- 西暦 2012 年 12 月 21 日を 13.0.0.0.0 とし（GMT 相関）、ターンの月（`getTurnMonthForGame`。1 か月 = 365.2425 / 12 日）から日数を数える。
- 紀元前 3114 年 8 月 11 日の基準日より前は、その前の周期（同じく 13.0.0.0.0 で終わる）で数える。
- ゲームの暦は細かくても月までなので、下の 2 桁はおおよその値。
- 暦がターン数（`CALENDAR_TURNS`）のときは変えない。
- 日付にマウスを重ねると、ゲームの暦の日付（「紀元前3760年」など。`setTimeStr`）が出る（`CvMainInterface.py` の `TimeText` の 2 か所を `WIDGET_GENERAL` の `iData1` = -20261124（`CvDefines.h` の `WIDGET_DATA_INTERFACE_TIME`）にし、`CvDLLWidgetData::parseHelp` の `WIDGET_GENERAL` で出す。BtS ではこの部品に説明がなかった。長期暦でないプレイヤーには何も出さない）。
- イベントログ・リプレイ・セーブ名・勝利の画面などの日付は西暦のまま。
- 説明文 `TXT_KEY_TRAIT_LONG_COUNT_CALENDAR`、日付 `TXT_KEY_TIME_LONG_COUNT`。
- Python: `CyPlayer.isLongCountCalendar()`、`CvTraitInfo.isLongCountCalendar()`。
- 偉人の選択（`chooseGreatPeopleByCalendar`）とは別のタグなので、片方だけでも使える
- 偉人の解禁と同じ技術で暦を切り替えるには、このタグを子の志向に持たせ、`chooseGreatPeopleByCalendar` の `szTechTrait` でその志向を与える

## `TraitInfo/PyOnPlayerEliminated`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-26（大キュロスの案）。
- そのプレイヤーが最後の都市を占領した相手が滅亡した後（`CvPlayer::setAlive(false)`。`bGainTraitsOnElimination` と同じく `m_eLastConqueror` で判定し、その処理の後）に呼ぶ。
- 名前は `pPlayer`（志向の持ち主）・`eTrait`・`iEliminatedPlayer`。
- 占領した相手の最後の都市がほかの理由（文化による寝返り・取引など）で失われたときも、最後に都市を占領したプレイヤーに呼ぶ。
- 「完全な征服」のオプションでは、ユニットもすべて失って滅亡したときに呼ぶ。
- 最後の都市の占領では、先に `PyOnCityAcquired` が呼ばれる（両方に書けば両方の効果が起きる）。
- DLL は `doTraitPythonHookInts(TRAIT_PYHOOK_PLAYER_ELIMINATED, 1, eEliminated)`。
- 共通関数 `startGoldenAge(pPlayer, iPercent = 100)`（2026-11-26 に `iPercent` を追加）: 通常の長さ（`getGoldenAgeLength`。ゲーム速度などの補正込み）の iPercent % の黄金期を始める。最低 1 ターン。
- 進行中なら延びる（`PyOnGoldenAgeStart` は呼ばれない）。
- 偉人を消費した回数に数えないので、次に必要な偉人の数は増えない

## `TraitInfo/PyOnImprovementChanged`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-26（毛沢東の大躍進の案）。
- そのプレイヤーのタイルの施設が変わった後に呼ぶ（`CvPlot::setImprovementType`。作業員の建設、小屋の成長、略奪、イベントなど）。
- 持ち主のいないタイルと、ゲームの初期化が終わる前（`isFinalInitialized`）は呼ばない。
- 古い施設の文化で国境が変わる処理より前に呼ぶので、持ち主は変わる前のプレイヤー。
- 名前は `pPlayer`・`eTrait`・`pPlot`（`CyPlot`）・`eOld`・`eNew`（施設の番号。なしは -1）。
- DLL はタイルを座標で渡し、`CvTraitInterface.py` の `onImprovementChanged` が `CyMap().plot` で `pPlot` にする（`CvPlayer::doImprovementChangedPythonHook`）。
- 共通関数:
  - `improvementReplacedValue(eOld, eNew, szNew, dValues)`: 新しい施設が `szNew` なら、辞書 `dValues`（施設の種類 → 数）の古い施設の値。ほかは 0
  - `gainProduction(pCity, iProduction)`: 都市がいま作っているもの（ユニット・建物・プロジェクト）に生産を足す。生産の種類や作るものがないとき、`pCity.isNone()` のときは何もしない
  - `gainImprovementProduction(pCity, iProduction)`: 伐採の生産と同じく都市に貯め、次の生産に生産の補正込みで足す（生産の種類の間は残る）。都市画面の生産の内訳に「施設の転換により：30」の行を出す（`CvCity::m_iImprovementProduction`、`TXT_KEY_MISC_HELP_PROD_IMPROVEMENTS`。`CvCity` の `uiFlag` を 7 にした。Python は `CyCity.getImprovementProduction()`・`changeImprovementProduction(iChange)`）
  - `greatMerchantTradeGold(iTradeProfit)`: 大商人（`UNITCLASS_MERCHANT` の標準ユニット）の交易任務の金。`(iBaseTrade + iTradeMultiplier × iTradeProfit) × iUnitTradePercent / 100`（`CvUnit::getTradeGold` と同じ）。`iTradeProfit` は行き先の交易の利益の代わりの値（5 で外国の人口 12 程度の都市）
- 例（農地を工房に替えると、そのタイルの都市に生産 30 × 速度）:
  - `gainImprovementProduction(pPlot.getWorkingCity(), improvementReplacedValue(eOld, eNew, 'IMPROVEMENT_WORKSHOP', {'IMPROVEMENT_FARM': scaleBySpeed(30, 'Train')}))`

## `gainExperience(pUnit, iExperience)`、`keepExperienceOnUpgrade(pUnit)`（共通関数）

- ファイル: `CvHookUtils.py`
- 2026-11-26（シャルルマーニュの聖騎士の案）。ユニットの `PyOnCreated` などから使う。
- `gainExperience`: ユニットの経験値 +iExperience（`CyUnit.changeExperience`。戦闘の経験値ではないので、経験値の補正・大将軍ポイント・上限なし）。
- `keepExperienceOnUpgrade`: ユニットを、自分と同じ種類の大将軍が合流したユニットとして扱う（`CyUnit.setLeaderUnitType`）。
- BtS はアップグレードのとき、大将軍が合流していないユニットの経験値を `MAX_EXPERIENCE_AFTER_UPGRADE`（10）で切り捨てる（`CvUnit::upgrade`）。この関数を使ったユニットは切り捨てられない。
- アップグレードした後のユニットも同じ扱い（`CvUnit::convert` が引き継ぐ）。
- 大将軍が合流したユニットと同じく、倒されると全プレイヤーに「将軍が戦死」のメッセージ（`TXT_KEY_MISC_GENERAL_KILLED`）が出て、戦闘の演出と守るユニットの選択（同じ強さなら合流したユニットが後）も変わる。
- 大将軍の昇進（`PROMOTION_LEADER`）は付けない（必要ならユニットの `FreePromotions` で付ける。BtS の大将軍の昇進は `iUpgradeDiscount` 100 で、アップグレードが無料になる）
