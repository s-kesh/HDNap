import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Window 2.2
import QtCharts

import HDNap

Item {
    // Windows for undocking
    // View Camera
    Window {
        id: vwwindow
        objectName: "vwobj"
        visible: false
        width: 640
        height: 480
        title: qsTr("HDNap Camera Live View")

        onClosing: {
            vwpanel.state = "docked"
        }

        Rectangle {
            id: vtopbox
            anchors.fill: parent
        }
    }

    // View Avg Camera
    Window {
        id: vswindow
        objectName: "vsobj"
        visible: false
        width: 640
        height: 480
        title: qsTr("HDNap Camera Avg View")

        onClosing: {
            vspanel.state = "docked"
        }

        Rectangle {
            id: vstopbox
            anchors.fill: parent
        }
    }

    // View ToF
    Window {
        id: vwtwindow
        objectName: "vwtobj"
        visible: false
        width: 640
        height: 480
        title: qsTr("HDNap ToF Live View")

        onClosing: {
            vwtpanel.state = "docked"
        }

        Rectangle {
            id: vttopbox
            anchors.fill: parent
        }
    }

    // View Avg ToF
    Window {
        id: vstwindow
        objectName: "vstobj"
        visible: false
        width: 640
        height: 480
        title: qsTr("HDNap ToF Avg View")

        onClosing: {
            vstpanel.state = "docked"
        }

        Rectangle {
            id: vsttopbox
            anchors.fill: parent
        }
    }

    Window {
        id: hwwindow
        objectName: "hwobj"
        visible: false
        width: 640
        height: 480
        title: qsTr("HDNap Camera Hits")

        onClosing: {
            vhpanel.state = "docked"
        }

        Rectangle {
            id: htopbox
            anchors.fill: parent
        }
    }



    // Docking Station
    Rectangle {
        id: mainViewDockRect
        anchors.fill: parent
        border.width: 2

        property int gmargin: 10
        property real hunit: (height - 4 * gmargin ) / 20

        Rectangle {
            id: statusRect
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            width: parent.width - 2 * parent.gmargin
            height: parent.hunit
            border.width: 2
        }

        Rectangle {
            id: viewdock
            anchors.left: parent.left
            anchors.top: statusRect.bottom
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            height: parent.hunit * 9.5
            width: height //parent.width / 2 - 1.5 * parent.gmargin
            border.width: 2
        }

        Rectangle {
            id: viewsumdock
            anchors.left: parent.left
            anchors.top: viewdock.bottom
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            height: parent.hunit * 9.5
            width: height //parent.width / 2 - 1.5 * parent.gmargin
            border.width: 2
        }

        Rectangle {
            id: dviewdock
            anchors.left: viewdock.right
            anchors.top: statusRect.bottom
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            height: parent.hunit * 9.5
            width: 1.5 * height
            border.width: 2
        }

        Rectangle {
            id: dviewsumdock
            anchors.left: viewsumdock.right
            anchors.top: dviewdock.bottom
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            height: parent.hunit * 9.5
            width: 1.5 * height
            border.width: 2
        }

        Rectangle {
            id: hitviewdock
            anchors.left: dviewdock.right
            anchors.top: statusRect.bottom
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            anchors.right: parent.right
            anchors.rightMargin: parent.gmargin
            height: parent.hunit * 19
            border.width: 2
        }

    }


    // Live Image Rect
    Rectangle {
        id: vwpanel
        anchors.fill: parent
        border.width: 1
        state: "docked"

        ImageBuffer {
            id: imageview
            objectName: "imageviewobj"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: parent.height
        }

        states: [
            State {
                name: "undocked"
                ParentChange { target: vwpanel; parent: vtopbox }
            },
            State {
                name: "docked"
                ParentChange { target: vwpanel; parent: viewdock }
            }
        ]

        Button {
            onClicked: {
                if (vwpanel.state === "docked") {
                    vwpanel.state = "undocked";
                } else {
                    vwpanel.state = "docked";
                }
                vwwindow.visible = !vwwindow.visible;
            }
        }
    }

    // Avg Image Rect
    Rectangle {
        id: vspanel
        anchors.fill: parent
        border.width: 1
        state: "docked"

        ImageBuffer {
            objectName: "simageviewobj"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: parent.height
        }

        states: [
            State {
                name: "undocked"
                ParentChange { target: vspanel; parent: vstopbox }
            },
            State {
                name: "docked"
                ParentChange { target: vspanel; parent: viewsumdock }
            }
        ]

        Button {
            onClicked: {
                if (vspanel.state === "docked") {
                    vspanel.state = "undocked";
                } else {
                    vspanel.state = "docked";
                }
                vswindow.visible = !vswindow.visible;
            }
        }

        MouseArea {
            width: parent.width / 2
            height: parent.height / 2
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                cameraDevice.showSum = !cameraDevice.showSum;
                if (!cameraDevice.isAcquiring) {
                    cameraDevice.forceEmit();
                }
            }
        }
    }

    // Live ToF Chart
    Rectangle {
        id: vwtpanel
        anchors.fill: parent
        border.width: 1
        state: "docked"

        onParentChanged: {
            tofview.changeWindow();
        }

        Connections {
            target: datacardDevice
            function onResultReady() {
                datacardDevice.updateSeries(tofview.series(0));
            }
        }

        ZChartView {
            id: tofview
            objectName: "tofviewobj"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: parent.height
            animationOptions: ChartView.NoAnimation
            theme: ChartView.ChartThemeDark

            openGL: true
            openGLSupported: true

            xScaleZoom: 0
            yScaleZoom: 0
            xmin: 0
            xmax: 0.06
            ymin: 0
            ymax: 25
            title: "ToF"
        }

        states: [
            State {
                name: "undocked"
                ParentChange { target: vwtpanel; parent: vttopbox }
            },
            State {
                name: "docked"
                ParentChange { target: vwtpanel; parent: dviewdock }
            }
        ]

        Button {
            onClicked: {
                if (vwtpanel.state === "docked") {
                    vwtpanel.state = "undocked";
                } else {
                    vwtpanel.state = "docked";
                }
                vwtwindow.visible = !vwtwindow.visible;
            }
        }
    }

    // Avg ToF ChartView
    Rectangle {
        id: vstpanel
        anchors.fill: parent
        border.width: 1
        state: "docked"

        property bool flag: true

        onParentChanged: {
            tofsview.changeWindow();
        }

        Connections {
            target: datacardDevice
            function onResultReady() {
                datacardDevice.updateAvgSeries(vstpanel.flag,
                                               tofsview.series(0));
            }
        }

        ZChartView {
            id: tofsview
            objectName: "tofsviewobj"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: parent.height
            animationOptions: ChartView.NoAnimation
            theme: ChartView.ChartThemeDark

            openGL: true
            openGLSupported: true

            xScaleZoom: 0
            yScaleZoom: 0
            xmin: 0
            xmax: 0.06
            ymin: 0
            ymax: 25
            title: "Avg ToF"
        }

        states: [
            State {
                name: "undocked"
                ParentChange { target: vstpanel; parent: vsttopbox }
            },
            State {
                name: "docked"
                ParentChange { target: vstpanel; parent: dviewsumdock }
            }
        ]

        Button {
            onClicked: {
                if (vwtpanel.state === "docked") {
                    vstpanel.state = "undocked";
                } else {
                    vstpanel.state = "docked";
                }
                vstwindow.visible = !vstwindow.visible;
            }
        }

        MouseArea {
            width: parent.width / 2
            height: parent.height / 2
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                vstpanel.flag = !vstpanel.flag;
            }
        }
    }

    // Hit Buffer
    Rectangle {
        id: vhpanel
        anchors.fill: parent
        border.width: 1
        state: "docked"

        HitBuffer {
            id: hitview
            objectName: "hitviewobj"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: parent.height
        }

        states: [
            State {
                name: "undocked"
                ParentChange { target: vhpanel; parent: htopbox }
            },
            State {
                name: "docked"
                ParentChange { target: vhpanel; parent: hitviewdock }
            }
        ]

        Button {
            onClicked: {
                if (vhpanel.state === "docked") {
                    vhpanel.state = "undocked";
                } else {
                    vhpanel.state = "docked";
                }
                hwwindow.visible = !hwwindow.visible;
            }
        }
    }
}
