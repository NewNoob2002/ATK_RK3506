# UI Pages Optimization Plan

## Baseline and scope

- Branch `feature/page-ui-optimization`, baseline `master` at `f3c51f9`.
- LVGL 8.3.11, C11/C++17, **294×126**, English/Russian; reuse existing assets/styles/router.
- Latest behavior correction: SystemDash, SystemInfos and SystemSettings request a hidden StatusBar and use the full screen. SystemDash's Settings child does not reveal the bar while navigating the menu flow. Following HC32, the bar slides y=−26↔0 over 500ms (ease-out show, overshoot hide), not a root hidden flag; repeated requests do not restart and reversals continue from the current y. Status publications never change its position.
- Real Wi-Fi/radio/recording/persistence/power and target validation are separate work. Current power/time services are explicitly simulated.

## Phase 1 — visual refresh (implemented)

RecordConfig/WorkSettings use compact dark cards, external icons, orange focus and staggered fades. Roller row pitch is 26px with fixed-width clipped viewports. Oswald fonts cover Latin/Cyrillic; Rajdhani resources are numeric-only. WorkSettings' old reset-looking icon actually means Back and was corrected, not turned into a fake reset operation. Transitions now follow page context instead of a uniform wipe:

| Context / destination | Transition | Duration |
| --- | --- | --- |
| Startup / SystemLoading / working Dialplate | Fade-in over the previous page | 280 / 360 / 320ms |
| SystemInfos overview | Fade | 280ms |
| WorkSettings / RecordConfig / SystemSettings | Horizontal push; Back reverses right | 300ms |
| SystemDash menu | Horizontal push; Back reverses right | 320ms |
| StarMap detail / SaveConfig progress | Upward push from below; Back reverses down | 280 / 320ms |

Ease-in-out except Startup's ease-out. PageManager resets the non-animated axis/opacity on cached re-entry, preventing a horizontal history offset from surviving a vertical return-to-root. Animation getters use alignment-relative style offsets to match their setters: StarMap's bottom-aligned y=26 origin must not become a 26px jump at the start of Back.

SystemDash follows the confirmed `/home/gtc/Pictures/6b1acacb-cd49-4d4e-a206-0b81bdd9858e.png` design tokens: black screen/status strip, #333333 header/cards, #FF931E focus outline, white text/settings icon, #D03C3B power icon/actions, #666666 Cancel and #999999 supporting text. The latest requested layout is **[Power][Settings][Return]**, left-to-right, with 88px cards and 7px gaps. Return uses the existing router `pop()` on ordinary Enter/Power-single/pointer selection; no new root-reset behavior or test-key footer is added. StarMap's yellow focused-root outline is removed without removing its keyboard focus, constellation colors or click-to-return behavior.

Latest icon refresh replaces the enlarged 16px artwork with newly drawn **40×40 native menu bitmaps** and a separate **24×24 power-modal bitmap**, without LVGL zoom. `Resource/img/img_src_system_dash_power.c`, `img_src_system_dash_settings.c` and `img_src_system_dash_return.c` contain 8-bit alpha masks, recolored by the existing theme (5376 bytes of pixel data total); original icons remain unchanged for other pages. Return artwork is rasterized from the included `system_dash_return.svg`. Reviewed bilingual preview: `out/ui-pages/SystemDash/three-tile-return-starmap.png`.

## Phase 2 — page-owned MVC / reference layers (implemented)

Every routed page has `<Page>.h/.cpp`, `<Page>View.h/.cpp`, **`<Page>Model.h/.cpp` in its own directory**. Includes Dialplate, RecordConfig, WorkSettings, SystemDash, SystemSettings, SystemInfos, StarMap, Startup, SystemLoading and SaveConfig. StatusBar is a shared component.

- `Utils/DataCenter` ports/adapts X-TRACK Account/DataCenter, preserving MIT license. Routing, subscriptions, Publish/Pull/Notify live there.
- `Common/DataProc` initializes named processing Accounts from `DP_LIST.inc` and exposes `DataProc::Center()`. It is not an injected router object.
- Each Model owns its Account and snapshots, initializes/deinitializes with the page, subscribes/pulls Status/System, handles publications and peer pulls, and sends StatusBar/service requests. Shared `Common/PageModel` was removed; Model constructors do not accept DataProc.
- `Common/ModelUtils.h` shares transport validation only, not Model ownership or a base Model.
- `DP_Status.cpp` and `SystemService.cpp` own processing behavior. Providers outlive Models; teardown refuses to destroy providers beneath live UI Accounts.
- Synchronous LVGL-thread delivery only; callbacks borrow payloads during the call. Owned arrays avoid retained producer string pointers. No ISR/background-thread transport or wire-format ABI claim.
- Account registry is bounded at 16 (excluding CENTER); serial-checked publication snapshots tolerate subscriber removal/recreation. A publishing Account/center must not be destroyed from its own callback.

## Phase 3 — system flow and separate input adapters (implemented)

The real RK3506J product uses **two GPIO inputs sampled as HIGH/LOW**, not keyboard F/P keys, ADC/evdev events or host controls. `Key::Function` and `Key::Power` name logical roles only:

| Product GPIO gesture | Behavior |
| --- | --- |
| Function single | NEXT, wrap focus |
| Function double | BACK/CANCEL, close modal / return |
| Power single | ENTER, toggle language / select visible Back |
| Power double | COMMIT for supported actions; dangerous power actions remain explicit |

20ms debounce and 250ms double-click window. Single clicks wait for the window; a second press begun inside it remains double even when its release crosses the deadline. Product context changes and transitions discard stale gestures. Host controls bypass this gesture timing entirely.

```text
Dialplate system entry → SystemDash [Power][Settings][Return]
  Power → modal [Shutdown 2x][Reboot 2x][Cancel]
    Power GPIO double-click on chosen action → SaveConfig timer → SystemService::power(OFF|REBOOT)
  Settings → Pages/SystemSettings
    Language: inline English ↔ Russian
    Wi-Fi: OFF, disabled until backend exists
    Back: visible row, select with one Power click to return to SystemDash
  Return → previous page (Dialplate in the normal system-menu flow)
```

Power modal defaults to Cancel. A single Select cannot execute power actions. Token checking prevents duplicate service execution; leaving SaveConfig before completion cancels its pending transaction. Startup requires a 2s host/test press-and-hold; single Confirm never starts it. SystemLoading is a startup page, not a power-off step. Both boot legs use the existing PageManager `replace()` API: Startup → SystemLoading → Dialplate, so each previous boot page is unloaded and Dialplate becomes the sole navigation root. Back cannot revisit Startup/SystemLoading. SaveConfig completion distinguishes the actions: Off resets the navigation root to Startup if the process remains powered, while Reboot returns to the working Dialplate root. The shared `PageManager::reset_root(name)` validates the name/busy state before unloading history; registered controllers remain available for a fresh boot. Simulation does not terminate Linux or the device. Loading/save progress and completion timers begin in `on_view_did_appear()` so a stalled host frame cannot race an unfinished page transition.

### Boot presentation (implemented)

- Startup initially requests a hidden StatusBar. After **10s from did_appear**, if Startup is still current and the hold has not completed, its one-shot timer requests slide-in. Short presses/cancelled holds do not reset this idle deadline; leaving/unloading cancels it. The 2s hold remains unchanged.
- SystemLoading always requests slide-out, including when Startup's idle bar was visible. It uses the full 294×126 screen: **800ms logo → 3s simulated initialization → 200ms at 100% → 1s System Ready → Dialplate fade**. Timing starts after the page transition; leaving any phase cancels timers and all child animations, including when the page stays cached.
- Default logo is `img_src_SystemLoading_XYZ.c` (existing exported descriptor `img_src_startupLogo`). Existing `RGK_LOGO_USE` / `MIDDLE_LOGO_USE` build definitions select `img_src_SystemLoading_RGK.c` / `img_src_SystemLoading_Neutral.c` and their legacy descriptors. No replacement artwork is generated.
- Loading now follows `/home/gtc/Pictures/95cb84f4-2086-43b1-8667-a0c650dba59e.png`: centered rotating settings gear/progress ring, black rounded panel, #333333 8px track, #FF931E initialization accent, white stage headline, gray supporting text and GNSS RTK Receiver footer. Five connected dots and “Step N of 5” advance with configuration → services → GNSS → network → finalization; the headline and active dot fade on changes. Logo fades into initialization over 220ms. At completion, the orange ring/bar/dots blend to #28C76F and the gear fades into a checkmark; System Ready, availability text, Startup completed and 100% stay for 1s before the working-page fade.
- Per the latest request, SystemLoading's outer colored border is removed in both Initialization and Ready. Progress ring/bar/dots retain their orange/green accents. Bilingual regressions assert zero panel border/outline throughout all five steps and Ready; host 9/9, SDL build/smoke and scoped LSP checks passed. Boot contact sheets/GIFs were regenerated; no hardware validation.
- **The entire initialization/Ready sequence is marked DEMO.** These are simulated stage labels and success visuals requested for preview, not real configuration, GNSS or network checks. No backend is enabled; real readiness/failure/timeout handling remains reserved. English/Russian text and percentage use Oswald (Rajdhani lacks `%`). The check uses LVGL's existing draw primitives; no widget configuration, dependency or replacement asset is added.

**User-approved product simplification:** remove manual Date & Time entry, overlay, field navigation and page-owned editing/commit state. Settings focus cycles between Language and Back; disabled Wi-Fi is skipped; the shared StatusBar remains hidden for the entire SystemDash→Settings flow. This avoids five-field adjustments without Up/Down and exposes a return path without requiring discovery of double-click Back. Existing RAM-only service/calendar validation (2000–2099, leap years, 00–23 / 00–59) remains a shared contract, not a UI feature. Real clock/time-source policy is unimplemented; no GNSS/network synchronization, `clock_settime`, persistence, shutdown or reboot syscall is enabled.

**Host SDL controls and ordinary preview actions use direct logical actions, not GPIO bindings or double clicks.** Arrows/wheel change focus, Enter/middle mouse send press/release (hold 2s on Startup; short press selects elsewhere), Esc/right click cancel/back, Ctrl+Enter commits, and Q closes the preview. Left-click pointer interaction is restored. Host Commit remains distinct from Select, so an ordinary click/Enter cannot execute a dangerous action.

The current test-board `p4_screen` also keeps V+/V-/MENU/ESC controls; optional KEY_ENTER supplies direct Commit for testing. Neither host nor test-board adapters call `on_key()`/`poll_keys()`. `linux_button_read_keys()` is an optional evdev test helper, **not** the product GPIO adapter. Displayed `2x` captions describe the intended product UI, not host keyboard bindings.

The reserved product platform adapter must identify both GPIO chip/line offsets, input configuration and each active HIGH/LOW level. Sample levels, normalize `pressed = (level == active_level)`, call `on_key(role, pressed)` and `poll_keys(monotonic_ms)` on the UI thread. Read errors must cancel pending input and report failure, not synthesize a press. No line, polarity, pull configuration, GPIO API/library or PMIC behavior is guessed or installed. Product startup hold/power-on sequencing remains reserved until its electrical/platform contract is confirmed.

## Host simulation coverage and hardware reserves

| Situation | Host behavior / reserve |
| --- | --- |
| Startup | StatusBar initially hidden, slides in after 10s if still waiting; 2s hold progresses; short release, pointer press loss and window focus loss cancel; repeat keydown cannot restart the hold |
| SystemLoading / Dialplate | StatusBar slides out; logo precedes bilingual loading/progress; five-stage initialization and green DEMO Ready; cancellation cleans up all three phases; boot pages replaced, Dialplate root cannot Back into boot; real device initialization and failure recovery reserved |
| Page navigation | Focus, modal open/cancel, settings, information cards and return paths use direct host actions |
| Language / Settings | English/Russian switching; disabled Wi-Fi; visible Back; no manual date/time editor |
| Power | Explicit Commit only; early cancel and token-idempotent simulation; Off → fresh Startup root, Reboot → Dialplate root; no OS/device power change |
| GNSS / battery / recording / radio | DEMO status or N/A; real acquisition, recording and radio configuration reserved, no backend success implied |
| Wi-Fi | Disabled; actual network setup and failure/timeout handling reserved |

No new selectable hardware-failure scenarios are introduced without a backend contract. Product GPIO lines/active levels, startup power sequencing, PMIC, persistence and target timing remain explicit platform reserves.

## Shared logging (implemented)

`Utils/Log/Log.h/.cpp`: C-compatible DEBUG/INFO/WARN/ERROR/OFF level control, configurable synchronous sink, stderr default, 256-byte bounded formatting/truncation, recursive logging suppression. PageManager/DataCenter/service lifecycle and delivery use this API; routine deliveries are DEBUG. Main/LVGL thread only, no ISR/concurrent sink transport, no sensitive payload dumps. Example: `app_log_set_level(APP_LOG_DEBUG); APP_LOG_D("Input", "focus changed");`.

## Phase 4 — validation

```sh
cmake -S application -B out/ui-page-check -DCMAKE_BUILD_TYPE=Debug
cmake --build out/ui-page-check --parallel 2
ctest --test-dir out/ui-page-check --output-on-failure
cmake -S application -B out/sdl -DCMAKE_BUILD_TYPE=Debug -DRK3506_SDL_PREVIEW=ON
cmake --build out/sdl --target p4_sdl --parallel 2
SDL_VIDEODRIVER=dummy timeout --preserve-status --signal=TERM 2s ./out/sdl/p4_sdl
```

Host suite now has 9 tests. Framework tests exercise logger filtering/format/truncation/recursion, account subscription teardown and callback-driven subscriber deletion. Status tests exercise page-owned snapshots, peer pulls, lifecycle/reinitialization, calendar validation, language and simulated idempotent power requests. SystemDash host tests dispatch separate Next/Select/Back/Commit actions and check bilingual labels/image bounds and glyph coverage, native bitmap format/dimensions/1:1 zoom, enabled/disabled focus, absence of the date editor, inert Settings Commit, Select-vs-Commit safety, save cancellation, both power actions and status-bar restoration. Explicit normalized two-key sample regressions verify Function-only cyclic Language/Back and Power/Settings/Return focus, Power-single return selection, working-root coordinates/StatusBar restoration and re-entry into the fresh Power-first menu in both languages; this exercises UI logic, not physical GPIO or the SDL adapter. Pure ButtonGesture checks retain exclusivity and boundary coverage. Inactive SystemInfos cards intentionally move outside the viewport; bounds checks target each focused card.

Navigation regressions also exercise Startup hold/release/focus cancellation/repeated press, discarded boot Models, root Back rejection and a stalled host frame. StatusBar checks cover sliding endpoints, duplicate requests, mid-flight reversals, publications and animation cleanup. Two separate temporary SDL event-injection runs (not CTests or hardware checks) each passed 12/12 page checkpoints through the actual event loop: keyboard holds/focus loss and left-mouse holds/focus loss, plus Ctrl+Enter, save cancellation and settings/back. Mouse focus loss initially reproduced an uncancelled Startup timer because LVGL reset alone emits no PRESS_LOST; shared input cancellation now handles LVGL-owned focused pressed controls, with a persistent navigation regression.

Boot regressions check Startup's hidden/delayed StatusBar, slide-out into SystemLoading, logo exclusivity, all five English/Russian stage captions/counts, un-clipped headline glyphs, green Ready/100%/completed text and cancellation in Logo, Initialization and Ready. Navigation checks inspect mid-animation horizontal/vertical offsets or opacity, settled endpoints and reversed Back directions; power tests check cached-root coordinates/opacity after mixed-axis Reboot. Animated frames are actual 100ms host LVGL samples, exported by the same test.

Earlier boot validation: full host suite **9/9**, rebuilt SDL and dummy-video SIGTERM smoke passed; the temporary keyboard SDL injection was rescheduled for staged initialization/Ready and passed **12/12** page checkpoints. Mouse injection remains historical, not rerun for this change. Six changed loading/router/test paths were confirmed clean by active LSP at warning level; this does not replace the earlier inconclusive bulk probes.

Reviewed screenshots and animations: `out/ui-pages/Boot/current-flow.png` (logo / GNSS stage / Ready, both languages), `steps-flow.png` (all five stages), `boot-en.gif` and `boot-ru.gif`. Source PPMs are under `out/ui-page-check/system-dash-pages/`. Older `SystemLoading-Progress-*` and previous loading screenshots are historical; use the new Step1–Step5/Ready captures.

Earlier icon/date-removal validation: full host **9/9**, SDL rebuild/dummy-video smoke, and `git diff --check` passed. Active LSP initially flagged asset naming; new resource names were changed to project-conforming lowercase. Other paths included inconclusive timeout/silent-on-clean outcomes, not proof of clean. Earlier SDL keyboard/mouse injection results above are historical, not rerun for this update. New PPMs include `Settings-Back-en/ru`; its historical two-tile montage is `out/ui-pages/SystemDash/native-icons-settings.png`. Older DateTime/TimeSaved, Language/PowerConfirm and contact sheets are historical, not the current product UI.

Latest SystemDash subpage validation: SystemSettings now requests StatusBar hidden on appear; bilingual tests assert no DEMO bar while Settings is open and after Return. Full **9/9 host tests** passed, with SDL rebuild/smoke and LSP checks.

Latest three-tile/StarMap validation: **9/9 host tests**, rebuilt SDL and dummy-video smoke, clean active LSP on seven changed implementation/test paths, and `git diff --check`. Assertions cover all three native 40px assets at 1:1 zoom, natural English/Russian caption widths, Return's Enter-vs-Commit behavior and no power transaction, normalized two-key navigation/return/re-entry, and zero StarMap focused outline/border. Latest captures are `SystemDash-Return-en/ru`, `StarMap-no-outline-en/ru` and `out/ui-pages/SystemDash/three-tile-return-starmap.png`. SDL event injection was not rerun for this update.

No ARM build, deployment or physical/electrical/power validation was performed for this branch. Historical target records do not validate these changes. Remaining backends need real success/failure/timeout handling and an explicitly identified platform/power contract before enabling actual operations.
