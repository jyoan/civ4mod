# 呪文のタグ

2026-11-28 に `docs/tags/promotion.md` から分けた。仕組みの全体は `docs/spells.md`。

## `SpellInfo` の `PromotionPrereq1` / `PromotionPrereq2`、`UnitPrereq`、`UnitClassPrereq`、`UnitCombatPrereq`、`CivilizationPrereq`、`TechPrereq`、`iCasterMinLevel`、`bInBordersOnly`、`bInCityOnly`、`bHasCasted`、`bIgnoreHasCasted`、`bDisplayWhenDisabled`、`PyRequirement`、`PyResult`（と `Type`・`Description`・`Civilopedia`・`Help`・`Button`・ホットキー）

- ファイル: `CIV4SpellInfos.xml`（新規）
- 呪文（第 1 段階。名前は FFH2 と同じ）。
- 仕組みは `docs/spells.md`

## `SpellInfo` の `bAllowAI`、`iAIWeight`

- ファイル: `CIV4SpellInfos.xml`
- 呪文の第 3 段階（名前は FFH2 と同じ）。
- AI が使ってよい呪文と、AI の評価値に足す値。
- 仕組みは `docs/spells.md`

## `SpellInfo` の `CreateUnitType`、`iCreateUnitNum`、`bPermanentUnitCreate`、`bCopyCastersPromotions`、`CreateUnitPromotion`、`bCasterNoDuration`、`iMiscastChance`、`PyMiscast`、`bGlobal`、`iDelay`

- ファイル: `CIV4SpellInfos.xml`
- 呪文の第 2 段階 c（名前は FFH2 と同じ）。
- 召喚・失敗・世界呪文・遅延。
- FFH2 との違いは `docs/spells.md`

## `SpellInfo` の `iRange`、`iDamage`、`iDamageLimit`、`iImmobileTurns`、`bImmuneTeam`、`bImmuneEnemy`、`bImmuneFlying`（と、`bBuffCasterOnly=0` の `AddPromotionType1`〜`3` / `RemovePromotionType1`〜`3`）

- ファイル: `CIV4SpellInfos.xml`
- 呪文の第 2 段階 b（名前は FFH2 と同じ）。
- 周りのユニットへの効果。
- 交戦中でない他国のユニットは常に対象外など、FFH2 との違いは `docs/spells.md`

## `SpellInfo` の前提 `UnitInStackPrereq`、`BuildingPrereq`、`BuildingClassOwnedPrereq`、`CorporationPrereq`、`FeatureOrPrereq1` / `2`、`ImprovementPrereq`、`PromotionInStackPrereq`、`StateReligionPrereq`、`bAdjacentToWaterOnly` と、効果 `iCost`、`AddPromotionType1`〜`3`、`RemovePromotionType1`〜`3`、`bBuffCasterOnly`、`ConvertUnitType`、`CreateBuildingType`、`CreateFeatureType`、`CreateImprovementType`、`SpreadReligion`、`iChangePopulation`、`bSacrificeCaster`、`bRemoveHasCasted`、`Effect`、`Sound`

- ファイル: `CIV4SpellInfos.xml`
- 呪文の第 2 段階 a（名前は FFH2 と同じ）。
- 術者自身・今いるタイル・今いる都市への効果。
- FFH2 との違い（自分の都市・領土に限るものなど）は「呪文」の節
