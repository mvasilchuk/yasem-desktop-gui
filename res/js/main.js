
function Main()
{

}

Main.tr = function(name)
{
    return this.translations[name] || name;
};

Main.createElement = function(tag, attrs, children)
{
    var elem = document.createElement(tag);

    attrs = attrs || {};
    for(var id in attrs)
    {
        if(attrs[id] && attrs.hasOwnProperty(id))
        {
            switch(id)
            {
                case    'innerHTML':       elem.innerHTML  = attrs[id]; break;
                case    'onclick':         elem.onclick    = attrs[id]; break;
                case    'options': {
                    attrs[id].forEach(function(item)
                    {
                        elem.appendChild(
                            Main.createElement('option', {
                                'innerHTML': item.name,
                                'value': item.value
                            })
                        );
                    });
                    break;

                }
                default:
                {
                    if(id[0] == '$')
                        elem[id] = attrs[id];
                    else
                        elem.setAttribute(id, attrs[id]);
                } break;
            }
        }
    }

    children = children || [];
    for(var index=0; index < children.length; index++)
    {
        var child = children[index];
        elem.appendChild(Main.createElement(child.tag, child.attrs, child.children));
    }
    return elem;
};

Main.applyTranslations = function()
{
    var elements = document.getElementsByClassName('tr');
    for(var index = 0; index < elements.length; index++)
    {
        var el = elements[index];
        if(el.value)                el.value        = this.tr(el.value);
        else if(el.innerHTML)       el.innerHTML    = this.tr(el.innerHTML);
        else if(el.innerText)       el.innerText    = this.tr(el.innerText);
        else                        console.error('no value found for ' + el);
    }
};

Main.onLoad = function()
{

    Main.loadProfiles();

    this.translations = __GUI__.getTranslations();
    this.applyTranslations();
};

Main.loadProfiles = function()
{
    var profiles = JSON.parse(__GUI__.getProfilesInfoJson(true));
    console.log(profiles);
    var profileBlocks = document.getElementById('profile-blocks');
    while(profileBlocks.hasChildNodes()) profileBlocks.removeChild(profileBlocks.firstChild);

    for(var index=0; index < profiles.length; index++)
    {
        var profile = /** @type Profile */ profiles[index];
        var child  = this.createProfileBlock(profile);
        profileBlocks.appendChild(child);

        if(index == 0) child.focus();
    }
};


Main.createProfileBlock = function(/** @type Profile */profile)
{
    var children = [
        {
            tag:    'img',
            attrs:  {
                'class':  'profile-image',
                'src': profile.image,
                'onclick': function(e) {
                    //window.open('profile_config.html#' + profile.classId + ":" + profile.id.replace('-', '.'));
                    //e.preventDefault();
                    //return true;
                }
            }
        },
        {
            tag:    'div',
            attrs:  {
                'innerHTML': profile.name,
                'class': 'profile-name ellipsis'
            }
        }
    ];

    if(profile.id != -1)
    {
        children.push({
            tag:    'div',
            attrs:  {
                'innerHTML': profile.classId,
                'class': 'profile-classId ellipsis'
            }
        });

        children.push({
            tag: 'div',
            attrs: {
                'innerHTML': profile.url,
                'class': 'profile-url ellipsis'
            }
        });
    }

    return Main.createElement('a',{
        'id':       'profile-' + profile.id,
        'class':    'profile-block',
        'href':     'javascript:void(0)',
        'onclick': function()
        {
            console.log(profile.id);
            if(profile.id == -1)
            {
                Main.showStbTypesList(true);
                return true;
            }
            else
            {
                __GUI__.loadProfile(profile.id);
                return true;
            }
        }
    }, children);
};

Main.fillStbTypes = function()
{
    var blocks = document.getElementById('new-profile-blocks');
    while(blocks.hasChildNodes())   blocks.removeChild(blocks.firstChild);

    var stbTypes = JSON.parse(__GUI__.getStbTypes());

    var createProfile = function()
    {
        window.open('profile_config.html#' + this.$stbType + ":");
    };

    for(var index = 0; index < stbTypes.length; index++)
    {
        var link = Main.createElement('a', {
            'class':    'stb-type-block',
            'href':       'javascript:void(0)',
            '$stbType':   stbTypes[index].classId,
            'onclick':    createProfile
        }, [
            {
                tag:    'div',
                attrs:  {
                    'class': 'stb-type-title',
                    innerHTML: stbTypes[index].name
                }
            }
        ]);

        blocks.appendChild(link);
    }

    return blocks;
};

Main.removeFromFocusChain = function(className)
{
    var elements = document.getElementsByClassName(className);
    for(var index=0; index < elements.length; index++)
        elements[index].tabIndex = -1;
};

Main.addToFocusChain = function(className, start)
{
    start = start || 1;

    var elements = document.getElementsByClassName(className);
    for(var index=0; index < elements.length; index++)
    {
        elements[index].tabIndex = index + start;

        if(index == 0)
            elements[index].focus();
    }

};

Main.showStbTypesList = function(show)
{
    var cont = document.getElementById('new-profile-container');
    cont.style.display = show ? 'block': 'none';

    if(show)
    {
        this.fillStbTypes();
        this.removeFromFocusChain('profile-block');
    }
    else
    {
        this.addToFocusChain('profile-block');
    }
};

Main.onKeyPress = function(/** KeyEvent */event)
{
    switch(event.keyCode)
    {
        case 27: // ESC
        {
            Main.showStbTypesList(false);
            break;
        }
    }
};
