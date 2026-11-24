# 諜報任務のタグ

`CIV4EspionageMissionInfo.xml` は BtS のものを Mod に入れた（Schema は `GameInfo\CIV4GameInfoSchema.xml`）。志向側にはタグを持たせず、任務の側で志向を指定する。効果を強めた任務は、強めた値の任務を別に足して `PrereqOrTraits` で使えるようにし、元の任務を `DisableTraits` で使えなくする（例: 検証用の `ESPIONAGEMISSION_TEST_CITY_POISON_WATER`）。

## `EspionageMissionInfo/PrereqOrTraits`・`DisableTraits`

- ファイル: `CIV4EspionageMissionInfo.xml`
- どちらも `<TraitType>` を何個でも書ける（省略で制限なし）。
- `PrereqOrTraits` の志向のどれかを持つプレイヤーだけがその任務を使える。
- `DisableTraits` の志向のどれかを持つプレイヤーはその任務を使えない（2026-11-22 に志向 1 つの `PrereqTrait`・`DisableTrait` から変えた）。
- 判定は `CvPlayer::isEspionageMissionAllowed`。
- `CvPlayer::getEspionageMissionBaseCost` が -1（無効）を返すので、スパイの任務の一覧・諜報顧問の費用・AI のどれにも効く（諜報顧問の一覧には費用なしの行として残る。BtS で `iCost` が -1 でない任務は一覧に出るため）。
- 志向は諜報任務より先に読み込まれるので `read` で番号に変える。
- 志向の説明文に、その志向が `PrereqOrTraits` にある任務を `TXT_KEY_TRAIT_ENABLED_ESPIONAGE_MISSIONS`（「使える諜報任務: 水源への毒混入（検証用）, …」）、`DisableTraits` にある任務を `TXT_KEY_TRAIT_DISABLED_ESPIONAGE_MISSIONS`（「使えない諜報任務: 水源への毒混入, …」）で出す。
- 諜報任務は `LoadPostMenuGlobals`（ゲームを始めるとき）で読み込まれるので、文明選択の画面など、ゲームの前の説明文には出ない。
- Python は `CyPlayer.isEspionageMissionAllowed(eMission)`（任務のタグは Python に公開していない）

## `EspionageMissionInfo/bKillGreatPerson`・`bKidnapGreatPerson`

- ファイル: `CIV4EspionageMissionInfo.xml`
- 都市に定住した偉人を 1 人取り除く（`bKillGreatPerson`。任務 `ESPIONAGEMISSION_KILL_GREAT_PERSON`「要人の殺害」）。
- `bKidnapGreatPerson` はさらに、その偉人のユニットを任務を行ったプレイヤーの首都に作る（任務 `ESPIONAGEMISSION_KIDNAP_GREAT_PERSON`「要人の誘拐」）。
- `bIsTwoPhases` を `1` にすると、建物破壊と同じ標的選択の画面で偉人の種類を選べる（ボタンは種類ごとに「大科学者 x2 (費用)」。ランダムではない）。
- 定住偉人は、偉人ユニットが定住する専門家（ユニットクラスの既定ユニットの `GreatPeoples` にある専門家）の無料の専門家のうち、建物が与える分を除いた数（BtS の `CvCity::getAddedFreeSpecialistCount`。イベントで得た偉人の専門家も含む）。
- 誘拐で作るユニットは、そのプレイヤーの文明・志向の固有ユニットを含めた、その専門家に定住するユニットクラスのユニット（`CvPlayer::getGreatPersonUnit`）。
- 首都がなければ誘拐はできない。
- 必要 EP は `iCost`（費用の係数を持たない BtS の任務と同じく、ゲーム速度の研究の補正をかける。距離・諜報への防御などの補正も BtS どおり）。
- 足した 2 つの任務は `iCost` 500・800、`iDifficultyMod` 25・50、`PrereqOrTraits` なし（誰でも使える）。
- メッセージ: 相手に `TXT_KEY_ESPIONAGE_TARGET_GREAT_PERSON_KILLED` / `_KIDNAPPED`、誘拐した側に `TXT_KEY_ESPIONAGE_GREAT_PERSON_KIDNAPPED_TO`。
- 費用のツールチップ `TXT_KEY_ESPIONAGE_HELP_KILL_GREAT_PERSON` / `_KIDNAP_GREAT_PERSON`。
- AI は使わない（`AI_espionageVal` が 0）。
- Python は `CyPlayer.getGreatPersonUnit(eSpecialist)`

## `EspionageMissionInfo/bSwitchAnyCivic`

- ファイル: `CIV4EspionageMissionInfo.xml`
- `iSwitchCivicCostFactor` と一緒に使う。
- BtS の「体制変革」は自分が採用している社会制度しか押し付けられないが、`1` にすると相手が採用でき、まだ採用していない社会制度をすべて選べる（`CvPlayer::canForceCivics` の `bAnyCivic`）。
- 選ぶ画面は BtS どおり、建物の破壊と同じ標的選択の画面（`bIsTwoPhases` 1。社会制度のアイコンと費用のボタン）。
- 効果・費用の式・メッセージは BtS の体制変革と同じ。
- AI は BtS でも社会制度の変更の任務を使わない（`CvPlayerAI::AI_bestPlotEspionage` が評価しない）。
- 社会制度の枠が複数ある項目では枠 0 の社会制度と置き換わる（`setCivics`）。
- 足した任務 `ESPIONAGEMISSION_SUBVERT_CIVIC`「体制転覆」: `iSwitchCivicCostFactor` 90000（体制変革は 60000。普通の速度で 900 と 600）、`iDifficultyMod` 50、`PrereqOrTraits` なし（誰でも使える）。
- 任務のタグは Python に公開していない

## `EspionageMissionInfo/bSwitchAnyReligion`・`bSwitchNoReligion`

- ファイル: `CIV4EspionageMissionInfo.xml`
- `iSwitchReligionCostFactor` と一緒に使う。
- BtS の「国教変更」は自分の国教しか押し付けられないが、`bSwitchAnyReligion` を `1` にすると、相手が国教にでき（相手のチームの都市にある）、今の国教ではない宗教をすべて選べる（`CvPlayer::canForceReligion` の `bAnyReligion`）。
- `bSwitchNoReligion` を `1` にすると、相手に国教があれば「国教なし」も選べる（ボタンは宗教の画面と同じ ×。任務の追加データは `GC.getNumReligionInfos()`。追加データが -1 のとき（諜報顧問の費用）は、押し付けられる宗教がなければ国教なしの費用を出す）。
- 選ぶ画面は BtS どおり、建物の破壊と同じ標的選択の画面。
- 費用の式・改宗の待ち期間は BtS の国教変更と同じ。
- 国教なしにされた相手には `TXT_KEY_ESPIONAGE_TARGET_SWITCH_NO_RELIGION`（「敵スパイが宗教界を乗っ取り、我が国の国教を廃止させました！」）、費用のツールチップは BtS のキーに「国教なし」（`TXT_KEY_RELIGION_SCREEN_NO_STATE`）を入れる。
- AI は BtS でも宗教の変更の任務を使わない（`CvPlayerAI::AI_bestPlotEspionage` が評価しない。`AI_espionageVal` も国教なしを評価しない）。
- 足した任務 `ESPIONAGEMISSION_SUBVERT_RELIGION`「国教転覆」: 両方 `1`、`iSwitchReligionCostFactor` 90000（国教変更は 60000）、`iDifficultyMod` 50、`PrereqOrTraits` なし
