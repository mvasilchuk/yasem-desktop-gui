import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2

Rectangle {
    width: 200
    height: 180

    ColumnLayout {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            horizontalAlignment: Text.AlignHCenter
            id: title
            text: qsTr("Statistics")
            font.pointSize: 16
            font.bold: true
        }

        Text {
            id: network_title
            text: qsTr("Network statistics:")
            font.pointSize: 12
        }

        GridLayout {
            id: grid
            columns: 2

            Text {
                id: total_requests_title
                text: qsTr("Total requests:")
            }
            Text {
                id: total_requests_value
                text: network_total_count
            }
            Text {
                id: successful_requests_title
                text: qsTr("Successful requests:")
            }
            Text {
                id: successful_requests_value
                text: network_successful_count
            }
            Text {
                id: failed_requests_title
                text: qsTr("Failed requests:")
            }
            Text {
                id: failed_requests_value
                text: network_failed_count
            }
            Text {
                id: slow_requests_title
                text: qsTr("Slow requests:")
            }
            Text {
                id: slow_requests_value
                text: network_slow_count
            }
            Text {
                id: pending_requests_title
                text: qsTr("Pending requests:")
            }
            Text {
                id: pending_requests_value
                text: network_pending_count
            }

        }
    }

}

