class SUI_SpeedUISettings
{
	bool SpeedUI_Enable_Bar = true;
	bool SpeedUI_Enable_Units = true;
	int SpeedUI_Units = 0;
}

class SUI_SpeedUISettingsSubMenu : SCR_SettingsSubMenuBase
{
	// Zu Fuß
	private SCR_SpinBoxComponent m_EnableBarSpinBox;
	private SCR_SpinBoxComponent m_EnableUnitsSpinBox;
	private SCR_SpinBoxComponent m_UnitsSpinBox;

	// Fahrzeug
	private SCR_SpinBoxComponent m_EnableBarSpinBox_v;
	private SCR_SpinBoxComponent m_EnableUnitsSpinBox_v;
	private SCR_SpinBoxComponent m_UnitsSpinBox_v;
	
	//Main
	private SCR_SpinBoxComponent m_SpeedUI_HUD_Transparency;

	//------------------------------------------------------------------------------------------------
	override void OnTabCreate(Widget menuRoot, ResourceName buttonsLayout, int index)
	{
		super.OnTabCreate(menuRoot, buttonsLayout, index);

		// Hole die SpinBoxes für Zu Fuß
		m_EnableBarSpinBox = SCR_SpinBoxComponent.Cast(menuRoot.FindAnyWidget("SpeedUI_Enable_Bar").FindHandler(SCR_SpinBoxComponent));
		m_EnableUnitsSpinBox = SCR_SpinBoxComponent.Cast(menuRoot.FindAnyWidget("SpeedUI_Enable_Units").FindHandler(SCR_SpinBoxComponent));
		m_UnitsSpinBox = SCR_SpinBoxComponent.Cast(menuRoot.FindAnyWidget("SpeedUI_Units").FindHandler(SCR_SpinBoxComponent));

		// Hole die SpinBoxes für Fahrzeug
		m_EnableBarSpinBox_v = SCR_SpinBoxComponent.Cast(menuRoot.FindAnyWidget("SpeedUI_Enable_Bar_v").FindHandler(SCR_SpinBoxComponent));
		m_EnableUnitsSpinBox_v = SCR_SpinBoxComponent.Cast(menuRoot.FindAnyWidget("SpeedUI_Enable_Units_v").FindHandler(SCR_SpinBoxComponent));
		m_UnitsSpinBox_v = SCR_SpinBoxComponent.Cast(menuRoot.FindAnyWidget("SpeedUI_Units_v").FindHandler(SCR_SpinBoxComponent));
		
		//Main
		m_SpeedUI_HUD_Transparency = SCR_SpinBoxComponent.Cast(menuRoot.FindAnyWidget("SpeedUI_HUD_Transparency").FindHandler(SCR_SpinBoxComponent));

		// Lade aktuelle Werte aus Settings
		LoadSpeedUISettings();

		// Registriere Callbacks für Zu Fuß
		if (m_EnableBarSpinBox)
			m_EnableBarSpinBox.m_OnChanged.Insert(OnEnableBarChanged);
		if (m_EnableUnitsSpinBox)
			m_EnableUnitsSpinBox.m_OnChanged.Insert(OnEnableUnitsChanged);
		if (m_UnitsSpinBox)
			m_UnitsSpinBox.m_OnChanged.Insert(OnUnitsChanged);

		// Registriere Callbacks für Fahrzeug
		if (m_EnableBarSpinBox_v)
			m_EnableBarSpinBox_v.m_OnChanged.Insert(OnEnableBarChanged_v);
		if (m_EnableUnitsSpinBox_v)
			m_EnableUnitsSpinBox_v.m_OnChanged.Insert(OnEnableUnitsChanged_v);
		if (m_UnitsSpinBox_v)
			m_UnitsSpinBox_v.m_OnChanged.Insert(OnUnitsChanged_v);
		
		//main
		if (m_SpeedUI_HUD_Transparency)
			m_SpeedUI_HUD_Transparency.m_OnChanged.Insert(Onm_SpeedUI_HUD_TransparencyChanged);
	}

	//------------------------------------------------------------------------------------------------
	private void LoadSpeedUISettings()
	{
		super.LoadSettings();

		SUI_SpeedUISettingsManager settingsManager = SUI_SpeedUISettingsManager.GetInstance();
		if (!settingsManager)
		{
			Print("DEBUG: Settings manager not found");
			return;
		}

		Print("DEBUG LoadSettings: enableBar=" + settingsManager.SpeedUI_Enable_Bar + ", enableUnits=" + settingsManager.SpeedUI_Enable_Units + ", unitType=" + settingsManager.SpeedUI_Units);

		// Lade Zu Fuß Settings
		if (m_EnableBarSpinBox)
		{
			if (settingsManager.SpeedUI_Enable_Bar)
				m_EnableBarSpinBox.SetCurrentItem(1);
			else
				m_EnableBarSpinBox.SetCurrentItem(0);
		}

		if (m_EnableUnitsSpinBox)
		{
			if (settingsManager.SpeedUI_Enable_Units)
				m_EnableUnitsSpinBox.SetCurrentItem(1);
			else
				m_EnableUnitsSpinBox.SetCurrentItem(0);
		}

		if (m_UnitsSpinBox)
			m_UnitsSpinBox.SetCurrentItem(settingsManager.SpeedUI_Units);

		// Lade Fahrzeug Settings
		if (m_EnableBarSpinBox_v)
		{
			if (settingsManager.SpeedUI_Enable_Bar_v)
				m_EnableBarSpinBox_v.SetCurrentItem(1);
			else
				m_EnableBarSpinBox_v.SetCurrentItem(0);
		}

		if (m_EnableUnitsSpinBox_v)
		{
			if (settingsManager.SpeedUI_Enable_Units_v)
				m_EnableUnitsSpinBox_v.SetCurrentItem(1);
			else
				m_EnableUnitsSpinBox_v.SetCurrentItem(0);
		}

		if (m_UnitsSpinBox_v)
			m_UnitsSpinBox_v.SetCurrentItem(settingsManager.SpeedUI_Units_v);
		
		//main
		if (m_SpeedUI_HUD_Transparency)
			m_SpeedUI_HUD_Transparency.SetCurrentItem(settingsManager.SpeedUI_HUD_Transparency);
	}

	//------------------------------------------------------------------------------------------------
	private void OnEnableBarChanged(SCR_SpinBoxComponent comp, int selectedIndex)
	{
		bool value = (selectedIndex == 1);
		SUI_SpeedUISettingsManager settingsManager = SUI_SpeedUISettingsManager.GetInstance();
		settingsManager.SpeedUI_Enable_Bar = value;
		settingsManager.Save();
		Print("DEBUG OnEnableBarChanged: Saved to " + value);
	}

	//------------------------------------------------------------------------------------------------
	private void OnEnableUnitsChanged(SCR_SpinBoxComponent comp, int selectedIndex)
	{
		bool value = (selectedIndex == 1);
		SUI_SpeedUISettingsManager settingsManager = SUI_SpeedUISettingsManager.GetInstance();
		settingsManager.SpeedUI_Enable_Units = value;
		settingsManager.Save();
	}

	//------------------------------------------------------------------------------------------------
	private void OnUnitsChanged(SCR_SpinBoxComponent comp, int selectedIndex)
	{
		SUI_SpeedUISettingsManager settingsManager = SUI_SpeedUISettingsManager.GetInstance();
		settingsManager.SpeedUI_Units = selectedIndex;
		settingsManager.Save();
	}

	//------------------------------------------------------------------------------------------------
	// Callbacks für Fahrzeug Settings (_v)
	//------------------------------------------------------------------------------------------------
	private void OnEnableBarChanged_v(SCR_SpinBoxComponent comp, int selectedIndex)
	{
		bool value = (selectedIndex == 1);
		SUI_SpeedUISettingsManager settingsManager = SUI_SpeedUISettingsManager.GetInstance();
		settingsManager.SpeedUI_Enable_Bar_v = value;
		settingsManager.Save();
		Print("DEBUG OnEnableBarChanged_v: Saved to " + value);
	}

	//------------------------------------------------------------------------------------------------
	private void OnEnableUnitsChanged_v(SCR_SpinBoxComponent comp, int selectedIndex)
	{
		bool value = (selectedIndex == 1);
		SUI_SpeedUISettingsManager settingsManager = SUI_SpeedUISettingsManager.GetInstance();
		settingsManager.SpeedUI_Enable_Units_v = value;
		settingsManager.Save();
	}

	//------------------------------------------------------------------------------------------------
	private void OnUnitsChanged_v(SCR_SpinBoxComponent comp, int selectedIndex)
	{
		SUI_SpeedUISettingsManager settingsManager = SUI_SpeedUISettingsManager.GetInstance();
		settingsManager.SpeedUI_Units_v = selectedIndex;
		settingsManager.Save();
	}	
	//------------------------------------------------------------------------------------------------
	private void Onm_SpeedUI_HUD_TransparencyChanged(SCR_SpinBoxComponent comp, int selectedIndex)
	{
		SUI_SpeedUISettingsManager settingsManager = SUI_SpeedUISettingsManager.GetInstance();
		settingsManager.SpeedUI_HUD_Transparency = selectedIndex;
		settingsManager.Save();
	}
}
