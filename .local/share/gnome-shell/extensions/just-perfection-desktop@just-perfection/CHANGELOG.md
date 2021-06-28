# Changelog

All notable changes to this project will be documented in this file.

We go to the next version after each release on [GNOME Shell Extensions website](https://extensions.gnome.org/).

## [Unreleased]

## [11.0.0 Whisper] - 2021-05-20

### Changed

- Shell theme override is now disabled by default.
- Workspace switcher size for GNOME Shell 40 is now maxed out to 15%.
- Donation icon to GTK4 non-compatible icon sets.
- App gesture now only works on GNOME 3.36 and 3.38.

### Fixed

- Workspace switcher size for multi monitor setup.
- Gap when panel posision is at the bottom and shell override theme happens.
- Panel menu margin when panel is in bottom.
- Window picker icon visiblity on drag.
- Arabic translation by [@AliGalal](https://gitlab.com/AliGalal).
- Italian translation by [@l3nn4rt](https://gitlab.com/l3nn4rt).
- Swedish translation by [@MorganAntonsson](https://gitlab.com/MorganAntonsson).
- Chinese translation by [@wsxy162](https://gitlab.com/wsxy162).

### Added

- Activities button icon.
- Window demands attention focus.
- Dash icon size.

## [10.0.0] - 2021-03-26

### Changed

- Removed quotes and side bar image from prefs UI.
- Organized prefs UI for icons and behavior.

### Fixed

- Horizontal scroll in prefs.
- Primary Monitor Panel Position.
- Fake hot corner primary monitor position.
- Arabic translation by [@karem34](https://gitlab.com/karem34).
- Russian translation by [@librusekus35790](https://gitlab.com/librusekus35790).

### Added

- Disable panel arrow in GNOME 3.36 and 3.38.
- Disable panel notification icon.
- Disable app menu icon.
- Clock menu position.
- No results found for prefs window.
- Disable applications button in dash.
- Disable animation or change the animation speed.
- Galician translation by [@frandieguez](https://gitlab.com/frandieguez).
- Brazilian Portuguese translation by [@Zelling](https://gitlab.com/Zelling).
- Catalan translation by [@juxuanu](https://gitlab.com/juxuanu).

## [9.0.0] - 2021-03-06

### Changed

- Prefs interface.

### Fixed

- GNOME Shell 40.beta version.
- Default value for hot corner on extension disable.

### Added

- Disable power icon.
- Panel position.
- Support to prefs window.

## [8.0.0] - 2021-02-22

### Changed

- Holding back lonely overview until the final GNOME 40 release.

### Fixed

- Nepali translation filename by [@IBA4](https://gitlab.com/IBA4).
- Focus for find entry on prefs.
- Workspace switcher enable related to workspace switcher size.
- Start search for GNOME Shell 40 beta.
- Search controller for GNOME Shell 40 beta.
- Dash override theme on GNOME Shell 40 beta.

## [7.0.0] - 2021-02-12

### Fixed

- GNOME Shell 40 hidden side by side workspace preview.
- GNOME Shell 40 search padding when panel is disabled.
- Initial prefs window size.

### Added

- Spanish translation by [@oscfdezdz](https://gitlab.com/oscfdezdz).
- Nepali translation by [@IBA4](https://gitlab.com/IBA4).
- Panel corner size to the settings.
- GNOME Shell 40 window picker icon visibility to the settings.
- GNOME Shell 40 workspace switcher size to the settings.
- Type to Search to the settings.
- Search feature to the settings.

## [6.0.0] - 2021-01-29

### Fixed

- GNOME Shell 3.38 extra padding on no workspace switcher.
- GNOME Shell 40 support for search entry.
- GNOME Shell 40 and GTK4 support for prefs.
- GNOME Shell 40 support for workspace switcher.

## [5.0.0] - 2021-01-05

### Added

- Activities button visibility to the settings.
- App menu visibility to the settings.
- Clock menu visibility to the settings.
- Keyboard Layout visibility to the settings.
- Accessibility Menu visibility to the settings.
- System Menu (Aggregate Menu) visibility to the settings.

### Changed

- OSD in settings to "On Screen Display (OSD)".

### Fixed

- Padding on no dash.
- Search top padding on no top panel.
- Hot corner when top panel is visible.

## [4.0.0] 2020-12-25

### Added

- API to decouple all libraries from using GNOME Shell ui directly.
- Compatibility layer for API.
- Translation automation script by [@daPhipz](https://gitlab.com/daPhipz).
- Automate build process by [@daPhipz](https://gitlab.com/daPhipz).
- CHANGELOG.md file.

### Changed

- Improve German translation by [@M4he](https://gitlab.com/M4he).
- Displaying error for generate-mo.sh by [@daPhipz](https://gitlab.com/daPhipz).
- Default settings to enable.

### Fixed

- Top padding on no search and no top panel.

## [3.0.0] - 2020-12-21

### Added

- Extension logo.
- CONTRIBUTING.md file.
- Prefs as extension settings.
- Initial Translations.
- Decoupled library from GNOME Shell ui.

## [2.0.0] - 2020-11-18

### Fixed

- Destroy hot corner on disable.

## [1.0.0] - 2020-11-15

### Added

- Hide top panel.
- Hide search.
- Hide dash.
- Hide workspace switcher.
- Disable background menu.
- Disable app gesture.
- Hot corner to toggle overview visibility.
