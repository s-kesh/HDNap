import QtQuick
import QtQuick.Controls
import Qt.labs.platform
import Qt.labs.settings 1.1

Item {
    id: filenamebox
    anchors.fill: parent
    property bool running
    property date currentDate: new Date()
    property string dateString
    property string cleanPath
    property string folderPath
    property int run_no: 0

    function substring(str) {
        return str.substring(0, 2);
    }

    Settings {
        id: filenamesettings
        fileName: "fnamesettings"
        property alias dirpath: filenamebox.cleanPath
        property alias data: prefix.text
        property alias runNo: runno.value
        property alias postfixstring: postfix.text
    }

    FolderDialog {
        id: folderDialog
        folder: StandardPaths.standardLocations(StandardPaths.DesktopLocation)[0]
        onAccepted: {
            var path = folderDialog.folder.toString();
            path = path.replace(/^(file:\/{2})/,"");
            cleanPath = decodeURIComponent(path);
        }
    }

    Rectangle {
        id: mainrect
        anchors.fill: parent
        border.width: 2
        property int gmargin: 10

        Row {
            id: row1
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            height: parent.height / 3 - 1.5 * parent.gmargin
            width: parent.width
            spacing: parent.gmargin

            Button {
                id: filebrowser
                width: parent.width / 3 - 1.5 * mainrect.gmargin
                height: parent.height - mainrect.gmargin
                text: "Browse"
                onClicked: {
                    folderDialog.open()
                }
            }

            Button {
                id: todaydate
                width: parent.width / 3 - 1.5 * mainrect.gmargin
                height: parent.height - mainrect.gmargin
                text: "get Date"
                onClicked: {
                    dateString = currentDate.toLocaleDateString(Qt.locale(), "yyyyMMdd");
                    prefix.text = dateString
                }
            }

            Button {
                id: increrunno
                width: parent.width / 3 - 1.5 * mainrect.gmargin
                height: parent.height - mainrect.gmargin
                text: "Run++"
                onClicked: {
                    runno.value = runno.value + 1;
                }
            }
        }

        Row {
            id: row2
            anchors.top: row1.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            height: parent.height / 3 - 1.5 * parent.gmargin
            width: parent.width
            spacing: parent.gmargin

            property real unit: (parent.width - 2 * parent.gmargin - 4 * parent.gmargin) / 10

            Rectangle {
                width: 5 * parent.unit
                height : parent.height

                TextField {
                    id: directoryname
                    anchors.fill: parent
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    text: cleanPath
                    wrapMode: TextInput.WrapAnywhere
                }
                border.width: 1
            }

            Rectangle {
                width: 2 * parent.unit
                height : parent.height
                TextField {
                    id: prefix
                    anchors.fill: parent
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    text: ""
                }
                border.width: 1
            }

            Rectangle {
                width: parent.unit
                height : parent.height
                SpinBox {
                    id: runno
                    width: parent.width
                    height: parent.height
                    value: 0
                }
                border.width: 1
            }

            Rectangle {
                width: 2 * parent.unit
                height : parent.height
                TextField {
                    id: postfix
                    anchors.fill: parent
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    text: "postfix"
                }
                border.width: 1
            }
        }

        Row {
            id: row3
            anchors.top: row2.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: parent.gmargin
            anchors.leftMargin: parent.gmargin
            height: parent.height / 3 - 1.5 * parent.gmargin
            width: parent.width
            spacing: parent.gmargin

            property real unit: (parent.width - 2 * parent.gmargin - 3 * parent.gmargin) / 10

            Rectangle {
                width: 7 * parent.unit
                height : parent.height
                border.width: 1

                TextField {
                    id:filename
                    anchors.fill: parent
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    text: cleanPath + "/" + prefix.text + "/run_" + runno.value.toString() + "_" + postfix.text
                    onTextChanged: {
                        folderPath = cleanPath + "/" + prefix.text;
//                        maincontroller.filename = filename.text;
                    }
                }
            }

            Button {
                id: startrun
                text: "Run"
                width: parent.unit
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
//                    maincontroller.createFolderPath(folderPath);
//                    if (maincontroller.isFilenameExist()) {
//                        console.log("File Already Exist");
//                    }
//                    else {
//                        came.startacquiring(true);
//                        running = true;
//                    }
                    cameraDevice.isSaving = true;
                    cameraDevice.acquireImages(1000, "/var/home/keshav/Phd/beamlines/MPIK/HDNap/Files/");
                }
            }

            Button {
                id: stopacq
                text: "Stop"
                width: parent.unit
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
//                    running = false;
//                    maincontroller.stopacquiring();
                    cameraDevice.stopacquiringImages();
                }
            }

            Button {
                id: testrun
                text: "Test"
                width: parent.unit
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
//                    maincontroller.counttof = 1000000;
//                    maincontroller.countvmi = 1000000;
//                    maincontroller.startacquiring(false);

                    cameraDevice.isSaving = false;
                    cameraDevice.acquireImages(10000000, "/var/home/keshav/Phd/beamlines/MPIK/HDNap/Files/");

                    datacardDevice.isSaving = false;
                    datacardDevice.acquireData(10000000, "/var/home/keshav/Phd/beamlines/MPIK/HDNap/Files/");
                }
            }
        }
    }
}
