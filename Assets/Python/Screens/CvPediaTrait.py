## Civilopedia screen for traits (added by this mod)
## Based on CvPediaPromotion.py. Traits have no buttons, so the page shows
## the leaders and civilizations that have the trait instead of an icon.

from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvPediaTrait:
	"Civilopedia Screen for Traits"

	def __init__(self, main):
		self.iTrait = -1
		self.top = main

		self.X_LEADER_PANE = 50
		self.Y_LEADER_PANE = 60
		self.W_LEADER_PANE = 250
		self.H_LEADER_PANE = 300

		self.X_CIV_PANE = 50
		self.Y_CIV_PANE = 374
		self.W_CIV_PANE = 250
		self.H_CIV_PANE = 300

		self.X_ALLOWS_PANE = 330
		self.Y_ALLOWS_PANE = 60
		self.W_ALLOWS_PANE = 420
		self.H_ALLOWS_PANE = 124

		self.X_SPECIAL_PANE = 330
		self.Y_SPECIAL_PANE = 198
		self.W_SPECIAL_PANE = 420
		self.H_SPECIAL_PANE = 476

	# Screen construction function
	def interfaceScreen(self, iTrait):
		self.iTrait = iTrait

		self.top.deleteAllWidgets()

		screen = self.top.getScreen()

		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getTraitInfo(self.iTrait).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Top
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, CivilopediaPageTypes.CIVILOPEDIA_PAGE_TRAIT, -1)

		if self.top.iLastScreen != CvScreenEnums.PEDIA_TRAIT or bNotActive:
			self.placeLinks(True)
			self.top.iLastScreen = CvScreenEnums.PEDIA_TRAIT
		else:
			self.placeLinks(False)

		self.placeLeaders()
		self.placeCivs()
		self.placeAllows()
		self.placeSpecial()

	# Leaders that have this trait themselves
	def placeLeaders(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_CATEGORY_LEADER", ()), "", True, True,
			self.X_LEADER_PANE, self.Y_LEADER_PANE, self.W_LEADER_PANE, self.H_LEADER_PANE, PanelStyles.PANEL_STYLE_BLUE50 )

		szTable = self.top.getNextWidgetName()
		screen.addTableControlGFC(szTable, 1,
			self.X_LEADER_PANE + 10, self.Y_LEADER_PANE + 40, self.W_LEADER_PANE - 20, self.H_LEADER_PANE - 50, False, False, 24, 24, TableStyles.TABLE_STYLE_EMPTY)

		iBarbarianLeader = gc.getDefineINT("BARBARIAN_LEADER")
		listLeaders = self.top.getSortedList( gc.getNumLeaderHeadInfos(), gc.getLeaderHeadInfo )
		iRow = 0
		for item in listLeaders:
			iLeader = item[1]
			if iLeader != iBarbarianLeader and gc.getLeaderHeadInfo(iLeader).hasTrait(self.iTrait):
				# Same as the leader list: pass the civilization only when the leader belongs to exactly one
				iNumCivs = 0
				iLeaderCiv = -1
				for iCiv in range(gc.getNumCivilizationInfos()):
					if gc.getCivilizationInfo(iCiv).isLeaders(iLeader):
						iNumCivs += 1
						iLeaderCiv = iCiv
				if iNumCivs != 1:
					iLeaderCiv = -1

				screen.appendTableRow(szTable)
				screen.setTableText(szTable, 0, iRow, u"<font=2>" + item[0] + u"</font>", gc.getLeaderHeadInfo(iLeader).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_LEADER, iLeader, iLeaderCiv, CvUtil.FONT_LEFT_JUSTIFY)
				iRow += 1

	# Civilizations that have this trait (<Traits> in CIV4CivilizationInfos.xml)
	def placeCivs(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_CATEGORY_CIV", ()), "", True, True,
			self.X_CIV_PANE, self.Y_CIV_PANE, self.W_CIV_PANE, self.H_CIV_PANE, PanelStyles.PANEL_STYLE_BLUE50 )

		szTable = self.top.getNextWidgetName()
		screen.addTableControlGFC(szTable, 1,
			self.X_CIV_PANE + 10, self.Y_CIV_PANE + 40, self.W_CIV_PANE - 20, self.H_CIV_PANE - 50, False, False, 24, 24, TableStyles.TABLE_STYLE_EMPTY)

		listCivs = self.top.getSortedList( gc.getNumCivilizationInfos(), gc.getCivilizationInfo )
		iRow = 0
		for item in listCivs:
			iCiv = item[1]
			if gc.getCivilizationInfo(iCiv).hasTrait(self.iTrait):
				screen.appendTableRow(szTable)
				screen.setTableText(szTable, 0, iRow, u"<font=2>" + item[0] + u"</font>", gc.getCivilizationInfo(iCiv).getButton(), WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIV, iCiv, -1, CvUtil.FONT_LEFT_JUSTIFY)
				iRow += 1

	# Units, buildings and civics that list this trait in <PrereqOrTraits>
	def placeAllows(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_ALLOWS", ()), "", False, True,
			self.X_ALLOWS_PANE, self.Y_ALLOWS_PANE, self.W_ALLOWS_PANE, self.H_ALLOWS_PANE, PanelStyles.PANEL_STYLE_BLUE50 )

		screen.attachLabel(panelName, "", "  ")

		for iUnit in range(gc.getNumUnitInfos()):
			if self.isPrereqTrait(gc.getUnitInfo(iUnit)) or gc.getTraitInfo(self.iTrait).getUniqueUnitForClass(gc.getUnitInfo(iUnit).getUnitClassType()) == iUnit:
				szButton = gc.getUnitInfo(iUnit).getButton()
				if self.top.iActivePlayer != -1:
					szButton = gc.getPlayer(self.top.iActivePlayer).getUnitButton(iUnit)
				screen.attachImageButton( panelName, "", szButton, GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, iUnit, 1, False )

		for iBuilding in range(gc.getNumBuildingInfos()):
			if self.isPrereqTrait(gc.getBuildingInfo(iBuilding)) or gc.getTraitInfo(self.iTrait).getUniqueBuildingForClass(gc.getBuildingInfo(iBuilding).getBuildingClassType()) == iBuilding:
				screen.attachImageButton( panelName, "", gc.getBuildingInfo(iBuilding).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iBuilding, 1, False )

		for iCivic in range(gc.getNumCivicInfos()):
			if self.isPrereqTrait(gc.getCivicInfo(iCivic)):
				screen.attachImageButton( panelName, "", gc.getCivicInfo(iCivic).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIVIC, iCivic, 1, False )

	# info: a unit, building or civic info
	def isPrereqTrait(self, info):
		for i in range(info.getNumPrereqOrTraits()):
			if info.getPrereqOrTraits(i) == self.iTrait:
				return True
		return False

	# Effects, from the same text as the trait help
	def placeSpecial(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_SPECIAL_ABILITIES", ()), "", True, False,
			self.X_SPECIAL_PANE, self.Y_SPECIAL_PANE, self.W_SPECIAL_PANE, self.H_SPECIAL_PANE, PanelStyles.PANEL_STYLE_BLUE50 )

		# parseTraits starts with a newline and the trait name; the header already shows the name
		szSpecialText = CyGameTextMgr().parseTraits(self.iTrait, -1, False)
		iNameEnd = szSpecialText.find(u"\n", 1)
		if iNameEnd >= 0:
			szSpecialText = szSpecialText[iNameEnd + 1:]
		else:
			szSpecialText = u""

		listName = self.top.getNextWidgetName()
		screen.addMultilineText(listName, szSpecialText, self.X_SPECIAL_PANE+5, self.Y_SPECIAL_PANE+30, self.W_SPECIAL_PANE-10, self.H_SPECIAL_PANE-35, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeLinks(self, bRedraw):
		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		listSorted = self.top.getSortedList( gc.getNumTraitInfos(), gc.getTraitInfo )

		i = 0
		iSelected = 0
		for item in listSorted:
			if bRedraw:
				screen.appendListBoxStringNoUpdate( self.top.LIST_ID, item[0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_TRAIT, item[1], 0, CvUtil.FONT_LEFT_JUSTIFY )
			if item[1] == self.iTrait:
				iSelected = i
			i += 1

		if bRedraw:
			screen.updateListBox(self.top.LIST_ID)

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0
