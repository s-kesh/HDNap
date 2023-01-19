#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QOpenGLFunctions_4_3_Core>

struct Texture {
    unsigned int ID=0;
    std::uint64_t width;
    std::uint64_t height;
    std::uint8_t *data; //[TEXTURE_HEIGHT * TEXTURE_WIDTH];
};

class ImageItemRenderer: public QObject, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT

public:
    ~ImageItemRenderer();

    void setViewportSize(const QSize &size);
    void setWindow(QQuickWindow *window);
    void setTexture(std::uint64_t width,
                     std::uint64_t height,
                     std::uint8_t *data);

public slots:
    void init();
    void paint();

private:
    void checkCompileErrors(GLuint shader, std::string type);
    void createShader();
    void createGeometry();
    void createTexture();

private:
    QSize m_viewportSize;
    QQuickWindow *m_window = nullptr;

    unsigned int m_shader = 0;
    unsigned int m_VBO = 0;
    unsigned int m_VAO = 0;
    unsigned int m_EBO = 0;

    Texture m_texture;
};

class ImageItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Texture image READ image WRITE setImage NOTIFY imageChanged)

    QML_ELEMENT

public:
    ImageItem();

    const Texture &image() const;
    void setImage(const Texture &newImage);

signals:
    void imageChanged();

public slots:
    void sync();
    void cleanup();

    void updateImage(std::uint64_t width,
                     std::uint64_t height,
                     std::uint8_t *data);

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    void releaseResources() override;

    Texture m_image;

    ImageItemRenderer *m_renderer;
};

#endif // IMAGEITEM_H
