const Me = imports.misc.extensionUtils.getCurrentExtension();
const {API, Settings, Manager, HotCorner} = Me.imports.lib;
const {GLib, Gio, St, Clutter} = imports.gi;

const Config = imports.misc.config;
const ShellVersion = parseFloat(Config.PACKAGE_VERSION);

const Main = imports.ui.main;
const BackgroundMenu = imports.ui.backgroundMenu;
const OverviewControls = imports.ui.overviewControls;
const WorkspaceSwitcherPopup = imports.ui.workspaceSwitcherPopup;
const ViewSelector = (ShellVersion < 40) ? imports.ui.viewSelector : null;
const WorkspaceThumbnail = imports.ui.workspaceThumbnail;
const SearchController = (ShellVersion >= 40) ? imports.ui.searchController : null;
const Panel = imports.ui.panel;
const WorkspacesView = imports.ui.workspacesView;


let manager;
let api;


function init() {}

function enable()
{
    // <3.36 can crash by enabling the extension
    // since <3.36 is not supported we simply throw error
    // to avoid bad experience for <3.36 users. 
    if (ShellVersion < 3.36) {
        throw new Error('GNOME Shell is not Supported');
    }
    
    let schemasFolderPath = Me.dir.get_child("schemas").get_path();
    let schemaID = Me.metadata['schema-id'];

    let interfaceSettings = new Gio.Settings({schema_id: 'org.gnome.desktop.interface'});
    
    api = new API.API({
        'Main': Main,
        'BackgroundMenu': BackgroundMenu,
        'OverviewControls': OverviewControls,
        'WorkspaceSwitcherPopup': WorkspaceSwitcherPopup,
        'InterfaceSettings': interfaceSettings,
        'SearchController': SearchController,
        'ViewSelector': ViewSelector,
        'WorkspaceThumbnail': WorkspaceThumbnail,
        'WorkspacesView': WorkspacesView,
        'St': St,
        'Gio': Gio,
        'GLib': GLib,
        'Clutter': Clutter,
        'Panel': Panel,
    }, ShellVersion);
    
    api.open();
    
    let settings = Settings.getSettings(Gio, schemaID, schemasFolderPath);
    let hotCorner = new HotCorner.HotCorner({ 'API': api, 'St': St });
    
    manager = new Manager.Manager({
        'API': api,
        'Settings': settings,
        'HotCorner': hotCorner,
    }, ShellVersion);
        
    manager.registerSettingsSignals();
    manager.applyAll();
}

function disable()
{
    manager.revertAll();
    manager = null;
    api.close();
}

