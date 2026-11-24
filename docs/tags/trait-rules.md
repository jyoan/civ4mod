# 志向のタグ（ルール）

社会制度・技術・陳腐化・外交（`FoundingVassal`・`Deals` を含む）・固有ユニット・緊急生産・ランダムイベント。Python フックと `ConditionalFreeTraits` は `trait-hooks.md`。

## `TraitInfo/iReligionSpreadChange`・`iCorporationSpreadChange`

- ファイル: `CIV4TraitInfos.xml`
- そのプレイヤーの宣教師の布教（`CvUnit::spread`）と幹部の企業の進出（`CvUnit::spreadCorporation`）の成功率（%）に足す。
- 本体の成功率（ユニットの `ReligionSpreads`・`CorporationSpreads`、他チームの都市なら半分、都市の宗教・企業が少ないほど上がる）を計算した後に足すので、100 なら必ず成功する。
- 志向の合計。
- 企業の進出の費用は本体どおり。
- 都市から自然に広まる宗教の伝播（`CvCity::doReligion`）は対象外。
- 成功率は画面に出ない（本体と同じ）。
- 説明文: 100 以上は `TXT_KEY_TRAIT_RELIGION_SPREAD_ALWAYS` / `TXT_KEY_TRAIT_CORPORATION_SPREAD_ALWAYS`、それ以外は `_CHANGE`

## `TraitInfo/ForbiddenCivics`

- ファイル: `CIV4TraitInfos.xml`
- `<CivicType>` を何個でも書ける。
- その志向を持つプレイヤーは、書いた社会制度を採用できない（ex 氏の CivGoldEmpires の機巧志向「奴隷制採用不可」。ある区分を 1 つに固定するなら、その区分のほかの社会制度をすべて書く）。
- `CvPlayer::canDoCivics`（`PrereqOrTraits` の後）で判定し、社会制度の画面では採用できない社会制度として出る。
- 技術を得たときの採用のポップアップは出ない（`canRevolution` の修正。`misc.md` の「BtS 本体の修正」）。
- ゲームの途中でその志向を得たときは、BtS の `verifyCivics` がターンの初めに、その区分で採用できる最初の社会制度に無政府期間なしで変える（採用できるものがなければそのまま）。
- 初期の社会制度は止めない。
- 説明文: 志向に `TXT_KEY_TRAIT_FORBIDDEN_CIVICS`（「採用できない社会制度: 奴隷制」）、社会制度に `TXT_KEY_CIVIC_FORBIDDEN_TRAITS`（「次の志向を持つと採用できない: 検証用志向」。ペディアではすべての志向、ゲーム中は自分の志向だけ）。
- 社会制度は志向より後に読み込まれるので `readPass3()` で番号に変える。
- そのつど判定（`CvPlayer::isForbiddenCivic`）

## `TraitInfo/CivicOptionExtraSlots`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-22。
- `<CivicOptionExtraSlot>` に `CivicOptionType` と `iExtraSlots` を何個でも書ける。
- その分類の社会制度を同時に `1 + iExtraSlots` 個採用できる（複数の志向は合計。上限は `MAX_CIVIC_SLOTS` = 3。`CvPlayer::getNumCivicSlots`）。
- 書いていない分類は 1 個のまま。
- **仕組み**: `CvPlayer::m_paeCivics` を「分類 × 枠」の配列にした（添字は `iSlot * 分類の数 + 分類`。先頭の分類の数ぶんが枠 0 = BtS の `getCivics`。空の枠は `NO_CIVIC`）。
- `revolution`・`canRevolution`・`getCivicAnarchyLength`・`getCivicUpkeep` の配列も同じ長さ（`CvPlayer::getNumCivicEntries()`）。
- `isCivic` と、社会制度の値を直接合計していた箇所（兵科の経験値・生産・アップグレード割引・領土外アップグレード、内訳の説明文、財務の維持費、外交画面のアイコン）はすべての枠を見る。
- **ルール**: 同じ制度を 2 枠に入れられない。
- 2 つ目以降の枠は空でもよい（ゲームの途中で志向を得た直後は空）。
- 維持費は採用している制度ごと。
- 無政府は分類の制度の組が変わったら「1 分類の変更」（枠の入れ替えだけなら変更なし）で、まだ採用していない制度ごとにその制度の `iAnarchyLength` を足す。
- 決議でその分類が強制されている間は枠 0 だけ（ほかの枠は空にする）。
- 志向を失ったら、なくなった枠の制度を無政府なしで外す（`verifyCivicSlots`。`setCivicSlot` や `revolution` の途中では待ち、最後にまとめて外す。政治の制度を変えて `PrereqCivic` の志向を失うと、同じ革命で選んだ枠も外れる）。
- 外交の「制度を変えよ」・技術を得たときのポップアップ・AI・諜報・Python の `setCivics` は、制度をすでにどこかの枠で採用していなければ枠 0 に入れる（2 枠目にある制度を `setCivics` で指定すると、外さずに枠 0 へ移し、枠 0 の制度を外す）。
- AI は 2 つ目以降の枠を使わない。
- **画面**（`CvCivicsScreen.py`）: 枠が 2 以上の分類は見出しに「(2)」と出て、クリックで追加・外す（最後の 1 つは外せない。いっぱいなら最後の枠と入れ替え）。
- カーソルを乗せると、クリックした場合の無政府・維持費が出る。
- Python: `CyPlayer.getNumCivicSlots(分類)`・`getCivicSlot(分類, 枠)`・`getMaxCivicSlots()`。
- `getCivicAnarchyLength`・`getCivicUpkeep`・`sendUpdateCivics` には全枠のリストを渡す（分類の数だけのリストなら、ほかの枠は今のまま）。
- セーブ: `CvPlayer` の `uiFlag` 16（古いセーブは 2 つ目以降の枠が空）。
- 説明文 `TXT_KEY_TRAIT_CIVIC_OPTION_EXTRA_SLOTS`

## `TraitInfo/PeaceAnger`

- ファイル: `CIV4TraitInfos.xml`
- `<PeaceAnger>` に `iPerTurn`（必須）、`iDecayPerTurn`（省略可）、`iMax`（必須）を書く。
- 2026-11-21 に親タグにまとめた。
- 平和だと都市が不満になる（厭戦感情の逆）。
- 文明（マイナー文明を除く）と戦争していないターンに `iPerTurn` 増え、戦争中のターンに `iDecayPerTurn` 減る。
- 上限が `iMax`（0 なら効果なし）。
- 単位は社会制度の `iCivicPercentAnger` と同じ人口の 0.1%（50 で 5%）。
- 蛮族との戦争は数えない。
- 増減はゲーム速度の `iGrowthPercent` で割る（マラソンは 3 分の 1）。
- 厭戦感情の修正は受けない。
- 複数の志向の値は足す。
- 値はプレイヤーごとに `CvPlayer::doPeaceAnger()` で毎ターン更新し、セーブに入る（`CvPlayer` の `uiFlag` 6）。
- Python は `getPeaceAngerPercentAnger()`。
- 説明文は `TXT_KEY_TRAIT_PEACE_ANGER`、都市の怒りの内訳は `TXT_KEY_ANGER_PEACE`

## `TraitInfo/UniqueUnits`、`UniqueBuildings`

- ファイル: `CIV4TraitInfos.xml`
- 志向の固有ユニット・固有建造物。
- `<UniqueUnit><UnitClassType>…</UnitClassType><UnitType>…</UnitType></UniqueUnit>` の形で、文明の XML と同じくユニットクラス（建物クラス）ごとに書く。
- ユニットはそのクラスのものであること（違うと起動時にエラー）。
- **文明の固有より志向が優先**で、複数の志向が同じクラスを持つと XML の順で先のもの。
- 偉人と大将軍も固有にできる。
- 解決は `CvPlayer::getCivilizationUnit`・`getCivilizationBuilding` で、プレイヤーが分かる処理はすべてこれを通す（Python も `CyPlayer` の同名の関数。文明のペディアなど文明単位の表示は文明の XML のまま）。
- ゲーム中に志向を得ると、それ以降の生産が固有のものに変わる。
- すでにあるユニットと建物はそのまま残り、生産キューの元のものは続けられなくなる。
- BtS の大将軍の生成は全ユニットから `iLeaderExperience` などを持つものをランダムに選ぶだけだったので、そのプレイヤーのユニットに変換するよう直した（文明の固有の大将軍もこれで動く）。
- 説明文は `TXT_KEY_TRAIT_UNIQUE_UNIT`・`TXT_KEY_TRAIT_UNIQUE_BUILDING`。
- ユニットと建物の説明には `TXT_KEY_UNIQUE_UNIT`・`TXT_KEY_UNIQUE_BUILDING` で志向名を出す。
- Mod に入れていない BtS の Python（`CvAdvisorUtils`・`CvRandomEventInterface`・`CvTechSplashScreen`）と、捕獲したユニット（`getCaptureUnitType`）は文明の XML のまま

## `TraitInfo/KnownTechCost`

- ファイル: `CIV4TraitInfos.xml`
- `<KnownTechCost>` に `iModifier`（必須）と `iLimit`（省略可）を書く。
- 2026-11-21 に親タグにまとめた。
- 研究費 %。
- その技術を持つ、出会ったことのある他のチーム（生きている、マイナー文明と蛮族を除く）1 つにつき `iModifier`（-10 なら -10%）。
- 合計は `iLimit`（-75 なら -75% まで）で止める。
- `iLimit` 0 は上限なし（研究費は最低 1）。
- チームの研究費（`CvTeam::getResearchCost`）に掛ける。
- チームのメンバーのうちいちばん大きい割引を使う（`CvTeam::getKnownTechCostModifier`）。
- BtS の「技術を知っている国の数による研究速度」（研究力に足す）とは別に効く。
- 説明文は `TXT_KEY_TRAIT_KNOWN_TECH_COST_LIMIT` / `TXT_KEY_TRAIT_KNOWN_TECH_COST`

## `TraitInfo/iAnarchyGoldenAgePercent`

- ファイル: `CIV4TraitInfos.xml`
- 社会制度・宗教の変更による無政府期間が終わると、その無政府期間のターン数のこの % の黄金期になる（`CvPlayer::changeAnarchyTurns` で無政府が終わったとき）。
- ゲームの速度は BtS の黄金期と同じ比率で効かせる: 無政府のターン数を速度の `iAnarchyPercent` で割って通常の速度に直し、`iGoldenAgePercent` を掛ける（エピックは無政府 150%・黄金期 125%、クイックは 67%・80%）。
- 黄金期の期間 % `getGoldenAgeModifier` もかかる。
- 1/100 ターンの精度で計算して最後に四捨五入、最低 1 ターン。
- マップサイズ・都市数・開始時代は無政府の長さを通して効く（志向独自の補正はない）。
- 数えるのは実際に無政府で過ごしたターン数で、社会制度・宗教の変更で足された無政府のターン数まで（スパイの任務による無政府は数えない）。
- 無政府にならなければ黄金期もない（宗教志向やキリスト像では起きない）。
- 数えている値は `CvPlayer` のセーブ（`uiFlag=9`）に入る。
- フランスの「革命」の案

## `TraitInfo/iTechCostModifier`、`iAttitudeChange`

- ファイル: `CIV4TraitInfos.xml`
- RFC の固有能力から。
- `iTechCostModifier`: 技術の研究コスト %（`CvTeam::getResearchCost` の最後。チームの生きているメンバーの値の平均。RFC のマヤ「中世の終わりまで -40%」は `ConditionalFreeTraits` の `EraMax` と組み合わせる）。
- `iAttitudeChange`: ほかのプレイヤー（AI）からこのプレイヤーへの態度 +N（`CvPlayerAI::AI_getAttitudeVal`。外交画面の態度の内訳に「志向による印象」の行。RFC のフランス）

## `TraitInfo/iMaxNationalWondersPerCity`、`iMaxNationalWondersPerCityForOCC`

- ファイル: `CIV4TraitInfos.xml`
- 1 都市に建設できる国家遺産の数の上限。
- GlobalDefines の `MAX_NATIONAL_WONDERS_PER_CITY`（通常 2）・`MAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC`（1 つの都市でチャレンジの人間のプレイヤー。通常 5）を、その志向を持つプレイヤーについて**置き換える**（緩める値だけでなく、define より小さい値も効く）。
- `-1` で無制限、`0`（省略時）は指定なしで define のまま。
- 複数の志向が値を持つときは最も緩いもの（`-1` が最優先、ほかは最大値）。
- `CvPlayer::getMaxNationalWondersPerCity` にまとめ、`CvCity::isNationalWondersMaxed` と建物の説明文（上限に達したときの警告）がこれを使う。
- ゲーム中に志向が変われば即座に反映される（数えるだけで状態を持たない）。
- 上限を下げても建設済みの国家遺産は消えない。
- Python: `CyPlayer.getMaxNationalWondersPerCity()`

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

## `TraitInfo/NoObsolescence`（`bAllBuildings`・`BuildingClasses`・`SpecialBuildings`）

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-23 に `<NoObsolescence>` の子に移し、名前を `bIgnoreBuildingObsolescence`・`NoObsoleteBuildingClasses`・`NoObsoleteSpecialBuildings` から変えた（getter と Python の名前は元のまま）。
- その志向を持つプレイヤーの都市では、建物が陳腐化しない。
- `bAllBuildings` はすべての建物、`BuildingClasses`（`<BuildingClassType>` を何個でも。固有建物にも効く）と `SpecialBuildings`（`<SpecialBuildingType>` を何個でも。僧院は宗教ごとに建物クラスが違うので `SPECIALBUILDING_MONASTERY` で書く）は書いたものだけ。
- 建物自身の `ObsoleteTech` と特殊建物の `ObsoleteTech` の**両方**を無視する。
- BtS の陳腐化はチーム単位（`CvTeam::isObsoleteBuilding`）なので、プレイヤー単位の `CvPlayer::isObsoleteBuilding()`（チームとして陳腐化していて、志向で無視していない）を作り、都市の効果（`CvCity::processBuilding`・`getNumActiveBuilding`・`hasActiveWorldWonder`）、建設の可否（`CvPlayer::canConstruct`。陳腐化しないので技術の後も建てられる）、陳腐化した瞬間に効果を外す処理（`CvTeam::changeObsoleteBuildingCount`。志向を持つプレイヤーの都市は外さない）、チームの合流（`CvTeam::addTeam`）、商業・産出の内訳の表示をこれに置き換えた。
- 同じチームでも、志向を持つメンバーの都市だけ効き続ける。
- 説明文: 志向は `TXT_KEY_TRAIT_IGNORE_BUILDING_OBSOLESCENCE`（「建物が陳腐化しない」）か `TXT_KEY_TRAIT_NO_OBSOLETE_BUILDINGS`（「陳腐化しない: 城, 大図書館, 僧院」。建物はその文明の建物で、リンク付き）。
- ゲーム中は、志向で無視する建物について、建物の「○○によって陳腐化」と技術の「○○を陳腐化させる」を出さない（ペディアでは常に出す）。
- 資源（`TechObsolete`。BtS のデータでは未使用）とイベント（`ObsoleteTechs`）は対象外。
- 建物クラスは志向より後に読み込まれるので、`readPass3()` で番号に変える（このため `CvTraitInfo::readPass3()` を呼ぶ位置を建物クラスの読み込みの後に移した）。
- そのつど判定するのでセーブデータは変わらないが、**既存のセーブで志向を与えると、すでに陳腐化して外された効果は戻らず、その建物が壊されると値がずれる**ので、確認は新しいゲームで行う。
- 志向をゲームの途中で付け外しできるようにするときは、陳腐化した建物の効果の足し引きが必要

## `TraitInfo/NoObsolescence`（`bAllBonuses`・`Bonuses`）

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-23 に `<NoObsolescence>` の子に移し、名前を `bIgnoreBonusObsolescence`・`NoObsoleteBonuses` から変えた（getter と Python の名前は元のまま）。
- 資源の陳腐化を防ぐ（上の項目の `bAllBuildings`・`BuildingClasses` の資源版）。
- `bAllBonuses` はすべての資源、`Bonuses`（`BonusType` を何個でも）は書いた資源。
- BtS で陳腐化する資源は毛皮（プラスチック）、象牙（工業主義）、鯨（内燃機関）。
- 資源の陳腐化はチーム単位（`CvTeam::isBonusObsolete`）なので、チームの生きているプレイヤーの誰かが志向を持てば、チーム全体でその資源が陳腐化しない（`isBonusObsolescenceIgnored`）。
- ゲームの途中で志向を得た・失ったときは、そのチームのタイルの資源を数え直す（`CvPlayer::updateTraitBonusPlots`）。
- 取引は本体どおり、受け取る側で陳腐化していれば取引できない。
- 資源は志向より後に読み込まれるので、リストは `readPass3()` で番号に変える。
- 説明文 `TXT_KEY_TRAIT_IGNORE_BONUS_OBSOLESCENCE` / `TXT_KEY_TRAIT_NO_OBSOLETE_BONUSES`

## `TraitInfo/Hurrys`

- ファイル: `CIV4TraitInfos.xml`
- 緊急生産の解禁と補正（2026-11-19 に `WonderHurrys`・`bIgnoreWonderHurryCostModifier`・`UnitHurrys`・`HurryUnitExperiences` をまとめて作り直した）。
- `<Hurry>` を何個でも書ける。
- 各項目は `HurryType` と、省略可の対象の絞り込み `bUnitsOnly`（ユニット）・`bBuildingsOnly`（建造物すべて）・`bWondersOnly`（世界遺産・チーム遺産・国家遺産）・`bWorldWondersOnly`・`bNationalWondersOnly`（どれもなければユニットと建造物すべて、複数あればそのどれか）、`iCostModifier`（その緊急生産の費用 %）、`bIgnoreCostModifier`（遺産などの追加の費用 `iHurryCostModifier` を無視。割引はそのまま）、`iExperience`（その方法で緊急生産したユニットの経験値。ユニット以外では何もしない。エラーも出さない）。
- 対象の項目があれば、その緊急生産を社会制度なしで使える（`CvPlayer::isTraitHurry`。`CvCity::canHurry` / `canHurryUnit` / `canHurryBuilding`、緊急生産のボタンの説明で必要な社会制度を出さない）。
- 費用・追加費用の無視・経験値は、社会制度で解禁された緊急生産にも効く（`CvPlayer::getTraitHurryCostModifier` / `isTraitHurryIgnoreCostModifier` / `getTraitHurryExperience`）。
- 費用は `CvCity::getHurryCostModifier` に緊急生産の種類を渡して掛ける（`hurryGold` / `hurryPopulation` / `hurryProduction` も同じ種類で計算するので、人口による緊急生産でも完成までの生産になる。AI の見積もりの一部は種類を渡さない）。
- 経験値は、緊急生産した都市で次に完成した同じユニットに付く（`CvCity` の `m_eHurriedUnit`・`m_eHurriedUnitHurry`、セーブの `uiFlag=6`）。
- 緊急生産の種類は志向より後に読み込まれるので `readPass3()` で番号に変える。
- 説明文 `TXT_KEY_TRAIT_HURRY`（「社会制度なしで資金による緊急生産ができる（ユニット）：費用-25%、緊急生産したユニットの経験値+3」。対象は `TXT_KEY_TRAIT_HURRY_TARGET_*`、補正は `TXT_KEY_TRAIT_HURRY_COST` / `_IGNORE_COST` / `_EXPERIENCE`）。
- Python は `CvTraitInfo.getNumHurrys()` / `getHurryType(i)` / `getHurryCostModifier(i)` / `getHurryExperience(i)`、`CyPlayer.isTraitHurry(eHurry, eUnit, eBuilding)` / `getTraitHurryCostModifier(eHurry, eUnit, eBuilding)` / `getTraitHurryExperience(eHurry, eUnit)`

## `TraitInfo/NoTechProcesses`

- ファイル: `CIV4TraitInfos.xml`
- 生産の種類（`ProcessType`。富・研究・文化）を何個でも書ける。
- その生産の `TechPrereq` なしで都市が選べる（`CvPlayer::canMaintain`）。
- アルファベットなしで研究、通貨なしで富。
- 志向を失うと、次に都市が生産を続けられるか確かめたときに外れる（BtS の判定）。
- 生産の種類は `LoadPostMenuGlobals`（ゲームを始めるとき）で読み込まれるので、`CvTraitInfo::readPass4()` で番号に変える（文字列を残しておき、読み込み直されても同じ結果になる）。
- このため文明選択の画面など、ゲームの前の説明文には出ない。
- 説明文 `TXT_KEY_TRAIT_NO_TECH_PROCESSES`（「技術なしで選べる生産: 富, 研究」）。
- Python は `CyPlayer.isNoTechProcess(eProcess)`、`CvTraitInfo.isNoTechProcess(iProcess)`

## `TraitInfo/FreeTechs`

- ファイル: `CIV4TraitInfos.xml`
- 文明の `FreeTechs` と同じ書式（`<FreeTech>` が `TechType` + `bFreeTech`）。
- ゲーム開始時にその技術をチームに与える（`CvPlayer::initTraitFreeTechs`。`CvGame::setInitialItems` で、`ConditionalFreeTraits` の後、`PyOnGameStart` の前。開始時に条件で得た志向の分も与える）。
- 文明の初期技術と同じく、前提の技術は見ない。
- ゲームの途中で得た志向では与えない。
- 技術の数は開始時代で価値が変わる（後の時代から始めると、すでに持っている）。
- 説明文 `TXT_KEY_TRAIT_FREE_TECHS`（「ゲーム開始時に得る技術: 車輪, 陶器」。リンク）。
- Python は `CvTraitInfo.isFreeTech(iTech)`

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

## `TraitInfo/bIgnoreEvents`、`iEventChanceChange`、`bNoFirstEventDelay`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-23。
- `bIgnoreEvents`: その志向を持つプレイヤーのランダムイベントの画面に、選択肢「このイベントを無視する」（`TXT_KEY_POPUP_IGNORE_EVENT`）が増える（`CvDLLButtonPopup::launchEventPopup`。選べる選択肢が 1 つもなく画面が出ないイベントには出ない）。
- イベントの XML は変えず、良いイベントにも出る。
- 選ぶとどの選択肢も適用されず、トリガーは選択肢を選んだときと同じく発生済みになる（繰り返さないトリガーは二度と来ない。グローバル・チームのトリガーは他のプレイヤーでも発生済み。世界ニュースは出さない。トリガーの `PythonCallback` は呼ばれる。BtS のトリガーには設定されていない）（`CvPlayer::ignoreEvent`）。
- 結果は `ModNetMessage`（`MOD_NET_MESSAGE_IGNORE_EVENT` = -20261126。`CvNetModNetMessage::Execute` が DLL で処理し、Python には渡さない）で全員のコンピューターに送る。
- 無視できないもの: クエストは断ることになる（完了報酬の画面で選ぶと報酬を失う）、前の選択で予約された後続のイベント（`AdditionalEvent` のカウントダウン。画面なしで適用）、他国の選択による効果。
- AI は使わない（`AI_chooseEvent` で選ぶ）。
- `iEventChanceChange`: 毎ターンの新しいイベントの判定（`CvPlayer::doEvents`。100 面のうち時代の `iEventChancePerTurn`。古代 1 〜 未来 10）に志向の合計を足す。
- 100 以上なら毎ターン判定に通る（そのうえで発生できるトリガーの重みから 1 つ選ぶ）。
- 最初の 20 ターン（`FIRST_EVENT_DELAY_TURNS`）は変わらず発生しない。
- `bNoFirstEventDelay`: その最初の 20 ターンの待ちをなくし、ゲームの開始直後から新しいイベントの判定をする（`doEvents`）。
- 説明文 `TXT_KEY_TRAIT_IGNORE_EVENTS`・`TXT_KEY_TRAIT_EVENT_CHANCE_CHANGE`・`TXT_KEY_TRAIT_NO_FIRST_EVENT_DELAY`（`FIRST_EVENT_DELAY_TURNS` の値を渡す）。
- Python: `CvTraitInfo.isIgnoreEvents()`・`getEventChanceChange()`、`CvTraitInfo.isNoFirstEventDelay()`、`CyPlayer.isIgnoreEvents()`・`getEventChanceChange()`・`isNoFirstEventDelay()`・`ignoreEvent(iEventTriggeredId)`（全員のコンピューターで呼ぶこと）

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

## `TraitInfo/FoundingVassal`

- ファイル: `CIV4TraitInfos.xml`
- 2026-11-23。
- 最初の都市を建てると、AI の文明がその近くでゲームに参加し、永久同盟としてチームに加わる。
- `<FoundingVassal>` に `CivilizationType`・`LeaderType`・`HandicapType`・`iRange` を書く（すべて省略可。タグを書けば有効）。
- **発動**: `CvPlayer::found` の最後（`PyOnCityFounded`・`CityFoundedUnits` の後）の `doFoundingVassal`。
- 1 プレイヤーにつき 1 回だけ（`m_bFoundingVassalDone`。都市をすべて失って建て直しても発動しない。何も起きなかったときも済みになる）。
- 複数の志向が持つときは番号の小さい志向。
- 蛮族・小文明は対象外。
- 上級スタートでは、最初の都市を置いた時点で発動する（都市を売り戻しても AI は残る）。
- **プレイヤーの枠**: 一度も参加していない枠 → 滅んだプレイヤーの枠（チームも滅んでいるもの）の順（植民地の分離と同じ）。
- 空きがなければ何も起きない。
- **文明と指導者**: タグの値（組み合わせは自由）。
- すでに参加した（`isEverAlive`）プレイヤーが使っている、蛮族、`NONE`、省略のときは自動。
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
