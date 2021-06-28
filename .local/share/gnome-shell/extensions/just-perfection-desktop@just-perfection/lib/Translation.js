/**
 * Translation Library
 * 
 * @author     Javad Rahmatzadeh <j.rahmatzadeh@gmail.com>
 * @copyright  2021
 * @license    GNU General Public License v3.0
 */


/**
 * initiate translation
 *
 * @param object Gettext
 * @param string domain
 * @param string localeFolderPath
 * 
 * @return void
 */
function init(Gettext, domain, localeFolderPath)
{
    Gettext.bindtextdomain(domain, localeFolderPath);
    Gettext.textdomain(domain);
}

