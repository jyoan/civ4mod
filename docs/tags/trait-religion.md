# 志向のタグ（宗教）

2026-11-28 に作った。宗教の数・宣教師・宗教の伝播・文化による不満のタグ。`iHappinessPerReligion`・`CommercePerReligion` は `docs/tags/trait-economy.md` から移した（`iHealthPerReligion`・`YieldPerReligion` は `trait-economy.md` の `YieldChanges` の項目）。

## `TraitInfo/iHappinessPerReligion`、`CommercePerReligion`

- ファイル: `CIV4TraitInfos.xml`
- 都市にある宗教 1 つにつき、その都市の幸福（マイナスなら不満）と商業（`<iCommerce>` は金・研究・文化・諜報の順）を足す（インドの「多宗教の受容」）。
- 幸福はその都度数える（`CvCity::getTraitReligionHappiness`。`happyLevel`・`unhappyLevel`、都市の幸福の内訳に「都市の宗教の数により」の行 `TXT_KEY_HAPPY_TRAIT_RELIGIONS` / `TXT_KEY_ANGER_TRAIT_RELIGIONS`）。
- 商業は BtS の宗教の商業（聖都の聖堂など）の合計に足す（`CvCity::getTraitReligionCommerce`、`updateReligionCommerce`）ので、宗教が広まった・消えたときに計算し直され、都市画面の内訳では宗教の分に入る。
- 志向が変わったときは `updateTraitDependents` が全都市の宗教の商業を計算し直す。
- 志向の値の合計（`CvPlayer::getHappinessPerReligion` / `getCommercePerReligion`）。
- 説明文 `TXT_KEY_TRAIT_HAPPINESS_PER_RELIGION`・`TXT_KEY_TRAIT_COMMERCE_PER_RELIGION`（「都市の宗教1つにつき+1[文化]」）。
- Python は `CyPlayer` / `CvTraitInfo` の同名の関数

## `TraitInfo/CommercePercentPerReligion`、`iExperiencePerReligion`、`iGreatPeopleRatePerReligion`

- ファイル: `CIV4TraitInfos.xml`
- 都市にある宗教 1 つにつき、その都市の値を足す（2026-11-28。インド）:
  - `CommercePercentPerReligion`: 商業 %（`<iCommerce>` は金・研究・文化・諜報の順。`CvCity::getTotalCommerceRateModifier`）
  - `iExperiencePerReligion`: その都市で作ったユニットの経験値（`CvCity::getProductionExperience`。徴兵は BtS どおり半分）
  - `iGreatPeopleRatePerReligion`: 偉人率 %（`CvCity::getTotalGreatPeopleRateModifier`）
- その都度数えるので、宗教が広まる・消えるとすぐに変わる。志向の値の合計（`CvPlayer` の同名の関数）。
- 内訳: 商業の内訳に `TXT_KEY_MISC_HELP_YIELD_RELIGIONS`、偉人の内訳に `TXT_KEY_MISC_HELP_GREATPEOPLE_TRAIT_RELIGIONS`。
- 都市画面の宗教のアイコンの説明に、宗教ごとの値（「+5%研究」、BtS の `TXT_KEY_RELIGION_BIRTH_RATE_MOD`・`TXT_KEY_RELIGION_FREE_XP`。`CvGameTextMgr::setReligionHelpCity`）。
- 説明文 `TXT_KEY_TRAIT_COMMERCE_PERCENT_PER_RELIGION`・`TXT_KEY_TRAIT_EXPERIENCE_PER_RELIGION`・`TXT_KEY_TRAIT_GREAT_PEOPLE_RATE_PER_RELIGION`。
- Python は `CvTraitInfo`・`CyPlayer` の同名の関数（`CyPlayer` は未公開）

## `TraitInfo/SpecialBuildingNotRequireds`（`CivicInfo` にも同名タグ）

- ファイル: `CIV4TraitInfos.xml`
- 社会制度（組織宗教）と同じ書式（`SpecialBuildingType` + `bNotRequired`）。2026-11-28。インド。
- その特別な建物（僧院）を前提にするユニット（宣教師）を、その建物なしで作れる（`CvPlayer::isSpecialBuildingNotRequired` に志向を足した。`CvPlot::canTrain`）。
- 宗教の条件（都市にその宗教があること）は BtS のまま。
- 特別な建物は志向より後に読み込まれるので、`readPass3()` で番号に変える。
- 説明文 `TXT_KEY_TRAIT_SPECIAL_BUILDING_NOT_REQUIRED`（「僧院がなくても宣教師を作れる」）。
- Python は `CvTraitInfo.isSpecialBuildingNotRequired(i)`

## `TraitInfo/bOtherReligionsToCapital`

- ファイル: `CIV4TraitInfos.xml`
- ほかの文明が宗教を創始すると、自分の首都にその宗教が広まる（2026-11-28。ザラ・ヤコブ）。
- 創始した側の `PyOnReligionFounded` の後（`CvPlayer::foundReligion`）。すでに創始された宗教にはさかのぼらない。
- ゲーム開始前とワールドビルダーでは起きない。首都がなければ何もしない。
- 説明文 `TXT_KEY_TRAIT_OTHER_RELIGIONS_TO_CAPITAL`。
- Python は `CvTraitInfo.isOtherReligionsToCapital()`

## `TraitInfo/PyOnOtherReligionFounded`

- ファイル: `CIV4TraitInfos.xml`
- ほかの文明（生きているプレイヤー）が宗教を創始した後に呼ぶフック（2026-11-28。今は検証用志向だけが使う）。
- `bOtherReligionsToCapital` で首都に広めた後。名前は `pPlayer`・`eTrait`・`pCity`（創始した側の聖地）・`eReligion`・`iFounder`。
- ほかのフックと同じく、ゲーム開始前とワールドビルダーでは呼ばない（`CvTraitInterface.onOtherReligionFounded`）。
- 例: `createMissionaries(pPlayer, eReligion, 1, pPlayer.getCapitalCity())`

## `TraitInfo/bNoForeignCultureAnger`

- ファイル: `CIV4TraitInfos.xml`
- 他国の文化による不満がない（2026-11-28。スレイマン）。
- BtS の `CvCity::getCulturePercentAnger`（ほかの生きているチームの文化を数え、戦争中の相手の分を `AT_WAR_CULTURE_ANGER_MODIFIER` で増やす）を 0 にする。
- 説明文 `TXT_KEY_TRAIT_NO_FOREIGN_CULTURE_ANGER`。
- Python は `CvTraitInfo.isNoForeignCultureAnger()`
