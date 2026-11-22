## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import string
import CvScreensInterface

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvCivicsScreen:
	"Civics Screen"

	def __init__(self):
		self.SCREEN_NAME = "CivicsScreen"
		self.CANCEL_NAME = "CivicsCancel"
		self.EXIT_NAME = "CivicsExit"
		self.TITLE_NAME = "CivicsTitleHeader"
		self.BUTTON_NAME = "CivicsScreenButton"
		self.TEXT_NAME = "CivicsScreenText"
		self.AREA_NAME = "CivicsScreenArea"
		self.HELP_AREA_NAME = "CivicsScreenHelpArea"
		self.HELP_IMAGE_NAME = "CivicsScreenCivicOptionImage"
		self.DEBUG_DROPDOWN_ID =  "CivicsDropdownWidget"
		self.BACKGROUND_ID = "CivicsBackground"
		self.HELP_HEADER_NAME = "CivicsScreenHeaderName"

		self.HEADINGS_WIDTH = 199
		self.HEADINGS_TOP = 70
		self.HEADINGS_SPACING = 5
		self.HEADINGS_BOTTOM = 280
		self.HELP_TOP = 350
		self.HELP_BOTTOM = 610
		self.TEXT_MARGIN = 15
		self.BUTTON_SIZE = 24
		self.BIG_BUTTON_SIZE = 64
		self.BOTTOM_LINE_TOP = 630
		self.BOTTOM_LINE_WIDTH = 1014
		self.BOTTOM_LINE_HEIGHT = 60

		# Added by this mod: HEADINGS_BOTTOM and HELP_TOP above are recalculated in updateLayout(),
		# so that the headings have room for the civic option with the most civics
		self.MIN_ROWS = 5
		self.MAX_ROWS_HEIGHT = 240		# with more rows than fit here, the rows get closer so that the help text keeps its room
		self.HEADINGS_BOTTOM_MARGIN = 45
		self.iRowHeight = 2 * self.TEXT_MARGIN

		self.X_EXIT = 994
		self.Y_EXIT = 726

		self.X_CANCEL = 552
		self.Y_CANCEL = 726

		self.X_SCREEN = 500
		self.Y_SCREEN = 396
		self.W_SCREEN = 1024
		self.H_SCREEN = 768
		self.Z_SCREEN = -6.1
		self.Y_TITLE = 8		
		self.Z_TEXT = self.Z_SCREEN - 0.2

		self.CivicsScreenInputMap = {
			self.BUTTON_NAME		: self.CivicsButton,
			self.TEXT_NAME			: self.CivicsButton,
			self.EXIT_NAME			: self.Revolution,
			self.CANCEL_NAME		: self.Cancel,
			}

		self.iActivePlayer = -1

		# Changed by this mod (civic slots): the civic lists have an entry for each slot of each civic option,
		# entry iSlot * NumCivicOptionInfos + iCivicOption (see entry()); slot 0 is the civic of BtS, an empty slot is -1
		self.m_paeCurrentCivics = []
		self.m_paeDisplayCivics = []
		self.m_paeOriginalCivics = []
		self.m_paeHelpCivics = []		# added by this mod: the civic of the help text of each civic option

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.CIVICS_SCREEN)

	def setActivePlayer(self, iPlayer):

		self.iActivePlayer = iPlayer
		activePlayer = gc.getPlayer(iPlayer)

		self.m_paeCurrentCivics = []
		self.m_paeDisplayCivics = []
		self.m_paeOriginalCivics = []
		self.m_paeHelpCivics = []
		for iSlot in range(activePlayer.getMaxCivicSlots()):
			for i in range (gc.getNumCivicOptionInfos()):
				self.m_paeCurrentCivics.append(activePlayer.getCivicSlot(i, iSlot))
				self.m_paeDisplayCivics.append(activePlayer.getCivicSlot(i, iSlot))
				self.m_paeOriginalCivics.append(activePlayer.getCivicSlot(i, iSlot))
		for i in range (gc.getNumCivicOptionInfos()):
			self.m_paeHelpCivics.append(activePlayer.getCivics(i))

	# Added by this mod (civic slots): the entry of the slot in the civic lists
	def entry(self, iCivicOption, iSlot):
		return iSlot * gc.getNumCivicOptionInfos() + iCivicOption

	# Added by this mod (civic slots): how many civics of the civic option the player can adopt (1 while a resolution forces the option)
	def getNumSlots(self, iCivicOption):
		if gc.getGame().isForceCivicOption(iCivicOption):
			return 1
		return gc.getPlayer(self.iActivePlayer).getNumCivicSlots(iCivicOption)

	# Added by this mod (civic slots): the civics of the civic option in the list
	def getOptionCivics(self, paeCivics, iCivicOption):
		aeCivics = []
		for iSlot in range(gc.getPlayer(self.iActivePlayer).getMaxCivicSlots()):
			iCivic = paeCivics[self.entry(iCivicOption, iSlot)]
			if iCivic != -1:
				aeCivics.append(iCivic)
		return aeCivics

	def isSelected(self, paeCivics, iCivic):
		return iCivic in self.getOptionCivics(paeCivics, gc.getCivicInfo(iCivic).getCivicOptionType())

	# Added by this mod (civic slots): a copy of the list after a click on the civic. With one slot the civic replaces
	# the old one. With more slots a click adds the civic to the empty slot, or to the last slot when every slot is
	# used, and a click on a selected civic takes it away (not the last one)
	def getClickedCivics(self, paeCivics, iCivic):
		paeNew = paeCivics[:]
		iCivicOption = gc.getCivicInfo(iCivic).getCivicOptionType()
		iNumSlots = self.getNumSlots(iCivicOption)
		aeCivics = self.getOptionCivics(paeCivics, iCivicOption)
		if iNumSlots <= 1:
			aeCivics = [iCivic]
		elif iCivic in aeCivics:
			if len(aeCivics) > 1:
				aeCivics.remove(iCivic)
		elif len(aeCivics) < iNumSlots:
			aeCivics.append(iCivic)
		else:
			aeCivics[iNumSlots - 1] = iCivic
		for iSlot in range(gc.getPlayer(self.iActivePlayer).getMaxCivicSlots()):
			if iSlot < len(aeCivics):
				paeNew[self.entry(iCivicOption, iSlot)] = aeCivics[iSlot]
			else:
				paeNew[self.entry(iCivicOption, iSlot)] = -1
		return paeNew

	# Added by this mod (civic slots): whether a civic option has other civics than in the original list (slot order does not count)
	def isOptionChanged(self, paeCivics, iCivicOption):
		aeNew = self.getOptionCivics(paeCivics, iCivicOption)
		aeOld = self.getOptionCivics(self.m_paeOriginalCivics, iCivicOption)
		aeNew.sort()
		aeOld.sort()
		return aeNew != aeOld

	def interfaceScreen (self):

		screen = self.getScreen()
		if screen.isActive():
			return
		screen.setRenderInterfaceOnly(True);
		screen.showScreen( PopupStates.POPUPSTATE_IMMEDIATE, False)
	
		# Set the background and exit button, and show the screen
		screen.setDimensions(screen.centerX(0), screen.centerY(0), self.W_SCREEN, self.H_SCREEN)
		screen.addDDSGFC(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addPanel( "TechTopPanel", u"", u"", True, False, 0, 0, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_TOPBAR )
		screen.addPanel( "TechBottomPanel", u"", u"", True, False, 0, 713, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_BOTTOMBAR )
		screen.showWindowBackground(False)
		screen.setText(self.CANCEL_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_SCREEN_CANCEL", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_CANCEL, self.Y_CANCEL, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

		# Header...
		screen.setText(self.TITLE_NAME, "Background", u"<font=4b>" + localText.getText("TXT_KEY_CIVICS_SCREEN_TITLE", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_SCREEN, self.Y_TITLE, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		self.setActivePlayer(gc.getGame().getActivePlayer())						

		if (CyGame().isDebugMode()):
			self.szDropdownName = self.DEBUG_DROPDOWN_ID
			screen.addDropDownBoxGFC(self.szDropdownName, 22, 12, 300, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for j in range(gc.getMAX_PLAYERS()):
				if (gc.getPlayer(j).isAlive()):
					screen.addPullDownString(self.szDropdownName, gc.getPlayer(j).getName(), j, j, False )

		screen.addPanel("CivicsBottomLine", "", "", True, True, self.HEADINGS_SPACING, self.BOTTOM_LINE_TOP, self.BOTTOM_LINE_WIDTH, self.BOTTOM_LINE_HEIGHT, PanelStyles.PANEL_STYLE_MAIN)

		# Draw Contents
		self.drawContents()

		return 0

	# Draw the contents...
	def drawContents(self):
	
		self.updateLayout()

		# Draw the radio buttons
		self.drawAllButtons()
				
		# Draw Help Text
		self.drawAllHelpText()
		
		# Update Maintenance/anarchy/etc.
		self.updateAnarchy()

	def drawCivicOptionButtons(self, iCivicOption):

		activePlayer = gc.getPlayer(self.iActivePlayer)
		screen = self.getScreen()
		
		for j in range(gc.getNumCivicInfos()):

			if (gc.getCivicInfo(j).getCivicOptionType() == iCivicOption and self.isCivicShown(j)):										
				screen.setState(self.getCivicsButtonName(j), self.isSelected(self.m_paeCurrentCivics, j))	# civic slots (changed by this mod)
							
				if (self.isSelected(self.m_paeDisplayCivics, j) or self.m_paeHelpCivics[iCivicOption] == j):
					#screen.setState(self.getCivicsButtonName(j), True)
					screen.show(self.getCivicsButtonName(j))
				elif (activePlayer.canDoCivics(j)):
					#screen.setState(self.getCivicsButtonName(j), False)
					screen.show(self.getCivicsButtonName(j))
				else:
					screen.hide(self.getCivicsButtonName(j))
								
	# Will draw the radio buttons (and revolution)
	def drawAllButtons(self):				

		for i in range(gc.getNumCivicOptionInfos()):
		
			fX = self.HEADINGS_SPACING  + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * i
			fY = self.HEADINGS_TOP
			szAreaID = self.AREA_NAME + str(i)
			screen = self.getScreen()
			screen.addPanel(szAreaID, "", "", True, True, fX, fY, self.HEADINGS_WIDTH, self.HEADINGS_BOTTOM - self.HEADINGS_TOP, PanelStyles.PANEL_STYLE_MAIN)
			# Added by this mod (civic slots): the number of civics the player can adopt after the name, when more than one
			szHeading = gc.getCivicOptionInfo(i).getDescription().upper()
			if self.getNumSlots(i) > 1:
				szHeading += u" (%d)" % self.getNumSlots(i)
			screen.setLabel("", "Background",  u"<font=3>" + szHeading + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, fX + self.HEADINGS_WIDTH/2, self.HEADINGS_TOP + self.TEXT_MARGIN, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

			fY += self.TEXT_MARGIN
			
			for j in range(gc.getNumCivicInfos()):
				if (gc.getCivicInfo(j).getCivicOptionType() == i):										
					if not self.isCivicShown(j):
						screen.deleteWidget(self.getCivicsButtonName(j))
						screen.deleteWidget(self.getCivicsTextName(j))
						continue
					fY += self.iRowHeight
					screen.addCheckBoxGFC(self.getCivicsButtonName(j), gc.getCivicInfo(j).getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), fX + self.BUTTON_SIZE/2, fY, self.BUTTON_SIZE, self.BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
					# Added by this mod: the name is yellow if the civic gives the player traits (bFavoriteCivic)
					szCivicName = gc.getCivicInfo(j).getDescription()
					if gc.getPlayer(self.iActivePlayer).isFavoriteCivicWithTraits(j):
						szCivicName = localText.changeTextColor(szCivicName, gc.getInfoTypeForString("COLOR_YELLOW"))
					screen.setText(self.getCivicsTextName(j), "", szCivicName, CvUtil.FONT_LEFT_JUSTIFY, fX + self.BUTTON_SIZE + self.TEXT_MARGIN, fY, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

			self.drawCivicOptionButtons(i)
					
							
	# Added by this mod: civics that need a trait (PrereqOrTraits) the player does not have, or that another civic
	# replaces for the player (ReplacedCivic), are not listed.
	# The civic the player has now is always listed.
	def isCivicShown(self, iCivic):
		info = gc.getCivicInfo(iCivic)
		if self.isSelected(self.m_paeOriginalCivics, iCivic):	# any civic slot
			return True
		activePlayer = gc.getPlayer(self.iActivePlayer)
		if activePlayer.isCivicReplaced(iCivic):
			return False
		if info.getNumPrereqOrTraits() == 0:
			return True
		for i in range(info.getNumPrereqOrTraits()):
			if activePlayer.hasTrait(info.getPrereqOrTraits(i)):
				return True
		return False

	# Added by this mod: sets the row height, HEADINGS_BOTTOM and HELP_TOP from the civic option with the most listed civics
	def updateLayout(self):
		iMaxRows = self.MIN_ROWS
		for i in range(gc.getNumCivicOptionInfos()):
			iRows = 0
			for j in range(gc.getNumCivicInfos()):
				if (gc.getCivicInfo(j).getCivicOptionType() == i and self.isCivicShown(j)):
					iRows += 1
			iMaxRows = max(iMaxRows, iRows)

		self.iRowHeight = min(2 * self.TEXT_MARGIN, self.MAX_ROWS_HEIGHT // iMaxRows)
		self.HEADINGS_BOTTOM = self.HEADINGS_TOP + self.TEXT_MARGIN + self.iRowHeight * iMaxRows + self.HEADINGS_BOTTOM_MARGIN
		self.HELP_TOP = self.HEADINGS_BOTTOM + self.BIG_BUTTON_SIZE + 6

	# Changed by this mod (civic slots): the display list is what a click on the civic would make (the help text and
	# the anarchy and upkeep show it); nothing changes for a civic that is selected already
	def highlight(self, iCivic):
		iCivicOption = gc.getCivicInfo(iCivic).getCivicOptionType()
		if self.m_paeHelpCivics[iCivicOption] == iCivic:
			return False
		self.m_paeHelpCivics[iCivicOption] = iCivic
		if self.isSelected(self.m_paeCurrentCivics, iCivic) or not gc.getPlayer(self.iActivePlayer).canDoCivics(iCivic):
			self.m_paeDisplayCivics = self.m_paeCurrentCivics[:]
		else:
			self.m_paeDisplayCivics = self.getClickedCivics(self.m_paeCurrentCivics, iCivic)
		self.drawCivicOptionButtons(iCivicOption)
		return True
		
	def unHighlight(self, iCivic):		
		iCivicOption = gc.getCivicInfo(iCivic).getCivicOptionType()
		if self.m_paeDisplayCivics != self.m_paeCurrentCivics or not self.isSelected(self.m_paeCurrentCivics, self.m_paeHelpCivics[iCivicOption]):
			self.m_paeDisplayCivics = self.m_paeCurrentCivics[:]
			if not self.isSelected(self.m_paeCurrentCivics, self.m_paeHelpCivics[iCivicOption]):
				self.m_paeHelpCivics[iCivicOption] = self.m_paeCurrentCivics[iCivicOption]
			self.drawCivicOptionButtons(iCivicOption)
			return True
		return False
		
	def select(self, iCivic):
		activePlayer = gc.getPlayer(self.iActivePlayer)
		if (not activePlayer.canDoCivics(iCivic) and not self.isSelected(self.m_paeCurrentCivics, iCivic)):
			# If you can't even do this, get out....
			return 0
			
		iCivicOption = gc.getCivicInfo(iCivic).getCivicOptionType()
		
		# Changed by this mod (civic slots): the civic is added, replaces one, or is taken away
		self.m_paeCurrentCivics = self.getClickedCivics(self.m_paeCurrentCivics, iCivic)
		self.m_paeDisplayCivics = self.m_paeCurrentCivics[:]
		if self.isSelected(self.m_paeCurrentCivics, iCivic):
			self.m_paeHelpCivics[iCivicOption] = iCivic
		else:
			self.m_paeHelpCivics[iCivicOption] = self.m_paeCurrentCivics[iCivicOption]
		self.drawCivicOptionButtons(iCivicOption)
		
		return 0

	def CivicsButton(self, inputClass):
	
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			if (inputClass.getFlags() & MouseFlags.MOUSE_RBUTTONUP):
				CvScreensInterface.pediaJumpToCivic((inputClass.getID(), ))
			else:
				# Select button
				self.select(inputClass.getID())
				self.drawHelpText(gc.getCivicInfo(inputClass.getID()).getCivicOptionType())
				self.updateAnarchy()
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_ON) :
			# Highlight this button
			if self.highlight(inputClass.getID()):
				self.drawHelpText(gc.getCivicInfo(inputClass.getID()).getCivicOptionType())
				self.updateAnarchy()
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_OFF) :
			if self.unHighlight(inputClass.getID()):
				self.drawHelpText(gc.getCivicInfo(inputClass.getID()).getCivicOptionType())
				self.updateAnarchy()

		return 0

		
	def drawHelpText(self, iCivicOption):
		
		activePlayer = gc.getPlayer(self.iActivePlayer)
		iCivic = self.m_paeHelpCivics[iCivicOption]	# civic slots (changed by this mod)

		szPaneID = "CivicsHelpTextBackground" + str(iCivicOption)
		screen = self.getScreen()

		szHelpText = u""

		# Upkeep string
		if ((gc.getCivicInfo(iCivic).getUpkeep() != -1) and not activePlayer.isNoCivicUpkeep(iCivicOption)):
			szHelpText = gc.getUpkeepInfo(gc.getCivicInfo(iCivic).getUpkeep()).getDescription()
		else:
			szHelpText = localText.getText("TXT_KEY_CIVICS_SCREEN_NO_UPKEEP", ())

		szHelpText += CyGameTextMgr().parseCivicInfo(iCivic, False, True, True)

		fX = self.HEADINGS_SPACING  + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * iCivicOption

		screen.setLabel(self.HELP_HEADER_NAME + str(iCivicOption), "Background",  u"<font=3>" + gc.getCivicInfo(iCivic).getDescription().upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, fX + self.HEADINGS_WIDTH/2, self.HELP_TOP + self.TEXT_MARGIN, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		fY = self.HELP_TOP - self.BIG_BUTTON_SIZE
		szHelpImageID = self.HELP_IMAGE_NAME + str(iCivicOption)		
		screen.setImageButton(szHelpImageID, gc.getCivicInfo(iCivic).getButton(), fX + self.HEADINGS_WIDTH/2 - self.BIG_BUTTON_SIZE/2, fY, self.BIG_BUTTON_SIZE, self.BIG_BUTTON_SIZE, WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIVIC, iCivic, 1)

		fY = self.HELP_TOP + 3 * self.TEXT_MARGIN
		szHelpAreaID = self.HELP_AREA_NAME + str(iCivicOption)		
		screen.addMultilineText(szHelpAreaID, szHelpText, fX+5, fY, self.HEADINGS_WIDTH-7, self.HELP_BOTTOM - fY-2, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)				
		
		
	# Will draw the help text
	def drawAllHelpText(self):
		for i in range (gc.getNumCivicOptionInfos()):		

			fX = self.HEADINGS_SPACING  + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * i

			szPaneID = "CivicsHelpTextBackground" + str(i)
			screen = self.getScreen()
			screen.addPanel(szPaneID, "", "", True, True, fX, self.HELP_TOP, self.HEADINGS_WIDTH, self.HELP_BOTTOM - self.HELP_TOP, PanelStyles.PANEL_STYLE_MAIN)

			self.drawHelpText(i)


	# Will Update the maintenance/anarchy/etc
	def updateAnarchy(self):

		screen = self.getScreen()

		activePlayer = gc.getPlayer(self.iActivePlayer)

		bChange = False
		i = 0
		while (i  < gc.getNumCivicOptionInfos() and not bChange):
			if (self.isOptionChanged(self.m_paeCurrentCivics, i)):	# civic slots (changed by this mod)
				bChange = True
			i += 1		
		
		# Make the revolution button
		screen.deleteWidget(self.EXIT_NAME)
		if (activePlayer.canRevolution(0) and bChange):			
			screen.setText(self.EXIT_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_CONCEPT_REVOLUTION", ( )).upper() + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_REVOLUTION, 1, 0)
			screen.show(self.CANCEL_NAME)
		else:
			screen.setText(self.EXIT_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ( )).upper() + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, -1)
			screen.hide(self.CANCEL_NAME)

		# Anarchy
		iTurns = activePlayer.getCivicAnarchyLength(self.m_paeDisplayCivics);
		
		if (activePlayer.canRevolution(0)):
			szText = localText.getText("TXT_KEY_ANARCHY_TURNS", (iTurns, ))
		else:
			szText = CyGameTextMgr().setRevolutionHelp(self.iActivePlayer)

		screen.setLabel("CivicsRevText", "Background", u"<font=3>" + szText + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_SCREEN, self.BOTTOM_LINE_TOP + self.TEXT_MARGIN//2, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Maintenance		
		szText = localText.getText("TXT_KEY_CIVIC_SCREEN_UPKEEP", (activePlayer.getCivicUpkeep(self.m_paeDisplayCivics, True), ))
		screen.setLabel("CivicsUpkeepText", "Background", u"<font=3>" + szText + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_SCREEN, self.BOTTOM_LINE_TOP + self.BOTTOM_LINE_HEIGHT - 2 * self.TEXT_MARGIN, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
	# Revolution!!!
	def Revolution(self, inputClass):

		activePlayer = gc.getPlayer(self.iActivePlayer)

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			if (activePlayer.canRevolution(0)):
				messageControl = CyMessageControl()
				messageControl.sendUpdateCivics(self.m_paeCurrentCivics)	# every civic slot (changed by this mod)
			screen = self.getScreen()
			screen.hideScreen()


	def Cancel(self, inputClass):
		screen = self.getScreen()
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			# Changed by this mod (civic slots): every slot
			self.m_paeCurrentCivics = self.m_paeOriginalCivics[:]
			self.m_paeDisplayCivics = self.m_paeOriginalCivics[:]
			for i in range (gc.getNumCivicOptionInfos()):
				self.m_paeHelpCivics[i] = self.m_paeOriginalCivics[i]
			
			self.drawContents()
			
	def getCivicsButtonName(self, iCivic):
		szName = self.BUTTON_NAME + str(iCivic)
		return szName

	def getCivicsTextName(self, iCivic):
		szName = self.TEXT_NAME + str(iCivic)
		return szName

	# Will handle the input for this screen...
	def handleInput(self, inputClass):
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			screen = self.getScreen()
			iIndex = screen.getSelectedPullDownID(self.DEBUG_DROPDOWN_ID)
			self.setActivePlayer(screen.getPullDownData(self.DEBUG_DROPDOWN_ID, iIndex))
			self.drawContents()
			return 1
		elif (self.CivicsScreenInputMap.has_key(inputClass.getFunctionName())):	
			'Calls function mapped in CvCivicsScreen'
			# only get from the map if it has the key		

			# get bound function from map and call it
			self.CivicsScreenInputMap.get(inputClass.getFunctionName())(inputClass)
			return 1
		return 0
		
	def update(self, fDelta):
		return




