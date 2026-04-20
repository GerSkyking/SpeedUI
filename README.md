# SpeedUI Mod für Arma Reforger

Ein einfaches HUD-Mod, das die aktuelle Geschwindigkeit des Spielers anzeigt.

## Features

- 📊 **Numerische Anzeige**: Zeigt die aktuelle Geschwindigkeit in km/h an (z.B. "45.3 km/h")
- 📈 **Progress Bar**: Visueller Balken zeigt die Geschwindigkeit relativ zur Maximalgeschwindigkeit
- ⌨️ **Keybind Toggle**: Schaltet das HUD mit einer Taste ein/aus
- 👥 **Multiplayer-ready**: Funktioniert für mehrere Spieler gleichzeitig

## Installation & Setup

1. Diesen Ordner in `Arma Reforger\addons\` kopieren
2. Im Workbench laden: File → Open Project → `addon.gproj`

### Komponente zum Player hinzufügen

Das Speed HUD funktioniert in **jedem GameMode**, aber die `TAG_SpeedHUD` Komponente muss zum Player-Prefab hinzugefügt werden:

**Option 1: Im Base Game Player**
1. Öffne den Player-Prefab (z.B. `{GUID}Characters/Blufor/US_Rifleman.et`)
2. Im Workbench: Inspector → Components → + Add
3. Suche `TAG_SpeedHUD` und füge hinzu
4. Speichern

**Option 2: Custom Player Prefab**
1. Erstelle ein neues Prefab mit deinem Player
2. Füge die `TAG_SpeedHUD` Komponente hinzu
3. Nutze dieses Prefab in deinen Missionen

## Verwendung

### Im Workbench
1. Öffne eine Welt mit einem Player, der `TAG_SpeedHUD` hat
2. Starten (Play)
3. Das HUD sollte sichtbar sein und updaten

### Im Spiel
- **Standard Keybind**: Muss in der Engine konfiguriert werden
- **HUD Toggle**: Drücke die Taste um HUD an/aus zu schalten
- Das HUD zeigt Geschwindigkeit in Echtzeit

## Konfiguration

### Keybind-Einrichtung

Die Aktion `SpeedHUDToggle` ist registriert, benötigt aber eine Keybind-Konfiguration:

**Option 1: Engine-Konfiguration**
- Bearbeite `chimeraInputCommon.conf` und füge hinzu:
```
Action "SpeedHUDToggle" {
 Type Digital
 InputSources {
  InputSourceValue { Input "keyboard:DIK_V" }
 }
}
```

**Option 2: Automatisch im Code**
- Die `TAG_SpeedUIGameMode` versucht, die Action automatisch zu registrieren

### Maximalgeschwindigkeit anpassen

In `TAG_SpeedHUD.c`:
```c
private float m_MaxSpeed = 50.0;  // Ändere hier auf gewünschte km/h
```

Oder zur Laufzeit:
```c
speedHUD.SetMaxSpeed(100.0);
```

## Dateistruktur

```
SpeedUI/
├── Scripts/Game/
│   ├── TAG_SpeedHUD.c              // HUD-Widget Manager
│   ├── TAG_SpeedHUDManager.c       // Input Handler
│   └── TAG_SpeedUIGameMode.c       // Game Mode
├── UI/layouts/
│   └── PlayerSpeed.layout          // HUD Layout
├── Missions/
│   └── SpeedUIConfig.conf          // Mission Header
└── addon.gproj                     // Projekt-Datei
```

## Technische Details

- **Geschwindigkeitsmessung**: Nutzt `CharacterControllerComponent.GetVelocity()`
- **Einheit**: Konvertierung von m/s zu km/h (×3.6)
- **Update-Frequenz**: Jeder Frame (abhängig von `OnFrameUpdate`)
- **HUD-Position**: Bottom-Left, 20px vom Rand

## Fehlerbehebung

### HUD wird nicht angezeigt
- Überprüfe, ob die GameMode richtig zugewiesen ist
- Prüfe die Script-Logs auf Fehler
- Stelle sicher, dass `TAG_SpeedHUD.c` die richtige Layout-GUID hat

### Keybind funktioniert nicht
- Überprüfe die Konfiguration in `chimeraInputCommon.conf`
- Versuche eine andere Taste
- Überprüfe die Konsole auf Input-Fehler

## Erweiterungsmöglichkeiten

- Verschiedene Speed-Einheiten (m/s, mph)
- Durchschnittliche Geschwindigkeit anzeigen
- Geschwindigkeit für Fahrzeuge
- Sound-Feedback
- Anpassbare HUD-Position und Größe

## Lizenz

Frei verwendbar für private und öffentliche Mods
