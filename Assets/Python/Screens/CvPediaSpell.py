## Civilopedia screen for spells (added by this mod)
## Based on CvPediaPromotion.py: the spell button, the prerequisites and the effects as buttons,
## the spell help (the same text as the action button) and the Civilopedia text.

from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvPediaSpell:
	"Civilopedia Screen for Spells"

	def __init__(self, main):
		self.iSpell = -1
		self.top = main

		self.X_ICON_PANE = 50
		self.Y_ICON_PANE = 60
		self.W_ICON_PANE = 250
		self.H_ICON_PANE = 230

		self.X_ICON = 100
		self.Y_ICON = 100
		self.W_ICON = 150
		self.H_ICON = 150
		self.ICON_SIZE = 64

		self.X_PREREQ_PANE = 330
		self.Y_PREREQ_PANE = 60
		self.W_PREREQ_PANE = 420
		self.H_PREREQ_PANE = 110

		self.X_EFFECTS_PANE = 330
		self.Y_EFFECTS_PANE = 180
		self.W_EFFECTS_PANE = 420
		self.H_EFFECTS_PANE = 110

		self.X_SPECIAL_PANE = 330
		self.Y_SPECIAL_PANE = 300
		self.W_SPECIAL_PANE = 420
		self.H_SPECIAL_PANE = 374

		self.X_HISTORY_PANE = 50
		self.Y_HISTORY_PANE = 300
		self.W_HISTORY_PANE = 250
		self.H_HISTORY_PANE = 374

	# Screen construction function
	def interfaceScreen(self, iSpell):
		self.iSpell = iSpell

		self.top.deleteAllWidgets()

		screen = self.top.getScreen()

		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getSpellInfo(self.iSpell).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Top
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, CivilopediaPageTypes.CIVILOPEDIA_PAGE_SPELL, -1)

		if self.top.iLastScreen != CvScreenEnums.PEDIA_SPELL or bNotActive:
			self.placeLinks(True)
			self.top.iLastScreen = CvScreenEnums.PEDIA_SPELL
		else:
			self.placeLinks(False)

		# Icon
		screen.addPanel( self.top.getNextWidgetName(), "", "", False, False,
			self.X_ICON_PANE, self.Y_ICON_PANE, self.W_ICON_PANE, self.H_ICON_PANE, PanelStyles.PANEL_STYLE_BLUE50)
		screen.addPanel(self.top.getNextWidgetName(), "", "", False, False,
			self.X_ICON, self.Y_ICON, self.W_ICON, self.H_ICON, PanelStyles.PANEL_STYLE_MAIN)
		screen.addDDSGFC(self.top.getNextWidgetName(), gc.getSpellInfo(self.iSpell).getButton(),
			self.X_ICON + self.W_ICON/2 - self.ICON_SIZE/2, self.Y_ICON + self.H_ICON/2 - self.ICON_SIZE/2, self.ICON_SIZE, self.ICON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		self.placePrereqs()
		self.placeEffects()
		self.placeSpecial()
		self.placeHistory()

	# The civilization whose units and buildings are shown: CivilizationPrereq of the spell, or the active player's
	def getCiv(self):
		iCiv = gc.getSpellInfo(self.iSpell).getCivilizationPrereq()
		if iCiv == -1 and self.top.iActivePlayer != -1:
			iCiv = gc.getPlayer(self.top.iActivePlayer).getCivilizationType()
		return iCiv

	def attachUnit(self, panelName, iUnit):
		screen = self.top.getScreen()
		szButton = gc.getUnitInfo(iUnit).getButton()
		if self.top.iActivePlayer != -1:
			szButton = gc.getPlayer(self.top.iActivePlayer).getUnitButton(iUnit)
		screen.attachImageButton( panelName, "", szButton, GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, iUnit, 1, False )

	def attachPromotion(self, panelName, iPromotion):
		screen = self.top.getScreen()
		screen.attachImageButton( panelName, "", gc.getPromotionInfo(iPromotion).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROMOTION, iPromotion, 1, False )

	def attachBuilding(self, panelName, iBuilding):
		screen = self.top.getScreen()
		screen.attachImageButton( panelName, "", gc.getBuildingInfo(iBuilding).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iBuilding, 1, False )

	def attachFeature(self, panelName, iFeature):
		screen = self.top.getScreen()
		screen.attachImageButton( panelName, "", gc.getFeatureInfo(iFeature).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_FEATURE, iFeature, 1, False )

	def attachImprovement(self, panelName, iImprovement):
		screen = self.top.getScreen()
		screen.attachImageButton( panelName, "", gc.getImprovementInfo(iImprovement).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_IMPROVEMENT, iImprovement, 1, False )

	def attachReligion(self, panelName, iReligion):
		screen = self.top.getScreen()
		screen.attachImageButton( panelName, "", gc.getReligionInfo(iReligion).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_RELIGION, iReligion, 1, False )

	# Prerequisites, in the order of the spell help (the text says which unit has to be on the same tile)
	def placePrereqs(self):
		screen = self.top.getScreen()
		info = gc.getSpellInfo(self.iSpell)
		iCiv = self.getCiv()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_REQUIRES", ()), "", False, True,
			self.X_PREREQ_PANE, self.Y_PREREQ_PANE, self.W_PREREQ_PANE, self.H_PREREQ_PANE, PanelStyles.PANEL_STYLE_BLUE50 )

		screen.attachLabel(panelName, "", "  ")

		iPrereqCiv = info.getCivilizationPrereq()
		if iPrereqCiv != -1:
			screen.attachImageButton( panelName, "", gc.getCivilizationInfo(iPrereqCiv).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIV, iPrereqCiv, 1, False )

		iTech = info.getTechPrereq()
		if iTech != -1:
			screen.attachImageButton( panelName, "", gc.getTechInfo(iTech).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_TECH, iTech, 1, False )

		iReligion = info.getStateReligionPrereq()
		if iReligion != -1:
			self.attachReligion(panelName, iReligion)

		iUnit = info.getUnitPrereq()
		if iUnit != -1:
			self.attachUnit(panelName, iUnit)

		iUnitCombat = info.getUnitCombatPrereq()
		if iUnitCombat != -1:
			screen.attachImageButton( panelName, "", gc.getUnitCombatInfo(iUnitCombat).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT_COMBAT, iUnitCombat, 1, False )

		# A unit class: the unit of the civilization, or the default unit of the class
		iUnitClass = info.getUnitClassPrereq()
		if iUnitClass != -1:
			iUnit = -1
			if iCiv != -1:
				iUnit = gc.getCivilizationInfo(iCiv).getCivilizationUnits(iUnitClass)
			if iUnit == -1:
				iUnit = gc.getUnitClassInfo(iUnitClass).getDefaultUnitIndex()
			if iUnit != -1:
				self.attachUnit(panelName, iUnit)

		for iPromotion in [info.getPromotionPrereq1(), info.getPromotionPrereq2()]:
			if iPromotion != -1:
				self.attachPromotion(panelName, iPromotion)

		# A building class owned anywhere: the building of the civilization, or the default building of the class
		iBuildingClass = info.getBuildingClassOwnedPrereq()
		if iBuildingClass != -1:
			iBuilding = -1
			if iCiv != -1:
				iBuilding = gc.getCivilizationInfo(iCiv).getCivilizationBuildings(iBuildingClass)
			if iBuilding == -1:
				iBuilding = gc.getBuildingClassInfo(iBuildingClass).getDefaultBuildingIndex()
			if iBuilding != -1:
				self.attachBuilding(panelName, iBuilding)

		iBuilding = info.getBuildingPrereq()
		if iBuilding != -1:
			self.attachBuilding(panelName, iBuilding)

		iCorporation = info.getCorporationPrereq()
		if iCorporation != -1:
			screen.attachImageButton( panelName, "", gc.getCorporationInfo(iCorporation).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_CORPORATION, iCorporation, 1, False )

		iFeature1 = info.getFeatureOrPrereq1()
		iFeature2 = info.getFeatureOrPrereq2()
		if iFeature1 != -1:
			self.attachFeature(panelName, iFeature1)
			if iFeature2 != -1:
				screen.attachLabel(panelName, "", localText.getText("TXT_KEY_OR", ()))
				self.attachFeature(panelName, iFeature2)

		iImprovement = info.getImprovementPrereq()
		if iImprovement != -1:
			self.attachImprovement(panelName, iImprovement)

		iPromotion = info.getPromotionInStackPrereq()
		if iPromotion != -1:
			self.attachPromotion(panelName, iPromotion)

		iUnit = info.getUnitInStackPrereq()
		if iUnit != -1:
			self.attachUnit(panelName, iUnit)

	# What the spell creates or gives, in the order of the spell help (removed promotions are only in the text)
	def placeEffects(self):
		screen = self.top.getScreen()
		info = gc.getSpellInfo(self.iSpell)

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_EFFECTS", ()), "", False, True,
			self.X_EFFECTS_PANE, self.Y_EFFECTS_PANE, self.W_EFFECTS_PANE, self.H_EFFECTS_PANE, PanelStyles.PANEL_STYLE_BLUE50 )

		screen.attachLabel(panelName, "", "  ")

		iUnit = info.getCreateUnitType()
		if iUnit != -1:
			self.attachUnit(panelName, iUnit)

			iPromotion = info.getCreateUnitPromotion()
			if iPromotion != -1:
				self.attachPromotion(panelName, iPromotion)

		for iPromotion in [info.getAddPromotionType1(), info.getAddPromotionType2(), info.getAddPromotionType3()]:
			if iPromotion != -1:
				self.attachPromotion(panelName, iPromotion)

		iUnit = info.getConvertUnitType()
		if iUnit != -1:
			self.attachUnit(panelName, iUnit)

		iBuilding = info.getCreateBuildingType()
		if iBuilding != -1:
			self.attachBuilding(panelName, iBuilding)

		iFeature = info.getCreateFeatureType()
		if iFeature != -1:
			self.attachFeature(panelName, iFeature)

		iImprovement = info.getCreateImprovementType()
		if iImprovement != -1:
			self.attachImprovement(panelName, iImprovement)

		iReligion = info.getSpreadReligion()
		if iReligion != -1:
			self.attachReligion(panelName, iReligion)

	# The spell help: the same text as the action button, without the name
	def placeSpecial(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_SPECIAL_ABILITIES", ()), "", True, False,
			self.X_SPECIAL_PANE, self.Y_SPECIAL_PANE, self.W_SPECIAL_PANE, self.H_SPECIAL_PANE, PanelStyles.PANEL_STYLE_BLUE50 )

		# The help starts with a newline
		szSpecialText = CyGameTextMgr().getSpellHelp(self.iSpell, True)[1:]

		listName = self.top.getNextWidgetName()
		screen.addMultilineText(listName, szSpecialText, self.X_SPECIAL_PANE+5, self.Y_SPECIAL_PANE+30, self.W_SPECIAL_PANE-10, self.H_SPECIAL_PANE-35, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	# The Civilopedia text of the spell
	def placeHistory(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_CIVILOPEDIA_HISTORY", ()), "", True, True,
			self.X_HISTORY_PANE, self.Y_HISTORY_PANE, self.W_HISTORY_PANE, self.H_HISTORY_PANE, PanelStyles.PANEL_STYLE_BLUE50 )

		szText = gc.getSpellInfo(self.iSpell).getCivilopedia()

		listName = self.top.getNextWidgetName()
		screen.addMultilineText(listName, szText, self.X_HISTORY_PANE+5, self.Y_HISTORY_PANE+30, self.W_HISTORY_PANE-10, self.H_HISTORY_PANE-35, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

	def placeLinks(self, bRedraw):
		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		listSorted = self.top.getSortedList( gc.getNumSpellInfos(), gc.getSpellInfo )

		i = 0
		iSelected = 0
		for item in listSorted:
			if bRedraw:
				screen.appendListBoxStringNoUpdate( self.top.LIST_ID, item[0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_SPELL, item[1], 0, CvUtil.FONT_LEFT_JUSTIFY )
			if item[1] == self.iSpell:
				iSelected = i
			i += 1

		if bRedraw:
			screen.updateListBox(self.top.LIST_ID)

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0
