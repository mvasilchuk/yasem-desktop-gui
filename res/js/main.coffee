

class Main

    STORAGE_CURRENT_MENU = 'current_menu'
    STORAGE_CURRENT_ITEM_ID = 'current_item_id'

    appInfo = {}

    menu_container = null

    PAGE_IDENT = {
        MENU: "menu",
        PROFILE_CONFIG: "profile-configuration",
        HELP: "help"
    }

    menu_stack = []
    page_stack = []

    central_item_index = 0

    page_state = PAGE_IDENT.MENU

    items_visible = 5
    menu_visual_type = null;

    menu_size = {
        width: 5
        height: 3
    }

    menu_position = {
        x: 0,
        y: 0
    }

    Menu = {}

    by_id = (id) ->
        return document.getElementById(id)

    _createNode = (tag, jsonData, children) ->
        node = document.createElement(tag)
        for id, data_value of jsonData
            if data_value == null then continue

            switch id
                when 'value'
                    if ['label'].indexOf(tag) != -1
                        node.innerHTML = data_value
                    else
                        node.value = data_value

                when 'text'
                    node.innerHTML = data_value;

                when 'options'
                    for value, text of data_value
                        item = _createNode('option', { value: value, text: text});
                        if value == jsonData.value
                            item.selected = 'selected'
                        node.appendChild(item);

                else
                    node.setAttribute(id, data_value);


        children = children || [];
        for child in children
            node.appendChild(child)

        return node

    load: () ->
        console.log('Loading...')

        @load_app_info()
        @open_page(PAGE_IDENT.MENU)
        @load_menu()
        @prepare_menu()
        #@menu_move_horizontal()
        @select_menu_item(0, 0)
        @show_shortcuts_for_current_page()

        #@load_profile_config();
        #@show_profile_configuration_dialog();

        document.body.focus();
        console.log('Loading finished')

    load_app_info: () ->
        @appInfo = JSON.parse(__GUI__.getAppInfo())

        by_id('app-copyright').innerHTML = @appInfo.copyright
        by_id('app-version').innerHTML = @appInfo.version

    load_menu: () ->
        if __GUI__?
            Menu = JSON.parse(__GUI__.makeJsonMenu())
            console.log(Menu)

        if(!Menu.items[Menu.current].count)
            console.error('Menu is either empty or items count not set')

        items_visible = if Menu.items[Menu.current].count > items_visible then items_visible else Menu.items[Menu.current].count

        central_item_index = Math.floor(items_visible / 2)

    clear_menu: () ->
        menu_container = document.getElementById('main-menu')
        while(menu_container.hasChildNodes())
            menu_container.removeChild(menu_container.firstChild)

    ###*
    #*
    #* @param {string?} menu_name
    *###
    prepare_menu: (menu_name) ->

        @clear_menu()

        #current_menu = localStorage.getItem(STORAGE_CURRENT_MENU)

        if(typeof menu_name != 'undefined')
            Menu.current = menu_name;

        console.log('current menu: ' + Menu.current )


        if(typeof Menu.items[Menu.current].items != 'object')
            console.error("Menu items not found! Did you forget to add 'items' into menu object?")
            return false
        #else if(current_menu != null)
        #    Menu.current = current_menu;

        ###*
         1 = Left   mouse button
         2 = Centre mouse button
         3 = Right  mouse button
         *###
        self = @
        onclick = (e) ->
            console.log('click:' + e);
            if(e.which == 1)
                self.onkeydown({keyCode: KEY_CODE.ENTER, '$item': this.id})

        onmouseover = (e) ->
            coord = self.find_menu_item_by_id(this.id)
            self.select_menu_item(coord.x, coord.y)

        for id, item of Menu.items[Menu.current].items
            menu = Menu.items[Menu.current].items[id]

            node = _createNode('div', {'class': 'item', 'id': id})
            image = _createNode('div', {'class': 'image'})
            img = _createNode('img', {'src': menu.image })

            image.appendChild(img);
            title = _createNode('div', {'class': 'title', 'text': menu.title})
            #type = _createNode('div', {'class': 'type'}, menu.type);

            node.appendChild(image);
            node.appendChild(title);
            #node.appendChild(type);

            node.addEventListener('click', onclick);
            node.addEventListener('mouseover', onmouseover);

            if(typeof menu.submenu == 'string')
                parent_mark = _createNode('div', {'class': 'parent_mark' })
                node.appendChild(parent_mark);

            if(typeof Menu.items[Menu.current].parent_menu == 'string')
                child_mark = _createNode('div', {'class': 'child_mark' })
                node.appendChild(child_mark)

            menu_container.appendChild(node)
    ###*
     *
     * @param {Event|object} e
     *###
    onkeydown: (e) ->
        preventDefault = false;
        switch page_state
            when PAGE_IDENT.MENU
                preventDefault = @on_menu_keydown(e)
            when PAGE_IDENT.PROFILE_CONFIG
                preventDefault = @on_profile_config_keydown(e, true)
            when PAGE_IDENT.HELP
                preventDefault = @on_help_keydown(e, true)
            else
                console.warn('No key handler for page state', page_state)
        if(preventDefault && e.preventDefault)
            e.preventDefault()

    ###*
     *
     * @param {Event} e
     *###
    on_menu_keydown: (e) ->
        switch e.keyCode
            when KEY_CODE.LEFT
                #@menu_move_horizontal(-1)
                @select_menu_item(menu_position.x - 1, menu_position.y)
            when KEY_CODE.TOP
                #@move_menu_up_down(-1)
                @select_menu_item(menu_position.x, menu_position.y - 1)
                break
            when KEY_CODE.RIGHT
                #@menu_move_horizontal(1);
                @select_menu_item(menu_position.x + 1, menu_position.y)
            when KEY_CODE.DOWN
                #@move_menu_up_down(1)
                @select_menu_item(menu_position.x, menu_position.y + 1)
                break;
            when KEY_CODE.ENTER
                ###
                    If clicked on menu item then item's ID is in e.$item
                ###
                menu_id = if e.$item? then e.$item else Menu.current_item_id
                menu = Menu.items[Menu.current].items[menu_id];

                console.log('Menu:', menu)

                if menu.type == MENU_TYPE.PROFILE
                    __GUI__.loadProfile(menu.id);
                if menu.type == MENU_TYPE.NEW_PROFILE
                    #__GUI__.createProfile(menu.id, menu.data);
                    @create_new_profile(menu.class, menu.submodel, "{}");

                else if typeof menu.submenu != 'undefined'
                    submenu = menu.submenu
                    menu_stack.push(Menu.current)
                    Menu.items[submenu].parent_menu = Menu.current
                    @prepare_menu(submenu)
                    #@menu_move_horizontal(0)

            when KEY_CODE.ESC
                if(menu_stack.length > 0)
                    this.prepare_menu(menu_stack.pop())
                    #this.menu_move_horizontal(0)
                else
                    console.warn('No menu in stack!')

            when KEY_CODE.KEY_CODE_F1
                @open_page(PAGE_IDENT.HELP)
            when KEY_CODE.KEY_CODE_F2
                @load_profile_config(Menu.current_item_id)
            when KEY_CODE.KEY_CODE_F3
                console.log("F3")
            when KEY_CODE.KEY_CODE_F4
                if confirm('Remove profile '+menu.title+'?')
                    __GUI__.removeProfile(Menu.current_item_id);
                    document.location.reload()
            else
                return false
        return true

    ###*
     *
     * @param {Event} e
     *###
    on_profile_config_keydown: (e) ->
        switch e.keyCode
            when KEY_CODE.KEY_CODE_F1
                console.log("F1")
            when KEY_CODE.KEY_CODE_F2
                console.log("F2")
                @save_profile_config()
            when KEY_CODE.KEY_CODE_F3
                console.log("F3")
            when KEY_CODE.KEY_CODE_F4
                console.log("F4");
            when KEY_CODE.ESC
                @close_page()
            else
                return false
        return true

    ###*
     *
     * @param {Event} e
     *###
    on_help_keydown: (e) ->
        switch e.keyCode
            when KEY_CODE.ESC
                @close_page()
            else
                return false
        return true

    move_menu_blocks: (offset) ->
        elements = document.getElementsByClassName('item')
        for element, index in elements
            element.className = 'item'

        index = 0
        for id, item of Menu.items[Menu.current].items
            if index == offset
                element = document.getElementById(id)
                element.className += ' selected'
                Menu.current_item_id = element.id
                break
            index++

    find_menu_item_by_id: (id) ->
        elements = document.getElementsByClassName('item')
        for element, index in elements
            if element.id == id
                return {
                    x: index % menu_size.width
                    y: Math.floor(index / menu_size.width)
                }
        return null


    select_menu_item: (x, y) ->

        menu_item_count = Menu.items[Menu.current].count

        if x >= menu_size.width
            x = x - menu_size.width
            y++

        if x < 0
            x = menu_size.width + x
            y--

        if y >= menu_size.height
            y = y - menu_size.height

        if y < 0
            y = menu_size.height + y

        if (x + 1) * (y + 1) >= menu_size.width * menu_size.height
            x = 0
            y = 0

        if x + (y * menu_size.width)  >= menu_item_count
            y = Math.floor(menu_item_count / menu_size.width)
            x = menu_item_count - y * menu_size.width - 1

        menu_position.x = x
        menu_position.y = y

        @move_menu_blocks(x + menu_size.width * y)

    create_new_profile: (class_id, submodel, data) ->
        id = __GUI__.createProfile(class_id, submodel, data)
        @load_profile_config(id)

    save_profile_config: () ->
        data = []
        elements = document.getElementsByClassName('config-option')
        for element, index in elements
            console.log('el', element.id, element.value)
            name = /^config-(.*?)$/.exec(element.id)[1]
            if name == null
                console.log('name:' + name)
                continue
            console.log('name', name)
            data.push({
                name: name
                value: element.value
            })

        #console.log('saving config for ' + @profile_id + " ->" + JSON.stringify(data))
        __GUI__.saveProfile(@profile_id, JSON.stringify(data))
        document.location.reload()

    ###*
    * @param {string} profile_id
    * @param {string} class_id?
    *###
    load_profile_config: (profile_id, class_id) ->
        @open_page(PAGE_IDENT.PROFILE_CONFIG)
        @profile_id = profile_id
        config = null;

        config_data = JSON.parse(__GUI__.getProfileConfigOptions(@profile_id));

        submodel_key = config_data['submodel_key'];
        config = config_data.options;

        config_table = document.getElementById('config-container')
        while(config_table.hasChildNodes())
            config_table.removeChild(config_table.firstChild)

        for config_item, index in config
            config_item = config[index];

            elemTag = null
            inputType = null
            options = null;

            #if(config_item.tag == submodel_key)


            #console.log(config_item  + typeof config_item.value);

            switch config_item.type
                when CONFIG_ITEM_TYPES.STRING, CONFIG_ITEM_TYPES.INT
                    elemTag = 'input'
                when CONFIG_ITEM_TYPES.BOOL
                    elemTag = 'select';
                    options = {
                        'true': 'TRUE',
                        'false': 'FALSE'
                    }
                else
                    console.log('options' + typeof config_item.options)
                    if(typeof config_item.options != 'undefined' && config_item.options)
                        elemTag = 'select'
                        options = config_item.options;
                    else
                        elemTag = 'input'
                    break;


            edit_tag_id = 'config-' + config_item.tag + '/' + config_item.name

            new_node_options = [
                _createNode('td', {},
                    [
                        _createNode('label', {
                            'value': config_item.title,
                            'for': edit_tag_id
                        })
                    ])
                _createNode('td', {},
                    [
                        _createNode(elemTag, {
                            value: config_item.value,
                            class: 'config-option',
                            id: edit_tag_id,
                            type: inputType
                            options: options
                        })
                    ]
                )
            ]

            tr = _createNode('tr', { 'class': 'config-row' }, new_node_options )

            config_table.appendChild(tr);

            if(typeof config_item.comment != 'undefined')
                _comment = _createNode('tr', {
                        class : 'comment'
                    },
                    [
                        _createNode('td', { colspan: 2}, [
                            _createNode('span', {
                                class: 'text',
                                text: config_item.comment
                            })
                        ])
                    ]
                );
                config_table.appendChild(_comment)

    ###*
    *
    * @param {boolean} show
    *###
    show_shortcuts: (show)->
        shortcuts = document.getElementById('shortcuts')
        shortcuts.className = if show then '' else 'hidden'

    show_shortcuts_for_current_page: ()->
        switch page_state
            when PAGE_IDENT.MENU
                @set_shortcuts(
                    [
                        {
                            id: 'ESC'
                            text: 'NONE'
                        }
                        {
                            id: 'F1'
                            text: 'Help'
                        }
                        {
                            id: 'F2'
                            text: 'Edit profile'
                        }
                        {
                            id: 'F4'
                            text: 'Remove profile'
                        }
                    ]
                )
            when PAGE_IDENT.PROFILE_CONFIG
                @set_shortcuts([
                    {
                        id: 'ESC'
                        text: 'Return'
                    }
                    {
                        id: 'F2'
                        text: 'Save profile'
                    }
                ])
            when PAGE_IDENT.HELP
                @set_shortcuts([
                    {
                        id: 'ESC'
                        text: 'Return'
                    }
                ])
            else
                @set_shortcuts([])


    set_shortcuts: (data) ->
        shortcuts = document.getElementById('shortcuts')
        while shortcuts.hasChildNodes() then shortcuts.removeChild(shortcuts.firstChild)

        for item in data
            shortcut = _createNode('div', {
                class: 'shortcut'
                'data-id': item.id
            },
            [
                _createNode('span', {
                    class: 'key ' + item.id
                    text: item.id
                })
                _createNode('span', {
                    class: 'title'
                    text: item.text
                })
            ])

            shortcuts.appendChild(shortcut)
        return

    close_page: () ->

        console.log(page_stack)
        if(page_stack.size < 2)
            return

        this_page = page_stack.pop()
        last_page = page_stack[page_stack.length - 1]

        console.log(this_page, last_page)

        container = document.getElementById(this_page.id)
        container.className = 'page hidden';

        page_state = last_page.page_state
        @show_shortcuts_for_current_page()

    open_page: (id) ->

        page_state = null

        container = document.getElementById(id)

        if !container
            console.error('Page ID not found', id)
            return

        switch id
            when "menu"
                page_state = PAGE_IDENT.MENU;
            when "profile-configuration"
                page_state = PAGE_IDENT.PROFILE_CONFIG;
            when "help"
                page_state = PAGE_IDENT.HELP;
            else
                console.log('Undefined page', id)

        container.className = 'page top';

        page_stack.push({
            id: id
            page_state: page_state
        })

        @show_shortcuts_for_current_page()


window.Main = new Main()
