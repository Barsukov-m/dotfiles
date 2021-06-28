/**
 * Prefs Library
 * 
 * @author     Javad Rahmatzadeh <j.rahmatzadeh@gmail.com>
 * @copyright  2021
 * @license    GNU General Public License v3.0
 */


/**
 * prefs widget for showing prefs window
 */
var Prefs = class
{
    /**
     * class contructor
     *
     * @param object dependecies
     *   'Builder' instance of Gtk::Builder
     *   'Settings' instance of Gio::Settings
     *   'GObjectBindingFlags' instance of GObject::BindingFlags
     *   'Gtk' reference to Gtk
     *   'Gio' reference to Gio
     * @param float shellVersion
     */
    constructor(dependecies, shellVersion)
    {
        this._settings = dependecies['Settings'] || null;
        this._builder = dependecies['Builder'] || null;
        this._GObjectBindingFlags = dependecies['GObjectBindingFlags'] || null;
        this._Gtk = dependecies['Gtk'] || null;
        this._gio = dependecies['Gio'] || null;
        
        this._shellVersion = shellVersion;
        
        // @var int initial window size
        this._windowWidth = 500;
        this._windowHeight = 750;
        
        // @var array
        // key name for supports popover
        // array index should indicate to 'support_comboboxtext'
        this._supports = [
            'bitcoin',
            'bitcoincash',
            'ethereum',
            'doge',
            'patreon',
        ];
        
        // @var object
        // for structure see self::_setKey()
        this._keys = {};
        
        // setting all the main keys
        
        // visiblity
        this._setKey('visiblity', 'panel', 'GtkSwitch', true);
        this._setKey('visiblity', 'activities-button', 'GtkSwitch', true);
        this._setKey('visiblity', 'app-menu', 'GtkSwitch', true);
        this._setKey('visiblity', 'clock-menu', 'GtkSwitch', true);
        this._setKey('visiblity', 'keyboard-layout', 'GtkSwitch', true);
        this._setKey('visiblity', 'accessibility-menu', 'GtkSwitch', true);
        this._setKey('visiblity', 'aggregate-menu', 'GtkSwitch', true);
        this._setKey('visiblity', 'search', 'GtkSwitch', true);
        this._setKey('visiblity', 'dash', 'GtkSwitch', true);
        this._setKey('visiblity', 'osd', 'GtkSwitch', true);
        this._setKey('visiblity', 'workspace-popup', 'GtkSwitch', true);
        this._setKey('visiblity', 'workspace', 'GtkSwitch', true);
        this._setKey('visiblity', 'background-menu', 'GtkSwitch', true);
        this._setKey('visiblity', 'show-apps-button', 'GtkSwitch', true);
            
        // icons
        this._setKey('icons', 'app-menu-icon', 'GtkSwitch', true);
        this._setKey('icons', 'panel-notification-icon', 'GtkSwitch', true);
        this._setKey('icons', 'power-icon', 'GtkSwitch', true);
        this._setKey('icons', 'window-picker-icon', 'GtkSwitch',
            this._shellVersion >= 40);
        this._setKey('icons', 'panel-arrow', 'GtkSwitch',
            this._shellVersion < 40);
        this._setKey('icons', 'activities-button-icon-path', 'GtkEntry', true);
        this._setKey('icons', 'activities-button-icon-monochrome', 'GtkSwitch', true);
        this._setKey('icons', 'activities-button-label', 'GtkSwitch', true);
        
        
        // behavior
        this._setKey('behavior', 'type-to-search', 'GtkSwitch', true);
        this._setKey('behavior', 'hot-corner', 'GtkSwitch', true);
        this._setKey('behavior', 'gesture', 'GtkSwitch', this._shellVersion < 40);
        this._setKey('behavior', 'window-demands-attention-focus', 'GtkSwitch', true);
        
        // customize
        this._setKey('customize', 'top-panel-position', 'GtkComboBoxText', true);
        this._setKey('customize', 'panel-corner-size', 'GtkComboBoxText', true);
        this._setKey('customize', 'clock-menu-position', 'GtkComboBoxText', true);
        this._setKey('customize', 'clock-menu-position-offset', 'GtkComboBoxText', true);
        this._setKey('customize', 'workspace-switcher-size', 'GtkComboBoxText',
            this._shellVersion >= 40);
        this._setKey('customize', 'animation', 'GtkComboBoxText', true);
        this._setKey('customize', 'dash-icon-size', 'GtkComboBoxText', true, {
            '1' : 32,
            '2' : 48,
            '3' : 64,
        });
        
        // override
        this._setKey('override', 'theme', 'GtkSwitch', true);
    }
    
    /**
     * set key
     *
     * @param string category
     * @paramm string name should be the same as gsettings key name
     * @param string widgetType gtk widget type like 'GtkSwitch'.
     * @param bool supported
     * @param object [maps] for example for combobox you can specify if the index
     *  is 1 go use 32 as value:
     *  {1 : 32}
     *
     * @return void
     */
    _setKey(category, name, widgetType, supported, maps)
    {
        let id = name.replace(/-/g, "_");
        let widgetId = id + '_' + widgetType.toLowerCase().replace('gtk', '');
        
        if (typeof maps === 'undefined') {
            maps = {};
        }
        
        this._keys[id] = {
            'category' : category,
            'widgetType' : widgetType,
            'name' : name,
            'id' : id,
            'widgetId' : widgetId,
            'supported' : supported,
            'maps' : maps,
        }
    }
    
    /**
     * register all signals
     *
     * @paramm string UIFilePath
     * @param string binFolderPath
     * @param string gettextDoamin
     *
     * @return object
     */
    getMainPrefs(UIFilePath, binFolderPath, gettextDomain)
    {
        this._builder.set_translation_domain(gettextDomain);
        this._builder.add_from_file(UIFilePath);
        
        let obj = this._builder.get_object('main_prefs');
        
        this._fixImageObjects(binFolderPath);
        this._fixIconObjects();
        this._setCurrentValues();
        
        // applying search here makes all the supported elements by current
        // shell version be visible and other not
        this._search('');
        
        obj.connect('realize', () => {
        
            let window = (this._shellVersion < 40) ? obj.get_toplevel() : obj.get_root();
        
            // default window size
            window.default_width = this._windowWidth;
            window.set_size_request(this._windowWidth, this._windowHeight);
            if (this._shellVersion < 40) {
                window.resize(this._windowWidth, this._windowHeight);
            }
            
            // csd
            let headerBar = this._builder.get_object("header_bar");
            window.set_titlebar(headerBar);
            if (this._shellVersion < 40) {
                headerBar.set_title('Just Perfection');
                headerBar.set_show_close_button(true);
            }
            
            this._registerSignals(window);
        });
                
        return obj;
    }
    
    /**
     * fixing image widget
     *
     * on GTK4 we need Gtk.Picture to have correct aspect ratio
     * on GTK3 we only need to set the path
     *
     * @param Gtk.Widget widget
     * @param string filePath
     *
     * @return void
     */
    _fixImageWidget(widget, filePath)
    {
        if (this._shellVersion < 40) {
            widget.set_from_file(filePath);
            return;
        }
        
        let parent = widget.get_parent();
        let pic = this._Gtk.Picture.new_for_filename(filePath);
        
        pic.set_size_request(widget.width_request, widget.height_request);
        widget.hide();
        parent.append(pic);
    }
    
    /**
     * fix properties path for builder objects
     *
     * @param string binFolderPath
     *
     * @return void
     */
    _fixImageObjects(binFolderPath)
    {
        // since we are using file property on .ui file we need
        // the actual path for the gtkImage::file
        
        this._supports.forEach( name => {
            let image = this._builder.get_object(`support_${name}_image`);
            this._fixImageWidget(image, `${binFolderPath}/support-${name}.svg`);
        });
    }
    
    /**
     * fix images that holding icons for GTK4
     *
     * @return void
     */
    _fixIconObjects()
    {
        if (this._shellVersion < 40) {
            return;
        }
        
        let icons = [
            'support_icon',
            'search_icon',
        ];
        
        icons.forEach( id => {
            let elm = this._builder.get_object(id);
            let parent = elm.get_parent();
            let iconName = elm.get_icon_name();
            let iconSize = elm.get_icon_size();
            
            elm.hide();
            parent.icon_name = iconName;
            parent.icon_size = iconSize;
        });
    }
  
    /**
     * register all signals
     *
     * @param GtkWindow window
     *
     * @return void
     */
    _registerSignals(window)
    {
        let searchEntry = this._builder.get_object("search_entry");
        let searchBtn = this._builder.get_object("search_togglebutton");
        let searchBar = this._builder.get_object("searchbar");
        let fileChooser = this._builder.get_object("file_chooser");
        let activitiesButtonIconPath = {
            button: this._builder.get_object('activities_button_icon_path_button'),
            entry: this._builder.get_object('activities_button_icon_path_entry'),
            empty: this._builder.get_object('activities_button_icon_path_empty_button'),
        };
        
        let supportWindow = this._builder.get_object("support_window");
        
        for (let [id, key] of Object.entries(this._keys)) {
        
            switch (key.widgetType) {
            
                case 'GtkSwitch':
                    this._builder.get_object(key.widgetId).connect("state-set", (w) => {
                        this._settings.set_boolean(key.name, w.get_active());
                    });
                    break;
                    
                case 'GtkComboBoxText':
                    this._builder.get_object(key.widgetId).connect("changed", (w) => {
                        let index = w.get_active();
                        let value = (index in key.maps) ? key.maps[index] : index; 
                        this._settings.set_int(key.name, value);
                    });
                    break;
                    
                case 'GtkEntry':
                    this._builder.get_object(key.widgetId).connect("changed", (w) => {
                        this._settings.set_string(key.name, w.text);
                    });
                    break;
            }
        }
        
        searchEntry.connect("changed", (w) => {
            this._search(w.get_text());
        });
        
        if (this._shellVersion < 40) {
            window.connect('key-press-event', (w, e) => {
                return searchBar.handle_event(e);
            });
        } else {
            searchBar.set_key_capture_widget(window);
        }
        
        searchBtn.bind_property('active', searchBar, 'search-mode-enabled',
            this._GObjectBindingFlags.BIDIRECTIONAL);
        
        searchBar.connect_entry(searchEntry);
        
        this._builder.get_object('support_comboboxtext').connect("changed", (w) => {
            let index = w.get_active();
            for (let i = 0; i < this._supports.length; i++) {
                let name = this._supports[i];
                let visible = (index === i);
                this._builder.get_object(`support_${name}`).visible = visible;
            }
        });
        
        activitiesButtonIconPath['entry'].connect('changed', (w) => {
            this._setFileChooserValue('activities_button_icon_path', w.text);
        });
        
        activitiesButtonIconPath['empty'].connect('clicked', () => {
            this._setFileChooserValue('activities_button_icon_path', '');
        });
        
        activitiesButtonIconPath['button'].connect('clicked', (w) => {
            this.currentFileChooserEntry = activitiesButtonIconPath['entry'];
            
            let uri = activitiesButtonIconPath['entry'].text;
            let file = this._gio.File.new_for_uri(uri);
            let fileExists = file.query_exists(null);
            if (fileExists) {
                let fileParent = file.get_parent();
                fileChooser.set_current_folder(
                    (this._shellVersion >= 40) ? fileParent : fileParent.get_path());
            }
            
            fileChooser.set_transient_for(window);
            fileChooser.show();
        });
        
        fileChooser.connect('response', (w, response) => {
            if (response !== this._Gtk.ResponseType.ACCEPT) {
                return;
            }
            let fileURI = w.get_file().get_uri();
            this.currentFileChooserEntry.text = fileURI;
        });
    }
    
    /**
     * set file chooser button value
     *
     * @param string id element starter id
     * @param string uri
     *
     * @return void
     */
    _setFileChooserValue(id, uri)
    {
        let preview = this._builder.get_object(`${id}_preview`);
        let emptyButton = this._builder.get_object(`${id}_empty_button`);
        let entry = this._builder.get_object(`${id}_entry`);
        
        if (!entry) {
            return;
        }
        
        let file = this._gio.File.new_for_uri(uri);
        let fileExists = file.query_exists(null);
        let uriPrepared = (fileExists) ? uri : ''; 
        
        let visible = uriPrepared !== '';
        
        entry.text = uriPrepared;
        emptyButton.visible = visible;
        
        preview.clear();
        
        if (fileExists) {
            let gicon = this._gio.icon_new_for_string(file.get_path());
            if (this._shellVersion < 40) {
                preview.set_from_gicon(gicon, 1);
            } else {
                preview.set_from_gicon(gicon);
            }
        } else {
            preview.icon_name = 'document-open-symbolic';
        }
    }
  
    /**
     * set current values for all elements
     *
     * @return void
     */
    _setCurrentValues()
    {
        for (let [id, key] of Object.entries(this._keys)) {
        
            let elm = this._builder.get_object(key.widgetId);
        
            switch (key.widgetType) {
            
                case 'GtkSwitch':
                    elm.set_active(this._settings.get_boolean(key.name));
                    break;
                    
                case 'GtkComboBoxText':
                    let index = this._settings.get_int(key.name);
                    for (let k in key.maps) {
                        if (key.maps[k] === index) {
                            index = k;
                            break;
                        }
                    }
                    elm.set_active(index);
                    break;
                    
                case 'GtkEntry':
                    elm.text = this._settings.get_string(key.name);
                    this._setFileChooserValue(key.id, elm.text);
                    break;
            }
        }
    }
    
    /**
     * search the query
     *
     * @return void
     */
    _search(q)
    {
        let categories = {};
        let noResultsFoundVisiblity = true;
    
        for (let [id, key] of Object.entries(this._keys)) {
        
            if (typeof categories[key.category] === 'undefined') {
                categories[key.category] = 0;
            }
        
            let text = this._builder.get_object(id + "_txt").get_text();
            let row = this._builder.get_object(id + "_row");
            
            let visible = key.supported && text.toLowerCase().includes(q);
            
            row.visible = visible;
            
            if (visible) {
                categories[key.category]++;
                noResultsFoundVisiblity = false;
            }
        }
        
        // hide the category when nothing is visible in it
        for (var category in categories) {
            
            let titleElm = this._builder.get_object(category + "_title");
            let frameElm = this._builder.get_object(category + "_frame");
            
            let visible = (categories[category] > 0);
            
            titleElm.visible = visible;
            frameElm.visible = visible;
        }
        
        this._builder.get_object("no_results_found").visible = noResultsFoundVisiblity;
    }
};

