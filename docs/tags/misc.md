# ゲームオプション・タグのない機能・BtS 本体の修正など

## ゲームオプション

| ファイル | タグ | 内容 |
|---|---|---|
| `CIV4GameOptionInfos.xml` | `GAMEOPTION_MULTIPLE_PRODUCTION`（ゲームオプション） | マルチプルプロダクション（denev 氏の Multiple Production Mod のルールを参考に実装）。完成した項目の余りで、キューの次の項目も同じターンに完成させる（`CvCity::doMultipleProduction`）。遺産（世界・チーム・国家）とプロジェクトは対象外（先に積み立てる不正を防ぐ）。食料を使うユニットも、最初の完成品がそのユニットでなければ対象外。そこで止まった余りは次のターンへ繰り越す。AI と自動の都市は、キューが空になると次の項目を選んで続ける。繰り越しの上限は BtS と同じ（品物の費用か、そのターンの生産力の大きい方）で、超えた分は `MAXED_*_GOLD_PERCENT` で金に変わる（Unofficial Patch の計算で、オプションによらない。その品物だけに効く生産力の補正の分は金にしない。BtS の計算は上限を掛けたあとの値から求めるのでほとんど金にならなかった）。オプションが有効なときはメッセージ `TXT_KEY_MISC_LOST_PROD_CONVERTED` を出す（`doOverflowGold`）。オプションの数が変わったので `CvInitCore` の `uiSaveFlag` を 2 に上げ、古いセーブでは無効として読む |
| `CIV4GameOptionInfos.xml` | `GAMEOPTION_ALWAYS_WAR_AI`（ゲームオプション「AI同士の戦争」） | 本体の「戦争」（`GAMEOPTION_ALWAYS_WAR`）は人間と AI のチームを常に戦争にするが、こちらは **AI のチームどうし**を常に戦争にする。人間のプレイヤーを含むチーム、蛮族、小文明、属国（宗主国に従う）は対象外（`CvGame::isAlwaysWarAI`）。出会ったときに宣戦し（`CvTeam::meet`）、毎ターンの `CvGame::updateWar` でも、出会い済みで戦争していない組（独立した属国など）に宣戦する。出会っていない相手とは戦争にならない（本体の「戦争」と同じ）。和平できない（`CvTeam::canChangeWarPeace`。戦争中で和平できない相手とは交渉もできない `CvPlayer::canContact`）。国連の停戦決議でも和平しない（`CvPlayer::forcePeace`）。AI の厭戦感情は本体の「戦争」と同じ補正（`FORCED_WAR_WAR_WEARINESS_MODIFIER`、平時と同じ減衰）。「平和」（`GAMEOPTION_ALWAYS_PEACE`）と両方選ぶと「平和」が優先。「戦争」と両方選ぶと全員が常に戦争。AI 観戦モードと組み合わせると、AI どうしの戦争を眺められる。セーブの形式: `CvInitCore` の `uiSaveFlag=3`（古いセーブではオフ） |

## タグのない機能

| ファイル | タグ | 内容 |
|---|---|---|
| （タグなし） | AI 観戦 | ゲーム中に Ctrl+Shift+O で確認のポップアップを出し、「はい」で始める（シングルプレイのみ。元に戻せない）。操作中の文明は都市とユニットをすべて失い、戦争・従属・取引もなくなるが、**生きたまま**残る（`CvPlayer::isObserver`。BtS の AI 自動プレイのように滅ぼすと、exe が地図のクリックやホイールを受け付けなくなる）。手番はユニットがないので Enter（ふつうのターン終了）を待って止まる。Space で連続再生と停止を切り替える（`CvEventManager.py`。連続再生は `CvGame::update` が手番を自動で終える）。観戦者は外交・宣戦の対象にならず（`canContact`・`canDeclareWar`）、イベントと志向の Python フック、定期的な文明のランキング（`CvTopCivs`）も動かない。滅びない（`verifyAlive`）。地図はデバッグモードで全表示する（`updateDebugModeCache` が観戦中はチートなしでも通す。保存されないので `CvGame::update` で戻す）。案内は `CvMainInterface.py` の `EndTurnText`（`TXT_KEY_OBSERVE_*`）。勝者が決まると連続再生が止まり、ふつうのゲームと同じ終了画面になる。セーブに入る（`CvGame` の `uiFlag` 2） |
| （タグなし） | 地域区分 | 陸地と、外洋でない水タイル（沿岸・湖）を 8 方向につないだまとまり（`CvMap::calculateRegions`）。境界は外洋（`DEEP_WATER_TERRAIN`）と通れない水タイル（極地の氷）。初期の船（ガレー船など）は外洋に入れないので、別の地域は外洋船がないと渡れない場所になる。浅い海でつながった島々は 1 つの地域。BtS の `CvArea`（陸塊。島ごとに別）とは別物で、`CvArea` を使う既存の効果（ノートルダムなど）は変えていない。陸地が `REGION_MIN_LAND_PLOTS` 未満の地域は、いちばん近い（歩数が最小の）大きな地域にまとめる（同点なら陸地の多い方）。大きな地域が 1 つもなければまとめない。ゲーム開始時（`CvGame::setInitialItems` の開始地点の調整の後）に 1 回計算してセーブする（`CvMap` の `uiFlag=1`）ので、後で地形が変わっても地域は動かない。地域の機能より前のセーブは、最初に使うときに計算する。判定: `CvPlot::isNewWorld`（開始時にどのプレイヤー（蛮族以外）の開始地点もない地域）、`isOverseas(ePlayer)`（そのプレイヤーの本国と別の地域。本国は首都の地域で、首都がなければ開始地点の地域。遷都すると本国も移る。`CvPlayer::getHomePlot`）、`isDistant(ePlayer)`（首都（なければ開始地点）からの距離が、マップの最大距離（`maxPlotDistance`）の `REGION_DISTANT_PERCENT`% 以上。地域は使わず、外洋も含む）。外洋と通れない水タイルはどの地域にも属さず（`getRegion` が -1）、新世界・海外にもならない。Python: `CyPlot.getRegion` / `isNewWorld` / `isOverseas` / `isDistant`、`CyMap.getNumRegions` / `isRegionNewWorld`。表示は「画面の表示の追加」の地球儀ビューのレイヤー。使う効果のタグ: 志向の `iNewWorldCityPopulation`・`iOverseasCityPopulation`・`iOverseasMaintenanceModifier`・`iDistantTradeRouteModifier`、昇進の `iCombatPercentOverseas` |

## 志向の付け外し（仕組み）

志向の付け外し（タグではなく仕組み）: `CvPlayer::changeFreeTraitCount(eTrait, iChange)` でゲームの途中に志向を与える・取り上げる（Python は `CyPlayer.changeFreeTraitCount` / `getFreeTraitCount` / `hasBaseTrait`）。`CvPlayer::hasTrait()` は「指導者・文明の志向（`hasBaseTrait`）」か「与えられた数（`m_paiFreeTraitCount`。セーブに保存、`CvPlayer` の `uiFlag=3`）が 1 以上」。数を数えるので、2 か所から与えて 1 か所が取り上げても残る。元から持つ志向を与えても二重には効かない。志向を得た・失ったときは、ゲーム開始時に 1 回だけ足していた効果（`CvPlayer::processTrait`。`init()` もこれを使う）を足し引きし、志向で決まる保存された値を計算し直す（`CvPlayer::updateTraitDependents`: 無政府期間の上限、金融志向などの閾値、陳腐化を無視する建物の効果、都市圏の半径、自国の領土のタイルの産出、`FeatureRoutes` があれば交易網）。その他の志向の効果はそのつど合計しているので、すぐに反映される。志向の昇進（`FreePromotions`・`FreePromotionSets`）は生産したユニットだけに付き、今いるユニットには付けない・外さない。ゲーム状態を変えるので、すべてのマシンで同じように呼ぶ（呪文の `PyResult` など。画面の Python からは呼ばない）

基本志向（2026-11-22 に一段落）: BtS の指導者の志向 11 個（攻撃・防衛・カリスマ・帝国主義・金融・組織・創造・勤労・哲学・拡張・宗教）に `bBasicTrait` を付け、ほかの指導者から基本志向を得る・入れ替える志向のタグを作った。どれも候補はほかの経路で持っていない基本志向で、相手から得るものは相手の指導者と文明の志向（`hasBaseTrait`）が候補になる。仕様は `docs/tags/trait-military.md`。

| タグ | 契機 | 得る数 | 失うか |
|---|---|---|---|
| `bGainTraitOnCapitalCapture` | 戦争で相手の首都を占領 | 1 つ（ランダム） | 失わない |
| `bGainTraitsOnElimination` | 相手を滅ぼす（最後の都市を占領したプレイヤー） | すべて | 失わない |
| `GainTraitsOnFriendly`（`bAll`） | AI の指導者が親密になる（1 人につき 1 回） | 1 つ（ランダム）／ `bAll` 1 ですべて | 失わない |
| `RandomBasicTraits`（`iTurns`・`iCount`） | `iTurns` ターンごと（ゲーム速度で変わる） | `iCount` 個（ランダム） | 次の入れ替えで外れる |

共通の処理は `CvPlayer::gainBasicTraitsOf`（相手から得る 3 つ）と `CvPlayer::doRandomBasicTraits`。新しい契機を足すときは `gainBasicTraitsOf(eFrom, bAll, szMessageKey)` を呼ぶ（メッセージの `%s1` は相手の指導者、`%s2` は志向）。

## ゲームルール（`GlobalDefinesAlt.xml`）

- `RIVER_CROSSING_MOVE_COST`: 陸上ユニットが川を渡るときの追加の移動コスト（移動力。0 で BtS と同じ）。`CvPlot::movementCost` で地形のコストに足し、ユニットの移動力が上限（大きな値にすると渡河で移動が終わる。移動力 1 のユニットはもともと 1 タイルで止まるので変わらない）。経路探索も同じ関数なので、AI も避ける。払わないのは、川の戦闘ペナルティを無視するユニット（`isRiver`。水陸両用）、地形のコストを無視するユニット（`ignoreTerrainCost`）、橋梁建設を持ち道路でつながっている移動（道路のコストが優先）
- `REGION_MIN_LAND_PLOTS`（10）: 地域区分で、陸地がこの数未満の地域を近くの大きな地域にまとめる（`CvMap::calculateRegions`）。ゲーム開始時の計算に使うので、途中で変えても既存のゲームの地域は変わらない
- `REGION_DISTANT_PERCENT`（30）: 首都からの距離が、マップの最大距離のこの % 以上のタイルを「首都から遠い」とする（`CvPlot::isDistant`）。志向の `iDistantTradeRouteModifier` の「遠く離れた都市」（交易路の 2 つの都市の距離）にも使う

## BtS 本体の修正

- **Unofficial 3.19 Patch 1.6**（jdog5000。https://forums.civfanatics.com/resources/unofficial-3-19-patch-sources.12820/ ）を取り込んだ。DLL の変更は `UNOFFICIAL_PATCH` の印の中にある（1 か所だけ、会っていない文明の名前の修正が `BETTER_BTS_AI` の印の中）。変更の一覧は UP の配布物の `unofficial patch list.txt`。UP のソースのうち、英語版が土台なので日本語版の `CYBERFRONT` の部分（日付の表記、フォントの文字表）を消している箇所と、jdog5000 の AI 自動プレイ（`AI_AUTO_PLAY_MOD`）、`PROFILE_FUNC` の付け外しは取り込んでいない。XML の修正（`CIV4BuildingInfos.xml` の音、`CIV4EventInfos.xml`・`CIV4EventTriggerInfos.xml` のイベント、`CIV4UnitInfos.xml` のミサイル巡洋艦の `iPower` とステルス駆逐艦の AI、`CIV4UnitClassInfos.xml` の戦列艦の名前、`Art\CIV4ArtDefines_Unit.xml` のユニット完成時の音）と Python の修正（`CvAdvisorUtils.py`・`Screens\CvDomesticAdvisor.py`・`EntryPoints\CvRandomEventInterface.py`・`CvMapGeneratorUtil.py`・`pyHelper\Popup.py`）も入れた。どれも日本語版の元ファイルに UP の差分を当てたもの。UP の新しいテキストキー（`TXT_KEY_TERRAIN_TURN_DAMAGE`・`TXT_KEY_TERRAIN_TURN_HEALING`、UP がテキストを持たない `TXT_KEY_CHEATERS_NEVER_PROSPER`）は日本語を付けて `CIV4GameText_ClaudeSample.xml` に足した。UP の `TXT_KEY_BUILDING_REQUIRES_NUM_SPECIAL_BUILDINGS`（リンク付きの上書き）は入れず、日本語版のテキストのまま。国旗のマウスオーバーに「Unofficial 3.19 Patch v1.60」と出る
- UP とこの Mod の改変が重なった箇所: 生産の余りの金（UP の計算をオプションによらず使い、`GAMEOPTION_MULTIPLE_PRODUCTION` のときだけメッセージを出す。`CvCity::doOverflowGold`）、`CvUnit::canMoveInto` のユニットの `TerrainImpassables`（BtS は地形の特徴があるタイルでは確かめなかった。UP は常に確かめる。この Mod の `ImpassablePlotPassages` の条件はその一部なので UP に合わせた）、`CvUnit::shouldLoadOnMove`（UP の `canMoveAllTerrain` とこの Mod の `canEnterImpassablePlot` の両方で除く）
- `CvUnit::upgradePrice` は、割引の合計が 100% を超えると費用がマイナスになり、アップグレードで金が増えていた。この Mod では費用を 0 未満にしない
- `CvUnit::isPromotionValid` は「ユニットの撤退率 + その昇進の撤退率」が `MAX_WITHDRAWAL_PROBABILITY`（90）を超える昇進を選べなくする（迎撃率・回避率も同じ）。ユニットがすでに上限を超えていると、撤退率を上げない昇進や大将軍（指導者の昇進）まで**すべて**選べなくなっていた。BtS では昇進で上限を超えられないので起きないが、この Mod では選ばずに得る昇進（志向の昇進の組、`PromotionVictory`、`PromotionCombatApply`）で超えられる。この Mod では、その昇進が値を上げる（> 0）ときだけ上限で止める
- `CvPlayer::canRevolution` は、新しい社会制度を採用できるか（`canDoCivics`）を確かめない（BtS は、社会制度の画面とポップアップが採用できるものだけを出す前提）。ところが技術を得たときの「○○を採用しますか？」のポップアップは、その技術で解禁される社会制度を `canDoCivics` を確かめずに出すので、`ForbiddenCivics`・`PrereqOrTraits` で採用できない社会制度でも OK を押すと革命になり、無政府状態の後、ターンの初めの `verifyCivics` で元に戻されていた。この Mod では、変える区分の新しい社会制度が採用できなければ革命できない（そのためこのポップアップも出ない）
- AI の宗主国が属国に資源の貢物を求めるとき（`CvPlayerAI::AI_doDiplo`）、属国が交易に出せる数しか見ず、2 国の間の交易網（`canTradeNetworkWith`）と陳腐化を確かめていなかった（人間の宗主国の要求画面 `launchVassalDemandTributePopup` は交易網を確かめる）。交易網のない資源の取引は `CvDeal::verify` ですぐ取り消されるので、AI の属国には実害がなかったが、`bHumanCanBeVassal` で人間が属国になると、受けても資源は渡らず、断ると宗主国と戦争になる要求が来ていた。この Mod では `canTradeItem` で渡せる資源だけを求める（AI の属国にも同じ）
- パルチザン（`EVENTTRIGGER_PARTISANS`。都市が破壊されると、その都市で文化が最も多い文明（多くは元の持ち主）に、解放を採用していて破壊した文明と交戦中なら起きる。`CvEventManager.onCityRazed` と `CvRandomEventInterface.py` の `Partisans`）: BtS は人口の条件を `city.getPopulation > 1`（`()` なし。常に真）と書いていたので、人口 1 の都市（育たずに自動で破壊された都市も）でも起きていた。選択肢 2（首都にユニット）の条件 `max(1, ...) > 0` も常に真で、都市のタイルに文化がなくても 1 体出ていた。選択肢 1（都市の周りにユニット）は、選べるかの判定では破壊した都市のタイルも数えるのにユニットはそこに置かないので、選べても何も出ないことがあり、逆に置く方では都市を除いていなかったので、この Mod の `iMinCityRangeChange` で隣にある都市（破壊した側の都市も）にユニットを置けた。この Mod では、人口 2 以上の都市に限り、選択肢 2 も選択肢 1 と同じく文化を要し、選択肢 1 の判定と配置を同じタイル（周りの 8 タイルのうち、見えている敵ユニット・水・通行不能・都市のないもの。`getPartisanPlots`）にした。首都で徴兵できるユニットがない（`getConscriptUnit` が -1）ときはどちらの選択肢も選べない（BtS は `initUnit(-1)` になっていた）。首都がないときの説明文の Python エラーも直した
- イベントの発生条件の社会制度（`EventTriggerInfo/Civic`）は、`ReplacedCivic` で置き換えた社会制度も採用中として数える（`CvPlayer::getEventTriggerWeight`）。以前の版ではクエストの期限切れの判定だけが置換先を数え、発生の判定は数えていなかった（解放を置き換えた文明ではパルチザンが起きなかった）
- シッズ・スシ社の重役のユニットクラス（`UNITCLASS_EXECUTIVE_2`）の名前のキーが、BtS(J) の `CIV4UnitClassInfos.xml` では `CHANGED_TXT_KEY_UNIT_EXECUTIVE_2` で、そのテキストはどこにもなかった（テキストは `CIV4GameText_BTS_Fixed.xml` の `TXT_KEY_UNIT_EXECUTIVE_2`「シド寿司社重役」に改名されている）。ユニットクラスの名前を出す所（志向の `FreePromotionSets` の説明など）に「CHANGED TXT KEY UNIT EXECUTIVE 2」と出ていた。この Mod のコピーで `TXT_KEY_UNIT_EXECUTIVE_2` に直した（2026-11-19）。ほかの `CHANGED_TXT_KEY_*`（企業 2、三段櫂船のペディア）はテキストがある

## 画面の表示の追加

- 都市画面の産出の内訳: 生産力の説明（`setProductionHelp`）・商業の内訳の説明（`setCommerceHelp`）・人口のバーの説明（食料。`CvDLLWidgetData::parsePopulationHelp`）で、「基本○○」の下に出元の小計を出す（`CvGameTextMgr::setBaseYieldBreakdownHelp`。`setYieldHelp` から呼ぶ）: タイル（働いているタイル。都市の中心を含む）、専門家（専門家・無償の専門家の産出と、社会制度・志向による追加分）、建造物（建物の産出とイベントによる増減、世界遺産の数に比例する分）、交易路、企業。0 の小計は出さない（タイルは常に出す）。これらは `CvCity` が基本値に足すものすべてなので合計は基本値と一致し、合わない分（Python で直接変えた場合など）があるときだけ「その他」を出す。食料はさらに、補正があれば補正後の食料、人口による消費（人口 × 2）、不衛生による消費、余剰（`CvCity::foodDifference`。暴動中は 0、食料を生産に回している間は 0 以下）を出す（`CvGameTextMgr::setFoodHelp`）。表示だけでゲーム状態は変えない
- 地球儀ビューの「地域」レイヤー（`GLOBE_LAYER_REGION`。一覧は `CvGame::getGlobeLayers`、色は `CvPlayer::getRegionLayerColors`）: 地球儀ビューのボタンの列の 6 つ目。ボタンの見た目は、BtS が使っていない戦略レイヤーのもの（`Button_HUDGlobeStrategy_Style`）。ボタンの列は `CvMainInterface.py` がレイヤーの一覧から作るので、Python は変えていない。切り替え（`GlobeLayerRegionOptionTypes`）: 地域ごとの色分け（色相を黄金比ずつ回す）、新世界（緑）と旧世界（橙）、本国（青）と海外（赤）、首都から遠い場所（黄）。プレイヤーが見たことのあるタイルだけを塗る（未探索の大陸の形を見せない）。表示だけでゲーム状態にも乱数にも触れない。タイルの説明（`setPlotHelp`）にも「新世界」「海外」「首都から遠い」を出す（ゲーム開始後のみ。デバッグモードでは地域の番号も）

## この Mod で追加した志向

この Mod で追加した志向: `TRAIT_MARITIME`（海洋志向、短縮名「海」。昇進の組: 海洋ユニット → 航海術Ⅰ、白兵戦ユニット → 水陸両用。今はどの文明・指導者も持っていない）。`CIV4TraitInfos.xml` の末尾に追加（既存の志向の番号を変えないため、新しい志向は常に末尾に足す）
