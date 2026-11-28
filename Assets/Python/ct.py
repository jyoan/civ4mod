# ct.py (added by this mod): short commands for the Python console (Shift + ~ with CheatCode = chipotle) to test the mod.
# Test only, left out of the release. In the console: import ct, then ct.ls(), ct.war(2) and so on. "me" is the active team.
# The console does not print the values of expressions, so the results go to the message area of the screen
#
#   ct.ls()          the alive players: player, team, civilization, at war with me, vassal / former master flags
#   ct.war(t)        team t declares war on me
#   ct.dow(t)        I declare war on team t
#   ct.peace(t)      peace between me and team t
#   ct.free(t)       team t becomes my former master (I become its vassal, then the vassal state ends)

from CvPythonExtensions import *

gc = CyGlobalContext()


def me():
	return gc.getGame().getActiveTeam()


def show(szText):
	for szLine in szText.split('\n'):
		CyInterface().addImmediateMessage(szLine, "")


def ls():
	eMe = me()
	pMyTeam = gc.getTeam(eMe)
	lRows = []
	for iPlayer in range(gc.getMAX_CIV_PLAYERS()):
		pPlayer = gc.getPlayer(iPlayer)
		if not pPlayer.isAlive():
			continue
		eTeam = pPlayer.getTeam()
		szFlags = ''
		if eTeam == eMe:
			szFlags = ' (me)'
		else:
			if pMyTeam.isAtWar(eTeam):
				szFlags += ' WAR'
				if gc.getTeam(eTeam).isWarDeclaredOn(eMe):
					szFlags += '(they declared)'
				elif pMyTeam.isWarDeclaredOn(eTeam):
					szFlags += '(I declared)'
			if pMyTeam.isVassal(eTeam):
				szFlags += ' MY_MASTER'
			if pMyTeam.isFormerMaster(eTeam):
				szFlags += ' MY_FORMER_MASTER'
			if gc.getTeam(eTeam).isVassal(eMe):
				szFlags += ' MY_VASSAL'
		lRows.append('player %d team %d %s%s' % (iPlayer, eTeam, gc.getCivilizationInfo(pPlayer.getCivilizationType()).getType(), szFlags))
	show('\n'.join(lRows))


def war(eTeam):
	gc.getTeam(eTeam).declareWar(me(), True, -1)
	ls()


def dow(eTeam):
	gc.getTeam(me()).declareWar(eTeam, True, -1)
	ls()


def peace(eTeam):
	gc.getTeam(me()).makePeace(eTeam)
	ls()


def free(eTeam):
	pMyTeam = gc.getTeam(me())
	pMyTeam.setVassal(eTeam, True, False)
	pMyTeam.setVassal(eTeam, False, False)
	ls()
