// SpeedUI Settings Manager - Persistiert Settings im Profil
class SUI_SpeedUISettingsManager
{
	private static ref SUI_SpeedUISettingsManager s_Instance;
	private string m_SettingsPath = "$profile:SpeedUI_Settings.txt";

	// Zu Fuß
	bool SpeedUI_Enable_Bar = true;
	bool SpeedUI_Enable_Units = true;
	int SpeedUI_Units = 0;

	// Fahrzeug
	bool SpeedUI_Enable_Bar_v = true;
	bool SpeedUI_Enable_Units_v = true;
	int SpeedUI_Units_v = 0;
	
	//Main
	int SpeedUI_HUD_Transparency = 0;

	static SUI_SpeedUISettingsManager GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new SUI_SpeedUISettingsManager();
			s_Instance.Load();
		}
		return s_Instance;
	}

	void Load()
	{
		// Lade Settings aus Datei
		FileHandle handle = FileIO.OpenFile(m_SettingsPath, FileMode.READ);
		if (!handle)
		{
			Print("DEBUG: Settings file not found, using defaults");
			return;
		}

		string line;
		int lineNum = 0;

		while (handle.ReadLine(line) > 0)
		{
			if (lineNum == 0)
				SpeedUI_Enable_Bar = (line == "1");
			else if (lineNum == 1)
				SpeedUI_Enable_Units = (line == "1");
			else if (lineNum == 2)
				SpeedUI_Units = line.ToInt();
			else if (lineNum == 3)
				SpeedUI_Enable_Bar_v = (line == "1");
			else if (lineNum == 4)
				SpeedUI_Enable_Units_v = (line == "1");
			else if (lineNum == 5)
				SpeedUI_Units_v = line.ToInt();
			else if (lineNum == 6)
				SpeedUI_HUD_Transparency = line.ToInt();

			lineNum++;
		}

		delete handle;

		Print("DEBUG Load: enableBar=" + SpeedUI_Enable_Bar + ", enableUnits=" + SpeedUI_Enable_Units + ", units=" + SpeedUI_Units);
		Print("DEBUG Load (Vehicle): enableBar_v=" + SpeedUI_Enable_Bar_v + ", enableUnits_v=" + SpeedUI_Enable_Units_v + ", units_v=" + SpeedUI_Units_v);
	}

	void Save()
	{
		// Speichere Settings in Datei - einfach als einzelne Zeilen
		FileHandle handle = FileIO.OpenFile(m_SettingsPath, FileMode.WRITE);
		if (!handle)
		{
			Print("DEBUG: Could not open settings file for writing");
			return;
		}

		// Zu Fuß
		if (SpeedUI_Enable_Bar)
			handle.WriteLine("1");
		else
			handle.WriteLine("0");

		if (SpeedUI_Enable_Units)
			handle.WriteLine("1");
		else
			handle.WriteLine("0");

		handle.WriteLine(SpeedUI_Units.ToString());

		// Fahrzeug
		if (SpeedUI_Enable_Bar_v)
			handle.WriteLine("1");
		else
			handle.WriteLine("0");

		if (SpeedUI_Enable_Units_v)
			handle.WriteLine("1");
		else
			handle.WriteLine("0");

		handle.WriteLine(SpeedUI_Units_v.ToString());
		
		//Main
		handle.WriteLine(SpeedUI_HUD_Transparency.ToString());


		delete handle;
		Print("DEBUG: Settings saved to profile");
	}
}