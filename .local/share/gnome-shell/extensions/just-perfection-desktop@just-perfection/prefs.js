const Me = imports.misc.extensionUtils.getCurrentExtension();
const {Settings, Translation, Prefs} = Me.imports.lib;
const {Gtk, Gio, GObject} = imports.gi;
const Gettext = imports.gettext;

const Config = imports.misc.config;
const ShellVersion = parseFloat(Config.PACKAGE_VERSION);


function init () {}

function buildPrefsWidget ()
{
    let schemasFolderPath = Me.dir.get_child("schemas").get_path();
    let schemaID = Me.metadata['schema-id'];
    let gettextDomain = Me.metadata['gettext-domain'];
    let UIFilePath = Me.dir.get_child("ui").get_path() + '/prefs.ui';
    let localeFolderPath = Me.dir.get_child("locale").get_path();
    let binFolderPath = Me.dir.get_child("bin").get_path();
    
    Translation.init(Gettext, gettextDomain, localeFolderPath);

    let builder = new Gtk.Builder();
    let settings = Settings.getSettings(Gio, schemaID, schemasFolderPath);
    let prefs = new Prefs.Prefs({
        'Builder': builder,
        'Settings': settings,
        'GObjectBindingFlags' : GObject.BindingFlags,
        'Gtk': Gtk,
        'Gio': Gio,
    }, ShellVersion);
    
    return prefs.getMainPrefs(UIFilePath, binFolderPath, gettextDomain);
}

