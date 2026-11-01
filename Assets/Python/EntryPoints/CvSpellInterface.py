## Sid Meier's Civilization 4
## CvSpellInterface.py (added by this mod; the same structure as FFH2)
##
## The DLL calls canCast and cast here for the spells of Assets/XML/Units/CIV4SpellInfos.xml.
## PyRequirement and PyResult of a spell are Python expressions evaluated here, where pCaster is the casting
## unit (CyUnit) and eSpell is the spell. Write common functions below and give them values in the XML, e.g.
##   <PyRequirement>reqNotHasPromotion(pCaster, 'PROMOTION_X')</PyRequirement>
##   <PyResult>spellGainGold(pCaster, 25)</PyResult>
##
## PyRequirement returns True when the spell can be cast. It is also called each time the action buttons are
## drawn (separately on each computer), so it must not change the game.
## PyResult is called on every computer of a multiplayer game: use CyGame().getSorenRandNum for random numbers.
## To kill the caster, use the delayed kill: pCaster.kill(True, -1)

from CvPythonExtensions import *

gc = CyGlobalContext()

def canCast(argsList):
	pCaster, eSpell = argsList
	spell = gc.getSpellInfo(eSpell)
	return eval(spell.getPyRequirement())

def cast(argsList):
	pCaster, eSpell = argsList
	spell = gc.getSpellInfo(eSpell)
	eval(spell.getPyResult())

## Common functions for PyRequirement

def reqNotHasPromotion(pCaster, szPromotion):
	"True if the caster does not have the promotion yet"
	return not pCaster.isHasPromotion(gc.getInfoTypeForString(szPromotion))

## Common functions for PyResult

def spellAddPromotion(pCaster, szPromotion):
	"The caster gains the promotion"
	pCaster.setHasPromotion(gc.getInfoTypeForString(szPromotion), True)

def spellGainGold(pCaster, iGold):
	"The owner of the caster gains gold"
	gc.getPlayer(pCaster.getOwner()).changeGold(iGold)
