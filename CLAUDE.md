# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SpeedUI is an Arma Reforger mod (Enfusion engine) that adds a speed HUD showing player/vehicle velocity with a progress bar and text readout. It is built with Enforce Script (`.c` files) and the Arma Reforger Workbench.

## Development Workflow

There are no build commands or test runners. Development is done entirely in the **Arma Reforger Workbench**:

1. Open Workbench → File → Open Project → `addon.gproj`
2. Scripts are compiled automatically when saved
3. Test by pressing Play in the Workbench (opens a test session)
4. Script errors appear in the Workbench console/logs

The `TAG_SpeedHUD` component must be attached to the player prefab (`DefaultPlayerController.et` in this repo). Changes to `.c` files take effect after Workbench reload/recompile.

## Architecture

### Component Flow

```
DefaultPlayerController.et
  └── TAG_SpeedHUD (ScriptComponent)
        ├── reads SUI_SpeedUISettingsManager (singleton)
        └── manages HUD widgets from PlayerSpeed.layout
```

**TAG_SpeedHUD.c** — the entire HUD logic lives here as a `ScriptComponent`. It:
- Initializes via `EOnInit()` → deferred `InitializeHUD()` (100ms delay via `CallLater`)
- Updates every 50ms via a `CallLater` loop (not `EOnFrame` — unreliable in ScriptComponent)
- Detects vehicle entry via `SCR_CompartmentAccessComponent.GetVehicle()`
- Gets vehicle speed via `vehicleEntity.GetPhysics().GetVelocity()` (must go through Physics)
- Gets on-foot speed via `CharacterControllerComponent.GetVelocity()`
- Toggles visibility via the `SpeedHUDToggle` input action (bound to CTRL+5)

**SUI_SpeedUISettings.c** — singleton settings manager. Reads/writes 6 settings (3 on-foot, 3 vehicle) to `$profile:SpeedUI_Settings.txt`, one value per line.

**SUI_SpeedUISettingsSubMenu.c** — in-game settings UI handler. SpinBox widgets map directly to the 6 settings. Changes are persisted immediately on each widget change.

### Settings

6 settings, on-foot and vehicle variants (vehicle uses `_v` suffix):
- `SpeedUI_Enable_Bar` — show/hide progress bar
- `SpeedUI_Enable_Units` — show/hide text readout
- `SpeedUI_Units` — unit: `0`=km/h, `1`=m/s, `2`=kn, `3`=mph

### UI Layouts

- `UI/layouts/HUD/Player/PlayerSpeed.layout` — HUD overlay; contains `SpeedText`, `SpeedBar`, `SpeedCar` (vehicle bar), `SpeedPlayer` (foot bar)
- `UI/layouts/Menus/SettingsMenu/SUI_SpeedUISettings.layout` — settings submenu with 6 spinboxes
- `UI/layouts/Menus/SettingsMenu/SUI_SettingsSuperMenu.layout` — registers "Speed UI" as a settings tab

## Enforce Script Quirks

- **No ternary operator** — `condition ? a : b` does not work; use `if/else`
- **`EOnFrame()` unreliable** in `ScriptComponent` — use `GetGame().GetCallqueue().CallLater()` instead
- **Vehicle physics** must be accessed via `entity.GetPhysics().GetVelocity()`, not directly
- **String formatting** uses concatenation: `value.ToString() + " km/h"`
- **File I/O** uses `$profile:` prefix for the player profile directory
- **Singleton pattern** used for settings: `SUI_SpeedUISettingsManager.GetInstance()`
