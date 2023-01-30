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

    property int ymin: 0
    property int ymax: 0
    property int ystep: 0
    property int runno: 0
    property bool running: false
    property string filename

    FileValidator {
        id: fvalidator
        isExist: false
    }

    // Initalise Camera Class
    Camera {
        id: cameraDevice
        objectName: "cameraDeviceobj"
        liverefreshrate: 30
        sumrefreshrate: 10
    }

    // Initalise DataCard
    DataCard {
        id: datacardDevice
        objectName: "datacardDeviceobj"
    }

    // Initalise Mirror Control
    Mirror {
        id: mirrors
        objectName: "mirrorDeviceobj"
    }


    function scanMirror() {
        // Function to get data at different mirror positions
        for (let i = ymin; i < ymax; i += ystep) {
            if (running) {
                // stop acquisition
                datacardDevice.stopacquiringData();
                cameraDevice.stopacquiringImages();
            }

            // move mirror
            mirrors.xyzpos[1] = i;
            mirrors.setPos();

            // Increase run no
            runno += 1;

            // start acquisition
            datacardDevice.acquiringData();
            cameraDevice.acquireImages()

            // wait till completed
        }
    }

    // Tab Bar
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

    // Tab Content
    StackLayout {
        id: slayout
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
            running: running
            run_no: runno
            filename: filename
        }

        // View
        ViewWindow {
            id: vwwi
        }

    }
}
