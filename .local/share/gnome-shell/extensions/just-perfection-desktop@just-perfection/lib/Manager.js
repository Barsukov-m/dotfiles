/**
 * Manager Library
 * 
 * @author     Javad Rahmatzadeh <j.rahmatzadeh@gmail.com>
 * @copyright  2021
 * @license    GNU General Public License v3.0
 */


/**
 * Apply settings to the GNOME Shell 
 */
var Manager = class
{
    /**
     * Class Constructor
     *
     * @param object dependecies
     *   'API' instance of lib::API
     *   'HotCorner' instance of lib::HotCorner
     *   'Settings' instance of Gio::Settings
     * @param float shellVersion
     */
    constructor(dependecies, shellVersion)
    {
        this._api = dependecies['API'] || null;
        this._hotCorner = dependecies['HotCorner'] || null;
        this._settings = dependecies['Settings'] || null;
        
        this._shellVersion = shellVersion;
    }
    
    /**
     * register all signals for settings
     *
     * @return void
     */
    registerSettingsSignals()
    {
        this._settings.connect('changed::panel', () => {
            this._applyPanel(false);
        });
        
        this._settings.connect('changed::search', () => {
            this._applySearch(false);
        });
        
        this._settings.connect('changed::dash', () => {
            this._applyDash(false);
        });
        
        this._settings.connect('changed::osd', () => {
            this._applyOSD(false);
        });
        
        this._settings.connect('changed::workspace-popup', () => {
            this._applyWorkspacePopup(false);
        });
        
        this._settings.connect('changed::workspace', () => {
            this._applyWorkspace(false);
        });
        
        this._settings.connect('changed::background-menu', () => {
            this._applyBackgroundMenu(false);
        });
        
        this._settings.connect('changed::gesture', () => {
            this._applyGesture(false);
        });
        
        this._settings.connect('changed::hot-corner', () => {
            this._applyHotCorner(false);
        });
        
        this._settings.connect('changed::theme', () => {
            this._applyTheme(false);
        });
        
        this._settings.connect('changed::activities-button', () => {
            this._applyActivitiesButton(false);
        });
        
        this._settings.connect('changed::app-menu', () => {
            this._applyAppMenu(false);
        });
        
        this._settings.connect('changed::clock-menu', () => {
            this._applyClockMenu(false);
        });
        
        this._settings.connect('changed::keyboard-layout', () => {
            this._applyKeyboardLayout(false);
        });
        
        this._settings.connect('changed::accessibility-menu', () => {
            this._applyAccessibilityMenu(false);
        });
        
        this._settings.connect('changed::aggregate-menu', () => {
            this._applyAggregateMenu(false);
        });
        
        this._settings.connect('changed::panel-corner-size', () => {
            this._applyPanelCornerSize(false);
        });
        
        this._settings.connect('changed::window-picker-icon', () => {
            this._applyWindowPickerIcon(false);
        });
        
        this._settings.connect('changed::type-to-search', () => {
            this._applyTypeToSearch(false);
        });
        
        this._settings.connect('changed::workspace-switcher-size', () => {
            this._applyWorkspaceSwitcherSize(false);
        });
        
        this._settings.connect('changed::power-icon', () => {
            this._applyPowerIcon(false);
        });
        
        this._settings.connect('changed::top-panel-position', () => {
            this._applyTopPanelPosition(false);
        });
        
        this._settings.connect('changed::panel-arrow', () => {
            this._applyPanelArrow(false);
        });
        
        this._settings.connect('changed::panel-notification-icon', () => {
            this._applyPanelNotificationIcon(false);
        });
        
        this._settings.connect('changed::app-menu-icon', () => {
            this._applyAppMenuIcon(false);
        });
        
        this._settings.connect('changed::clock-menu-position', () => {
            this._applyClockMenuPosition(false);
        });
        
        this._settings.connect('changed::clock-menu-position-offset', () => {
            this._applyClockMenuPosition(false);
        });
        
        this._settings.connect('changed::show-apps-button', () => {
            this._applyShowAppsButton(false);
        });
        
        this._settings.connect('changed::animation', () => {
            this._applyAnimation(false);
        });
        
        this._settings.connect('changed::activities-button-icon-path', () => {
            this._applyActivitiesButtonIcon(false);
        });
        
        this._settings.connect('changed::activities-button-icon-monochrome', () => {
            this._applyActivitiesButtonIcon(false);
        });
        
        this._settings.connect('changed::activities-button-label', () => {
            this._applyActivitiesButtonIcon(false);
        });
        
        this._settings.connect('changed::window-demands-attention-focus', () => {
            this._applyWindowDemandsAttentionFocus(false);
        });
        
        this._settings.connect('changed::dash-icon-size', () => {
            this._applyDashIconSize(false);
        });
    }
    
    
    /**
     * apply everything to the GNOME Shell
     *
     * @return void
     */
    applyAll()
    {
        this._applyTheme(false);
        this._applyPanel(false);
        this._applySearch(false);
        this._applyDash(false);
        this._applyOSD(false);
        this._applyWorkspacePopup(false);
        this._applyWorkspace(false);
        this._applyBackgroundMenu(false);
        this._applyGesture(false);
        this._applyHotCorner(false);
        this._applyActivitiesButton(false);
        this._applyAppMenu(false);
        this._applyClockMenu(false);
        this._applyKeyboardLayout(false);
        this._applyAccessibilityMenu(false);
        this._applyAggregateMenu(false);
        this._applyPanelCornerSize(false);
        this._applyWindowPickerIcon(false);
        this._applyTypeToSearch(false);
        this._applyWorkspaceSwitcherSize(false);
        this._applyPowerIcon(false);
        this._applyTopPanelPosition(false);
        this._applyPanelArrow(false);
        this._applyPanelNotificationIcon(false);
        this._applyAppMenuIcon(false);
        this._applyClockMenuPosition(false);
        this._applyShowAppsButton(false);
        this._applyAnimation(false);
        this._applyActivitiesButtonIcon(false);
        this._applyWindowDemandsAttentionFocus(false);
        this._applyDashIconSize(false);
    }
    
    /**
     * revert everything that is done by this class to the GNOME Shell
     *
     * @return void
     */
    revertAll()
    {
        this._applyTheme(true);
        this._applyPanel(true);
        this._applySearch(true);
        this._applyDash(true);
        this._applyOSD(true);
        this._applyWorkspace(true);
        this._applyWorkspacePopup(true);
        this._applyBackgroundMenu(true);
        this._applyGesture(true);
        this._applyHotCorner(true);
        this._applyActivitiesButton(true);
        this._applyAppMenu(true);
        this._applyClockMenu(true);
        this._applyKeyboardLayout(true);
        this._applyAccessibilityMenu(true);
        this._applyAggregateMenu(true);
        this._applyPanelCornerSize(true);
        this._applyWindowPickerIcon(true);
        this._applyTypeToSearch(true);
        this._applyWorkspaceSwitcherSize(true);
        this._applyPowerIcon(true);
        this._applyTopPanelPosition(true);
        this._applyPanelArrow(true);
        this._applyPanelNotificationIcon(true);
        this._applyAppMenuIcon(true);
        this._applyClockMenuPosition(true);
        this._applyShowAppsButton(true);
        this._applyAnimation(true);
        this._applyActivitiesButtonIcon(true);
        this._applyWindowDemandsAttentionFocus(true);
        this._applyDashIconSize(true);
    }
    
    /**
     * apply panel settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyPanel(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('panel')) {
            this._api.panelShow();
        } else {
            this._api.panelHide();
        }
        // since we use lib::HotCorner on hidden panel we need to
        // apply hot corner on each call of this metod
        this._applyHotCorner(false);
    }
    
    /**
     * apply search settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applySearch(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('search')) {
            this._api.searchEntryShow(false);
        } else {
            this._api.searchEntryHide(false);
        }
    }
    
    /**
     * apply type to search settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyTypeToSearch(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('type-to-search')) {
            this._api.startSearchEnable();
        } else {
            this._api.startSearchDisable();
        }
    }
    
    /**
     * apply dash settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyDash(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('dash')) {
            this._api.dashShow();
        } else {
            this._api.dashHide();
        }
    }

    /**
     * apply osd settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyOSD(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('osd')) {
            this._api.OSDEnable();
        } else {
            this._api.OSDDisable();
        }
    }
    
    /**
     * apply workspace popup settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyWorkspacePopup(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('workspace-popup')) {
            this._api.workspacePopupEnable();
        } else {
            this._api.workspacePopupDisable();
        }
    }
    
    /**
     * apply workspace settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyWorkspace(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('workspace')) {
            this._api.workspaceSwitcherShow();
        } else {
            this._api.workspaceSwitcherHide();
        }
    }
    
    /**
     * apply background menu settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyBackgroundMenu(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('background-menu')) {
            this._api.backgroundMenuEnable();
        } else {
            this._api.backgroundMenuDisable();
        }
    }
    
    /**
     * apply gesture settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyGesture(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('gesture')) {
            this._api.gestureEnable();
        } else {
            this._api.gestureDisable();
        }
    }
    
    /**
     * apply hot corner settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyHotCorner(forceOriginal)
    {
        if (forceOriginal) {
            this._api.hotCornersDefault();
            this._hotCorner.removeOveriewButton();
        } else if (!this._settings.get_boolean('hot-corner')) {
            this._api.hotCornersDisable();
            this._hotCorner.removeOveriewButton();
        } else {
            this._api.hotCornersEnable();
            // gnome hot corner won't work when the panel is hidden
            // so we use lib::HotCorner instead
            if (!this._api.isPanelVisible()) {
                this._hotCorner.addOveriewButton();
            } else {
                this._hotCorner.removeOveriewButton();
            }
        }
    }
    
    /**
     * apply theme settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyTheme(forceOriginal)
    {
        let className = 'just-perfection';
        let fallbackClassName = 'just-perfection-gnome3';
        
        if (forceOriginal || !this._settings.get_boolean('theme')) {
            this._api.UIstyleClassRemove(className);
            this._api.UIstyleClassRemove(fallbackClassName);
        } else {
            this._api.UIstyleClassAdd(className);
            if (this._shellVersion < 40) {
                this._api.UIstyleClassAdd(fallbackClassName);
            }
        }
    }
    
    /**
     * apply activites button settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyActivitiesButton(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('activities-button')) {
            this._api.activitiesButtonShow();
        } else {
            this._api.activitiesButtonHide();
        }
    }
    
    /**
     * apply app menu settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyAppMenu(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('app-menu')) {
            this._api.appMenuShow();
        } else {
            this._api.appMenuHide();
        }
    }
    
    /**
     * apply clock menu (aka date menu) settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyClockMenu(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('clock-menu')) {
            this._api.dateMenuShow();
        } else {
            this._api.dateMenuHide();
        }
    }
    
    /**
     * apply keyboard layout settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyKeyboardLayout(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('keyboard-layout')) {
            this._api.keyboardLayoutShow();
        } else {
            this._api.keyboardLayoutHide();
        }
    }
    
    /**
     * apply accessibility menu settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyAccessibilityMenu(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('accessibility-menu')) {
            this._api.accessibilityMenuShow();
        } else {
            this._api.accessibilityMenuHide();
        }
    }
    
    /**
     * apply aggregate menu settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyAggregateMenu(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('aggregate-menu')) {
            this._api.aggregateMenuShow();
        } else {
            this._api.aggregateMenuHide();
        }
    }
    
    /**
     * apply panel corner size settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyPanelCornerSize(forceOriginal)
    {
        let size = this._settings.get_int('panel-corner-size');
        
        if (forceOriginal || size === 0) {
            this._api.panelCornerSetDefault();
        } else {
            this._api.panelCornerSetSize(size - 1);
        }
    }

    /**
     * apply window picker icon settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyWindowPickerIcon(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('window-picker-icon')) {
            this._api.windowPickerIconEnable();
        } else {
            this._api.windowPickerIconDisable();
        }
    }
    
    /**
     * apply workspace switcher size settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyWorkspaceSwitcherSize(forceOriginal)
    {
        let size = this._settings.get_int('workspace-switcher-size');
        
        if (forceOriginal || size === 0) {
            this._api.workspaceSwitcherSetDefaultSize();
        } else {
            this._api.workspaceSwitcherSetSize(size / 100, false);
        }
    }
    
    /**
     * apply power icon settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyPowerIcon(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('power-icon')) {
            this._api.powerIconShow();
        } else {
            this._api.powerIconHide();
        }
    }
    
    /**
     * apply top panel position settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyTopPanelPosition(forceOriginal)
    {
        if (forceOriginal || this._settings.get_int('top-panel-position') === 0) {
            this._api.panelSetPosition('top');
        } else {
            this._api.panelSetPosition('bottom');
        }
    }
    
    /**
     * apply panel arrow settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyPanelArrow(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('panel-arrow')) {
            this._api.panelArrowEnable();
        } else {
            this._api.panelArrowDisable();
        }
    }
    
    /**
     * apply panel notification icon settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyPanelNotificationIcon(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('panel-notification-icon')) {
            this._api.panelNotificationIconEnable();
        } else {
            this._api.panelNotificationIconDisable();
        }
    }
    
    /**
     * apply app menu icon settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyAppMenuIcon(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('app-menu-icon')) {
            this._api.appMenuIconEnable();
        } else {
            this._api.appMenuIconDisable();
        }
    }

    /**
     * apply clock menu position settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyClockMenuPosition(forceOriginal)
    {
        if (forceOriginal) {
            this._api.clockMenuPositionSet(0, 0);
        } else {
            let pos = this._settings.get_int('clock-menu-position');
            let offset = this._settings.get_int('clock-menu-position-offset');
            this._api.clockMenuPositionSet(pos, offset);
        }
    }
    
    /**
     * apply show apps button settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyShowAppsButton(forceOriginal)
    {
        if (forceOriginal || this._settings.get_boolean('show-apps-button')) {
            this._api.showAppsButtonEnable();
        } else {
            this._api.showAppsButtonDisable();
        }
    }
    
    /**
     * apply animation settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyAnimation(forceOriginal)
    {
        let animation = this._settings.get_int('animation');
        
        let factors = [
            0.4, // fastest
            0.6, // faster
            0.8, // fast
            1.3, // slow
            1.6, // slower
            2.8, // slowest
        ];
        
        if (forceOriginal) {
            this._api.animationSpeedSetDefault();
            this._api.enablenAimationsSetDefault();
        } else if (animation === 0) {
            // disabled
            this._api.animationSpeedSetDefault();
            this._api.enablenAimationsSet(false);
        } else if (animation === 1) {
            // default speed
            this._api.animationSpeedSetDefault();
            this._api.enablenAimationsSet(true);
        } else if (typeof factors[animation - 2] !== 'undefined') {
            // custom speed
            this._api.animationSpeedSet(factors[animation - 2]);
            this._api.enablenAimationsSet(true);
        }
    }
    
    /**
     * apply show apps button settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyActivitiesButtonIcon(forceOriginal)
    {
        let iconPath = this._settings.get_string('activities-button-icon-path');
        let monochrome = this._settings.get_boolean('activities-button-icon-monochrome');
        let label = this._settings.get_boolean('activities-button-label');
        
        if (forceOriginal) {
            this._api.ativitiesButtonRemoveIcon();
        } else {
            this._api.ativitiesButtonAddIcon(1, iconPath, monochrome, label);
        }
    }
    
    /**
     * apply window demands attention focus settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyWindowDemandsAttentionFocus(forceOriginal)
    {
        if (forceOriginal || !this._settings.get_boolean('window-demands-attention-focus')) {
            this._api.windowDemandsAttentionFocusDisable();
        } else {
            this._api.windowDemandsAttentionFocusEnable();
        }
    }
    
    /**
     * apply dash icon size settings
     *
     * @param bool forceOriginal
     *
     * @return void
     */
    _applyDashIconSize(forceOriginal)
    {
        let size = this._settings.get_int('dash-icon-size');
        
        if (forceOriginal || size === 0) {
            this._api.dashIconSizeSetDefault();
        } else {
            this._api.dashIconSizeSet(size);
        }
    }
}

