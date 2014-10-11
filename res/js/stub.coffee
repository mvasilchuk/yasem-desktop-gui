if !__GUI__?
    ###*
    *STUB method definitions for IDE hints.
    *###

    class __GUI__
        makeJsonMenu: () ->
            console.log('makeJsonMenu')
            return JSON.stringify({
                'default': 'menu-menu',
                'current': 'main-menu',
                'current_item_id': '',
                'profile_id': '',
                'profile': '',
                'items':  {
                    'main-menu': {
                        count: 11
                        items: {
                            'id-0':  {
                                image: '',
                                title: 'ITEM 0',
                                type: MENU_TYPE.PARENT_MENU,
                                submenu: 'submenu-1'
                            },
                            'id-1': {
                                image: '',
                                title: 'ITEM 1',
                                type: MENU_TYPE.PROFILE,
                                'menu_type': 'stb-profile'
                            },
                            'id-2': {
                                image: '',
                                title: 'ITEM 2',
                                type: MENU_TYPE.PROFILE
                            },
                            'id-3': {
                                image: '',
                                title: 'ITEM 3',
                                type: MENU_TYPE.PROFILE
                            },
                            'id-4': {
                                image: '',
                                title: 'ITEM 4',
                                type: MENU_TYPE.PROFILE
                            },
                            'id-5': {
                                image: '',
                                title: 'ITEM 5',
                                type: MENU_TYPE.PROFILE
                            },
                            'id-6': {
                                image: '',
                                title: 'ITEM 6',
                                type: MENU_TYPE.PROFILE
                            },
                            'id-7': {
                                image: '',
                                title: 'ITEM 7',
                                type: MENU_TYPE.PROFILE
                            },
                            'id-8': {
                                image: '',
                                title: 'ITEM 8',
                                type: MENU_TYPE.PROFILE
                            },
                            'id-9': {
                                image: '',
                                title: 'ITEM 9',
                                type: MENU_TYPE.PROFILE
                            },
                            'id-10': {
                                image: '',
                                title: 'ITEM 10',
                                type: MENU_TYPE.PROFILE
                            }
                        }

                    },
                    'submenu-1': {
                        items:  {
                            'sub-id-0':  {
                                image: ''
                                title: 'submenu-0'
                                type: MENU_TYPE.PROFILE
                                class: 'mag'
                            }
                        }
                    }
                }
            });

        loadProfile: (id) ->
            console.log('Loading profile ' + id)
        createProfile: (classId, submodel, data) ->
            console.log('Creating profile ' + classId + "->" + submodel + " with data " + JSON.stringify(data))
            return "random-profile-id"
        saveProfile: (id, data) ->
            console.log('Saving profile ' + id + " with data " + JSON.stringify(data))
        removeProfile: (id) ->
            console.log('Removing profile ' + id)

        getProfileConfigOptions: (id) ->
            console.log('Loading profile ptions ' + id)
            return JSON.stringify([
                {
                    'name' : 'test-config-1',
                    'title': 'title-1',
                    'value' : 'value-1',
                    'comment' : 'comment-1',
                    'type'  : CONFIG_ITEM_TYPES.STRING
                },
                {
                    'name' : 'test-config-2',
                    'title': 'title-2',
                    'value' : true,
                    'comment' : 'comment-2',
                    'type'  : CONFIG_ITEM_TYPES.BOOL
                },
                {
                    'name' : 'test-config-3',
                    'title': 'title-3',
                    'value' : 100,
                    'comment' : 'comment-3',
                    'type'  : CONFIG_ITEM_TYPES.INT
                },
                {
                    'name' : 'select-name-4',
                    'title' : 'select-title-4',
                    'value' : 2,
                    'comment' : 'select-comment-4',
                    'type'  : CONFIG_ITEM_TYPES.VARIANT
                    'options' : [
                        {
                            'title' : 0,
                            'value' : 'text',

                        },
                        {
                            'title' : 1,
                            'value' : 1,

                        },
                        {
                            'title' : 2,
                            'value' : 2,

                        },
                        {
                            'title' : 3,
                            'value' : 3,

                        }
                    ]
                }
            ]);

        getAppInfo: () ->
            return JSON.stringify({
                name: "yasem"
                version: 'version 0.1'
                copyright: 'Copyright 2014 by Maxim Vasilchuk'
            });

    window.__GUI__ = new __GUI__()