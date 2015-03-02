import QtQuick 2.0
import QtQuick.Controls 1.3

Rectangle {
    width: 100
    height: 62

    Text {
        id: title
        text: qsTr("Application has been loaded but some required plugins are missed:")
    }

    Component
    {
        id: columnComponent
        TableViewColumn{width: 100 }
    }

    ListModel {
        id: libraryModel
        ListElement {
            title: "A Masterpiece"
            author: "Gabriel"
        }
        ListElement {
            title: "Brilliance"
            author: "Jens"
        }
        ListElement {
            title: "Outstanding"
            author: "Frederik"
        }
    }

    TableView {
        id: plugins_table
        objectName: "plugins_table"
        width: parent.width - 20
        anchors.rightMargin: 10
        TableViewColumn {
            role: "title"
            title: "Title"
            width: 100
        }
        TableViewColumn {
            role: "author"
            title: "Author"
            width: 200
        }
        model: libraryModel
        function readValues(anArray) {
            var temp = []
            for(var index = 0; index < anArray.length; index++)
            {
                var str = anArray[index]
                temp.push(columnComponent.createObject(this, { "str": str }))
            }
            return temp
        }
        resources: readValues

    }
}

