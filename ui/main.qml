import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Window 2.2
import QtQuick.Layouts
import Qt.labs.platform

import com.mycompany 1.0


ApplicationWindow {
    id: win1
    objectName: "rootwindow"
    width: 640
    height: 480
    visible: true
    title: qsTr("Helium Droplet Nano-plasma System (HDNap)")


    // Initalise Camera Class
    Camera {
        id: cameraDevice
        objectName: "cameraDeviceobj"
    }

    // Initalise DataCard
    DataCard {
        id: datacardDevice
        objectName: "datacardDeviceobj"
    }

    TabBar {
        id: topbar
        width: parent.width
        height: 20

        TabButton {
            id: controltab
            text: qsTr("Control Center")
        }

        TabButton {
            id: viewtab
            text: qsTr("View")
        }
    }

    StackLayout {
        currentIndex: topbar.currentIndex
        anchors.top: topbar.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        // Control Center
        ControlPanelWindow {
            id: cpwi
        }

        // View
        ViewWindow {
            id: vwwi
        }

    }
}
