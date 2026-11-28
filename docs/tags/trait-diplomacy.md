# 志向のタグ（外交）

AI の態度・情報の公開・属国・取引・ユニットの贈与（レンドリース）。2026-11-26 に `trait-rules.md` から分けた。

## `TraitInfo/bHumanCanBeVassal`

- ファイル: `CIV4TraitInfos.xml`
- 人間のプレイヤーでも AI のプレイヤーの属国（平和的な属国・降伏）になれる。
- BtS は人間を人間の属国にしかしない（`CvPlayer::canTradeItem` の `TRADE_VASSAL` / `TRADE_SURRENDER`。属国になる側のプレイヤーの志向を見る）。
- AI の宗主国は、AI 同士のときと同じく、属国の戦争・和平を引き受けられるときだけ受け入れる（`CvPlayerAI::AI_considerOffer` で、人間の申し出の属国の項目に `getTradeDenial` = `CvTeamAI::AI_vassalTrade` / `AI_surrenderTrade` を見る。属国の側の意思の判定は人間なので素通り。それ以外は贈り物と同じく受け入れる）。
- 属国になったあとは BtS の人間どうしの属国と同じ（宗主国の戦争に加わる、単独で宣戦・和平できない、平和的な属国は `PEACE_TREATY_LENGTH` ターン後に解消でき、降伏は `canVassalRevolt` を満たすと解消できる、宗主国は解消できない）。
- AI の側から人間に属国化・降伏を求めることはない。
- `ConditionalFreeTraits` の `bVassal` と組み合わせる。
- 説明文 `TXT_KEY_TRAIT_HUMAN_CAN_BE_VASSAL`。
- Python: `CyPlayer.isHumanCanBeVassal()`、`CvTraitInfo.isHumanCanBeVassal()`。
- 三国志の Mod（The History of Three Kingdoms）にも同様の仕組みがあったはずとユーザーから聞いたが、DLL のソースが同梱されておらず、Python・XML にも見当たらなかった

## `TraitInfo/bRevealWonderProduction`・`bRevealWarPlans`

- ファイル: `CIV4TraitInfos.xml`
- 他の文明の内情が、右下の指導者名と指導者の顔（外交画面・外交顧問）のツールチップに出る。
- どれかの志向が持っていれば有効（`CvPlayer::isRevealWonderProduction`・`isRevealWarPlans`）。
- **`bRevealWonderProduction`**: 相手の都市のうち、生産の先頭が世界遺産の都市ごとに「建造中「遺産名」：「都市名」」（`TXT_KEY_REVEALED_WONDER_PRODUCTION`）。
- 都市がこちらの地図で未確認なら都市名は「？？？」（`TXT_KEY_REVEALED_UNKNOWN`）。
- 人間の相手にも出る。
- **`bRevealWarPlans`**: 相手（AI だけ）のチームが戦争計画（準備中・自分から始めた戦争・応戦のすべて。`CvTeamAI::AI_getWarPlan` が `NO_WARPLAN` 以外）を持っていれば「戦争計画「文明名」・「都市名」」（`TXT_KEY_REVEALED_WAR_PLAN`）。
- 計画が複数あれば `CvTeamAI::AI_startWarVal` が最も大きいチーム。
- ただし表示では近さの項（`AI_teamCloseness`）を飛ばす（`bIgnoreCloseness`。都市の AI のキャッシュを書き換えるため、表示から呼ぶと同期がずれうる）。
- 都市はそのチームの全都市のうち、相手のプレイヤーから見た `CvPlayerAI::AI_targetCityValue`（乱数なし）が最も大きいもの。
- 文明名はその都市の持ち主（都市がなければチームのリーダー）。
- こちらが未接触の文明は「？？？」、未確認の都市も「？？？」。
- AI の文明の行は `CvGameTextMgr::getAttitudeString` の末尾で、態度の相手が操作中のプレイヤーのときだけ足す（外交顧問の AI 同士の関係には出ない）。
- 人間の文明の行は `CvDLLWidgetData::parseContactCivHelp` で足す。
- 本体は `CvGameTextMgr::getRevealedPlansString`（文言は `getRevealedWonderText`・`getRevealedWarPlanText`）。
- **アナウンス**: 志向を持つ人間のプレイヤー（相手のチームと接触済み・別のチーム）に、先頭に文明名を付けて同じ文言で知らせる（`TXT_KEY_REVEALED_ANNOUNCE`。例「モンゴル：建造中「ピラミッド」：「？？？」」）。
- 都市が見えていれば矢印でその場所を示す。
- 世界遺産は都市のターンの終わり（`CvCity::doTurn` の生産の後、`CvCity::doRevealedWonderAnnouncement`）に、生産の先頭の世界遺産が前回の判定から変わっていたら出す（AI は生産を選び直すときに予約を空にして積み直すので、先頭が変わるたびに判定すると一瞬だけ先頭に来た遺産も拾ってしまう）。
- 前回の遺産 `CvCity::m_eRevealedWonderProduction` はセーブせず、読み込み時に今の先頭の遺産を入れる（読み込み直後には出さない）。
- 遺産から別の遺産に切り替えるたび、また切り替えて戻ったときにも出る。
- 戦争計画は、AI のチームが計画なしから新しい計画を持ったとき（`CvTeamAI::AI_setWarPlan` から `AI_announceWarPlan`。準備中・戦争・応戦のどれでも。準備中から総力戦などへの切り替えでは出さない）。
- 都市はチームのリーダーから見て選ぶ。
- 説明文 `TXT_KEY_TRAIT_REVEAL_WONDER_PRODUCTION`・`TXT_KEY_TRAIT_REVEAL_WAR_PLANS`。
- Python は `CyPlayer.isRevealWonderProduction()`・`isRevealWarPlans()`、`CvTraitInfo.isRevealWonderProduction()`・`isRevealWarPlans()`。
- AI の隠れた挙動の全体は `docs/ai-hidden-behavior.md`

## `TraitInfo/bRevealHiddenAttitude`

- ファイル: `CIV4TraitInfos.xml`
- AI の態度のうち、BtS の外交のツールチップに出ない補正も、ほかの補正と同じく緑（プラス）・赤（マイナス）の行で出す（`CvGameTextMgr::getAttitudeString`）。
- 操作中のプレイヤーの志向で判定するので、外交顧問での AI 同士の態度にも出る。
- どれかの志向が持っていれば有効（`CvPlayer::isRevealHiddenAttitude`）。
- 項目は `CvPlayerAI::AI_getHiddenAttitude`（`HiddenAttitudeTypes`）: 指導者の性格（`iBaseAttitude`）、難易度（相手の難易度の `iAttitudeChange`）、和平への姿勢の近さ（AI 同士だけ。`4 - |PeaceWeight の差|`）、好戦性への敬意（AI 同士だけ。`iWarmongerRespect` の小さい方）、チームの人数の差、順位の差（`iWorseRankDifferenceAttitudeChange`・`iBetterRankDifferenceAttitudeChange`）、ともに下位（+1）、戦争での劣勢（`iLostWarAttitudeChange`）。
- これらは `CvPlayerAI::AI_getAttitudeVal` から値を変えずに移したもので、態度の計算もこの関数を使う（表示と計算がずれない）。
- 表示される行の合計が態度の値になる（-100〜100 に収める前）。
- テキストは `TXT_KEY_MISC_ATTITUDE_HIDDEN_*`（`CIV4GameText_ClaudeSample.xml`）。
- 説明文 `TXT_KEY_TRAIT_REVEAL_HIDDEN_ATTITUDE`。
- Python は `CyPlayer.isRevealHiddenAttitude()`、`CvTraitInfo.isRevealHiddenAttitude()`

## `TraitInfo/Deals`（`bBuyCities`・`iCityValueModifier`・`iGoldLimitModifier`・`iGoldValueModifier`）

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-24。
- AI との取引の条件。
- すべて省略可。
- `bBuyCities`: 平時でも AI がこのプレイヤーに都市を売る（`CvPlayerAI::AI_cityTrade` の `DENIAL_NEVER` を飛ばす。BtS の AI は戦争を終える取引でしか都市を出さない）。
- 首都・属国・「都市の排除」のオプションの制限は `canTradeItem` のまま。
- AI は都市の値段（下記）と釣り合えば売る。
- BtS の外交画面は、平時の人間と AI の取引で都市を都市としか組み合わせられない（`CvPlayer::updateTradeList`）。
- どちらかのプレイヤーが `bBuyCities` を持つときは、都市を毎ターンでない品物（金・技術など）と同じ取引に載せられる（毎ターンの品物とは BtS どおり組み合わせられない）。
- 都市の重要さ（聖都・遺産）は値段に入らない（BtS の値段の式のまま）。
- `iCityValueModifier`: このプレイヤーが取引で受け取る都市の値段 %（`AI_cityTradeVal` の最後。値段は受け取る側が計算するので、AI はこの値段を求める。-50 で半額）。
- 戦争を終える取引で都市を要求するときにも効く。
- このプレイヤーが AI に都市を渡すときは変わらない。
- `iGoldLimitModifier`: AI がこのプレイヤーに 1 回の取引で出す金の上限 %（一括 `AI_maxGoldTrade` と毎ターン `AI_maxGoldPerTurnTrade`。指導者の `iMaxGoldTradePercent` などの後に掛ける）。
- AI の所持金・研究の蓄え・収入による上限はそのまま。
- `iGoldValueModifier`: このプレイヤーが出す金（一括・毎ターン）を AI が評価する価値 %（`AI_goldTradeValuePercent(eGiver)`。BtS は金 1 = 価値 2、財政難なら 3。50 なら AI が求める金は 2/3）。
- `AI_dealVal`・逆提案（`AI_counterPropose`）・AI からの技術の取引の提案（`AI_doDiplo`）で使う。
- どれも志向の合計（`bBuyCities` はどれかが持っていれば有効）。
- AI どうしの取引には効かない（そのプレイヤーが AI でも、相手の AI がこのプレイヤーと取引するときは効く）。
- Python: `CyPlayer.isBuyCities()`・`getDealCityValueModifier()`・`getDealGoldLimitModifier()`・`getDealGoldValueModifier()`。
- 説明文 `TXT_KEY_TRAIT_DEAL_BUY_CITIES`・`_CITY_VALUE_MODIFIER`・`_GOLD_LIMIT_MODIFIER`・`_GOLD_VALUE_MODIFIER`。
- 外交画面で AI の都市にカーソルを乗せると「値段の目安: [金]N（価値 V）」が出る（志向に関係なく、相手が AI の都市すべて。`CvDLLWidgetData::parseTradeItem`、`TXT_KEY_TRADE_CITY_PRICE`）。
- V は受け取る側の `AI_cityTradeVal`、N は V を AI がこちらの金につける価値（`AI_goldTradeValuePercent(eGiver)`）で割って切り上げた額で、`AI_considerOffer` と同じ計算。
- 平時は金 N だけで取引が成立する。
- 戦争中は戦争を終える価値が取引に加わるので目安にならない

## `TraitInfo/LendLease`（`bLendLease`・`iResearchPercent`・`iGoldPercent`・`iAttitudeChange`・`iAttitudeLimit`）

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-26。
- レンドリース: 自国の都市にいるユニットを、平和な文明の首都に送ってその文明のものにする。
- すべて省略可。
- `bLendLease`: 自国の都市にいるユニットに行動ボタン「レンドリース」（`COMMAND_LEND_LEASE`。絵は「贈る」と同じ）が出る。都市の外では出ない。
- ボタンは、送れる文明が 1 つもなければ無効になる。
- ボタンを押すと送り先を選ぶポップアップが出る（`CvPlayer::launchLendLeasePopup`。BtS の「積み込む」と同じく、押しただけではネットメッセージを送らない）。
- 一覧に出る文明（`CvPlayer::canLendLeaseTo`）: 生きていて、出会っていて、戦争中でなく、首都がある。チームメイト・属国も含む。蛮族は含まない。
- 送れる文明はボタン: 指導者名（文明名）・その文明が戦争中の相手（こちらが出会っている文明）・態度（AI だけ）・研究と金の見込み。
- 一部のユニットだけ送れるときは、ボタンに「N 体中 M 体」と出る。
- 1 体も送れない文明は、本文に灰色の文字で理由と一緒に並ぶ（最初に送れなかったユニットの理由）。
- ユニットごとの条件（`CvUnit::getLendLeaseDenial`。BtS の `canGift` のうち場所によらないもの）:
  - 自国の都市にいる
  - 企業の幹部でない
  - 艦船は、相手の首都が沿岸（`MIN_WATER_SIZE_FOR_OCEAN` 以上の水域に接する）
  - 相手のユニットクラスの数の上限（チーム・プレイヤー）に達していない
  - 相手の AI が受け取る（`AI_acceptUnit`。財政難の AI は、必要な労働者・作業船・宣教師しか受け取らない）
- 文明を選ぶと、選択中のユニットを（移動の命令と同じく）1 つのグループにまとめてから、ModNetMessage `-20261128`（`MOD_NET_MESSAGE_LEND_LEASE`）で全マシンに送る。
- 送るのはそのグループのユニットとその積み荷のうち、条件を満たすもの（`CvPlayer::lendLease`）。
- ユニットは相手の首都に現れる。積み荷は降ろした状態で現れる。送れない積み荷は元の都市に降ろされる。
- 経験値・昇進・ダメージ・残りの移動力は引き継ぐ（BtS の `gift` と同じ `convert`）。
- UnitAI は、そのユニットの既定の UnitAI（`DefaultUnitAI`）になる（BtS の `gift` は贈った側の UnitAI を引き継ぐ）。
- Python のイベント `unitGifted` はユニットごとに出る。ユニットの `PyOnCreated` / `PyOnDeath` は呼ばれない。
- BtS の `gift` が足す「援助してくれた」の値（生産コストの 1/5）は足さない（下の `iAttitudeChange` で決める）。
- `iResearchPercent` / `iGoldPercent`: 送ったユニット（積み荷を含む）の生産コストの合計の ○% を研究・金として得る。
- 生産コストは贈る側の `getProductionNeeded`（ゲーム速度・時代などの補正込み）。送る前にまとめて計算し、合計に 1 回だけ割合を掛ける。
- 研究は研究中の技術に入る。研究中の技術がなければ繰り越し（overflow）に入る（`CvPlayer::receiveResearch`。探検の報酬と共通）。
- `iAttitudeChange`: 1 回送るごと（何体でも 1 回）に、相手の「取引」の態度（`AI_getTradeAttitude`）をちょうど ○ 上げるだけの値を `AI_changePeacetimeGrantValue` で足す（`CvPlayer::changeLendLeaseAttitude`）。
- 「取引」の態度は BtS の「(援助の値 + 貿易の値の差) / ((出会ってからのターン数 + 1) × 5)」で 0〜4。普通の取引で上がった分と上限を共有する。
- 値は減らないが割る数が毎ターン増えるので、時間で薄れる（BtS どおり）。
- BtS どおり、受け取った文明と戦争中の文明からの態度が下がることがある（`AI_getRivalTradeAttitude`）。
- `iAttitudeLimit`: `iAttitudeChange` で上げる上限。省略（0）か 5 以上なら 4（BtS の上限）。
- どれも志向の合計（`bLendLease` はどれかが持っていれば有効）。
- AI は使わない。
- 大量に送って相手を財政難にすることは防いでいない（ユーザーの判断）。
- Python: `CyPlayer.isLendLease()`・`getLendLeaseResearchPercent()`・`getLendLeaseGoldPercent()`・`getLendLeaseAttitudeChange()`・`getLendLeaseAttitudeLimit()`・`canLendLeaseTo(ePlayer)`、`CommandTypes.COMMAND_LEND_LEASE`。
- 説明文 `TXT_KEY_TRAIT_LEND_LEASE`・`_RESEARCH_PERCENT`・`_GOLD_PERCENT`・`_ATTITUDE`。

## `TraitInfo/FoundingVassal`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-23。
- 最初の都市を建てると、AI の文明がその近くでゲームに参加し、永久同盟としてチームに加わる。
- `<FoundingVassal>` に `CivilizationType`・`LeaderType`・`HandicapType`・`iRange`・`CivilizationTypes` を書く（すべて省略可。タグを書けば有効）。
- **発動**: `CvPlayer::found` の最後（`PyOnCityFounded`・`CityFoundedUnits` の後）の `doFoundingVassal`。
- 1 プレイヤーにつき 1 回だけ（`m_bFoundingVassalDone`。都市をすべて失って建て直しても発動しない。何も起きなかったときも済みになる）。
- 複数の志向が持つときは番号の小さい志向。
- 蛮族・小文明は対象外。
- 上級スタートでは、最初の都市を置いた時点で発動する（都市を売り戻しても AI は残る）。
- **プレイヤーの枠**: 一度も参加していない枠 → 滅んだプレイヤーの枠（チームも滅んでいるもの）の順（植民地の分離と同じ）。
- 空きがなければ何も起きない。
- **文明と指導者**: タグの値（組み合わせは自由）。
- すでに参加した（`isEverAlive`）プレイヤーが使っている、蛮族、`NONE`、省略のときは自動。
- **候補の文明** `CivilizationTypes`（`CivilizationType` を並べる。2026-11-28、神聖ローマの諸侯用）: `CivilizationType` が使えないか省略のとき、候補のうち使われていない文明からランダムに選ぶ（`bAIPlayable` は見ないので、選べない文明も候補にできる）。候補がすべて使われていれば自動。説明文は「A, B のどれか（ランダム）」（`TXT_KEY_TRAIT_FOUNDING_VASSAL_ONE_OF`）。Python は `CvTraitInfo.getNumFoundingVassalCivilizations()`・`getFoundingVassalCivilizations(i)`。
- 参加した AI の指導者が `RandomBasicTraits` の志向を持てば、参加したときにすぐ基本志向を得る（`iTurns` 0 で「1 つだけランダムに得て入れ替えない」。神聖ローマの諸侯の指導者用）。
- 文明の自動は植民地の分離と同じ（`getSplitEmpireLeaders`: 自分以外の、未使用で AI がプレイできる文明。`DerivativeCiv` を優先し、あとは乱数）。
- 指導者の自動は、その文明の指導者（「どの文明の指導者にも」オプションなら全員）→ 全指導者、の順に未使用のものから乱数。
- **場所**: 最初の都市から `iRange`（省略時 5、最小 1）タイル以内で 3 タイル以上（`iRange` が 3 未満ならその値以上）離れた、同じ大陸の陸地。
- 通行可能、誰の領土でもない、ユニットがいない、部族集落がないタイル。
- その中で、新しい AI の開始位置としての価値（`AI_foundValue` の `bStartingLoc`）が最大のタイル。
- 価値がどれも 0 なら最も近いタイル。
- 候補がなければ何も起きない（プレイヤーを足す前に調べる）。
- **参加の処理**: `CvGame::addPlayer`（植民地の分離と同じ）→ 開始時の資金（`initFreeState`）→ 開始位置 → `CvTeam::addTeam`（永久同盟の合流と同じ。技術・地図・接触・戦争をチームで共有）→ ユニット。
- AI 自身の難易度は `STANDARD_HANDICAP`（貴族）。
- **ユニット**: その難易度の AI がゲーム開始時に持つ初期ユニットすべて（`CvPlayer::addStartingUnits`。上級スタートなしの `initFreeUnits` から切り出した）。
- 文明の初期ユニット × （時代の `iStartingUnitMultiplier` + `HandicapType` の `iAIStartingUnitMultiplier`）、防衛ユニット・労働者・探索ユニットを それぞれ 時代の値 + 自分の難易度の値 + `HandicapType` の `iAI〜` 体。
- 種類はチームの技術で決まる（`addFreeUnitAI`）。
- 貴族・古代なら開拓者 1 体と探索ユニット（戦士か斥候）1 体。
- 上級スタートのゲームでも、開始ポイントではなくユニットを出す。
- **AI の補正**: `HandicapType` を書くと、この AI の AI 補正（`iAITrainPercent`・`iAIGrowthPercent`・`iAIUnitCostPercent` など）は、ゲームの難易度ではなくその難易度のものになる（`CvPlayer::getAIHandicapType`）。
- `NONE` または省略ならゲームの難易度（他の AI と同じ）。
- AI 補正を読む CvPlayer・CvPlayerAI・CvUnit の 34 か所を `getAIHandicapType()` に変えた（チーム単位の `iAIDeclareWarProb` はゲームの難易度のまま）。
- 研究はチームで共有するので、研究コストには効かない。
- **チームへの影響**: 参加した AI（`CvPlayer::isFoundingVassal`）は、研究コストのメンバー数の補正（`TECH_COST_EXTRA_TEAM_MEMBER_MODIFIER`）と、チームの難易度の平均（`CvTeam::getHandicapType`。研究コストに使う）に数えない（参加した AI しか生きていなければ数える）。
- そのほかはチーム戦の仕様どおり（AI の指導者の志向のうちチームで効くもの、諜報任務のコストがメンバー数倍など）。
- 参加した AI の志向に `FoundingVassal` があっても発動しない。
- ゲーム開始時の志向のフック（`PyOnGameStart`）は、参加した AI では呼ばれない。
- **メッセージ**: 「（文明）の（指導者）が（自分）と永久同盟を結び、ゲームに参加」（`TXT_KEY_MISC_FOUNDING_VASSAL_JOINED`）を、そのチームを知るプレイヤーに出し、リプレイにも残す。
- セーブは `CvPlayer` の `uiFlag=18`（`m_bFoundingVassalDone`・`m_bFoundingVassal`・`m_eAIHandicap`）。
- 説明文: `TXT_KEY_TRAIT_FOUNDING_VASSAL`（文明・指導者は `_ANY_CIV`・`_LED_BY`、難易度は `_HANDICAP`）。
- Python: `CvTraitInfo.isFoundingVassal`・`getFoundingVassalCivilization`・`getFoundingVassalLeader`・`getFoundingVassalHandicap`・`getFoundingVassalRange`、`CyPlayer.getAIHandicapType`・`isFoundingVassal`

## `TraitInfo/iDenounceAttitudeChange`（と宣戦した側の記録）

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-28。ガンディーの非難用。
- この志向を持つプレイヤーに宣戦したチームのプレイヤーへの、ほかのプレイヤー（AI）の態度が ○ 変わる（負の値で下がる）。その戦争が続く間だけ（講和で消える。記憶ではない）。
- 数えないもの: その志向を持つプレイヤーのチーム自身（BtS の「宣戦された」の記憶があるため）、宣戦したチーム自身、その志向を持つチームと出会っていないチーム。
- 複数のチームに宣戦していれば、その分を合計する（`CvPlayerAI::AI_getDenounceAttitude`。`AI_getAttitudeVal` に足す）。
- 外交の態度の内訳に「-2: 宣戦した戦争への非難」（`TXT_KEY_MISC_ATTITUDE_DENOUNCED`）を出す。
- 説明文: `TXT_KEY_TRAIT_DENOUNCE_ATTITUDE`。
- Python: `CvTraitInfo.getDenounceAttitudeChange()`、`CyPlayer.getDenounceAttitudeChange()`。
- **宣戦した側の記録**: チームごとに「今の戦争を自分が宣戦したか」を持つ（`CvTeam::m_abWarDeclaredOn`、`isWarDeclaredOn(eTeam)`。セーブに保存、`CvTeam` の `uiFlag=2`。古いセーブの戦争はどちらも宣戦していない扱い）。
  - `CvTeam::declareWar` で宣戦した側を記録（`setAtWar` の前なので、`ConditionalFreeTraits` の更新に間に合う）。講和（`setAtWar` が false）で消える。
  - 防衛協定で参戦したチームは宣戦した側にしない（そこから続けて起きる参戦も同じ）。
  - 宣戦した側の属国が付き合って宣戦した場合は、属国も宣戦した側。宣戦された側の属国・宗主国に宣戦した場合も、宣戦した側。
  - 戦争中に加わったチーム（永久同盟の合流 `addTeam`、属国になった・属国を得た `setVassal`）は、加わった先のチームの記録を写す（`CvTeam::setWarDeclaration`）。
  - ゲーム開始時からの戦争（蛮族、シナリオ）は、どちらも宣戦した側ではない。
  - `CvTeam::isAtDefensiveWar`: 自分のチームに宣戦した、蛮族・小文明以外のチームと交戦中（`ConditionalFreeTraits` の `bDefensiveWar`）。
  - Python: `CyTeam.isWarDeclaredOn(eTeam)`・`isAtDefensiveWar()`。
