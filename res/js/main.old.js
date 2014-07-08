var KEY_CODE = {
    LEFT: 37,
    TOP: 38,
    RIGHT: 39,
    DOWN: 40,
    ENTER: 13,
    ESC: 27,
    KEY_CODE_F1: 112,
    KEY_CODE_F2: 113,
    KEY_CODE_F3: 114,
    KEY_CODE_F4: 115
};

var MENU_TYPE = {
    PROFILE: "stb-profile",
    NEW_PROFILE: "new-stb-profile"
};

Main = (function() {
    'use strict';

    var STORAGE_CURRENT_MENU = 'current_menu';
    var STORAGE_CURRENT_ITEM_ID = 'current_item_id';

    var menu_container = null;

    var PAGE_STATES = {
        MENU: 0,
        PAGE: 1,
        PROFILE_CONFIG: 2
    };

    var menu_stack = [];

    var page_state = PAGE_STATES.MENU;

    var items_visible = 5;
    var central_item_index = Math.floor(items_visible / 2);

    var Menu = {
        'default': 'menu-menu',
        'current': 'main-menu',
        'current_item_id': '',
        'profile_id': '',
        'profile': '',
        'items': {
            'main-menu': {
                items: {
                    'id-0': {
                        image: '',
                        title: 'ITEM 0',
                        type: 'type 0',
                        submenu: 'submenu-1'
                    },
                    'id-1': {
                        image: '',
                        title: 'ITEM 1',
                        type: 'type 1',
                        'menu_type': 'stb-profile'
                    },
                    'id-2': {
                        image: '',
                        title: 'ITEM 2',
                        type: 'type 2'
                    },
                    'id-3': {
                        image: '',
                        title: 'ITEM 3',
                        type: 'type 3'
                    },
                    'id-4': {
                        image: '',
                        title: 'ITEM 4',
                        type: 'type 4'
                    },
                    'id-5': {
                        image: '',
                        title: 'ITEM 5',
                        type: 'type 5'
                    },
                    'id-6': {
                        image: '',
                        title: 'ITEM 6',
                        type: 'type 6'
                    },
                    'id-7': {
                        image: '',
                        title: 'ITEM 7',
                        type: 'type 7'
                    },
                    'id-8': {
                        image: '',
                        title: 'ITEM 8',
                        type: 'type 8'
                    },
                    'id-9': {
                        image: '',
                        title: 'ITEM 9',
                        type: 'type 9'
                    },
                    'id-10': {
                        image: '',
                        title: 'ITEM 10',
                        type: 'type 10'
                    }
                }

            },
            'submenu-1': {
                items: {
                    'sub-id-0': {
                        image: '',
                        title: 'submenu-0',
                        type: 'subtype-0'
                    }
                }
            }
        }
    };

    function _createNode(tag, jsonData, children)
    {
        var node = document.createElement(tag);
        for(var id in jsonData)
        {
            var data_value = jsonData[id];
            if(data_value == null)
                continue;

            switch(id)
            {
                case 'value' :
                {
                    if(['label'].indexOf(tag) != -1)
                    {
                        node.innerHTML = data_value;
                    }
                    else
                        node.value = data_value;
                    break;
                }
                case 'text' :
                {
                    node.innerHTML = data_value;
                    break;
                }
                case 'options' :
                {
                    for(var index = 0; index < data_value.length; index++)
                    {
                        var option = data_value[index];
                        var item = _createNode('option', { value: option.value, text: option.title});
                        node.appendChild(item);
                    }
                    break;
                }
                default:
                {
                    node.setAttribute(id, data_value);
                    break;
                }
            }
        }

        children = children || [];
        for(var index=0; index < children.length; index++)
        {
            var child = children[index];
            node.appendChild(_createNode(child.tag, child.attrs, child.children));
        }

        return node;
    }

    return {
        load: function()
        {
            console.log('Loading...');

            menu_container = document.getElementById(Menu.current);

            this.load_menu();
            this.prepare_menu();
            this.menu_move_horizontal();

            //this.load_profile_config();
            //Main.show_profile_configuration_dialog();

            document.body.focus();

            console.log('Loading finished');
        },
        load_menu: function()
        {
            if(typeof __GUI__ != 'undefined')
                Menu = JSON.parse(__GUI__.makeJsonMenu());
        },
        clear_menu: function()
        {
            while(menu_container.hasChildNodes())
                menu_container.removeChild(menu_container.firstChild);
        },
        /**
         *
         * @param {string?} menu_name
         */
        prepare_menu: function(menu_name)
        {
            this.clear_menu();

            var current_menu = localStorage.getItem(STORAGE_CURRENT_MENU);

            if(typeof menu_name !== 'undefined')
                Menu.current = menu_name;
            else if(current_menu != null)
                Menu.current = current_menu;

            /*
             1 = Left   mouse button
             2 = Centre mouse button
             3 = Right  mouse button
             */
            var onclick = function(e)
            {
                console.log('click:' + e);
                if(e.which == 1)
                {
                    Main.onkeydown({keyCode: KEY_CODE.ENTER, '$item': this.id});
                }
            };

            for(var id in Menu.items[Menu.current].items)
            {
                var menu = Menu.items[Menu.current].items[id];

                var item = _createNode('div', {'class': 'item', 'id': id});
                var image = _createNode('div', {'class': 'image'});
                var img = _createNode('img', {'src': menu.image });

                image.appendChild(img);
                var title = _createNode('div', {'class': 'title', 'text': menu.title});
                //var type = _createNode('div', {'class': 'type'}, menu.type);

                item.appendChild(image);
                item.appendChild(title);
                //item.appendChild(type);

                item.addEventListener('click', onclick);

                if(typeof menu.submenu === 'string')
                {
                    var parent_mark = _createNode('div', {'class': 'parent_mark' });
                    item.appendChild(parent_mark);
                }

                if(typeof Menu.items[Menu.current].parent_menu === 'string')
                {
                    var child_mark = _createNode('div', {'class': 'child_mark' });
                    item.appendChild(child_mark);
                }

                menu_container.appendChild(item);
            }
        },
        /**
         *
         * @param {Event|object} e
         */
        onkeydown: function(e)
        {
            switch(page_state)
            {
                case PAGE_STATES.MENU:
                {
                    return this.on_menu_keydown(e);
                }
                case PAGE_STATES.PROFILE_CONFIG:
                {
                    return this.on_profile_config_keydown(e, true);
                }
                default:
                {
                    console.warn('No key handler for page state', page_state);
                    break;
                }
            }

        },
        /**
         *
         * @param {Event} e
         */
        on_menu_keydown: function(e)
        {
            switch(e.keyCode)
            {
                case KEY_CODE.LEFT:
                {
                    this.menu_move_horizontal(-1);
                    break;
                }
                case KEY_CODE.TOP:
                {
                    break;
                }
                case KEY_CODE.RIGHT:
                {
                    this.menu_move_horizontal(1);
                    break;
                }
                case KEY_CODE.DOWN:
                {
                    break;
                }
                case KEY_CODE.ENTER: {
                    /**
                        If clicked on menu item then item's ID is in e.$item
                     */
                    var menu_id = typeof e.$item !== 'undefined' ? e.$item : Menu.current_item_id;
                    var menu = Menu.items[Menu.current].items[menu_id];

                    if(menu.menu_type == MENU_TYPE.PROFILE)
                    {
                        __GUI__.loadProfile(menu.id);
                    }
                    if(menu.menu_type == MENU_TYPE.NEW_PROFILE)
                    {
                        //__GUI__.createProfile(menu.id, menu.data);
                        Main.show_profile_configuration_dialog(true);
                    }
                    else if(typeof menu.submenu !== 'undefined')
                    {
                       var submenu = menu.submenu;
                       menu_stack.push(Menu.current);
                       Menu.items[submenu].parent_menu = Menu.current;
                       this.prepare_menu(submenu);
                       this.menu_move_horizontal(0);
                    }
                    break;
                }
                case KEY_CODE.ESC:
                {
                    if(menu_stack.length > 0)
                    {
                        this.prepare_menu(menu_stack.pop());
                        this.menu_move_horizontal(0);
                    }
                    else
                        console.warn('No menu in stack!');
                    break;
                }
            }
        },
        /**
         *
         * @param {Event} e
         */
        on_profile_config_keydown: function(e)
        {
            switch(e.keyCode)
            {
                case KEY_CODE.KEY_CODE_F1: {
                    console.log("F1");
                    break;
                }
                case KEY_CODE.KEY_CODE_F2: {
                    console.log("F2");
                    break;
                }
                case KEY_CODE.KEY_CODE_F3: {
                    console.log("F3");
                    break;
                }
                case KEY_CODE.KEY_CODE_F4: {
                    console.log("F4");
                    break;
                }
            }
        },
        /**
         *
         * @param {int?} offset
         */
        menu_move_horizontal: function(offset)
        {
            if(typeof offset === 'undefined')
            {
                offset = 0;
                var current_item_id = localStorage.getItem(STORAGE_CURRENT_ITEM_ID);
                if(current_item_id != null)
                {
                    var item_index = 0;

                    for(var id in Menu.items[Menu.current].items)
                    {
                        if(id == current_item_id)
                        {
                            offset = item_index - central_item_index;
                            break;
                        }
                        item_index++;
                    }
                }
            }

            var elements = document.getElementsByClassName('item');

            console.log('move_size:' + offset);
            var index = 0;
            if(offset >= 0)
            {
                for(index = 0; index < offset; index++)
                {
                    menu_container.appendChild(menu_container.firstElementChild);
                }
            }
            else
            {
                for(; offset < 0; offset++)
                {
                    menu_container.insertBefore(menu_container.lastElementChild, menu_container.firstElementChild);
                }
            }

            for(index=0; index < elements.length; index++)
            {
                var el = elements[index];
                if(el.parentNode == menu_container)
                {
                    if(index < items_visible)
                    {
                        el.className = 'item';
                        if(index == central_item_index)
                        {
                            Menu.current_item_id = el.id;
                            localStorage.setItem(STORAGE_CURRENT_ITEM_ID, Menu.current_item_id);
                            el.className += ' selected';
                        }

                    }
                    else
                    {
                        el.className = 'item hidden'
                    }
                }
            }

            localStorage.setItem(STORAGE_CURRENT_MENU, Menu.current);
            //var menu_offset = localStorage.getItem('menu_offset');
            //localStorage.setItem('menu_offset', menu_offset + offset);
        },
        show_profile_configuration_dialog: function(new_profile)
        {
            page_state = PAGE_STATES.PROFILE_CONFIG;
            Main.load_profile_config(new_profile ? null : Menu.current_item_id);
        },
        load_profile_config: function(profile_id)
        {
            var container = document.getElementById("profile-configuration");
            container.className = "";
            this.profile_id = profile_id;
            var config = null;

            if(typeof __GUI__ != 'undefined')
                config = JSON.parse(__GUI__.getProfileConfigOptions(this.profile_id));
            else
                config = [
                    {
                        'name' : 'test-config-1',
                        'title': 'title-1',
                        'value' : 'value-1',
                        'comment' : 'comment-1'
                    },
                    {
                        'name' : 'select-name-1',
                        'title' : 'select-title-1',
                        'value' : 2,
                        'comment' : 'select-comment-1',
                        'options' : [
                            {
                                'title' : 0,
                                'value' : 0
                            },
                            {
                                'title' : 1,
                                'value' : 1
                            },
                            {
                                'title' : 2,
                                'value' : 2
                            },
                            {
                                'title' : 3,
                                'value' : 3
                            }
                        ]


                    }
                ];

            var config_table = document.getElementById('config-container');
            while(config_table.hasChildNodes()) config_table.removeChild(config_table.firstChild);
            for(var index=0; index < config.length; index++)
            {
                var config_item = config[index];

                var elemTag;
                var options = [];

                console.log(config_item  + typeof config_item.value);

                switch(typeof config_item.value)
                {
                    case 'integer': {
                        elemTag = 'input';
                        break;
                    }
                    case 'boolean': {
                        elemTag = 'select';
                        options = [
                            {
                                title: 'true',
                                value: 'true'
                            },
                            {
                                title: 'false',
                                value: 'false'
                            }
                        ];

                        break;
                    }
                    case 'string':
                    default: {
                        console.log('options' + typeof config_item.options)
                        if(typeof config_item.options != 'undefined' && Array.isArray(config_item.options))
                        {
                            elemTag = 'select';
                            options = config_item.options;
                            console.log('AAAA:' + options.length)
                        }
                        else
                        {
                            elemTag = 'input';
                        }
                        break;
                    }
                }

                var edit_tag_id = 'config-' + config_item.tag + '/' + config_item.name;

                var new_node_options = [
                    {
                        tag: 'td',
                        children: [
                            {
                                tag: 'label',
                                attrs: {
                                    'value': config_item.title,
                                    'for': edit_tag_id
                                }
                            }
                        ]
                    },
                    {
                        tag: 'td',
                        children: [
                            {
                                tag: elemTag,
                                attrs: {
                                    value: config_item.value,
                                    'class': 'config-option',
                                    id: edit_tag_id,
                                    options: elemTag == 'select' ? options: null
                                }
                            }
                        ]
                    }
                ];

                var tr = _createNode('tr', { 'class': 'config-row' }, new_node_options );

                config_table.appendChild(tr);

                if(typeof config_item.comment != 'undefined')
                {
                    var _comment = _createNode('tr', {
                            'class' : 'comment'
                        },
                        [
                            {
                                tag: 'td',
                                attrs: {
                                    colspan: 2
                                },
                                children: [
                                    {
                                        tag: 'span',
                                        attrs: {
                                            'class': 'text',
                                            'text': config_item.comment
                                        }
                                    }
                                ]
                            }

                        ]
                    );
                    config_table.appendChild(_comment);
                }
            }
        }
    };
})();
