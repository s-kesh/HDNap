import QtQuick 2.6
import QtQuick.Controls 2.5

Item {
    id: boxslider

//    property var mymodel

    required property string lname
    required property double dvalue
    required property double minimumValue
    required property double maximumValue
    required property double singleStep
    required property int decimalPlaces
    property int psize: 14


    property real unit: width / 10

    signal valChanged(real val)

    onDvalueChanged: {
        valChanged(dvalue);
    }

    Text {
        id: label
        anchors.left: parent.left
        anchors.leftMargin: 0.3 * unit
        anchors.verticalCenter: parent.verticalCenter

        width: 1.7 * unit
        text: lname
        font.pixelSize: psize
    }

    Rectangle {
        id: valuebox
        anchors.left: label.right
        anchors.leftMargin: unit
        anchors.verticalCenter: parent.verticalCenter


        width: 3 * unit

        Button {
            id: butminus
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter

            width: 0.5 * unit
            text: "-"
            onClicked: {
                dvalue -= singleStep
                textInput.text = dvalue.toFixed(decimalPlaces)
            }
        }
        TextInput {
            id: textInput
            anchors.left: butminus.right
            anchors.verticalCenter: parent.verticalCenter

            width: 2 * unit

            verticalAlignment: textInput.AlignVCenter
            horizontalAlignment: textInput.AlignHCenter

            text: dvalue.toFixed(decimalPlaces)

            font.pixelSize: psize

            validator: DoubleValidator {
                bottom: minimumValue
                top: maximumValue
            }
            onAccepted: {
                dvalue = parseFloat(textInput.text)
            }

            Keys.onUpPressed: {
                dvalue += singleStep
                textInput.text = dvalue.toFixed(decimalPlaces)
            }

            Keys.onDownPressed: {
                dvalue -= singleStep
                textInput.text = dvalue.toFixed(decimalPlaces)
            }
        }
        Button {
            id: butplus
            anchors.left: textInput.right
            anchors.verticalCenter: parent.verticalCenter

            width: 0.5 * unit
            text: "+"
            onClicked: {
                dvalue += singleStep
                textInput.text = dvalue.toFixed(decimalPlaces)
            }
        }

    }

    Slider {
        id: slider
        anchors.left: valuebox.right
        anchors.leftMargin: unit
        anchors.verticalCenter: parent.verticalCenter
        width: 3 * unit
        value: dvalue
        from: minimumValue
        to: maximumValue
        stepSize: singleStep

        onValueChanged: {
            dvalue = slider.value
            textInput.text = dvalue.toFixed(decimalPlaces)
        }
    }


}

