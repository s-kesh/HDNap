//#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QXYSeries>
#include <QChartView>

#include "camera.h"
#include "imagebuffer.h"
#include "hitbuffer.h"
#include "datacard.h"
#include "qobject.h"
#include "qqml.h"
#include "qqmlcomponent.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<Camera>("com.mycompany", 1, 0, "Camera");
    qmlRegisterType<DataCard>("com.mycompany", 1, 0, "DataCard");

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/HDNap/ui/main.qml"_qs);

    QQmlComponent component(&engine, url);
    QObject *obj = component.create();

    Camera *cam = qobject_cast<Camera*>(obj->findChild<QObject *>("cameraDeviceobj"));
    DataCard *card = qobject_cast<DataCard*>(obj->findChild<QObject *>("datacardDeviceobj"));

    ImageBuffer *img = qobject_cast<ImageBuffer *>(obj->findChild<QObject *>("imageviewobj"));
    QObject::connect(cam, &Camera::resultReady,
                     img, &ImageBuffer::updateImage);



    ImageBuffer *aimg = qobject_cast<ImageBuffer *>(obj->findChild<QObject *>("simageviewobj"));
    QObject::connect(cam, &Camera::avgReady,
                     aimg, &ImageBuffer::updateImage);

    HitBuffer *hit = qobject_cast<HitBuffer *>(obj->findChild<QObject *>("hitviewobj"));
    QObject::connect(cam, &Camera::itisaHit,
                     hit, &HitBuffer::updateImage);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    QObject::connect(&engine, &QQmlApplicationEngine::quit, &QApplication::quit);

    return app.exec();
}
