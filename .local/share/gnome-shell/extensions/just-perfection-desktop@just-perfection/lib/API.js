/**
 * API Library
 * 
 * @author     Javad Rahmatzadeh <j.rahmatzadeh@gmail.com>
 * @copyright  2021
 * @license    GNU General Public License v3.0
 */

const PANEL_BOX_POSITION = {
    'CENTER' : 0,
    'RIGHT' : 1,
    'LEFT' : 2,
};

const ICON_TYPE = {
    'NAME' : 0,
    'URI' : 1,
};

const DASH_ICON_SIZES = [16, 22, 24, 32, 48, 64];

/**
 * API to avoid calling GNOME Shell directly
 * and make all parts compatible with different GNOME Shell versions 
 */
var API = class
{
    /**
     * Class Constructor
     *
     * @param object dependecies
     *   'Main' reference to ui::main
     *   'BackgroundMenu' reference to ui::backgroundMenu
     *   'OverviewControls' reference to ui::overviewControls
     *   'WorkspaceSwitcherPopup' reference to ui::workspaceSwitcherPopup
     *   'InterfaceSettings' reference to Gio::Settings for 'org.gnome.desktop.interface'
     *   'SearchController' reference to ui::searchController
     *   'ViewSelector' reference to ui::viewSelector
     *   'WorkspaceThumbnail' reference to ui::workspaceThumbnail
     *   'WorkspacesView' reference to ui::workspacesView
     *   'St' reference to St
     *   'Gio' reference to Gio
     *   'GLib' reference to GLib
     *   'Clutter' reference to Clutter
     *   'Panel' reference to ui::Panel
     * @param float shellVersion
     */
    constructor(dependecies, shellVersion)
    {
        this._main = dependecies['Main'] || null;
        this._backgroundMenu = dependecies['BackgroundMenu'] || null;
        this._overviewControls = dependecies['OverviewControls'] || null;
        this._workspaceSwitcherPopup = dependecies['WorkspaceSwitcherPopup'] || null;
        this._interfaceSettings = dependecies['InterfaceSettings'] || null;
        this._searchController = dependecies['SearchController'] || null;
        this._viewSelector = dependecies['ViewSelector'] || null;
        this._workspaceThumbnail = dependecies['WorkspaceThumbnail'] || null;
        this._workspacesView = dependecies['WorkspacesView'] || null;
        this._st = dependecies['St'] || null;
        this._gio = dependecies['Gio'] || null;
        this._glib = dependecies['GLib'] || null;
        this._clutter = dependecies['Clutter'] || null;
        this._panel = dependecies['Panel'] || null;
        
        this._shellVersion = shellVersion;
        this._originals = {};
        
        // bool
        // true means seach entry is visible, false otherwise
        this._searchEntryVisible = true;
        
        // float
        // last workspace switcher size
        this._workspaceSwitcherLastSize
        = (this._workspaceThumbnail && this._shellVersion >= 40)
        ? this._workspaceThumbnail.MAX_THUMBNAIL_SCALE
        : 0.0;
    }
    
    /**
     * prepare everything needed for API
     *
     * @return void
     */
    open()
    {
        this.UIstyleClassAdd(this._getAPIClassname('shell-version'));
    }
    
    /**
     * remove everything from GNOME Shell been added by this class 
     *
     * @return void
     */
    close()
    {
        this.UIstyleClassRemove(this._getAPIClassname('shell-version'));
        this._startSearchSignal(false);
    }
    
    /**
     * get the css classname for API
     *
     * @param string type possible types
     *  shell-version
     *  no-search
     *  no-workspace
     *  no-panel
     *  panel-corner
     *  no-window-picker-icon
     *  type-to-search
     *  no-power-icon
     *  bottom-panel
     *  no-panel-arrow
     *  no-panel-notification-icon
     *  no-app-menu-icon
     *  no-show-apps-button
     *  activities-button-icon
     *  activities-button-icon-monochrome
     *  activities-button-no-label
     *  dash-icon-size
     *
     * @return string
     */
    _getAPIClassname(type)
    {
        let starter = 'just-perfection-api-';
        
        let possibleTypes = [
            'shell-version',
            'no-search',
            'no-workspace',
            'no-panel',
            'panel-corner',
            'no-window-picker-icon',
            'type-to-search',
            'no-power-icon',
            'bottom-panel',
            'no-panel-arrow',
            'no-panel-notification-icon',
            'no-app-menu-icon',
            'no-show-apps-button',
            'activities-button-icon',
            'activities-button-icon-monochrome',
            'activities-button-no-label',
            'dash-icon-size',
        ];
        
        if (!possibleTypes.includes(type)) {
            return '';
        }
        
        if (type === 'shell-version') {
            let shellVerMajor = Math.trunc(this._shellVersion);
            return starter + 'gnome' + shellVerMajor;
        }
        
        return starter + type;
    }
    
    /**
     * allow shell theme to do whatever it likes to panel corner
     *
     * @return void
     */
    panelCornerSetDefault()
    {
        let classnameStarter = this._getAPIClassname('panel-corner');
        
        for (let size = 0; size <= 60; size++) {
            this.UIstyleClassRemove(classnameStarter + size);
        }
    }
    
    /**
     * change panel corner size
     *
     * @param int size 0 to 60
     *
     * @return void
     */
    panelCornerSetSize(size)
    {
        this.panelCornerSetDefault();
        
        if (size > 60 || size < 0) {
            return;
        }
    
        let classnameStarter = this._getAPIClassname('panel-corner');
        
        this.UIstyleClassAdd(classnameStarter + size);
    }
    
    /**
     * show panel
     *
     * @return void
     */
    panelShow()
    {
        if (!this._originals['panelHeight']) {
            return;
        }
        
        this._main.panel.show();
        this._main.panel.height = this._originals['panelHeight'];
        
        this.UIstyleClassRemove(this._getAPIClassname('no-panel'));
    }
    
    /**
     * hide panel
     *
     * @return void
     */
    panelHide()
    {
        if (!this._originals['panelHeight']) {
            this._originals['panelHeight'] = this._main.panel.height;
        }
        
        this._main.panel.hide();
        this._main.panel.height = 0;
        
        // when panel is hidden and search entry is visible,
        // the search entry gets too close to the top, so we fix it with margin
        // on GNOME 3 we need to have top and bottom margin for correct proportion
        // but on GNOME 40 we don't need to keep proportion but give it more
        // top margin to keep it less close to top
        this.UIstyleClassAdd(this._getAPIClassname('no-panel'));
    }
    
    /**
     * check whether panel is visible
     *
     * @return bool
     */
    isPanelVisible()
    {
        return this._main.panel.visible;
    }
    
    /**
     * show dash
     *
     * @return void
     */
    dashShow()
    {
        this._main.overview.dash.show();
        
        if (this._shellVersion >= 40) {
            this._main.overview.dash.height = -1;
            this._main.overview.dash.setMaxSize(-1, -1);
        } else {
            this._main.overview.dash.width = -1;
            this._main.overview.dash._maxHeight = -1;
        }
    }
    
    /**
     * hide dash
     *
     * @return void
     */
    dashHide()
    {
        this._main.overview.dash.hide();
        
        if (this._shellVersion >= 40) {
            this._main.overview.dash.height = 0;
        } else {
            this._main.overview.dash.width = 0;
        }
    }
    
    /**
     * enable gesture
     *
     * @return void
     */
    gestureEnable()
    {
        global.stage.get_actions().forEach(a => a.enabled = true);
    }
    
    /**
     * disable gesture
     *
     * @return void
     */
    gestureDisable()
    {
        global.stage.get_actions().forEach(a => a.enabled = false);
    }
    
    /**
     * add class name to the UI group
     *
     * @param string classname
     *
     * @return void
     */
    UIstyleClassAdd(classname)
    {
        this._main.layoutManager.uiGroup.add_style_class_name(classname);
    }
    
    /**
     * remove class name from UI group
     *
     * @param string classname
     *
     * @return void
     */
    UIstyleClassRemove(classname)
    {
        this._main.layoutManager.uiGroup.remove_style_class_name(classname);
    }
    
    /**
     * check whether UI group has class name
     *
     * @param string classname
     *
     * @return bool
     */
    UIstyleClassContain(classname)
    {
        return this._main.layoutManager.uiGroup.has_style_class_name(classname);
    }
    
    /**
     * enable background menu
     *
     * @return void
     */
    backgroundMenuEnable()
    {
        if (!this._originals['backgroundMenu']) {
            return;
        }
        
        this._backgroundMenu.BackgroundMenu.prototype.open
        = this._originals['backgroundMenu'];
    }
    
    /**
     * disable background menu
     *
     * @return void
     */
    backgroundMenuDisable()
    {
        if (!this._originals['backgroundMenu']) {
            this._originals['backgroundMenu']
            = this._backgroundMenu.BackgroundMenu.prototype.open;
        }
        
        this._backgroundMenu.BackgroundMenu.prototype.open = () => {};
    }
    
    /**
     * show search
     *
     * @param bool fake true means it just needs to do the job but don't need to
     *  change the search visiblity status
     *
     * @return void
     */
    searchEntryShow(fake)
    {
        this.UIstyleClassRemove(this._getAPIClassname('no-search'));
        
        this._main.overview.searchEntry.show();
        
        if (!fake) {
            this._searchEntryVisible = true;
        }
    }
    
    /**
     * hide search
     *
     * @param bool fake true means it just needs to do the job but don't need to
     *  change the search visiblity status
     *
     * @return void
     */
    searchEntryHide(fake)
    {
        this.UIstyleClassAdd(this._getAPIClassname('no-search'));
        
        this._main.overview.searchEntry.hide();
        
        if (!fake) {
            this._searchEntryVisible = false;
        }
    }

    /**
     * enable start search
     *
     * @return void
     */
    startSearchEnable()
    {
        this._startSearchSignal(true);
        
        if (!this._originals['startSearch']) {
            return;
        }
        
        let viewSelector
        = this._main.overview.viewSelector || this._main.overview._overview.viewSelector;
        
        if (this._shellVersion >= 40 && this._searchController) {
            this._searchController.SearchController.prototype.startSearch
            = this._originals['startSearch'];
        } else {
            viewSelector.startSearch = this._originals['startSearch'];
        }
    }
    
    /**
     * disable start search
     *
     * @return void
     */
    startSearchDisable()
    {
        this._startSearchSignal(false);
        
        let viewSelector
        = this._main.overview.viewSelector || this._main.overview._overview.viewSelector;
        
        if (!this._originals['startSearch']) {
            this._originals['startSearch']
            = (this._shellVersion >= 40 && this._searchController)
            ? this._searchController.SearchController.prototype.startSearch
            : viewSelector.startSearch;
        }
        
        if (this._shellVersion >= 40 && this._searchController) {
            this._searchController.SearchController.prototype.startSearch = () => {};
        } else {
            viewSelector.startSearch = () => {};
        }
    }

    /**
     * add search signals that needs to be show search entry when the
     * search entry is hidden
     *
     * @param bool add true means add the signal, false means remove the signal
     *
     * @return void
     */
    _startSearchSignal(add)
    {
        let controller
        = this._main.overview.viewSelector ||
          this._main.overview._overview.viewSelector || 
          this._main.overview._overview.controls._searchController;
    
        // remove
        if (!add) {
            if (this._searchActiveSignal) {
                controller.disconnect(this._searchActiveSignal);
                this._searchActiveSignal = null;
            }
            return;
        }
        
        // add
        if (this._searchActiveSignal) {
            return;
        }
    
        let bySearchController = this._shellVersion >= 40;

        let signalName = (bySearchController) ? 'notify::search-active' : 'page-changed';

        this._searchActiveSignal = controller.connect(signalName, () => {
        
            if (this._searchEntryVisible) {
                return;
            }
        
            let inSearch
            = (bySearchController)
            ? controller.searchActive
            : (controller.getActivePage() === this._viewSelector.ViewPage.SEARCH);
            
            if (inSearch) {
                this.UIstyleClassAdd(this._getAPIClassname('type-to-search'));
                this.searchEntryShow(true);
            } else {
                this.UIstyleClassRemove(this._getAPIClassname('type-to-search'));
                this.searchEntryHide(true);
            }
        });
    }
    
    /**
     * enable OSD
     *
     * @return void
     */
    OSDEnable()
    {
        if (!this._originals['osdWindowManager']) {
            return;
        }
        
        this._main.osdWindowManager.show = this._originals['osdWindowManager'];
    }
    
    /**
     * disable OSD
     *
     * @return void
     */
    OSDDisable()
    {
        if (!this._originals['osdWindowManager']) {
            this._originals['osdWindowManager']
            = this._main.osdWindowManager.show;
        }
    
        this._main.osdWindowManager.show = () => {};
    }
    
    /**
     * enable workspace popup
     *
     * @return void
     */
    workspacePopupEnable()
    {
        if (!this._originals['workspaceSwitcherPopup']) {
            return;
        }
    
        this._workspaceSwitcherPopup.WorkspaceSwitcherPopup.prototype._show
        = this._originals['workspaceSwitcherPopup'];
    }
    
    /**
     * disable workspace popup
     *
     * @return void
     */
    workspacePopupDisable()
    {
        if (!this._originals['workspaceSwitcherPopup']) {
            this._originals['workspaceSwitcherPopup']
            = this._workspaceSwitcherPopup.WorkspaceSwitcherPopup.prototype._show;
        }
        
        this._workspaceSwitcherPopup.WorkspaceSwitcherPopup.prototype._show = () => {
           return false;
        };
    }
    
    /**
     * show workspace switcher
     *
     * @return void
     */
    workspaceSwitcherShow()
    {
        if (this._shellVersion < 40) {
        
            if (!this._originals['getAlwaysZoomOut'] ||
                !this._originals['getNonExpandedWidth'])
            {
                return;
            }
            
            let TSProto = this._overviewControls.ThumbnailsSlider.prototype;
            
            TSProto._getAlwaysZoomOut = this._originals['getAlwaysZoomOut'];
            TSProto.getNonExpandedWidth = this._originals['getNonExpandedWidth'];
        }
        
        // it should be before setting the switcher size
        // because the size can be changed by removing the api class
        this.UIstyleClassRemove(this._getAPIClassname('no-workspace'));
        
        if (this._workspaceSwitcherLastSize) {
            this.workspaceSwitcherSetSize(this._workspaceSwitcherLastSize, false);
        } else {
            this.workspaceSwitcherSetDefaultSize();
        }
    }
    
    /**
     * hide workspace switcher
     *
     * @return void
     */
    workspaceSwitcherHide()
    {
        if (this._shellVersion < 40) {
        
            let TSProto = this._overviewControls.ThumbnailsSlider.prototype;
        
            if (!this._originals['getAlwaysZoomOut']) {
                this._originals['getAlwaysZoomOut'] = TSProto._getAlwaysZoomOut;
            }
            
            if (!this._originals['getNonExpandedWidth']) {
                this._originals['getNonExpandedWidth'] = TSProto.getNonExpandedWidth;
            }
            
            TSProto._getAlwaysZoomOut = () => {
                return false;
            };
            TSProto.getNonExpandedWidth = () => {
                return 0;
            };
        }
        
        this.workspaceSwitcherSetSize(0.0, true);
        
        // on GNOME 3.38
        //   fix extra space that 3.38 leaves for no workspace with css
        // on GNOME 40
        //   we can hide the workspace only with css by scale=0 and
        //   no padding
        this.UIstyleClassAdd(this._getAPIClassname('no-workspace'));
    }
    
    /**
     * check whether workspace switcher is visible
     *
     * @return bool
     */
    isWorkspaceSwitcherVisible()
    {
        return !this.UIstyleClassContain(this._getAPIClassname('no-workspace'));
    }
    
    /**
     * set workspace switcher to its default size
     *
     * @return void
     */
    workspaceSwitcherSetDefaultSize()
    {
        if (this._shellVersion < 40) {
            return;
        }
        
        if (typeof this._originals['MAX_THUMBNAIL_SCALE'] === 'undefined') {
            return;
        }
        
        let size = this._originals['MAX_THUMBNAIL_SCALE'];
        
        if (this.isWorkspaceSwitcherVisible()) {
            this._workspaceThumbnail.MAX_THUMBNAIL_SCALE = size;
        }
            
        if (typeof this._originals['smd_getThumbnailsHeight'] !== 'undefined') {
            let smd = this._workspacesView.SecondaryMonitorDisplay;
            smd._getThumbnailsHeight = this._originals['smd_getThumbnailsHeight'];
        }
        
        this._workspaceSwitcherLastSize = size;
    }
    
    /**
     * set workspace switcher size
     *
     * @param float size
     * @param bool fake true means don't change this._workspaceSwitcherLastSize,
     *   false otherwise
     *
     * @return void
     */
    workspaceSwitcherSetSize(size, fake)
    {
        if (this._shellVersion < 40) {
            return;
        }
        
        if (typeof this._originals['MAX_THUMBNAIL_SCALE'] === 'undefined') {
            this._originals['MAX_THUMBNAIL_SCALE']
            = this._workspaceThumbnail.MAX_THUMBNAIL_SCALE;
        }
        
        if (this.isWorkspaceSwitcherVisible()) {
            
            this._workspaceThumbnail.MAX_THUMBNAIL_SCALE = size;
            
            // >>
            // we are overriding the _getThumbnailsHeight() here with the same
            // function as original but we change the MAX_THUMBNAIL_SCALE to our
            // custom size.
            // we do this because MAX_THUMBNAIL_SCALE is const and cannot be cahnged
            let smd = this._workspacesView.SecondaryMonitorDisplay;
            
            if (typeof this._originals['smd_getThumbnailsHeight'] === 'undefined') {
                this._originals['smd_getThumbnailsHeight'] = smd._getThumbnailsHeight;
            }
            
            smd.prototype._getThumbnailsHeight = function(box) {
                if (!this._thumbnails.visible)
                    return 0;

                const [width, height] = box.get_size();
                const {expandFraction} = this._thumbnails;
                const [thumbnailsHeight] = this._thumbnails.get_preferred_height(width);
                
                return Math.min(
                    thumbnailsHeight * expandFraction,
                    height * size);
            }
            // <<
        }
        
        if (!fake) {
            this._workspaceSwitcherLastSize = size;
        }
    }
    
    /**
     * toggle overview visiblity
     *
     * @return void
     */
    overviewToggle()
    {
        this._main.overview.toggle();
    }
    
    /**
     * add element to stage
     *
     * @param object element
     *
     * @return void
     */
    chromeAdd(element)
    {
        this._main.layoutManager.addChrome(element, {
            affectsInputRegion : true,
            affectsStruts : false,
            trackFullscreen : true,
        });
    }
    
    /**
     * remove element from stage
     *
     * @param object element
     *
     * @return void
     */
    chromeRemove(element)
    {
        this._main.layoutManager.removeChrome(element);
    }
    
    /**
     * show activities button
     *
     * @return void
     */
    activitiesButtonShow()
    {
        if (!this.isLocked()) {
            this._main.panel.statusArea['activities'].container.show();
        }
    }
    
    /**
     * hide activities button
     *
     * @return void
     */
    activitiesButtonHide()
    {
        this._main.panel.statusArea['activities'].container.hide();
    }
    
    /**
     * show app menu
     *
     * @return void
     */
    appMenuShow()
    {
        if (!this.isLocked()) {
            this._main.panel.statusArea['appMenu'].container.show();
        }
    }
    
    /**
     * hide app menu
     *
     * @return void
     */
    appMenuHide()
    {
        this._main.panel.statusArea['appMenu'].container.hide();
    }
    
    /**
     * show date menu
     *
     * @return void
     */
    dateMenuShow()
    {
        if (!this.isLocked()) {
            this._main.panel.statusArea['dateMenu'].container.show();
        }
    }
    
    /**
     * hide date menu
     *
     * @return void
     */
    dateMenuHide()
    {
        this._main.panel.statusArea['dateMenu'].container.hide();
    }
    
    /**
     * show keyboard layout
     *
     * @return void
     */
    keyboardLayoutShow()
    {
        this._main.panel.statusArea['keyboard'].container.show();
    }
    
    /**
     * hide keyboard layout
     *
     * @return void
     */
    keyboardLayoutHide()
    {
        this._main.panel.statusArea['keyboard'].container.hide();
    }
    
    /**
     * show accessibility menu
     *
     * @return void
     */
    accessibilityMenuShow()
    {
        this._main.panel.statusArea['a11y'].container.show();
    }
    
    /**
     * hide accessibility menu
     *
     * @return void
     */
    accessibilityMenuHide()
    {
        this._main.panel.statusArea['a11y'].container.hide();
    }
    
    /**
     * show aggregate menu
     *
     * @return void
     */
    aggregateMenuShow()
    {
        this._main.panel.statusArea['aggregateMenu'].container.show();
    }
    
    /**
     * hide aggregate menu
     *
     * @return void
     */
    aggregateMenuHide()
    {
        this._main.panel.statusArea['aggregateMenu'].container.hide();
    }
    
    /**
     * set 'enableHotCorners' original value
     *
     * @return void
     */
    _setEnableHotCornersOriginal()
    {
        if (typeof this._originals['enableHotCorners'] !== 'undefined') {
            return;
        }
        
        this._originals['enableHotCorners']
        = this._interfaceSettings.get_boolean('enable-hot-corners');
    }
    
    /**
     * enable hot corners
     *
     * @return void
     */
    hotCornersEnable()
    {
        this._setEnableHotCornersOriginal();
        this._interfaceSettings.set_boolean('enable-hot-corners', true);
    }
    
    /**
     * disable hot corners
     *
     * @return void
     */
    hotCornersDisable()
    {
        this._setEnableHotCornersOriginal();
        this._interfaceSettings.set_boolean('enable-hot-corners', false);
    }
    
    /**
     * set the hot corners to default value
     *
     * @return void
     */
    hotCornersDefault()
    {
        this._setEnableHotCornersOriginal();
        
        this._interfaceSettings.set_boolean('enable-hot-corners',
            this._originals['enableHotCorners']);
    }
    
    /**
     * check whether lock dialog is currently showing
     *
     * @return bool;
     */
    isLocked()
    {
        if (this._main.sessionMode.currentMode === 'unlock-dialog') {
            return true;
        }
        
        return false;
    }
    
    /**
     * enable window picker icon
     *
     * @return void
     */
    windowPickerIconEnable()
    {
        if (this._shellVersion < 40) {
            return;
        }
        
        this.UIstyleClassRemove(this._getAPIClassname('no-window-picker-icon'));
    }
    
    /**
     * disable window picker icon
     *
     * @return void
     */
    windowPickerIconDisable()
    {
        if (this._shellVersion < 40) {
            return;
        }
        
        this.UIstyleClassAdd(this._getAPIClassname('no-window-picker-icon'));
    }
    
    /**
     * show power icon
     *
     * @return void
     */
    powerIconShow()
    {
        this.UIstyleClassRemove(this._getAPIClassname('no-power-icon'));
    }
    
    /**
     * hide power icon
     *
     * @return void
     */
    powerIconHide()
    {
        this.UIstyleClassAdd(this._getAPIClassname('no-power-icon'));
    }
    
    /**
     * get primary monitor information
     *
     * @return object
     *  x: int
     *  y: int
     *  width: int
     *  height: int
     *  geometryScale: float
     */
    monitorGetInfo()
    {
        let pMonitor = this._main.layoutManager.primaryMonitor;
        
        return {
            'x': pMonitor.x,
            'y': pMonitor.y,
            'width': pMonitor.width,
            'height': pMonitor.height,
            'geometryScale': pMonitor.geometry_scale,
        };
    }
    
    /**
     * move panel position
     *
     * @param string position 'top', 'bottom'
     *
     * @return void
     */
    panelSetPosition(position)
    {
        // @TODO this._panelBoxHeight...() functions got commented here because
        // while they are fixing the drop shadow issue they can create wrong
        // height when panel is going enable or disable
        
        if (!this._originals['panelHeight']) {
            this._originals['panelHeight'] = this._main.panel.height;
        }
        
        let monitorInfo = this.monitorGetInfo();
        let panelBox = this._main.layoutManager.panelBox;
        
        if (position === 'top') {
            if (this._workareasChangedSignal) {
                global.display.disconnect(this._workareasChangedSignal);
            }
            let topX = monitorInfo.x;
            let topY = monitorInfo.y;
            panelBox.set_position(topX, topY);
            this.UIstyleClassRemove(this._getAPIClassname('bottom-panel'));
            //this._panelBoxHeightSetDefault();
            return;
        }
        
        let BottomX = monitorInfo.x;
        let BottomY = monitorInfo.height - this._originals['panelHeight'];
        
        panelBox.set_position(BottomX, BottomY);
        this.UIstyleClassAdd(this._getAPIClassname('bottom-panel'));
        
        if (!this._workareasChangedSignal) {
            this._workareasChangedSignal
            = global.display.connect('workareas-changed', () => {
                this.panelSetPosition('bottom');
            });
        }
        
        // 1px to avoid losing hot corner
        //this._panelBoxHeightSet(1);
    }
    
    /**
     * set panel box height
     *
     * @param int height
     *
     * @return void
     */
    _panelBoxHeightSet(height)
    {
        let panelBox = this._main.layoutManager.panelBox;
        
        if (this._shellVersion < 40) {
            return;
        }
        
        if (!this.isPanelVisible()) {
            return;
        }
        
        // since the panelbox height will be set after idle we will have
        // wrong height (0) if we don't use idle
        this._glib.idle_add(this._glib.PRIORITY_DEFAULT, () => {
            if (typeof this._originals['panelBoxHeight'] === 'undefined') {
                // @TODO when panel height getting changed
                // this._originals['panelBoxHeight'] should also change
                this._originals['panelBoxHeight'] = panelBox.height;
            }
            panelBox.height = height;
            return this._glib.SOURCE_REMOVE;
        });
    }
    
    /**
     * set the default height panel box
     *
     * @return void
     */
    _panelBoxHeightSetDefault()
    {
        let panelBox = this._main.layoutManager.panelBox;
        
        if (typeof this._originals['panelBoxHeight'] === 'undefined') {
            return;
        }
        
        if (!this.isPanelVisible()) {
            return;
        }
        
        panelBox.height = this._originals['panelBoxHeight'];
    }
    
    /**
     * enable panel arrow
     *
     * @return void
     */
    panelArrowEnable()
    {
        if (this._shellVersion >= 40) {
            return;
        }
        
        this.UIstyleClassRemove(this._getAPIClassname('no-panel-arrow'));
    }
    
    /**
     * disable panel arrow
     *
     * @return void
     */
    panelArrowDisable()
    {
        if (this._shellVersion >= 40) {
            return;
        }
        
        this.UIstyleClassAdd(this._getAPIClassname('no-panel-arrow'));
    }
    
    /**
     * disable panel notifiction icon
     *
     * @return void
     */
    panelNotificationIconEnable()
    {
        this.UIstyleClassRemove(this._getAPIClassname('no-panel-notification-icon'));
    }
    
    /**
     * disable panel notifiction icon
     *
     * @return void
     */
    panelNotificationIconDisable()
    {
        this.UIstyleClassAdd(this._getAPIClassname('no-panel-notification-icon'));
    }
    
    /**
     * disable app menu icon
     *
     * @return void
     */
    appMenuIconEnable()
    {
        this.UIstyleClassRemove(this._getAPIClassname('no-app-menu-icon'));
    }
    
    /**
     * disable app menu icon
     *
     * @return void
     */
    appMenuIconDisable()
    {
        this.UIstyleClassAdd(this._getAPIClassname('no-app-menu-icon'));
    }
    
    /**
     * set the clock menu position
     *
     * @param int pos see PANEL_BOX_POSITION
     * @param int offset starts from 0 
     *
     * @return void
     */
    clockMenuPositionSet(pos, offset)
    {
        let dateMenu = this._main.panel.statusArea['dateMenu'];
        
        let panelBoxs = [
            this._main.panel._centerBox,
            this._main.panel._rightBox,
            this._main.panel._leftBox,
        ];
        
        let fromPos = -1;
        let fromIndex = -1;
        let toIndex = -1;
        let childLength = 0;
        for (let i = 0; i <= 2; i++) {
            let child = panelBoxs[i].get_children();
            let childIndex = child.indexOf(dateMenu.container);
            if (childIndex !== -1) {
                fromPos = i;
                fromIndex = childIndex;
                childLength = panelBoxs[pos].get_children().length;
                toIndex = (offset > childLength) ? childLength : offset;
                break;
            }
        }
        
        // couldn't find the from and to position because it has been removed
        if (fromPos === -1 || fromIndex === -1 || toIndex === -1) {
            return;
        }
        
        if (pos === fromPos && toIndex === fromIndex) {
            return;
        }
        
        panelBoxs[fromPos].remove_actor(dateMenu.container);
        panelBoxs[pos].insert_child_at_index(dateMenu.container, toIndex);
    }
    
    /**
     * enable show apps button
     *
     * @return void
     */
    showAppsButtonEnable()
    {
        let container = this._main.overview.dash.showAppsButton.get_parent();
        container.remove_style_class_name(this._getAPIClassname('no-show-apps-button'));
    }
    
    /**
     * disable show apps button
     *
     * @return void
     */
    showAppsButtonDisable()
    {
        let container = this._main.overview.dash.showAppsButton.get_parent();
        container.add_style_class_name(this._getAPIClassname('no-show-apps-button'));
    }
    
    /**
     * set animation speed as default
     *
     * @return void
     */
    animationSpeedSetDefault()
    {
        if (typeof this._originals['StSlowDownFactor'] === 'undefined') {
            return;
        }
        
        this._st.Settings.get().slow_down_factor = this._originals['StSlowDownFactor'];
    }
    
    /**
     * change animation speed
     *
     * @param float factor bigger number means slower
     *
     * @return void
     */
    animationSpeedSet(factor)
    {
        if (typeof this._originals['StSlowDownFactor'] === 'undefined') {
            this._originals['StSlowDownFactor']
            = this._st.Settings.get().slow_down_factor;
        }
        
        this._st.Settings.get().slow_down_factor = factor;
    }
    
    /**
     * set the enable animation as default
     *
     * @return void
     */
    enablenAimationsSetDefault()
    {
        if (typeof this._originals['enableAnimations'] === 'undefined') {
            return;
        }
        
        let status = this._originals['enableAnimations'];
        
        this._interfaceSettings.set_boolean('enable-animations', status);
    }
    
    /**
     * set the enable animation status
     *
     * @param bool status
     *
     * @return void
     */
    enablenAimationsSet(status)
    {
        if (typeof this._originals['enableAnimations'] === 'undefined') {
            this._originals['enableAnimations']
            = this._interfaceSettings.get_boolean('enable-animations');
        }
        
        this._interfaceSettings.set_boolean('enable-animations', status);
    }
    
    /**
     * add icon to the activities button
     *
     * @param int type see ICON_TYPE
     * @param string icon file URI or icon name 
     * @param bool monochrome
     * @param bool holdLabel
     *
     * @return void
     */
    ativitiesButtonAddIcon(type, icon, monochrome, holdLabel)
    {
        let iconSize = this._panel.PANEL_ICON_SIZE - this._panel.APP_MENU_ICON_MARGIN;
        let activities = this._main.panel.statusArea['activities'];
        
        this.ativitiesButtonRemoveIcon();
        
        if (!this._activitiesBtn) { 
            this._activitiesBtn = {};
        }
        
        let iconClassname
        = (monochrome)
        ? this._getAPIClassname('activities-button-icon-monochrome')
        : this._getAPIClassname('activities-button-icon');
        
        this._activitiesBtn.icon = new this._st.Icon({
            icon_size: iconSize,
            style_class: iconClassname,
            y_align: this._clutter.ActorAlign.CENTER,
        });
        
        if (monochrome) {
            let effect = new this._clutter.DesaturateEffect();
            this._activitiesBtn.icon.add_effect(effect);
            
            this._activitiesBtn.icon.connect('style-changed', () => {
                let themeNode = this._activitiesBtn.icon.get_theme_node();
                effect.enabled
                = themeNode.get_icon_style() == this._st.IconStyle.SYMBOLIC;
            });
        }
        
        switch (type) {
        
            case ICON_TYPE.NAME:
                if (!icon) {
                    return;
                }
                this._activitiesBtn.icon.set_icon_name(icon);
                break;
            
            case ICON_TYPE.URI:
                let file = this._gio.File.new_for_uri(icon);
                let filePathExists = file.query_exists(null);
                if (!filePathExists) {
                    return;
                }
                let gicon = this._gio.icon_new_for_string(file.get_path());
                this._activitiesBtn.icon.set_gicon(gicon);
                break;
                
            default:
                return;
        }
        
        activities.remove_actor(activities.label_actor);
        
        // add as icon
        if (!holdLabel) {
            this.UIstyleClassAdd(this._getAPIClassname('activities-button-no-label'));
            activities.add_actor(this._activitiesBtn.icon);
            return;
        }
        
        // add as container (icon and text)
        this._activitiesBtn.container = new this._st.BoxLayout();
        this._activitiesBtn.container.add_actor(this._activitiesBtn.icon);
        this._activitiesBtn.container.add_actor(activities.label_actor);
        
        activities.add_actor(this._activitiesBtn.container);
    }
    
    /**
     * remove icon from activities button if it has been added before
     *
     * @return void
     */
    ativitiesButtonRemoveIcon()
    {
        let activities = this._main.panel.statusArea['activities'];
        
        if (!this._activitiesBtn) {
            return;
        }
        
        if (this._activitiesBtn.container) {
            this._activitiesBtn.container.remove_actor(this._activitiesBtn.icon);
            this._activitiesBtn.container.remove_actor(activities.label_actor);
            activities.remove_actor(this._activitiesBtn.container);
            this._activitiesBtn.icon = null;
            this._activitiesBtn.container = null;
        }
        
        if (this._activitiesBtn.icon && activities.contains(this._activitiesBtn.icon)) {
            activities.remove_actor(this._activitiesBtn.icon);
            this._activitiesBtn.icon = null;
        }
        
        if (!activities.contains(activities.label_actor)) {
            activities.add_actor(activities.label_actor);
        }
        
        this.UIstyleClassRemove(this._getAPIClassname('activities-button-no-label'));
    }

    /**
     * enable focus when window demands attention happens
     *
     * @return void
     */
    windowDemandsAttentionFocusEnable()
    {
        if (this._displayWindowDemandsAttentionSignal) {
            return;
        }
    
        this._displayWindowDemandsAttentionSignal
        = global.display.connect('window-demands-attention', (display, window) => {
            this._main.activateWindow(window);
        });
    }
    
    /**
     * disable focus when window demands attention happens
     *
     * @return void
     */
    windowDemandsAttentionFocusDisable()
    {
        if (!this._displayWindowDemandsAttentionSignal) {
            return;
        }
        
        global.display.disconnect(this._displayWindowDemandsAttentionSignal);
        this._displayWindowDemandsAttentionSignal = null;
    }
    
    /**
     * set session mode has overview status
     *
     * @param bool status
     *
     * @return void
     */
    sessionModeHasOverviewStatus(status)
    {
        if (this._shellVersion < 40) {
            return;
        }
        
        this._main.sessionMode.hasOverview = status;
    }
    
    /**
     * set dash icon size to default
     *
     * @return void
     */
    dashIconSizeSetDefault()
    {
        let classnameStarter = this._getAPIClassname('dash-icon-size');
        
        DASH_ICON_SIZES.forEach((size) => {
            this.UIstyleClassRemove(classnameStarter + size);
        });
    }
    
    /**
     * set dash icon size
     *
     * @param int size in pixels
     *   see DASH_ICON_SIZES for available sizes
     *
     * @return void
     */
    dashIconSizeSet(size)
    {
        this.dashIconSizeSetDefault();
        
        if (!DASH_ICON_SIZES.includes(size)) {
            return;
        }
        
        let classnameStarter = this._getAPIClassname('dash-icon-size');
        
        this.UIstyleClassAdd(classnameStarter + size);
    }
}

