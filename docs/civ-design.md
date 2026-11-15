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
| 攻撃 | 略奪で得る資金 +100%、都市を征服したときの資金 +100% | `iPillageGoldModifier` 100、`iCityCaptureGoldModifier` 100 |
| 防衛 | 自国の領土では敵が略奪できない、弓兵のアップグレード費 -50% | `bNoPillageInBorders`、`UnitCombatUpgradeDiscounts` 弓兵 50 |
| 拡張 | 新しい都市の人口 +1、労働者の作業速度 +25% | `iNewCityPopulation` 1、`iWorkerSpeedModifier` 25 |
| 金融 | 国庫の利子 毎ターン 1%（上限なし）、交易路の収入 +25% | `iGoldInterestPercent` 1、`TradeYieldModifiers` 商業 25 |
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
| `iPillageGoldModifier`、`iCityCaptureGoldModifier` | 攻撃志向 |
| `bNoPillageInBorders`、`UnitCombatUpgradeDiscounts` | 防衛志向 |
| `iNewCityPopulation`、`iWorkerSpeedModifier` | 拡張志向 |
| `iGoldInterestPercent`、`TradeYieldModifiers` | 金融志向 |
| `iDistanceMaintenanceModifier`、`iNumCitiesMaintenanceModifier` | 組織志向 |
| `SpecialistExtraCommerces` | 哲学志向 |
| `iReligionSpreadChange` | 宗教志向 |
| `NewCityCultureLevel` | 創造志向 |
| `iMaxNationalWondersPerCity`、`FeatureProductionChanges` | 勤労志向 |
| `iAttitudeChange`、`iWarWearinessModifier` | カリスマ志向 |
| `iOccupationTimeModifier`、`iOverseasMaintenanceModifier` | 帝国主義志向 |
| `FreePromotionSets` + `bNoDamagePenalty` の昇進 | 日本の武士道志向 |
| `bInlandCanals` | メフメド２世（確定、未実装） |
| `LakePlotYieldChanges` | アステカ（確定、未実装） |
| `iEnslavementChance` の昇進 | モンテスマ（確定、未実装） |
| `iAnarchyGoldenAgePercent`、`iCombatPercent` | フランス（確定、未実装） |
| `bRiverNavigation`・`bFreeEmbark` の昇進 | ヴァイキング（確定、未実装） |
| `bPillageNoMoveCost`・`iPillageHealPercent` の昇進 | ラグナル（確定、未実装） |
| `WonderHurrys`、`bIgnoreWonderHurryCostModifier` | 始皇帝（確定、未実装） |

### 新しい志向で決まったこと（2026-11-15）

| 対象 | 内容 | タグ | 状態 |
|---|---|---|---|
| メフメド２世 | 艦隊の山越え: 艦船が、水に面していない要塞・都市にも入れる | `bInlandCanals` | 確定 |
| アステカ（文明） | テノチティトラン: 湖のタイルの産出が増える | `LakePlotYieldChanges` | 確定 |
| モンテスマ | 花の戦争: 倒した敵を捕虜（労働者）にする | 昇進の `iEnslavementChance` | 確定 |
| フランス（文明） | 革命: 無政府期間が明けると黄金期。無政府期間中は全ユニットの戦闘力が上がる | `iAnarchyGoldenAgePercent`、`iCombatPercent` を持つ志向を `ConditionalFreeTraits` の `bAnarchy` で与える | 確定（タグは master で作成済み、志向は未実装） |
| ヴァイキング（文明） | 古典時代までの艦船が川を進める。乗船・下船で移動力を余分に使わない | `FreePromotionSets` で、`bRiverNavigation` の昇進を作業船・ガレー船・三段櫂船に、`bFreeEmbark` の昇進を陸上ユニットに配る | 確定（タグは master で作成済み、志向は未実装） |
| ラグナル | 略奪で移動力を使わない。略奪するとユニットが回復する | `FreePromotionSets` で、`bPillageNoMoveCost`・`iPillageHealPercent` の昇進を陸上の戦闘ユニットに配る | 確定（タグは master で作成済み、志向は未実装） |
| 始皇帝 | 奴隷制なしで、遺産（世界遺産・国家遺産）を人口で緊急生産できる。遺産の緊急生産に追加の費用がかからない（人口でも資金でも。生産 0 のときの +50% と人口を使ったときの不満は残る） | `WonderHurrys`（`HURRY_POPULATION`）、`bIgnoreWonderHurryCostModifier` | 確定（タグは master で作成済み、志向は未実装） |
| サラディン | 占領した都市に国教の建物を建てる | `ConquestReligionBuildings` など（`iConquestBuildingSurviveModifier` も候補） | 方向だけ決定 |

- 始皇帝の最初の案「長城線」（万里の長城が陳腐化しない）は取り下げた。BtS の万里の長城はもともと陳腐化しない（`ObsoleteTech` が `NONE`）

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
