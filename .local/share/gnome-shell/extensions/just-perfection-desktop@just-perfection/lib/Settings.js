/**
 * Settings Library
 * 
 * @author     Javad Rahmatzadeh <j.rahmatzadeh@gmail.com>
 * @copyright  2021
 * @license    GNU General Public License v3.0
 */


/**
 * get the settings object
 *
 * @param object Gio gi::Gio
 * @param string schemaID
 *   for example: 'org.gnome.shell.extensions.just-perfection'
 * @param string schemasFolderPath
 *
 * @throws Error on
 *   - bad file
 *   - wrong schema id
 * 
 * @return object Gio::Settings
 */
function getSettings(Gio, schemaID, schemasFolderPath)
{
    let GioSSS = Gio.SettingsSchemaSource;
  
    let schemaSource = GioSSS.new_from_directory(
        schemasFolderPath,
        GioSSS.get_default(),
        false
    );
  
    let schemaObj = schemaSource.lookup(schemaID, true);
  
    if (!schemaObj) {
        throw new Error('cannot find schemas');
    }
  
    return new Gio.Settings({ settings_schema : schemaObj });
}

