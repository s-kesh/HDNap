import QtQuick
import QtQuick.Controls 2.15

Item {
    id: datacardcontrol
    anchors.fill: parent

    property int gmargin: 10

    Rectangle {
        id: indicatorbox
        width: parent.width
        height: parent.height / 10
        border.width: 2

        property real unit: (width - 6 * parent.gmargin) / 5

        SpinBox {
            id: countBox
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: datacardcontrol.gmargin
            width: parent.unit
            height: parent.height / 3
            value: datacardDevice.count
            from: 0
            to: 1000000000
        }

        Rectangle {
            id: indexbox
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: countBox.right
            anchors.leftMargin: datacardcontrol.gmargin
            width: parent.unit
            height: parent.height / 3
            border.width: 2

            Text {
                id: indexLabel
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                text: datacardDevice.index
            }
        }

        Rectangle {
            id: connectedFlag
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: indexbox.right
            anchors.leftMargin: datacardcontrol.gmargin
            width: parent.unit
            height: parent.height / 3
            border.width: 2
            color: "red"

            Connections {
                target: datacardDevice
                function onIsConnectedChanged() {
                    if (datacardDevice.isConnected) {
                        connectedFlag.color = "green";
                    }
                    else {
                        connectedFlag.color = "red";
                    }
                }
            }
        }

        Rectangle {
            id: acquiringFlag
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: connectedFlag.right
            anchors.leftMargin: datacardcontrol.gmargin
            width: parent.unit
            height: parent.height / 3
            border.width: 2
            color: "red"

            Connections {
                target: datacardDevice
                function onIsAcquiringChanged() {
                    if (datacardDevice.isAcquiring) {
                        acquiringFlag.color = "green";
                    }
                    else {
                        acquiringFlag.color = "red";
                    }
                }
            }
        }

        Rectangle {
            id: savingFlag
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: acquiringFlag.right
            anchors.leftMargin: datacardcontrol.gmargin
            width: parent.unit
            height: parent.height / 3
            border.width: 2
            color: "red"

            Connections {
                target: datacardDevice
                function onIsSavingChanged() {
                    if (datacardDevice.isSaving) {
                        savingFlag.color = "green";
                    }
                    else {
                        savingFlag.color = "red";
                    }
                }
            }
        }
    }


    function getComponent(name) {
        if (name === "Serial No") {
            return serialnoComponent;
        }
        else if (name === "Channel No") {
            return channelnoComponent;
        }
        else if (name === "Trigger Source") {
            return triggersourceComponent;
        }
        else {
            return dboxsliderComponent;
        }
    }

    Rectangle {
        id: dcbox
        anchors.top: indicatorbox.bottom
        anchors.topMargin: gmargin
        width: parent.width
        height: parent.height * 7 / 10
        border.width: 2

        ListView {
            id: dcListView
            anchors.top: parent.top
            anchors.topMargin: gmargin
            anchors.bottom: parent.bottom
            anchors.bottomMargin: gmargin
            anchors.left: parent.left
            anchors.leftMargin: gmargin
            anchors.right: parent.right
            anchors.rightMargin: gmargin

            model: datacardDevice.settings

            delegate: Item {
                width: parent.width
                height: 40
                Loader {
                    id: itemLoader

                    property string name: model.name
                    property var value: model.value
                    property real min: model.min
                    property real max: model.max
                    property real step: model.step
                    property int decimal: model.decimal

                    sourceComponent: getComponent(name)

                    onLoaded: {
                        item.lname = name;
                        item.dvalue = value;
                        item.minimumValue = min;
                        item.maximumValue = max;
                        item.singleStep = step;
                        item.decimalPlaces = decimal;
                    }

                    onValueChanged: {
                        item.dvalue = value;
                    }
                    onMinChanged: {
                        item.minimumValue = min;
                    }
                    onMaxChanged: {
                        item.maximumValue = max;
                    }
                }

                Connections {
                    target: itemLoader.item
                    function onDvalueChanged() {
                        model.value = target.dvalue;
                    }
                }
            }

            Component {
                id: serialnoComponent
                Item {
                    width: dcListView.width
                    property string lname
                    property string dvalue
                    property double minimumValue
                    property double maximumValue
                    property double singleStep
                    property int decimalPlaces

                    signal valChanged(string val)
                    Text {
                        id: label
                        anchors.left: parent.left
                        anchors.leftMargin: 0.3 * parent.width / 10
                        width: 1.7 * parent.width / 10
                        text: lname
                        font.pixelSize: 14
                    }

                    Text {
                        anchors.left: label.right
                        anchors.leftMargin: parent.width / 10
                        width: 3 * parent.width / 10
                        text: dvalue
                        font.pixelSize: 14
                    }
                }
            }

            Component {
                id: channelnoComponent
                Item {
                    width: dcListView.width
                    property string lname
                    property int dvalue
                    property double minimumValue
                    property double maximumValue
                    property double singleStep
                    property int decimalPlaces

                    signal valChanged(int val)

                    Text {
                        id: label
                        anchors.left: parent.left
                        anchors.leftMargin: 0.3 * parent.width / 10
                        width: 1.7 * parent.width / 10
                        text: lname
                        font.pixelSize: 14
                    }

                    ComboBox {
                        anchors.left: label.right
                        anchors.leftMargin: parent.width / 10
                        width: 3 * parent.width / 10
                        model: ["1", "2"]
                        currentIndex: dvalue
                        onCurrentIndexChanged: {
                            dvalue = currentIndex
                        }
                    }
                }
            }

            Component {
                id: triggersourceComponent
                Item {
                    width: dcListView.width
                    property string lname
                    property int dvalue
                    property double minimumValue
                    property double maximumValue
                    property double singleStep
                    property int decimalPlaces

                    signal valChanged(int val)

                    Text {
                        id: label
                        anchors.left: parent.left
                        anchors.leftMargin: 0.3 * parent.width / 10
                        width: 1.7 * parent.width / 10
                        text: lname
                        font.pixelSize: 14
                    }

                    ComboBox {
                        anchors.left: label.right
                        anchors.leftMargin: parent.width / 10
                        width: 3 * parent.width / 10
                        model: ["Internal 1",
                            "Internal 2",
                            "External 1",
                            "Imediate",
                            "Software",
                            "Self Trigger"]
                        currentIndex: dvalue
                        onCurrentIndexChanged: {
                            dvalue = currentIndex;
                        }
                    }
                }
            }

            Component {
                id: dboxsliderComponent

                DboxSlider {
                    width: dcListView.width

                    lname: name
                    dvalue: value
                    minimumValue: min
                    maximumValue: max
                    singleStep: step
                    decimalPlaces: decimal

                }

            }
        }
    }

    Rectangle {
        id: getsetbox
        anchors.bottom: parent.bottom
        anchors.bottomMargin: gmargin
        width: parent.width
        anchors.top: dcbox.bottom
        anchors.topMargin: gmargin
        border.width: 2

        Button {
            id: bconnect
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: gmargin
            width: 1.0 * parent.width / 4 - 1.5 * gmargin
            height: parent.height / 3
            text: "Connect"
            onClicked: {
                console.log("Clicked");
                datacardDevice.startsession();
            }
        }
        Button {
            id: bget
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: bconnect.right
            anchors.leftMargin: gmargin
            width: 1.0 * parent.width / 4 - 1.5 * gmargin
            height: parent.height / 3
            text: "Get"
            onClicked: {
                console.log("Get");
                datacardDevice.getProperties();
            }
        }
        Button {
            id: bset
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: bget.right
            anchors.leftMargin: gmargin
            width: 1.0 * parent.width / 4 - 1.5 * gmargin
            height: parent.height / 3
            text: "Set"
            onClicked: {
                console.log("Set");
                datacardDevice.setProperties();
            }
        }

        Button {
            id: bdisconnect
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: bset.right
            anchors.leftMargin: gmargin
            width: 1.0 * parent.width / 4 - 1.5 * gmargin
            height: parent.height / 3
            text: "Disconnect"
            onClicked: {
                datacardDevice.stopsession();
            }
        }

    }
}