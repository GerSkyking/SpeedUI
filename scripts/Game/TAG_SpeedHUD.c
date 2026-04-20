// Geschwindigkeit HUD Anzeige - Komplette Komponente für Player
// Zeigt die Spielergeschwindigkeit in km/h als Zahl und Balken an
// Input-Handling und HUD-Verwaltung inklusive

[ComponentEditorProps(category: "GameScripted", description: "Verwaltet die Geschwindigkeitsanzeige des HUD")]
class TAG_SpeedHUDClass : ScriptComponentClass
{
}

[ComponentEditorProps(category: "GameScripted", description: "Verwaltet die Geschwindigkeitsanzeige des HUD")]
class TAG_SpeedHUD : ScriptComponent
{
	private IEntity m_owner;
	private ref Widget m_HUDRoot;
	private TextWidget m_SpeedText;
	private ProgressBarWidget m_SpeedBar;
	private FrameWidget m_SpeedBarCar, m_SpeedBarPlayer, m_SpeedBarFrame;
	private VerticalLayoutWidget m_SpeedTextFrame;
	private float m_MaxSpeed = 50.0;
	private float m_MaxSpeedFoot = 50.0;
	private float m_MaxSpeedVehicle = 150.0;
	private bool m_Initialized = false;
	private bool m_UpdateRunning = false;
	private bool m_IsInVehicle = false;
	private bool m_EnableBar = true;
	private bool m_EnableUnits = true;
	private int m_UnitType = 0; // 0=km/h, 1=m/s, 2=kn, 3=mph
	private int m_SpeedUI_HUD_Transparency = 0;
	private float m_LastSpeed = -1.0;
	private float m_HideTimer = 0.0;
	private float m_CurrentOpacity = 0.0;
	private float m_TargetOpacity = 0.0;
	

	void TAG_SpeedHUD (IEntityComponentSource src, IEntity ent, IEntity parent)
	{
        // Enable init event
        SetEventMask(ent, EntityEvent.INIT);
    }
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		// HUD initialisieren später (Player ist noch nicht ready)
		GetGame().GetCallqueue().CallLater(InitializeHUD, 100, false);
	}

	private void EnsureOwner()
	{
		if (m_owner && m_owner.FindComponent(CharacterControllerComponent))
			return;

		// Hole den echten Player via PlayerController
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (playerController)
		{
			m_owner = playerController.GetMainEntity();
		}
	}

	private void InitializeHUD()
	{
		if (m_Initialized)
			return;

		EnsureOwner();

		if (!m_owner)
		{
			// Auf Clients ist die Entity beim ersten Versuch oft noch nicht bereit
			GetGame().GetCallqueue().CallLater(InitializeHUD, 500, false);
			return;
		}

		// HUD Layout laden
		WorkspaceWidget workspace = GetGame().GetWorkspace();
		if (!workspace)
		{
			GetGame().GetCallqueue().CallLater(InitializeHUD, 500, false);
			return;
		}

		string layoutPath = "{F46B6C1DAD1906F9}UI/layouts/HUD/Player/Player Speed.layout";
		m_HUDRoot = workspace.CreateWidgets(layoutPath);
		if (!m_HUDRoot)
		{
			GetGame().GetCallqueue().CallLater(InitializeHUD, 500, false);
			return;
		}

		m_Initialized = true;

		// Widgets finden
		m_SpeedText = TextWidget.Cast(m_HUDRoot.FindAnyWidget("SpeedText"));
		m_SpeedBar = ProgressBarWidget.Cast(m_HUDRoot.FindAnyWidget("SpeedBar"));
		m_SpeedBarCar = FrameWidget.Cast(m_HUDRoot.FindAnyWidget("SpeedCar"));
		m_SpeedBarPlayer = FrameWidget.Cast(m_HUDRoot.FindAnyWidget("SpeedPlayer"));
		m_SpeedBarFrame = FrameWidget.Cast(m_HUDRoot.FindAnyWidget("SpeedBarFrame"));
		m_SpeedTextFrame = VerticalLayoutWidget.Cast(m_HUDRoot.FindAnyWidget("SpeedTextFrame"));

		if (!m_SpeedText || !m_SpeedBar)
			return;

		// Initial Setup
		m_SpeedBar.SetMin(0);
		m_SpeedBar.SetMax(m_MaxSpeed);
		m_SpeedText.SetText("0 km/h");
		m_HUDRoot.SetVisible(true);
		m_HUDRoot.SetOpacity(0.0);
		m_CurrentOpacity = 0.0;
		m_TargetOpacity = 0.0;

		// Starte Speed-Update Loop
		StartSpeedUpdate();
	}

	private void StartSpeedUpdate()
	{
		if (m_UpdateRunning)
			return;

		m_UpdateRunning = true;
		GetGame().GetCallqueue().CallLater(UpdateSpeed, 50, true);
	}

	private float GetMaxOpacity()
	{
		// SpinBox index 0 = voll sichtbar (1.0), index 9 = fast unsichtbar (0.1)
		return 1.0 - m_SpeedUI_HUD_Transparency * 0.1;
	}

	protected void UpdateSpeed()
	{
		LoadSettings();
		float speed = GetPlayerSpeed();

		float speedDiff = speed - m_LastSpeed;
		if (speedDiff < 0)
			speedDiff = -speedDiff;

		float maxOpacity = GetMaxOpacity();

		if (speedDiff > 0.5 || m_LastSpeed < 0)
		{
			m_HideTimer = 3.0;
			m_TargetOpacity = maxOpacity;
		}
		else
		{
			m_HideTimer = m_HideTimer - 0.05;
			if (m_HideTimer <= 0)
				m_TargetOpacity = 0.0;
			else
				m_TargetOpacity = maxOpacity;
		}

		m_LastSpeed = speed;

		// Opacity schrittweise anpassen (0.1 pro Tick = ~500ms für 0→1)
		float fadeStep = 0.1;
		if (m_CurrentOpacity < m_TargetOpacity)
		{
			m_CurrentOpacity = m_CurrentOpacity + fadeStep;
			if (m_CurrentOpacity > m_TargetOpacity)
				m_CurrentOpacity = m_TargetOpacity;
		}
		else if (m_CurrentOpacity > m_TargetOpacity)
		{
			m_CurrentOpacity = m_CurrentOpacity - fadeStep;
			if (m_CurrentOpacity < m_TargetOpacity)
				m_CurrentOpacity = m_TargetOpacity;
		}

		if (m_HUDRoot)
			m_HUDRoot.SetOpacity(m_CurrentOpacity);

		if (m_CurrentOpacity <= 0 || !m_HUDRoot)
			return;

		UpdateSpeedDisplay(speed);
	}

	private void LoadSettings()
	{
		SUI_SpeedUISettingsManager settingsManager = SUI_SpeedUISettingsManager.GetInstance();
		if (!settingsManager)
			return;

		// Lade Settings basierend auf Fahrzeug-Status
		if (m_IsInVehicle)
		{
			m_EnableBar = settingsManager.SpeedUI_Enable_Bar_v;
			m_EnableUnits = settingsManager.SpeedUI_Enable_Units_v;
			m_UnitType = settingsManager.SpeedUI_Units_v;
		}
		else
		{
			m_EnableBar = settingsManager.SpeedUI_Enable_Bar;
			m_EnableUnits = settingsManager.SpeedUI_Enable_Units;
			m_UnitType = settingsManager.SpeedUI_Units;
		}
		
		//main
		m_SpeedUI_HUD_Transparency = settingsManager.SpeedUI_HUD_Transparency;
	}

	private float GetPlayerSpeed()
	{
		EnsureOwner();

		if (!m_owner)
			return 0.0;

		// Überprüfe ob Spieler in Fahrzeug sitzt
		SCR_CompartmentAccessComponent compartment = SCR_CompartmentAccessComponent.Cast(
			m_owner.FindComponent(SCR_CompartmentAccessComponent)
		);

		IEntity vehicleEntity = null;
		if (compartment)
			vehicleEntity = compartment.GetVehicle();

		bool wasInVehicle = m_IsInVehicle;
		m_IsInVehicle = (vehicleEntity != null);

		// Wenn Status geändert hat, update MaxSpeed
		if (m_IsInVehicle != wasInVehicle)
		{
			UpdateMaxSpeed(m_IsInVehicle);
		}

		// Wenn im Fahrzeug: Hole Fahrzeug-Geschwindigkeit über Physics
		if (vehicleEntity)
		{
			Physics phy = vehicleEntity.GetPhysics();
			if (phy && phy.IsActive())
			{
				vector vehicleVelocity = phy.GetVelocity();
				float speedMPS = vehicleVelocity.Length();
				float speedKMH = speedMPS * 3.6;
				return speedKMH;
			}
		}

		// Zu Fuß: Hole Charakter-Geschwindigkeit
		CharacterControllerComponent charController = CharacterControllerComponent.Cast(
			m_owner.FindComponent(CharacterControllerComponent)
		);

		if (charController)
		{
			vector velocity = charController.GetVelocity();
			float speedMPS = velocity.Length();
			float speedKMH = speedMPS * 3.6;
			return speedKMH;
		}

		return 0.0;
	}

	private void UpdateMaxSpeed(bool isVehicle)
	{
		// Lade neue Settings für den Fahrzeug-Status
		LoadSettings();

		if (isVehicle)
			m_MaxSpeed = m_MaxSpeedVehicle;
		else
			m_MaxSpeed = m_MaxSpeedFoot;

		if (m_SpeedBar)
			m_SpeedBar.SetMax(m_MaxSpeed);

		// Update UI Sichtbarkeit
		UpdateUIVisibility();
	}

	private void UpdateSpeedDisplay(float speedKMH)
	{
		if (!m_SpeedText || !m_SpeedBar)
			return;

		// Konvertiere Speed basierend auf UnitType
		float displaySpeed = ConvertSpeed(speedKMH, m_UnitType);
		string unitLabel = GetUnitLabel(m_UnitType);

		// Formatiere Speed
		int speedRounded = displaySpeed + 0.5;
		string speedText = speedRounded.ToString();
		if (m_EnableUnits)
			speedText = speedText + " " + unitLabel;
		m_SpeedText.SetText(speedText);

		// Berechne Prozentsatz für ProgressBar (0-100%)
		float clampedSpeed = Math.Clamp(speedKMH, 0, m_MaxSpeed);
		float percentage = (clampedSpeed / m_MaxSpeed) * 100.0;
		m_SpeedBar.SetCurrent(percentage);

		// Kontrolliere Sichtbarkeit basierend auf Settings
		UpdateUIVisibility();
	}

	private void UpdateUIVisibility()
	{
		// Kontrolliere Bar-Sichtbarkeit
		if (!m_EnableBar)
		{
			if (m_SpeedBarCar)
				m_SpeedBarCar.SetVisible(false);
			if (m_SpeedBarPlayer)
				m_SpeedBarPlayer.SetVisible(false);
			if (m_SpeedBar)
				m_SpeedBar.SetVisible(false);
			if (m_SpeedBarFrame)
				m_SpeedBarFrame.SetVisible(false);
		}
		else
		{
			if (m_SpeedBar)
				m_SpeedBar.SetVisible(true);
			if (m_SpeedBarFrame)
				m_SpeedBarFrame.SetVisible(true);

			// Zeige richtige Bar (Auto/Fuß)
			if (m_IsInVehicle)
			{
				if (m_SpeedBarCar)
					m_SpeedBarCar.SetVisible(true);
				if (m_SpeedBarPlayer)
					m_SpeedBarPlayer.SetVisible(false);
			}
			else
			{
				if (m_SpeedBarCar)
					m_SpeedBarCar.SetVisible(false);
				if (m_SpeedBarPlayer)
					m_SpeedBarPlayer.SetVisible(true);
			}
		}

		// Kontrolliere Text-Sichtbarkeit
		if (m_SpeedText)
			m_SpeedText.SetVisible(m_EnableUnits);

		if (m_SpeedTextFrame)
			m_SpeedTextFrame.SetVisible(m_EnableUnits);
	}

	private float ConvertSpeed(float speedKMH, int unitType)
	{
		switch(unitType)
		{
			case 0: return speedKMH; // km/h
			case 1: return speedKMH / 3.6; // m/s
			case 2: return speedKMH / 1.852; // kn (Knoten)
			case 3: return speedKMH / 1.609; // mph
		}
		return speedKMH;
	}

	private string GetUnitLabel(int unitType)
	{
		switch(unitType)
		{
			case 0: return "km/h";
			case 1: return "m/s";
			case 2: return "kn";
			case 3: return "mph";
		}
		return "km/h";
	}

	void SetMaxSpeed(float maxSpeed)
	{
		m_MaxSpeed = Math.Max(1.0, maxSpeed);
		if (m_SpeedBar)
		{
			m_SpeedBar.SetMax(m_MaxSpeed);
		}
	}
}