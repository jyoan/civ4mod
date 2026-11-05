# 検証用データ


| 項目 | 複製元 | 内容 |
|---|---|---|
| `CIVILIZATION_TEST`（検証用文明） | 日本 | プレイ可・**AI は選ばない**（`bAIPlayable=0`）。色 `PLAYERCOLOR_GOLD_AND_BLACK`。固有ユニット・固有建物なし。文明の志向 `TRAIT_TEST`、首都に `BUILDING_TEST` が無料、開始ユニットに `UNIT_TEST_A`。初期の社会制度は他の文明と同じ（専制政治など）で、`CIVIC_TEST` は革命で採用する（無政府 1 ターン） |
| `LEADER_TEST`（検証用指導者） | 徳川 | **指導者の志向なし**（検証の邪魔をしないため） |
| `TRAIT_TEST`（検証用志向） | — | 志向のタグをすべて持つ: 労働者速度 +50%、社会制度風の数値 9 個と配列 4 個（新ユニット経験値 +2、軍事生産 +10% など）、白兵戦に経験値 +2・生産 +25%・アップグレード割引 50%・領土外アップグレード（敵国でも可）、昇進の組（白兵戦 → `PROMOTION_TEST`、海洋 → 航海術Ⅰ、弓兵 → `PROMOTION_TEST_COMBAT`）、タイルの産出（砂漠・ツンドラ・雪原に生産 +1、丘陵に食料 +1、山岳に生産 +2、氷河に食料 +1・商業 +1、森に商業 +1、農場に生産 +1、鉱山に商業 +1、川沿いに食料 +1）、都市圏の半径 +1（文化レベル未熟以上の都市）、召喚したユニットが消えるまで +1 ターン、新しい都市の人口 +1・新しい都市に国教・占領した都市の反乱期間 -50%、施設を作っても森・ジャングルが残る、自国の領土の森・ジャングルを道路として扱う、黄金時代の期間 +50%・黄金時代の偉人ポイント +50%、首都にある建物の生産 +25%、略奪と都市の征服で得る資金 +200%、石材・馬・鉄のタイルに生産 +1、新しい都市は文化レベル未熟から、城・大図書館・僧院が陳腐化しない（`NoObsoleteBuildingClasses` / `NoObsoleteSpecialBuildings`。`bIgnoreBuildingObsolescence` は書いていない。確かめるときは一時的に `1` にする）。Python フック（説明は `Help` の `TXT_KEY_TRAIT_TEST_HELP`）: ゲーム開始時に資金 +100（開始時代が 1 つ後になるごとに +50。ゲーム速度で変わる: `scaleBySpeed(..., 'Train')`。普通 100、エピック 150）、5 ターンごとに首都に戦士、建設した都市に城壁、獲得した都市に兵舎。建設できる都市は「1 + マップの目標都市数の 40%」まで（`iMaxCities` 1・`iMaxCitiesPercent` 40。デュエル・小 2、やや小・標準・大・特大 3）。ユニットの生産コスト -20%・建造物の生産コスト -50%（例: 普通の速度で戦士 15 → 12、兵舎 50 → 25）。条件つきの志向（`ConditionalFreeTraits`）: 戦時は攻撃志向、平時は哲学志向、仏教が国教なら宗教志向、国教なしなら創造志向、古典～中世なら拡張志向、平時に検証用社会制度を採用していれば組織志向。自国の領土では他国のユニットが略奪できない（`bNoPillageInBorders`）。奴隷制を採用できない（`ForbiddenCivics`）。国庫の利子 3%（最大 25）。湖のタイルに生産 +1（`LakePlotYieldChanges`）。平和の不満: 毎ターン 5%・戦争中は毎ターン -10%・上限 20%（`iPeaceAngerPerTurn` 50、`iPeaceAngerDecayPerTurn` 100、`iPeaceAngerMax` 200）。怒っている市民 1 人につき研究 +1（`AngryCommerceChanges`）、不衛生 1 につき食料 +1（`UnhealthyYieldChanges`）。文化圏の広がり +1（`iCultureRangeChange`） |
| `CIVIC_TEST`（検証用社会制度） | 専制政治 | 政治の区分。前提技術なし・無政府期間 0・維持費なし・AI 評価 -10000。白兵戦に経験値 +1・生産 +10%・アップグレード割引 25%、弓兵に領土外アップグレード（敵国の領土を除く）。前提の志向 `TRAIT_TEST`。採用している間、金融志向を得る（`FreeTraits`。検証用志向に金融志向はない）。怒っている市民 1 人につき商業 +1（`AngryYieldChanges`）、不衛生 1 につき文化 +1（`UnhealthyCommerceChanges`） |
| `BUILDING_TEST` / `BUILDINGCLASS_TEST`（検証用建物） | 兵舎 | 前提技術なし・元の効果（攻撃志向の生産 2 倍、陸上ユニット経験値 +3）は削除・AI 評価 -10000。騎乗に領土外アップグレード。前提の志向 `TRAIT_TEST` |
| `BUILDING_TEST_RADIUS` / `BUILDINGCLASS_TEST_RADIUS`（検証用建物（都市圏）） | 検証用建物 | `iPlotRadius` 3（文化レベル未熟以上で）。前提の志向なし（どの文明も建てられる）・費用 20。検証用文明は志向で半径 3 になるので、建物の効果は他の文明（日本など）で確かめる |
| `BUILDING_TEST_CITY` / `BUILDINGCLASS_TEST_CITY`（検証用建物（新しい都市）） | 検証用建物（都市圏） | 都市圏の半径の代わりに、新しい都市の人口 +2・新しい都市に国教・占領した都市の反乱期間 +100%。前提の志向なし・費用 20。新しい都市は文化レベル発展途上から。志向と合わせると、検証用文明の新しい都市は人口 4（1 + 志向 1 + 建物 2）、反乱期間は +50%（-50 + 100）、文化レベルは発展途上（志向の未熟より高い方）。陳腐化していないものをどこかの都市に持つ間、博愛志向を得る（`FreeTraits`）。世界遺産の数に比例する効果: この都市の世界遺産 1 つにつき商業 +4・金 +1、所有する世界遺産 1 つにつき生産 +1・文化 +1 |
| `BUILDING_TEST_GREAT_PEOPLE` / `BUILDINGCLASS_TEST_GREAT_PEOPLE`（検証用建物（偉人）） | 検証用建物（新しい都市） | 新しい都市の効果の代わりに、前提「定住した偉大な商人 1 人 と 偉大な芸術家 1 人」（`PrereqAndSettledSpecialists`）と「定住した偉大な科学者 1 人 または 偉大な技術者 2 人」（`PrereqOrSettledSpecialists`）。効果はない。前提の志向なし・費用 20。偉人はワールドビルダーで置いて定住させる |
| `BUILDING_TEST_PREREQ` / `BUILDINGCLASS_TEST_PREREQ`（検証用建物（前提）） | 検証用建物（偉人） | 偉人の前提の代わりに、前提「兵舎 または 穀物庫」（`PrereqOrBuildingClasses`）、「検証用建物（都市圏）がない」（`PrereqNotInCityBuildingClasses`）、人口 3 以上（`iPrereqPopulation`）、文化レベル未熟以上（`PrereqCultureLevel`）。効果はない。前提の志向なし・費用 20。どの文明も兵舎と検証用建物（都市圏）を技術なしで建てられる |
| `BUILDING_TEST_PREREQ_PLOT` / `BUILDINGCLASS_TEST_PREREQ_PLOT`（検証用建物（都市圏の前提）） | 検証用建物（前提） | 前提の代わりに、都市圏に「丘陵 または 山岳」（`PrereqOrTerrains`）、「森 または ジャングル」（`PrereqOrFeatures`）、「鉄 または 銅」（`PrereqRawVicinityBonuses`）、「開発済みの鉄 または 銅」（`PrereqVicinityBonuses`）、「農場 または 小屋」（`PrereqOrImprovements`）。はじめの 3 つを満たすまで一覧に出ず、残り 2 つを満たすまで灰色。効果はない。前提の志向なし・費用 20。タイルと資源はワールドビルダーで置く |
| `BUILDING_TEST_PREREQ_CIVIC` / `BUILDINGCLASS_TEST_PREREQ_CIVIC`（検証用建物（社会制度の前提）） | 検証用建物（前提） | 前提の代わりに、「原始部族社会 と 精霊崇拝」（`PrereqAndCivics`。初期の社会制度なので最初は満たす）と「世襲統治 または 検証用社会制度」（`PrereqOrCivics`）。効果はない。前提の志向なし・費用 20。検証用文明なら検証用社会制度を技術なしで採用できる |
| `BUILDING_TEST_GROWTH` / `BUILDINGCLASS_TEST_GROWTH`（検証用建物（成長）） | 検証用建物（前提） | 前提なし（どの文明も最初から建てられる）・費用 20。技術で伸びる効果: `TECH_POTTERY` で金 +2・研究 +1、`TECH_MINING` で生産 +2、`TECH_MYSTICISM` で幸福 +2、`TECH_MASONRY` で不幸 +1（幸福 -1）、`TECH_HUNTING` で衛生 +2、`TECH_THE_WHEEL` で不衛生 +1（衛生 -1）、`TECH_WRITING` で科学者の枠 +1 |
| `BUILDING_TEST_GROWTH_BONUS` / `BUILDINGCLASS_TEST_GROWTH_BONUS`（検証用建物（資源で成長）） | 検証用建物（成長） | 前提なし・費用 20。銅を使えると金 +2（`BonusCommerceChanges`）、鉄を使えると生産 +1（`BonusYieldChanges`）、都市圏に鉄が開発済みなら食料 +2（`VicinityBonusYieldChanges`）。鉄の鉱山を道路でつながないと、都市圏の分だけが効く |
| `BUILDING_TEST_GROWTH_POP` / `BUILDINGCLASS_TEST_GROWTH_POP`（検証用建物（人口で成長）） | 検証用建物（成長） | 前提なし・費用 20。人口 2 につき幸福 +1（`iHappinessPercentPerPopulation` 50）、人口 4 につき不衛生 +1（`iHealthPercentPerPopulation` -25） |
| `BUILDING_TEST_GROWTH_GLOBAL` / `BUILDINGCLASS_TEST_GROWTH_GLOBAL`（検証用建物（全都市の商業）） | 検証用建物（成長） | 前提なし・費用 20。持つ間、全都市の兵舎 1 つにつき金 +1、検証用建物（都市圏）1 つにつき研究 +2（`GlobalBuildingExtraCommerces`）。`TECH_WRITING` で陳腐化する（陳腐化で効果が消えることの確認用） |
| `BUILDING_TEST_MISERY` / `BUILDINGCLASS_TEST_MISERY`（検証用建物（怒りの恩恵）） | 検証用建物（全都市の商業） | 前提なし・費用 20。怒っている市民 1 人につき生産 +1（`AngryYieldChanges`）、不衛生 1 につき金 +2（`UnhealthyCommerceChanges`） |
| `TECH_TEST`（検証用技術） | 神秘主義 | 技術ツリーの 1 列目の空き（`iGridX` 1・`iGridY` 9）。前提なし・費用 10・古代。交換できない・小屋から出ない（`bTrade` / `bGoodyTech` 0）・AI 評価 -10000。チームが持つ間、検証用志向を得る（`FreeTraits`）。日本などで研究すると検証用志向の効果がすべて付く（検証用文明は元から持つので変わらない） |
| `IMPROVEMENT_TEST_TERRACE`（検証用施設（棚田）） | 農場 | 隣接する山岳 1 つにつき食料 +1・生産 +1（`AdjacentPeakYieldChanges`）。作業は `BUILD_TEST_TERRACE` |
| `IMPROVEMENT_TEST_FARM`（検証用施設（強化農場）） | 農場 | 灌漑で食料 +2（農場は +1）。ほかは農場と同じ。`ReplacedImprovement` が農場なので、検証用志向の「農場に生産 +1」を受ける |
| `IMPROVEMENT_TEST_TUNNEL`（検証用施設（トンネル）） | 検証用施設（棚田） | 山岳だけに作れる（`TerrainMakesValids`）。`bMakesPassable` 1、`iPassageMoveCost` 1。産出なし。作業は `BUILD_TEST_TUNNEL` |
| `BUILD_TEST_FARM`（検証用施設（強化農場）の建設） | 農場の建設 | 前提の志向 `TRAIT_TEST`、`ReplacedBuild` が農場の建設（置換のパターン）。検証用志向を持つと農場の建設の代わりに出る。前提技術は農業のまま |
| `BUILD_TEST_TERRACE`（検証用施設（棚田）の建設） | 農場の建設 | 前提の志向 `TRAIT_TEST`、`UnitsFromBuild` が農場の建設（新規のパターン。労働者の `Builds` は書き換えていない）。前提技術なし、ホットキーなし |
| `BUILD_TEST_TUNNEL`（検証用施設（トンネル）の建設） | 検証用施設（棚田）の建設 | 前提の志向 `TRAIT_TEST`、`UnitsFromBuild` が農場の建設（前提技術なし） |
| `EVENTTRIGGER_TEST_TRAIT` / `EVENT_TEST_TRAIT`（検証用志向の固有イベント） | 森林火災 | 前提の志向 `TRAIT_TEST`（`PrereqOrTraits`）。すべてのゲームで有効、毎ターン必ず判定（`iWeight` -1）、1 回だけ、都市もタイルも選ばない。選ぶと資金 +10。検証用文明は最初のターンに起きる。日本などは検証用技術を得た次のターンに起きる |
| `UNIT_TEST_A` / `UNIT_TEST_B`（検証用ユニット A・B） | 戦士・斧兵 | 白兵戦。前提技術・資源なし。A は B にだけアップグレードできる（1 ターン目からアップグレード関連を試せる）。AI 評価 -10000。B だけ前提の志向 `TRAIT_TEST` または `TRAIT_AGGRESSIVE`（「どれか 1 つ」の確認用。攻撃志向の徳川の日本でも作れる）。B を作れる文明では A は生産候補に出ない（BtS の仕様: アップグレード先がすべて作れるユニットは候補から外れる。`CvCity::canTrain` → `allUpgradesAvailable`）。A のアップグレードは検証用文明の開始ユニットで試す |
| `PROMOTION_TEST`（検証用昇進） | 戦闘術Ⅰ | 戦闘力 +10%。`UnitCombats` が空なので通常のレベルアップでは誰も選べず、昇進の組でだけ付く。勝利すると `PROMOTION_TEST_STATE1` を得る。生き残った戦闘相手に `PROMOTION_TEST_POISON` を与える。毎ターン経験値 +1（10 まで）。撤退率 +100%（攻撃で負けそうになると必ず撤退するので、相手が生き残って毒を受ける）。勝利すると資金 +5、50% で捕獲。自国の領土で戦闘力 +15% |
| `PROMOTION_TEST_STATE1` / `STATE2`（検証用状態Ⅰ・Ⅱ） | 検証用昇進 | 状態の昇進（`UnitCombats` は空）。Ⅰ: 戦闘力 +20%、毎ターン 50% で消える、勝利すると 50% でⅡを得る・資金 +3・捕獲 50%（検証用昇進と合わせて資金 +8・捕獲 100%）。Ⅱ: 戦闘力 +10%、戦闘すると消える。ボタンは戦闘術Ⅱ・Ⅲ |
| `PROMOTION_TEST_POISON`（検証用毒） | 検証用状態Ⅱ | 状態の昇進。戦闘力 -20%、完全に回復すると消える、生き残った戦闘相手に 50% でうつる（毒を受けた敵と戦うと自分も受けることがある）。ボタンは戦闘術Ⅳ |
| `PROMOTION_TEST_COMBAT`（検証用昇進（戦闘結果）） | 検証用状態Ⅱ | 第 4 段階の戦闘結果の昇進（`UnitCombats` は空。検証用志向の昇進の組で弓兵ユニットに付く）。勝利すると 25 回復、50% で奴隷化（労働者。弓兵を倒すと斥候、戦士は捕らえない）、相手の国庫の 10%（最大 30）を奪う、100% で相手の昇進を 1 つ得る。戦闘力の補正はなし。ボタンはコマンドー |
| `PROMOTION_TEST_PASSAGE`（検証用昇進（通行不可タイル）） | 検証用昇進（戦闘結果） | 状態の昇進（`UnitCombats` は空。ワールドビルダーで付ける）。山岳に入れて最大 HP の 34% のダメージ（3 回で死ぬ）、氷河に入れてダメージなし（`ImpassablePlotPassages`）。戦闘の効果はない。川を渡るたびに最大 HP の 20% のダメージ（`iRiverCrossingDamagePercent`） |
| `PROMOTION_TEST_ICEBREAKER`（検証用昇進（砕氷船）） | 検証用昇進（通行不可タイル） | 状態の昇進（ワールドビルダーで船に付ける）。氷河に入れ、移動コスト +1、入ると氷河が消える（`iExtraMoveCost`・`bRemoveFeature`）。ダメージなし |
| `SPELL_TEST_GOLD`（検証用呪文（資金）） | — | 検証用文明の白兵戦ユニットが、自国の領土で資金 +25（`spellGainGold(pCaster, 25)`。`PyRequirement` なし）。使えないときも無効のボタンで出る（`bDisplayWhenDisabled`）。実行済みになる。ボタンは「富」 |
| `SPELL_TEST_PROMOTION`（検証用呪文（昇進）） | — | 検証用昇進を持つユニットが、都市で、検証用状態Ⅰを持っていなければ得る（`reqNotHasPromotion` / `spellAddPromotion`）。使えないときはボタンが出ない。実行済みになる。ボタンは戦闘術Ⅱ |
| `SPELL_TEST_CONVERT`（検証用呪文（変身）） | — | ユニット A が、資金 10 で検証用状態Ⅱを得てからユニット B に変わる（状態Ⅱ・経験値・実行済みは B に引き継ぐ。B はそのターン動けない）。演出 `EFFECT_SETTLERSMOKE`、音 `AS2D_GOODY_WARRIOR`。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_CITY`（検証用呪文（都市））| — | 検証用建物のある都市（検証用文明の首都）で、オベリスクを建て、仏教を広め、人口 +1、資金 +50（`iCost` -50）。実行したユニットは消える。オベリスクがあると使えない（ボタンも出ない）。音 `AS2D_GOODY_GOLD` |
| `SPELL_TEST_FOREST`（検証用呪文（森）） | — | 検証用建物を持つ文明が、自国の領土の水に隣接したタイルで、資金 5 で森を作り、検証用毒を外す（どちらかが効けば使える）。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_FORT`（検証用呪文（砦）） | — | 検証用文明の、検証用昇進を持たないユニット（開拓者など）が、検証用昇進を持つ自国のユニットと同じタイルにいるとき、森かジャングルのタイルに砦を作る。使えないときも無効のボタンで出る（同じタイルの条件を満たさないとボタンは出ない）。実行済みになる |
| `SPELL_TEST_REFRESH`（検証用呪文（再実行）） | — | 検証用文明のユニットが、砦のタイルで、資金 20 で実行済みを取り消す（`bIgnoreHasCasted`・`bRemoveHasCasted`）。実行済みでないときは無効のボタン（砦のタイルでないとボタンは出ない） |
| `SPELL_TEST_BLESS`（検証用呪文（祝福）） | — | 検証用文明のユニットが、1 タイル以内の自国（と同じチーム）のユニットに検証用状態Ⅱを与え、検証用毒を外す（`bImmuneEnemy`。兵科のないユニットには状態Ⅱは付かない）。演出 `EFFECT_PROMOTION_UNIT`。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_FIREBALL`（検証用呪文（火球）） | — | 検証用文明のユニットが、周囲 1 タイル（自分のタイルを除く）の交戦中の相手（動物・蛮族）に 40 前後のダメージ（`bImmuneTeam`。上限なしなので倒すこともある）。演出 `EFFECT_ARTILLERY_SHELL_EXPLODE`。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_SNARE`（検証用呪文（足止め）） | — | 検証用文明のユニットが、周囲 2 タイル（自分のタイルを除く）の交戦中の相手を 2 ターン動けなくし、10 前後のダメージ（25 まで）を与える（`bImmuneTeam`・`bImmuneFlying`）。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_SUMMON`（検証用呪文（召喚）） | — | 検証用文明の、検証用昇進を持つユニットが、戦士を 2 体召喚する（2 ターンで消える: 既定 1 + 検証用志向 1）。戦士は術者の昇進をすべて得る（`bCopyCastersPromotions`）。召喚した戦士は実行できない（`bCasterNoDuration`）。25% で失敗し、術者が 20 のダメージ（`spellDamageCaster`）。演出 `EFFECT_SETTLERSMOKE`。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_PERMANENT`（検証用呪文（永続召喚）） | — | 検証用文明の、検証用昇進を持つユニットが、資金 10 で、次のターンの初めに探検家を召喚する（`iDelay` 1。消えない。検証用状態Ⅱが付く。召喚したターンは動けない）。検証用昇進を持つ自国のユニットの数まで。使えないときも無効のボタンで出る。実行済みになる |
| `SPELL_TEST_WORLD`（検証用呪文（世界）） | — | 検証用文明のユニットが、資金 +100（世界呪文。確認のポップアップ。使うとボタンが出なくなる）。音 `AS2D_GOODY_GOLD`。実行済みになる |
| `SPELL_TEST_TRAIT`（検証用呪文（志向）） | — | 検証用昇進を持つユニット（どの文明でも。日本のユニットにはワールドビルダーで検証用昇進を付ける）が、持ち主に検証用志向を与える。与えてあれば取り上げる（`spellToggleFreeTrait(pCaster, 'TRAIT_TEST')` → `CyPlayer.changeFreeTraitCount`）。実行済みにならないので、同じターンに何度でも切り替えられる。AI は使わない。検証用文明は検証用志向を元から持つので、使っても効果は変わらない（二重に効かないことの確認）。ボタンは「研究」 |

検証用呪文は、検証用呪文（砦）以外すべて `bAllowAI`（AI も使う。確かめるには AI に検証用文明を割り当てる）。Python の効果しかない資金・昇進と、敵がいないと評価が 0 の永続召喚は `iAIWeight` を持つ（資金 100・昇進 50・永続召喚 100）。砦は `bAllowAI` がない（評価は正なので、AI が使わないことで `bAllowAI` を確かめる）。再実行は評価が負（資金 20）なので AI は使わない

検証用文明が `CIVIC_TEST` を採用したあと、白兵戦ユニット（都市で生産）で期待される値: 生産直後の経験値 5（志向 +2、白兵戦 +2、社会制度 +1。その後は毎ターン +1 で 10 まで増える）、ユニット A → B のアップグレード割引 75%（志向 50% + 社会制度 25%）、`PROMOTION_TEST` が付く

ゲームルールの確認: `GlobalDefinesAlt.xml` の `RIVER_CROSSING_MOVE_COST` はコミット時は 0（BtS と同じ）。確かめるときだけ一時的に 1 にする
