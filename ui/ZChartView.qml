import QtQuick 2.6
import QtCharts
import QtQuick.Controls 2.15

//Item {
//    anchors.fill: parent
//    id: zcview

//    required property int xScaleZoom
//    required property int yScaleZoom
//    required property int xmin
//    required property int xmax
//    required property int ymin
//    required property int ymax
//    required property bool openGL
//    required property string chartname
//    required property string seriesname


ChartView {
    id: zcview
//    objectName: "zcviewobj"
//    anchors.verticalCenter: parent.verticalCenter
//    anchors.horizontalCenter: parent.horizontalCenter
//    width: parent.width
//    height: parent.height
//    animationOptions: ChartView.NoAnimation
//    theme: ChartView.ChartThemeDark

    required property bool openGL
    required property bool openGLSupported

    required property int xScaleZoom
    required property int yScaleZoom
    required property real xmin
    required property real xmax
    required property real ymin
    required property real ymax
    required property string title

    onOpenGLChanged: {
        if (openGLSupported) {
            series("ToF").useOpenGL = openGL;
        }
    }

    Component.onCompleted: {
        if (!series("ToF").useOpenGL) {
            openGLSupported = false
            openGL = false
        }
    }

    function changeWindow() {
        zcview.removeAllSeries();
        var series
        series = zcview.createSeries(ChartView.SeriesTypeLine, zcview.title,
                                             axisx, axisy);
        series.useOpenGL = zcview.openGL
    }

    ValueAxis {
        id: axisx
        min: zcview.xmin
        max: zcview.xmax
    }

    ValueAxis {
        id: axisy
        min: zcview.ymin
        max: zcview.ymax
    }

    LineSeries {
        id: tofseries
        objectName: "tofseriesobj"
        name: zcview.title
        axisX: axisx
        axisY: axisy
        useOpenGL: zcview.openGL
    }

    Rectangle {
        id: recZoom
        border.color: "blue"
        border.width: 1
        color: "blue"
        opacity: 0.3
        visible: false
        transform: Scale {
            origin.x: 0;
            origin.y: 0;
            xScale: zcview.xScaleZoom;
            yScale: zcview.yScaleZoom;
        }
    }

    MouseArea {
        id: livetofmouse
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        acceptedButtons: Qt.AllButtons
        onPressed: {
            recZoom.x = mouseX;
            recZoom.y = mouseY;
            recZoom.visible = true;
        }
        onMouseXChanged: {
            if (mouseX - recZoom.x >= 0) {
                zcview.xScaleZoom = 1;
                recZoom.width = mouseX - recZoom.x;
            }
            else {
                zcview.xScaleZoom = -1;
                recZoom.width = recZoom.x - mouseX
            }
        }
        onMouseYChanged: {
            if (mouseY - recZoom.y >= 0) {
                zcview.yScaleZoom = 1;
                recZoom.height = mouseY - recZoom.y;
            }
            else {
                zcview.yScaleZoom = -1;
                recZoom.height = recZoom.y - mouseY
            }
        }
        onReleased: {
            var x = (mouseX >= recZoom.x) ? recZoom.x : mouseX
            var y = (mouseY >= recZoom.y) ? recZoom.y : mouseY
            zcview.zoomIn(Qt.rect(x, y, recZoom.width, recZoom.height));
            recZoom.visible = false;
        }

        onDoubleClicked: {
            zcview.zoomReset();
        }
    }
}

