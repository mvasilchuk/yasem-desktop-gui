function Profile()
{

}

Profile.onLoad = function()
{
    var profileData = document.location.hash.substring(1).split(":");
    console.log(profileData);
    this.classId = profileData[0];
    this.profile = profileData[1] ? profileData[1].replace('.', '-') : null;

    console.log(this.classId + ' -> ' + this.profile);
    if(!this.profile )
    {
        this.profile = __GUI__.createProfile(this.classId, "{}");
    }
    else
        console.log('profile:' + this.profile);
    Profile.fillTable();
};

Profile.saveConfig = function()
{
    var items = document.getElementsByClassName('config-option');

    var data = [];
    for(var index=0; index < items.length; index++)
    {
        var item = items[index];
        var name = /^config-(.*?)$/.exec(item.id)[1];
        console.log(name);
        var value = -1;
        switch(item.tagName.toLowerCase())
        {
            case 'select': {
                var option = item.options[item.selectedIndex];
                value = option ? option.value : null;
                break;
            }
            case 'textarea': value = item.innerHTML; break;
            case 'input':
            default:
                value = item.value;
                break;
        }

        data.push({
            'name': name,
            'value': value
        });
    }

    //__GUI__.createProfile(this.$stbType, JSON.stringify(data));
    __GUI__.saveProfile(this.profile, JSON.stringify(data));
    opener.Profiles.showStbTypesList(false);
    opener.Profiles.loadProfiles();
    window.close();

    console.log();
};

Profile.fillTable = function()
{
    var config = JSON.parse(__GUI__.getProfileConfigOptions(this.profile));
    var config_table = document.getElementById('config-container');
    while(config_table.hasChildNodes()) config_table.removeChild(config_table.firstChild);

    for(var index=0; index < config.length; index++)
    {
        var conf = config[index];

        var elemTag;
        var options = [];

        console.log(conf  + typeof conf.value);

        switch(typeof conf.value)
        {
            case 'integer': {
                elemTag = 'input';
                break;
            }
            case 'boolean': {
                elemTag = 'select';
                options = [
                    {
                        name: 'true',
                        value: 'true'
                    },
                    {
                        name: 'false',
                        value: 'false'
                    }
                ];

                break;
            }
            case 'string':
            default: {
                elemTag = 'input';
                break;
            }
        }

        var tr = Profiles.createElement('tr',
            {
                'class': 'config-row'
            },
            [
                {
                    tag: 'td',
                    children: [
                        {
                            tag: 'label',
                            attrs: {
                                innerHTML: conf.title
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
                                value: conf.value,
                                'class': 'config-option',
                                id: 'config-' + conf.tag + '/' + conf.name,
                                options: elemTag == 'select' ? options: null
                            }
                        }
                    ]
                }
            ]);

        config_table.appendChild(tr);
    }
};



