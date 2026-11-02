# 呪文

FFH2 の呪文（spell）の仕組みを移植した。タグ・ファイル・関数の名前は FFH2 と同じ。使えるのはユニットだけ。第 1 段階は土台と基本の条件、第 2 段階 a は XML だけで書ける効果のうち、術者自身・今いるタイル・今いる都市へのもの、第 2 段階 b は周りのユニットへのもの、第 2 段階 c は召喚・失敗・世界呪文（1 ゲーム 1 回）・遅延、第 3 段階は AI、第 4 段階はペディア。

- XML: `Units\CIV4SpellInfos.xml`。Schema は FFH2 と同じく別ファイルの `Units\CIV4SpellSchema.xml`。昇進・ユニット・技術・文明を参照するので、`LoadPreMenuGlobals` で文明の後に読み込む。`CvSpellInfo` はストリーム版の `read` / `write` を持たない（キャッシュを使わない）
- 行動ボタン: `SetGlobalActionInfo` が呪文ごとに `ACTIONSUBTYPE_SPELL` の行動を作る。コマンドは `COMMAND_CAST`、データは呪文の番号。ボタンを押すと、既存のコマンドのネットメッセージ（`GAMEMESSAGE_DO_COMMAND`）で全マシンの `CvUnit::doCommand` → `cast` が呼ばれる（マルチプレイでも同期する）。選択中のユニットはすべて実行する
- `COMMAND_CAST` は `CvEnums.h` の `CommandTypes` の末尾（`COMMAND_HOTKEY` の後）と、`CIV4CommandInfos.xml`（無印 Civ4(J) のコピー）の末尾に足した。**XML の並びは列挙型と同じでなければならない**。`<Help/>` は空にしてある（中身があると、全呪文の説明に同じ文が付く）
- `CvUnit::canCast(iSpell, bTestVisible)` の判定順（FFH2 と同じ）:
  1. 移動できないタイマー（`getImmobileTimer`）。AI のユニットは `bAllowAI` の呪文だけ（FFH2 と同じ）
  2. ユニットの条件: `PromotionPrereq1` / `2`・`UnitClassPrereq`・`UnitPrereq`・`UnitCombatPrereq`
  3. 文明・都市・タイルの条件: `BuildingPrereq`（今いる都市にその建物）・`BuildingClassOwnedPrereq`（自国のどこかにその建物）・`CivilizationPrereq`・`CorporationPrereq`（今いる都市にその企業）・`ImprovementPrereq`（今いるタイルの施設）・`StateReligionPrereq`・`TechPrereq`。`ConvertUnitType` の呪文は、すでにその種類のユニットなら使えない。`bGlobal`（世界呪文をまだ使っていない）。`iCasterMinLevel`。`bCasterNoDuration`（召喚した、いずれ消えるユニットでない）
  4. `PromotionInStackPrereq` / `UnitInStackPrereq`: 同じタイルに、その昇進を持つ・その種類の、自国のほかのユニットがいる（術者自身がその昇進を持つ・その種類だと使えない。FFH2 と同じ）
  5. ボタンを出すかの判定（`bTestVisible`）で `bDisplayWhenDisabled` なら、ここで「出す」と決まる。この後の条件を満たさないと無効のボタンになる。`bDisplayWhenDisabled` でない呪文は、使えないときはボタン自体が出ない
  6. `FeatureOrPrereq1` / `2`（今いるタイルの地形の特徴。どちらか）
  7. `bHasCasted`: `bIgnoreHasCasted` でなければ、このターンに実行済みのユニットは使えない
  8. `bAdjacentToWaterOnly`（周りの 8 タイルに水のタイル（海・湖）がある。川・オアシスは含まない。`CvPlot::isAdjacentToWater` は FFH2 と同じものを足した）・`bInBordersOnly`（自分の領土）・`bInCityOnly`（都市。他国の都市でも可）
  9. 効果の前提: `iChangePopulation` は自分の都市で、人口が 1 以上残ること（FFH2 はどの都市でも可）。`iCost` が正なら資金が足りること。`bRemoveHasCasted` は実行済みのユニットだけ（召喚した、いずれ消えるユニットは不可。FFH2 と同じ）。`CreateUnitType` は召喚できること・`CreateBuildingType` は建てられること（下記）
  10. `PyRequirement` があれば、その結果で決まる
  11. なければ、効果のどれかがここで効くときだけ使える（効果のない呪文は使えない）: 召喚、昇進の付与（まだ持っていない）・除去（持っている）、変身、建物、地形の特徴・施設・宗教（作れる・広められる）、人口の増加、資金を得る（`iCost` が負。数えるのはこの Mod の追加）、ダメージ・移動不能（効く相手がいる。FFH2 のダメージは相手がいなくても使える）、`bRemoveHasCasted`、`PyResult`
- `CvUnit::cast` の順（FFH2 と同じ）: 実行済みにする（`bHasCasted`）→ 世界呪文を使用済みにする（`bGlobal`）→ 失敗（`iMiscastChance`。失敗したらここで終わり）→ 資金の消費（`iCost` が正。この Mod では効果の前に払う）→ 遅延（`iDelay`。あればここで止め、遅延が終わったときに続きを実行）→ ここから `CvUnit::castEffects`: 召喚 → 昇進の付与・除去 → 変身 → 建物 → 地形の特徴 → 施設 → 宗教 → ダメージ → 移動不能 → 実行済みの取り消し（`bRemoveHasCasted`）→ 資金を得る（`iCost` が負）→ 人口 → 演出（`Effect`・`Sound`）とメッセージ（呪文の名前とボタン）→ `PyResult` → 術者が消える（`bSacrificeCaster`、遅延）
- 効果の細部（第 2 段階 a）:
  - `AddPromotionType1`〜`3` / `RemovePromotionType1`〜`3` は、`bBuffCasterOnly=1` なら術者自身、0 なら `iRange` の範囲のユニット（下記）
  - `ConvertUnitType`: 新しいユニットを作り、`CvUnit::convert` で経験値・昇進・実行済み・積み荷を引き継ぐ。新しいユニットはそのターン動けない（移動できないタイマー 1）。変身より後の効果と `PyResult` は元のユニット（遅延で消える）に対して働く
  - `CreateBuildingType`: 自分の都市だけ（FFH2 はどの都市でも可）。すでにその建物がある都市では、呪文全体が使えない。世界遺産などの数の制限は見ない
  - `CreateFeatureType`: 都市でなく、他チームの領土でなく、地形の特徴のないタイル。施設のあるタイルは、タイルの持ち主が志向の `MaintainFeatures` でその特徴を保つときだけ（FFH2 と同じ。施設を禁止する特徴（`bNoImprovement`）は不可）。地形との組み合わせは `canHaveFeature`
  - `CreateImprovementType`: 都市でなく、他チームの領土でなく、施設のないタイル（FFH2 は他国の領土でも可）。`canHaveImprovement`
  - `SpreadReligion`: 今いる都市（他国の都市でも可。まだ創始されていない宗教でも広まる。FFH2 と同じ）
  - `iCost`: 正なら消費、負なら得る
  - `Effect`（`CIV4EffectInfos.xml` の `EFFECT_`）と `Sound`（`AS2D_` などのスクリプト）は、タイルが見えているマシンでだけ出る（演出なので向きの乱数は `ASyncRand`）
- 周りのユニットへの効果（第 2 段階 b）:
  - 範囲: 術者から縦横 `iRange` タイル以内の正方形（FFH2 と同じ。0 なら術者のタイルだけ）。昇進の付与・除去は術者のタイル（術者自身を含む）も含み、ダメージと移動不能は術者のタイルを除く（FFH2 と同じ）。対象は `CvUnit::getSpellTargets` で先に ID で集めてから効果を与える（輸送ユニットが倒れると積み荷がその場で消えるため）
  - 対象にならないユニット（`CvUnit::isImmuneToSpell`）: `bImmuneTeam` なら自チーム、`bImmuneEnemy` なら交戦中の相手、`bImmuneFlying` なら航空ユニット（FFH2 の飛行ユニットの代わり）。**交戦中でない他チームのユニットは常に対象外**（FFH2 の `bImmuneNeutral` が常に有効な扱い。FFH2 は中立のユニットにダメージを与えると宣戦する。`bImmuneNeutral`・`bCausesWar` は入れていない）。私掠船など国籍を隠すユニットは、BtS の戦闘と同じく交戦中の扱い（`CvUnit::isEnemy`）。他チームのユニットは、術者のチームから見えているタイルにいて、見えているもの（潜水艦などの見えないユニットは除く）だけ
  - 昇進の付与は兵科のあるユニットだけ（FFH2 と同じ。開拓者・労働者には付かない）。昇進の `UnitCombats` は見ない（この Mod のほかの昇進のタグと同じ。FFH2 は見る）。除去は兵科を問わない
  - `iDamage`: 対象ごとに `iDamage` の 50〜149%（`iDamage / 2 + 乱数(iDamage)`）のダメージ。ただし `iDamageLimit`（省略時 100）を超えない。100 に達すると倒れる（倒したのは術者の持ち主）。FFH2 の、防御力・レベル・都市の防御・ダメージの種類（`DamageType`）による軽減は入れていない。メッセージは両方の持ち主に出る（`TXT_KEY_MESSAGE_SPELL_DAMAGED` / `KILLED`）
  - `iImmobileTurns`: まだ移動できないタイマーが 0 の対象を、**その持ち主のターンで数えて** ○ ターン動けなくする（今がその持ち主のターンなら、そのターンを含む）。タイマーは持ち主のターンの開始時に減るので、持ち主のターンがまだ来ていなければ 1 多く設定する。FFH2 はそのまま設定するので、1 だと後で手番の来る相手には効かない。動けないユニットも防御はする。ユニットの説明に「移動不能」（`TXT_KEY_UNIT_IMMOBILE`）と出る
  - 説明文: 範囲の昇進・ダメージ・移動不能の行と、対象外の行（自チーム・交戦中の相手・交戦中でない他国（常に表示）・航空ユニット）
- 召喚・失敗・世界呪文・遅延（第 2 段階 c）:
  - `CreateUnitType` を `iCreateUnitNum`（省略時 1。FFH2 は 0 で、召喚の呪文には必ず書いてある）体、術者のタイルに作る（`CvUnit::castCreateUnit`。1 体ずつ `canCreateUnit` を確かめる）。召喚できない（`CvUnit::canCreateUnit`）: 消えるユニットが自分と同じ種類を召喚する（FFH2 と同じ）・術者のタイルに見えている敵がいる（FFH2 と同じ）・そのユニットがいられないタイル（陸のユニットを水上に、海のユニットを陸に。都市・砦は可。この Mod の追加）・ユニットクラスの数の上限（国家・チーム・世界。この Mod の追加）
  - `bPermanentUnitCreate` でなければ、召喚したユニットは `1 + 志向の iSummonDuration の合計`（`CvPlayer::getSummonDuration`）ターンで消える（`CvUnit::m_iDuration`。持ち主のターンの開始時に `CvUnit::doTurn` で 1 減り、0 で消える）。召喚したターンは動ける。消えるユニットは: 維持費がかからない（`CvPlayer::calculateUnitCost` で無料のユニットに数える。FFH2 と同じ）、倒されても厭戦感情・戦争の成果にならない（FFH2 と同じ）、捕獲されない（BtS の非戦闘ユニットの捕獲と、この Mod の `iCombatCapturePercent`）、消えるユニットが `iCombatCapturePercent` で捕獲したユニットは同じターン数で消える（FFH2 と同じ）、アップグレード・変身しても残りのターン数を引き継ぐ（`CvUnit::convert`）、`bRemoveHasCasted` と `bCasterNoDuration` の呪文を使えない。領土外の維持費（`NumOutsideUnits`）は FFH2 と違い普通のユニットと同じ。ユニットの説明に「あと○ターンで消える」（`TXT_KEY_UNIT_DURATION`）
  - `bPermanentUnitCreate`: 消えない。召喚したターンは動けない（FFH2 と同じ）。数の上限は「呪文のユニットの条件（昇進・ユニット・ユニットクラス・兵科）を満たす自国のユニットの数 > そのユニットクラスの数」（術者になれるユニット 1 体につき 1 体。FFH2 は昇進と宗教で数える）
  - `bCopyCastersPromotions`: 術者の昇進をすべて付ける（大将軍の指導者の昇進（`isLeader`）を除く。FFH2 は装備・種族などを除く）。`CreateUnitPromotion`: その昇進を付ける。どちらも昇進の `UnitCombats` は見ない
  - `iMiscastChance`: ○% で失敗する（乱数は `getSorenRandNum`）。失敗すると効果はなく、資金も消費しない。実行済みにはなり、世界呪文は使用済みになる（FFH2 と同じ）。`PyMiscast` があれば、失敗したときに `CvSpellInterface.miscast` が `eval` する。メッセージ `TXT_KEY_MESSAGE_SPELL_MISCAST`。FFH2 のユニットの `iMiscastChance` は入れていない
  - `bGlobal`（世界呪文）: 各プレイヤーが 1 ゲームに 1 回。世界呪文すべてで合わせて 1 回（FFH2 と同じ。`FEAT_GLOBAL_SPELL` を `FeatTypes` の末尾に足した）。ボタンを押すと確認のポップアップが出る（`CvActionInfo::isConfirmCommand` が世界呪文で真になり、BtS の `BUTTONPOPUP_CONFIRMCOMMAND` を使う。文は `TXT_KEY_POPUP_CONFIRM_CAST_GLOBAL`）。実行すると全プレイヤーにメッセージ（出会っていなければ「未知の指導者」）。FFH2 のゲームオプション「世界呪文なし」は入れていない
  - `iDelay`: 術者が ○ ターン動けなくなり（移動できないタイマー）、効果は ○ ターン後の持ち主のターンの開始時に出る（`CvUnit::m_iDelayedSpell`）。効果が出るのは `CvPlayer::doTurnUnits` で全ユニットのターンの開始の処理が終わった後（召喚したユニットがそのターンに `doTurn` を受けて、すぐ消えたりしないため）。効果はそのときに、一つずつ効くかを確かめ直す（建物がすでにある、など）。資金は実行したときに払い、遅延の間に術者が倒されると戻らない。FFH2 は遅延の後にもう一度失敗の判定と世界呪文のメッセージを行うが、この Mod では 1 回だけ。ユニットの説明に「実行中: ○（残り○ターン）」（`TXT_KEY_UNIT_HELP_DELAYED_CASTING`）
  - 説明文: 世界呪文（先頭）、`bCasterNoDuration`（前提）、召喚の行（何ターンで消えるかは今のプレイヤーの志向で計算）、遅延・失敗率（費用の後）
  - セーブ: `CvUnit` の `uiFlag=4`（召喚の残りターンと遅延中の呪文。3 以下のセーブは召喚なし・遅延なし）、`CvPlayer` の `uiFlag=2`（世界呪文の使用済み。1 以下のセーブは未使用）
- 実行済み（`m_bHasCasted`）はターンの開始時（`CvUnit::doTurn`）に戻り、アップグレードでは引き継ぐ（`CvUnit::convert`）。ユニットの説明に「実行済み」（`TXT_KEY_UNIT_HAS_CASTED`）と出る。セーブは `CvUnit` の `uiFlag=3`（2 以下のセーブは実行済みでない扱い）
- Python: `EntryPoints\CvSpellInterface.py`（`CvDefines.h` の `PYSpellModule`）。DLL は `canCast([pCaster, eSpell])` / `cast([pCaster, eSpell])` を呼び、それぞれが `PyRequirement` / `PyResult` の文字列を `eval` する（FFH2 0.41 と同じ。インストール済みの FFH2（Denev さんの版）は関数名だけを書く別の方式）。式の中では `pCaster`（CyUnit）と `eSpell` が使えるので、引数つきの共通関数を XML から呼べる（例: `spellGainGold(pCaster, 25)`）。今ある共通関数は `reqNotHasPromotion`・`spellAddPromotion`・`spellGainGold`・`spellDamageCaster`。失敗したときは `miscast([pCaster, eSpell])` が `PyMiscast` を `eval` する
- `PyRequirement` はボタンを描くたびに各マシンで別々に呼ばれるので、ゲーム状態を変えない。`PyResult` で術者を消すときは遅延（`pCaster.kill(True, -1)`）にする（`cast` の後で `doCommand` がまだユニットを使うため）
- 説明文（`CvGameTextMgr::parseSpellHelp`。行動ボタンのマウスオーバーとペディア）: 世界呪文 → 前提（文明・技術・国教・ユニット・兵科・ユニットクラス・昇進・レベル・建物・企業・地形の特徴・施設・同じタイルのユニット・召喚したユニットでない）→ 効果 → `Help` → 場所の条件 → 実行済みの取り消し・費用・遅延・失敗率・術者が消える。見出しは「実行 (呪文名)」（`TXT_KEY_COMMAND_CAST`）。テキストキーは、FFH2 にあるものは同じ名前（`TXT_KEY_SPELL_*`）。Python の効果は自動では説明されないので、`Help` に書く
- Python への公開: `gc.getNumSpellInfos()` / `gc.getSpellInfo(i)`（`CvSpellInfo` の getter）、`CyUnit.canCast` / `cast`（`canCast` の判定を通さない）/ `isHasCasted` / `setHasCasted` / `getDuration` / `setDuration` / `changeDuration` / `getDelayedSpell`、`CyPlayer.getSummonDuration`、`FeatTypes.FEAT_GLOBAL_SPELL`、`CommandTypes.COMMAND_CAST`、`ActionSubTypes.ACTIONSUBTYPE_SPELL`、`SpellTypes.NO_SPELL`、`CyGameTextMgr.getSpellHelp(iSpell, bCivilopediaText)`、`WidgetTypes.WIDGET_PEDIA_JUMP_TO_SPELL`、`CivilopediaPageTypes.CIVILOPEDIA_PAGE_SPELL`
- ペディア（第 4 段階）:
  - カテゴリ「呪文」（`CIVILOPEDIA_PAGE_SPELL`、テキスト `TXT_KEY_PEDIA_CATEGORY_SPELL`。名前は FFH2 と同じ）。一覧の位置は「志向」の下（最後）。一覧は呪文のボタンと名前の表
  - 呪文のページ（`CvPediaSpell.py`。昇進のページと同じ配置）: 左上に呪文のボタン、右上に「必要条件」（文明・技術・国教・ユニット・兵科・ユニットクラス・昇進・建物・企業・地形の特徴・施設・同じタイルの昇進とユニットのボタン。説明文と同じ順。ユニットクラス・建物クラスは呪文の `CivilizationPrereq` の文明、なければ今のプレイヤーの文明のもの、なければ既定のもの）、その下に「効果」（召喚するユニットとその昇進・与える昇進・変身先のユニット・建物・地形の特徴・施設・宗教のボタン。外す昇進は説明文だけ）、右下に「特殊能力」（行動ボタンと同じ説明文から見出しを除いたもの）、左下に「歴史」（`Civilopedia`）、右端に呪文の一覧
  - DLL 側: `WIDGET_PEDIA_JUMP_TO_SPELL`（クリックで Python の `pediaJumpToSpell`、マウスオーバーで呪文の名前と説明文。昇進と同じく `iData2` が 0 なら説明を出さない（右端の一覧））、`CvGameTextMgr::setSpellHelp`（`setPromotionHelp` と同じく、ペディア用でなければ名前を先頭に付ける）、`CyGameTextMgr.getSpellHelp`。列挙型はどちらも末尾（志向の後）に足した。説明文の中の `[LINK=literal]` の呪文名もペディアで呪文のページへ移動する（`CvPediaMain.link`）
  - 召喚したユニットが消えるまでのターン数は、今のプレイヤーの志向で計算する（ゲームのプレイヤーがいないとき（メインメニューから開いたペディア）は 1）
  - 昇進・ユニットのページから呪文へのリンク（FFH2 の昇進のページの「呪文」）は入れていない（BtS の `CvPediaPromotion.py` などを Mod にコピーする必要がある）
- AI（第 3 段階）:
  - AI（蛮族を含む）は `bAllowAI`（省略時 0）の呪文だけを使う（FFH2 と同じ）。ユニットごとに、使える呪文のうち評価値が最大で 0 より大きいものを 1 つ実行する（`CvUnitAI::AI_chooseSpell`）。実行するのは自分のターンの初め（`CvPlayerAI::AI_doTurnUnitsPost` の最後。遅延の呪文・昇進・アップグレードの後）と終わり（`CvGame::updateMoves` で全ユニットが動き終わった後。FFH2 はここだけ）の 2 回（`CvPlayerAI::AI_doSpells`）。ネットメッセージを通さず `cast` を直接呼ぶ（FFH2 と同じ。AI の処理は全マシンで同じように動くので同期する）。世界呪文の確認のポップアップは出ない
  - 評価値（`CvUnitAI::AI_spellValue`）は FFH2 の `chooseSpell` の係数に `iAIWeight` を足したもの: 召喚は、召喚するユニットの移動力の範囲に見えている敵がいるときだけ「戦闘力 × 100 × 体数」（`bPermanentUnitCreate` でも同じ）。昇進は術者から見た昇進の評価（BtS の `AI_promotionValue`）。ダメージは敵 1 体につき「ダメージ × 10」（`iDamageLimit` に達した敵は 0）、自チーム 1 体につき「ダメージ × -20」。移動不能は動ける敵 1 体につき「ターン数 × 20」（自チームは -20）。変身は新旧のユニットの評価の差（`AI_unitValue`）。建物は都市の建物の評価（`AI_buildingValue`）。地形の特徴・施設は 10。人口 1 につき 50。資金は 1 につき -4（得るなら +4）。術者が消えるなら「レベル × 術者のユニットの評価」を引く。宗教・実行済みの取り消し・遅延・世界呪文・`PyResult` は評価しないので、`iAIWeight` で決める
  - FFH2 との違い: 昇進と移動不能は範囲の対象の数で数える（FFH2 は昇進が術者 1 体分、移動不能が範囲のタイルの数）。範囲の昇進は、自チームのユニットが得ると +、敵が得ると −（除去は逆）。今いる場所で効かない効果（地形の特徴を作れない、など）は数えない。効果の評価は失敗率の分だけ下げる（失敗すると効果も費用もないため。`iAIWeight` は下げない）。変身と術者が消えるときのユニットの評価は、FFH2 の `UNITAI_ATTACK` の代わりに術者のユニット AI で計る（変身後のユニットは術者のユニット AI を引き継ぐため）
  - AI のユニットは呪文を使うために移動しない（FFH2 の `UNITAI_MAGE` などは入れていない）。FFH2 の自動化のタグ（`bAllowAutomateTerrain` など）も入れていない
