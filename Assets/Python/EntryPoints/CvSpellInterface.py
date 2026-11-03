## Sid Meier's Civilization 4
## CvSpellInterface.py (added by this mod; the same structure as FFH2)
##
## The DLL calls canCast, cast and miscast here for the spells of Assets/XML/Units/CIV4SpellInfos.xml.
## PyRequirement, PyResult and PyMiscast of a spell are Python expressions evaluated here, where pCaster is the casting
## unit (CyUnit) and eSpell is the spell. Write common functions below and give them values in the XML, e.g.
##   <PyRequirement>reqNotHasPromotion(pCaster, 'PROMOTION_X')</PyRequirement>
##   <PyResult>spellGainGold(pCaster, 25)</PyResult>
##
## PyRequirement returns True when the spell can be cast. It is also called each time the action buttons are
## drawn (separately on each computer), so it must not change the game.
## PyResult and PyMiscast (when the spell is miscast, instead of its effects) are called on every computer of a
## multiplayer game: use CyGame().getSorenRandNum for random numbers.
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

def miscast(argsList):
	pCaster, eSpell = argsList
	spell = gc.getSpellInfo(eSpell)
	eval(spell.getPyMiscast())

## Common functions for PyRequirement

def reqNotHasPromotion(pCaster, szPromotion):
	"True if the caster does not have the promotion yet"
	return not pCaster.isHasPromotion(gc.getInfoTypeForString(szPromotion))

## Common functions for PyResult and PyMiscast

def spellAddPromotion(pCaster, szPromotion):
	"The caster gains the promotion"
	pCaster.setHasPromotion(gc.getInfoTypeForString(szPromotion), True)

def spellGainGold(pCaster, iGold):
	"The owner of the caster gains gold"
	gc.getPlayer(pCaster.getOwner()).changeGold(iGold)

def spellToggleFreeTrait(pCaster, szTrait):
	"The owner of the caster is given the trait, or loses it when it was given (not the traits of the leader or civilization)"
	pPlayer = gc.getPlayer(pCaster.getOwner())
	eTrait = gc.getInfoTypeForString(szTrait)
	if pPlayer.getFreeTraitCount(eTrait) > 0:
		pPlayer.changeFreeTraitCount(eTrait, -1)
	else:
		pPlayer.changeFreeTraitCount(eTrait, 1)

def spellDamageCaster(pCaster, iDamage):
	"The caster takes damage (it dies at 100, with a delay)"
	pCaster.changeDamage(iDamage, PlayerTypes.NO_PLAYER)
