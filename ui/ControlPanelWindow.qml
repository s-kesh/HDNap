import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Window 2.2

Item {

    // Windows for undocking
    // Control Panel Window
    Window {
        id: cpwindow
        objectName: "cpwobj"
        visible: false
        width: 640
        height: 480
        title: qsTr("HDNap Camera Control Panel")

        onClosing: {
            cpanel.state = "docked"
        }

        Rectangle {
            id: ctopbox
            anchors.fill: parent
        }
    }

    Window {
        id: dpwindow
        objectName: "dpwobj"
        visible: false
        width: 640
        height: 480
        title: qsTr("HDNap DataCard Control Panel")

        onClosing: {
            dpanel.state = "docked"
        }

        Rectangle {
            id: dtopbox
            anchors.fill: parent
        }
    }

    // Docking station
    Rectangle {
        id: mainContDockRect
        anchors.fill: parent
        border.width: 2
        property int gmargin: 10

        Rectangle {
            id: gfileBox
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            width: parent.width / 2 - 1.5 * parent.gmargin
            height: parent.height * 2 / 10 - 1.5 * parent.gmargin
            border.width: 2

            FileBox {
                id: filebox
            }
        }

        Rectangle {
            id: gstatusBox
            anchors.left: gfileBox.right
            anchors.top: parent.top
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            width: parent.width / 2 - 1.5 * parent.gmargin
            height: parent.height * 2 / 10 - 1.5 * parent.gmargin
            border.width: 2
            color: "green"
        }

        Rectangle {
            id: camcontroldock
            anchors.left: parent.left
            anchors.top: gfileBox.bottom
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            width: parent.width / 3 - 1.5 * parent.gmargin
            height: parent.height * 7 / 10 - 1.5 * parent.gmargin
            border.width: 2
        }

        Rectangle {
            id: datacardcontroldock
            anchors.left: camcontroldock.right
            anchors.top: gfileBox.bottom
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            width: parent.width / 3 - 1.5 * parent.gmargin
            height: parent.height * 7 / 10  - 1.5 * parent.gmargin
            border.width: 2
        }
    }

    // Indivisual Components

    // Top Control
    // Start Stop Buttons

    // FileName Control
    // No need to undock

    // Acquiris Control Panel
    Rectangle {
        id: dpanel
        anchors.fill: parent
        state: "docked"

        DatacardControl {
            id: datacardcontrol
            anchors.top: parent.top
            anchors.left: parent.left
            height: parent.height * 7 / 10
        }

        states: [
            State {
                name: "undocked"
                ParentChange { target: dpanel; parent: dtopbox }
            },
            State {
                name: "docked"
                ParentChange { target: dpanel; parent: datacardcontroldock }
            }
        ]

        Button {
            onClicked: {
                if (dpanel.state === "docked") {
                    dpanel.state = "undocked";
                } else {
                    dpanel.state = "docked";
                }

                dpwindow.visible = !dpwindow.visible;
            }
        }
    }

    // Camera Control Panel
    Rectangle {
        id: cpanel
        anchors.fill: parent
        state: "docked"

        CameraControl {
            id: cameracontrol
            anchors.top: parent.top
            anchors.left: parent.left
            height: parent.height * 7 / 10
        }

        states: [
            State {
                name: "undocked"
                ParentChange { target: cpanel; parent: ctopbox }
            },
            State {
                name: "docked"
                ParentChange { target: cpanel; parent: camcontroldock }
            }
        ]

        Button {
            onClicked: {
                if (cpanel.state === "docked") {
                    cpanel.state = "undocked";
                } else {
                    cpanel.state = "docked";
                }

                cpwindow.visible = !cpwindow.visible;
            }
        }
    }
}


