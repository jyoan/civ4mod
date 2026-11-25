# 志向のタグ（ルール）

社会制度・技術・陳腐化・固有ユニット・緊急生産・ランダムイベント。Python フックと `ConditionalFreeTraits` は `trait-hooks.md`、外交は `trait-diplomacy.md`。

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
