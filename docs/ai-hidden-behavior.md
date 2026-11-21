# AI の隠し補正と隠れた挙動

公式 SDK（Unofficial Patch 1.6 入り）のソースを読んで整理したもの（2026-11-21）。行番号はその時点のもの。

## 1. 態度（`CvPlayerAI::AI_getAttitudeVal`、CvPlayerAI.cpp:4985-5048）

外交画面のツールチップに出るのは、国境の近さ・宗教・開放国境・記憶（貢ぎ物、宣戦など）といった項目。次の値は**表示されないまま合計に入る**。

| 隠れた項目 | 内容 |
|---|---|
| `iBaseAttitude`（指導者） | 指導者ごとの基本の態度 |
| 難易度の `iAttitudeChange` | **相手側**の難易度で決まる。NOBLE 以上は -1、SETTLER は +2 |
| PeaceWeight の近さ（AI 同士だけ） | `4 - |両者の PeaceWeight の差|`。PeaceWeight は `iBasePeaceWeight` と `iPeaceWeightRand` から決まる。性格が近い AI 同士は仲良くなり、遠いとマイナスになる |
| `iWarmongerRespect`（AI 同士だけ） | 両者の値の小さい方が加わる |
| チーム人数 | 相手のチームの人数が多いと、差の分だけ減る |
| 順位の差 | 相手が上位なら `iWorseRankDifferenceAttitudeChange`、下位なら `iBetterRankDifferenceAttitudeChange` に、順位差 ÷（文明数+1）を掛けた値 |
| 下位どうし | 自分も相手も順位が下半分なら +1 |
| 戦争で負けた | 相手の戦果（WarSuccess）が自分より多いと `iLostWarAttitudeChange` |

態度は、宣戦の確率（`NoWarAttitudeProb`）と、宣戦先を選ぶときの評価値の倍率の両方に効く。

この Mod では、末尾に志向の `iAttitudeChange`（`GET_PLAYER(ePlayer).getAttitudeChange()`）も足している。

## 2. 宣戦布告を決めるまで（`CvTeamAI::AI_doWar`、CvTeamAI.cpp:3171-）

戦争中でもなく、戦争計画も持っていないときに、毎ターン次の順で判定する。

1. **全体の判定**: 難易度の `iAIDeclareWarProb` が通るか（SETTLER 25%、CHIEFTAIN 50%、WARLORD 75%、NOBLE 以上 100%）。攻撃的 AI のオプションも財政難もなければ、さらに 3/4 の確率になる。財政難のメンバーが半数以上いると戦争に反対する。逆に好戦的な指導者（各 WarRand が 100 未満）は、財政難のときに戦争に傾く
2. **戦争の種類を抽選**: `iMaxWarRand`（総力戦）→ `iLimitedWarRand`（限定戦）→ `iDogpileWarRand`（便乗戦）の順。値が小さいほど起きやすい。属国がいると確率が上がり、「最終戦争」戦略では総力戦の値が 1/4 になる
3. **相手ごとの絞り込み**
   - `iNoWarRoll`（0〜99 の乱数。限定戦は -10、便乗戦は -20）が、相手への態度に応じた `NoWarAttitudeProb` 以上であること。友好的な相手にはほぼ通らない
   - 兵力の比較: 総力戦は、相手の防衛兵力 × 2/3 が、自分の兵力 × `iMaxWarNearbyPowerRatio`（遠い相手は `iMaxWarDistantPowerRatio`）より小さいこと。限定戦は `iLimitedWarPowerRatio`。攻撃的 AI は自分の兵力を 4/3 倍に見積もる
   - 地上で攻め込める相手か、隣接する土地が `iMaxWarMinAdjacentLandPercent` 以上あるか
4. **準備期間（外からは見えない）**: すぐには宣戦せず、まず `WARPLAN_PREPARING_TOTAL` / `WARPLAN_PREPARING_LIMITED` の状態に入る。総力戦は約 10 ターン、限定戦は約 5 ターン準備する（ゲーム速度で伸び、兵力差が大きいと縮む）。総力戦は、主要な大陸の状況が「攻勢」（`AREAAI_OFFENSIVE`）と判定されたら宣戦する。20 ターン経っても準備が整わなければ計画を取り消す。「短剣」戦略中は準備を飛ばす

- **見分ける手がかり**: 戦争計画を持っている AI に他国への宣戦を頼むと、「手一杯だ」（`DENIAL_TOO_MANY_WARS`、CvTeamAI.cpp:2120）と断る。準備中もこの返事になるので、宣戦が近いことが分かる
- `AI_doWar` には Python のフックがある（`CvGameUtils.AI_doWar` が 1 を返すと既定の処理を飛ばす）

## 3. 狙う文明（`CvTeamAI::AI_startWarVal`、CvTeamAI.cpp:922）

2 の条件を通った相手のうち、次の評価値が最も高い文明を選ぶ。

- 相手の土地の価値（`AI_calculatePlotWarValue`）、首都どうしの近さ、領土の近さ（`AI_teamCloseness`）。近くない相手は評価値が 1/2（攻撃的 AI では 1/4）になる
- **自分より勝利に近い相手は ×8**
- **自分が制覇勝利の条件の 75% を超えていると ×4**（攻撃的 AI では ×6）
- **態度による倍率**: 激怒 ×16、不満 ×8、警戒 ×4、満足 ×2、友好 ×1
- 自分が文化勝利を狙っていると ÷3（CULTURE2）〜÷8（CULTURE3）

## 4. 狙う都市（`CvPlayerAI::AI_targetCityValue`、CvPlayerAI.cpp:2654）

- 人口 ×（50% + その都市にある自分の文化の割合）
- 防御力が削られている、沿岸（+1）、**稼働中の世界遺産**（1 + 遺産の数）、**聖地**（+2）
- **かつて自分の都市だった**（+3）
- 都市圏にある資源の数、自分の領土のタイル数、自分の領土に接するタイル数、自分の最寄りの都市からの近さ
- 近くにいる攻撃部隊の数と、人口に比例する乱数

## 5. 世界遺産

- **`iWonderConstructRand`（指導者）** が建設の熱心さを決める
  - 建物を選ぶとき、生産力が上位の都市では、この値の範囲の乱数が評価値に加わる。孤立した大陸では 2 倍（CvCityAI.cpp:2683-2701）
  - 都市が 3 つ以下で、危険がなく、まだ遺産を持っていない都市では、「乱数/5 + 7 ターン」以内に完成できる遺産を優先して建てる（CvCityAI.cpp:1256）
- **技術の取引**: 自分のチームが建設中の遺産の前提技術は、取引を拒否する（`DENIAL_MYSTERY`、CvTeamAI.cpp:1380）。まだ誰も建てていない遺産を開く技術は、取引での評価が +50 になる（CvTeamAI.cpp:3913）
- 大技術者は、遺産を一度に完成させられるならラッシュに使う（CvUnitAI.cpp:9594）
- **難易度による補正**: AI の建物と訓練のコストは `iAIConstructPercent` / `iAITrainPercent` で下がる（DEITY で 60%）。一方、**世界遺産の `iAIWorldConstructPercent` は NOBLE 以上ではすべて 100%**。遺産は難易度による割引を受けない（適用は CvPlayer.cpp:7069-7132）

| 難易度 | iAttitudeChange | iAIDeclareWarProb | iAITrainPercent / iAIConstructPercent | iAIWorldConstructPercent |
|---|---|---|---|---|
| SETTLER | 2 | 25 | 160 | 160 |
| CHIEFTAIN | 1 | 50 | 130 | 130 |
| WARLORD | 0 | 75 | 110 | 110 |
| NOBLE | -1 | 100 | 100 | 100 |
| PRINCE | -1 | 100 | 95 | 100 |
| MONARCH | -1 | 100 | 90 | 100 |
| EMPEROR | -1 | 100 | 85 | 100 |
| IMMORTAL | -1 | 100 | 80 | 100 |
| DEITY | -1 | 100 | 60 | 100 |

（BtS 本体の `CIV4HandicapInfo.xml` の値。`HANDICAP_` を省略）

## 6. その他の難易度による隠れた補正（HandicapInfo）

成長（`iAIGrowthPercent`）、労働の速さ（`iAIWorkRateModifier`）、ユニットの維持費・補給・アップグレード（`iAIUnitCostPercent`・`iAIUnitSupplyPercent`・`iAIUnitUpgradePercent`）、社会制度の維持費（`iAICivicUpkeepPercent`）、インフレ（`iAIInflationPercent`）、厭戦感情（`iAIWarWearinessPercent`）、時代ごとの補正（`iAIPerEraModifier`）、開始時のユニット（`iAIStartingUnitMultiplier` など）、動物・蛮族との戦闘ボーナス（`iAIAnimalBonus` / `iAIBarbarianBonus`）。

## Mod で使うときのメモ

- 上の指導者のタグ（`iBaseAttitude`・`iMaxWarRand`・`iWonderConstructRand` など）は、すでに `CIV4LeaderHeadInfos.xml` で設定できる
- 新しいタグにできそうなのは、志向から「隠れた態度」や「戦争で狙われやすさ」（`AI_startWarVal` や `AI_targetCityValue` への補正）を変える値
