var KEY_CODE = {
    LEFT: 37,
    TOP: 38,
    RIGHT: 39,
    DOWN: 40,
    ENTER: 13,
    ESC: 27
};

Main = (function() {
    'use strict';

    var menu_container = null;
    var current_menu = 'main-menu';
    var current_menu_id = '';

    var PAGE_STATES = {
        MENU: 0,
        PAGE: 1
    };

    var menu_stack = [];

    var page_state = PAGE_STATES.MENU;

    var items_visible = 3;
    var central_item_id = 1;

    var menu_list = {
        'main-menu': {
            items: {
                'id-0' : {
                    image: '',
                    title: 'ITEM 0',
                    type: 'type 0',
                    submenu: 'submenu-1'
                },
                'id-1' : {
                    image: '',
                    title: 'ITEM 1',
                    type: 'type 1'
                },
                'id-2' : {
                    image: '',
                    title: 'ITEM 2',
                    type: 'type 2'
                },
                'id-3' : {
                    image: '',
                    title: 'ITEM 3',
                    type: 'type 3'
                },
                'id-4': {
                    image: '',
                    title: 'ITEM 4',
                    type: 'type 4'
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
    };

    function _createNode(type, jsonData, text)
    {
        var el = document.createElement(type);
        for(var id in jsonData)
        {
            el.setAttribute(id, jsonData[id]);
        }
        if(typeof text != 'undefined')
        {
            if(typeof el.innerHTML != 'undefined')
                el.innerHTML = text;
            else if(typeof el.innerText != 'undefined')
                el.innerText  = text;
            else console.error('Unable to set inner value to item', el);
        }

        return el;
    }

    return {
        load: function()
        {
            console.log('Loading...');

            menu_container = document.getElementById(current_menu);
            //var menu_offset = localStorage.getItem('menu_offset');
            //menu_offset = menu_offset ? parseInt(menu_offset) : 0;

            this.prepare_menu();
            this.menu_move_side(0);

            document.body.focus();

            console.log('Loading finished');
        },
        /**
         *
         * @param {string?} menu_name
         */
        prepare_menu: function(menu_name)
        {
            while(menu_container.hasChildNodes())
                menu_container.removeChild(menu_container.firstChild);

            if(typeof menu_name !== 'undefined')
                current_menu = menu_name;

            for(var id in menu_list[current_menu].items)
            {
                var menu = menu_list[current_menu].items[id];

                var item = _createNode('div', {'class': 'item', 'id': id});
                var image = _createNode('div', {'class': 'image'});
                var img = _createNode('img');
                image.appendChild(img);
                var title = _createNode('div', {'class': 'title'}, 'ITEM ' + id);
                var type = _createNode('div', {'class': 'type'}, 'TYPE ' + id);

                item.appendChild(image);
                item.appendChild(title);
                item.appendChild(type);

                if(typeof menu.submenu === 'string')
                {
                    var parent_mark = _createNode('div', {'class': 'parent_mark' });
                    item.appendChild(parent_mark);
                }

                if(typeof menu_list[current_menu].parent_menu === 'string')
                {
                    var child_mark = _createNode('div', {'class': 'child_mark' });
                    item.appendChild(child_mark);
                }

                menu_container.appendChild(item);
            }
        },
        /**
         *
         * @param {Event} e
         */
        onkeydown: function(e)
        {
            switch(page_state)
            {
                case PAGE_STATES.MENU:
                {
                    return this.on_menu_keydown(e);
                }
                default:
                {
                    console.warn('No key handler for page state', page_state);
                    break;
                }
            }

        },
        on_menu_keydown: function(e)
        {
            switch(e.keyCode)
            {
                case KEY_CODE.LEFT:
                {
                    this.menu_move_side(-1);
                    break;
                }
                case KEY_CODE.TOP:
                {
                    break;
                }
                case KEY_CODE.RIGHT:
                {
                    this.menu_move_side(1);
                    break;
                }
                case KEY_CODE.DOWN:
                {
                    break;
                }
                case KEY_CODE.ENTER: {
                    var menu = menu_list[current_menu].items[current_menu_id];
                    if(typeof menu.submenu !== 'undefined')
                    {
                       var submenu = menu.submenu;
                       menu_stack.push(current_menu);
                       menu_list[submenu].parent_menu = current_menu;
                       this.prepare_menu(submenu);
                       this.menu_move_side(0);
                    }
                    break;
                }
                case KEY_CODE.ESC:
                {
                    if(menu_stack.length > 0)
                    {
                        this.prepare_menu(menu_stack.pop());
                        this.menu_move_side(0);
                    }
                    else
                        console.warn('No menu in stack!');
                    break;
                }
            }
        },
        menu_move_side: function(offset)
        {
            var elements = document.getElementsByClassName('item');

            console.log('move_size:' + offset);
            var index = 0;
            if(offset >= 0)
            {
                for(index = 0; index < offset; index++)
                {
                    console.log('appendChild', menu_container.firstElementChild);
                    menu_container.appendChild(menu_container.firstElementChild);
                }
            }
            else
            {
                for(; offset < 0; offset++)
                {
                    console.log('prependChild', menu_container.lastElementChild);
                    menu_container.insertBefore(menu_container.lastElementChild, menu_container.firstElementChild);
                }
            }

            for(index=0; index < elements.length; index++)
            {
                var el = elements[index];
                if(el.parentNode == menu_container)
                {
                    if(index < 3)
                    {
                        el.className = 'item';
                        console.log('index' , index);
                        if(index == 1)
                        {
                            current_menu_id = el.id;
                            localStorage.setItem('current_menu_id', el.id);
                            el.className += ' selected';
                        }

                    }
                    else
                    {
                        el.className = 'item hidden'
                    }
                }
            }

            localStorage.setItem('current_menu', current_menu);
            //var menu_offset = localStorage.getItem('menu_offset');
            //localStorage.setItem('menu_offset', menu_offset + offset);
        }
    };
})();
