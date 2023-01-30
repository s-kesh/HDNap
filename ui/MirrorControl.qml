import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts
import Qt.labs.settings 1.1



Item {
    id: mirrorcontrol
    anchors.fill: parent

    property int gmargin: 10
    property int munit: (height  - 7 * gmargin) / 12

    function sleep(milliseconds) {
      const date = Date.now();
      let currentDate = null;
      do {
        currentDate = Date.now();
      } while (currentDate - date < milliseconds);
    }


    Connections {
        target: maincontroller.mirror
        function onIsConnectedChanged() {
            var first = mirrors.isConnected[0];
            var second = mirrors.isConnected[1];
            var third = mirrors.isConnected[2];

            if(first) {
                serno0.color = "green";
            }
            else {
                serno0.color = "red";
            }

            if(second) {
                serno1.color = "green";
            }
            else {
                serno1.color = "red";
            }

            if(third) {
                serno2.color = "green";
            }
            else {
                serno2.color = "red";
            }
        }

        function onMotposChanged() {
            var mot1 = mirrors.motpos[0];
            var mot2 = mirrors.motpos[1];
            var mot3 = mirrors.motpos[2];

            mot1pos.dvalue = mot1;
            mot2pos.dvalue = mot2;
            mot3pos.dvalue = mot3;

        }

        function onMotdposChanged() {
            var mot1 = mirrors.motdpos[0];
            var mot2 = mirrors.motdpos[1];
            var mot3 = mirrors.motdpos[2];

            mot1dpos.dvalue = mot1;
            mot2dpos.dvalue = mot2;
            mot3dpos.dvalue = mot3;

        }

        function onXyzposChanged() {
            var x = mirrors.xyzpos[0];
            var y = mirrors.xyzpos[1];
            var z = mirrors.xyzpos[2];

            posx.dvalue = x;
            posy.dvalue = y;
            posz.dvalue = z;

        }

    }


    Rectangle {
        id: focsid
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: parent.gmargin
        anchors.leftMargin: parent.gmargin
        anchors.rightMargin: parent.gmargin
        height: parent.munit

        property int unit: (width - 5 * parent.gmargin)

        Text {
            id: foc
            anchors.left: parent.left
            anchors.leftMargin: gmargin


            text: mirrors.focus.toString()
        }

    }

    Row {
        id: serialnorow
        anchors.top: parent.top
        anchors.topMargin: 50
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20

        height: parent.height / 20
        spacing: 10

        Rectangle {
            id: focus
            border.width: 1
            width: parent.width / 8
            height: parent.height
            Column {
                anchors.fill: parent
                Text {
                    id: label1
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: parent.height / 2
                    text: qsTr("Focus")
                }

                TextInput {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: parent.height / 2
                    text: mirrors.focus.toString()
                    enabled: false
                    onTextChanged: {
                       mirrors.focus = parseFloat(text);
                    }
                }
            }
        }

        Rectangle {
            id: side
            border.width: 1
            width: parent.width / 8
            height: parent.height
            Column {
                anchors.fill: parent
                Text {
                    id: label2
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: parent.height / 2
                    text: qsTr("Triangle Side")
                }
                TextInput {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: parent.height / 2
                    text: mirrors.edge.toString()
                    enabled: false
                    onTextChanged: {
                        mirrors.edge = parseFloat(text);
                    }
                }
            }
       }

        Rectangle {
            id: serno0
            border.width: 1
            width: parent.width / 8
            height: parent.height
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: maincontroller.mirror.serialno[0]
            }
        }

        Rectangle {
            id: serno1
            border.width: 1
            width: parent.width / 8
            height: parent.height
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: maincontroller.mirror.serialno[1]
            }
        }

        Rectangle {
            id: serno2
            border.width: 1
            width: parent.width / 8
            height: parent.height
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: maincontroller.mirror.serialno[2]
            }
        }

        Button {
            id: connect
            width: parent.width / 8
            height: parent.height
            text: "Connect"
            onClicked: {
                maincontroller.mirror.connect()
            }
        }

        Button {
            id: disconnect
            width: parent.width / 8
            height: parent.height
            text : "Disconnect"
            onClicked: {
                maincontroller.mirror.disconnect()
            }
        }
    }


    /*
    Rectangle {
        id: getsetrect
        anchors.top: serialnorow.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 30
        width: parent.width  - 60
        height: parent.height / 20
        border.width: 1

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height - 20
            width: parent.width - 40
            spacing: 10

            Button {
                id: getbutton
                width: parent.width / 8
                height: parent.height
                text: "Get"
                onClicked: {
                    maincontroller.mirror.getPos()
                }
            }

            Button {
                id: homebutton
                width: parent.width / 8
                height: parent.height
                text: "Home"
                onClicked: {
                    maincontroller.mirror.settohome();
                }
            }

            Button {
                id: initbutton
                width: parent.width / 8
                height: parent.height
                text: "Intialize"
                onClicked: {
                    maincontroller.mirror.inimotpos()
                    sleep(4000);
                    maincontroller.mirror.getPos();
                }
            }

            SpinBox {
                id: inimot1
                width: parent.width / 8
                height: parent.height
                from: 0
                to: 500000
                value: maincontroller.mirror.modposini[0]
                editable: true
                onValueChanged: {
                    maincontroller.mirror.modposini[0] = inimot1.value;
                }
            }
            SpinBox {
                id: inimot2
                width: parent.width / 8
                height: parent.height
                from: 0
                to: 500000
                value: maincontroller.mirror.modposini[1]
                editable: true
                onValueChanged: {
                    maincontroller.mirror.modposini[1] = inimot2.value;
                }
            }
            SpinBox {
                id: inimot3
                width: parent.width / 8
                height: parent.height
                from: 0
                to: 500000
                value: maincontroller.mirror.modposini[2]
                editable: true
                onValueChanged: {
                    maincontroller.mirror.modposini[2] = inimot3.value;
                }
            }

        }
    }
    */

    /*
    PaddedRectangle {
        id: setmotors
        anchors.top: getsetrect.bottom
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 20
        width: parent.width / 4
        height: parent.height / 5
        border.width: 1
        padding: 20

        Column {
            id: setmotcolumn
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height - 3 * parent.padding
            width: parent.width - 3 * parent.padding
            spacing: 10

            Row {
                id: setmotrow
                width: parent.width
                height: (parent.height - 30) / 4

                Button {
                    id: setmotor
                    text: "Set Motors"
                    onClicked: {
                        maincontroller.mirror.setMotorPos();
                        sleep(4000);
                        maincontroller.mirror.getPos();
                    }
                }
            }

            Row {
                id: rowmot1
                width: parent.width
                height: (parent.height - 30) / 4
                spacing: 20

                Text { text: qsTr("Motor1") }

                DboxSlider {
                    id: mot1pos
                    width: parent.width / 3
                    height: parent.height
                    dvalue: maincontroller.mirror.motpos[0]
                    min: 0.0
                    max: 12
                    step: 0.01
                    decimal: 4
                    onDvalueChanged: {
                        maincontroller.mirror.motpos[0] = mot1pos.dvalue;
                    }
                }
            }

            Row {
                id: rowmot2
                width: parent.width
                height: (parent.height - 30) / 4
                spacing: 20

                Text { text: qsTr("Motor2") }

                DboxSlider {
                    id: mot2pos
                    width: parent.width / 3
                    height: parent.height
                    dvalue: maincontroller.mirror.motpos[1]
                    min: 0.0
                    max: 12
                    step: 0.01
                    decimal: 4
                    onDvalueChanged: {
                        maincontroller.mirror.motpos[1] = mot2pos.dvalue;
                    }
                }
            }

            Row {
                id: rowmot3
                width: parent.width
                height: (parent.height - 30) / 4
                spacing: 20

                Text { text: qsTr("Motor3") }

                DboxSlider {
                    id: mot3pos
                    width: parent.width / 2
                    height: parent.height
                    dvalue: maincontroller.mirror.motpos[2]
                    min: 0.0
                    max: 12
                    step: 0.01
                    decimal: 4
                    onDvalueChanged: {
                        maincontroller.mirror.motpos[2] = mot3pos.dvalue;
                    }
                }
            }
        }
    }
    */

    /*
    PaddedRectangle {
        id: setposs
        anchors.top: setmotors.bottom
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 20
        width: parent.width / 4
        height: parent.height / 5
        border.width: 1
        padding: 20

        Column {
            id: setposcolumn
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height - 3 * parent.padding
            width: parent.width - 3 * parent.padding
            spacing: 10

            Row {
                id: setposrow
                width: parent.width
                height: (parent.height - 30) / 4

                Button {
                    id: setpos
                    text: "Set Position"
                    onClicked: {
                        maincontroller.mirror.setPos();
                        sleep(4000);
                        maincontroller.mirror.getPos();
                    }
                }
            }

            Row {
                id: rowposx
                width: parent.width
                height: (parent.height - 30) / 4
                spacing: 20

                Text { text: qsTr("Pos X") }

                DboxSlider {
                    id: posx
                    width: parent.width / 2
                    height: parent.height
                    dvalue: maincontroller.mirror.xyzpos[0]
                    min: -20.0
                    max: 20.0
                    step: 0.01
                    decimal: 4
                    onDvalueChanged: {
                        maincontroller.mirror.xyzpos[0] = posx.dvalue;
                    }
                }
            }

            Row {
                id: rowposy
                width: parent.width
                height: (parent.height - 30) / 4
                spacing: 20

                Text { text: qsTr("Pos Y") }

                DboxSlider {
                    id: posy
                    width: parent.width / 2
                    height: parent.height
                    dvalue: maincontroller.mirror.xyzpos[1]
                    min: -20.0
                    max: 20.0
                    step: 0.01
                    decimal: 4
                    onDvalueChanged: {
                        maincontroller.mirror.xyzpos[1] = posy.dvalue;
                    }
                }
            }

            Row {
                id: rowposz
                width: parent.width
                height: (parent.height - 30) / 4
                spacing: 20

                Text { text: qsTr("Pos Z") }

                DboxSlider {
                    id: posz
                    width: parent.width / 2
                    height: parent.height
                    dvalue: maincontroller.mirror.xyzpos[2]
                    min: 70
                    max: 95
                    step: 0.01
                    decimal: 4
                    onDvalueChanged: {
                        maincontroller.mirror.xyzpos[2] = posz.dvalue;
                    }
                }
            }
        }
    }
    */

    /*
    PaddedRectangle {
        id: setdmotor
        anchors.top: setposs.bottom
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 20
        width: parent.width / 4
        height: parent.height / 5
        border.width: 1
        padding: 20

        Column {
            id: setmotdcolumn
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height - 3 * parent.padding
            width: parent.width - 3 * parent.padding
            spacing: 10

            Row {
                id: setmotdrow
                width: parent.width
                height: (parent.height - 30) / 4

                Button {
                    id: setmotord
                    text: "Set Motors (Device Unit)"
                    onClicked: {
                        maincontroller.mirror.setMotordPos();
                        sleep(4000);
                        maincontroller.mirror.getPos();
                    }
                }
            }

            Row {
                id: rowmotd1
                width: parent.width
                height: (parent.height - 30) / 4
                spacing: 20

                Text { text: qsTr("Motor1") }

                DboxSlider {
                    id: mot1dpos
                    width: parent.width / 2
                    height: parent.height
                    dvalue: maincontroller.mirror.motdpos[0]
                    min: 0
                    max: 500000
                    step: 1000
                    decimal: 0
                    onDvalueChanged: {
                        maincontroller.mirror.motdpos[0] = mot1dpos.dvalue;
                    }
                }
            }

            Row {
                id: rowmotd2
                width: parent.width
                height: (parent.height - 30) / 4
                spacing: 20

                Text { text: qsTr("Motor2") }

                DboxSlider {
                    id: mot2dpos
                    width: parent.width / 2
                    height: parent.height
                    dvalue: maincontroller.mirror.motdpos[1]
                    min: 0
                    max: 500000
                    step: 1000
                    decimal: 0
                    onDvalueChanged: {
                        maincontroller.mirror.motdpos[1] = mot2dpos.dvalue;
                    }
                }
            }

            Row {
                id: rowmotd3
                width: parent.width
                height: (parent.height - 30) / 4
                spacing: 20

                Text { text: qsTr("Motor3") }

                DboxSlider {
                    id: mot3dpos
                    width: parent.width / 2
                    height: parent.height
                    dvalue: maincontroller.mirror.motdpos[2]
                    min: 0
                    max: 500000
                    step: 1000
                    decimal: 0
                    onDvalueChanged: {
                        maincontroller.mirror.motdpos[2] = mot3dpos.dvalue;
                    }
                }
            }
        }
    }
    */
}
