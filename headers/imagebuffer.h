#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_3_Core>

struct Texture {
    unsigned int ID;
    std::uint64_t width;
    std::uint64_t height;
    std::uint8_t *data;
};

class ImageBuffer : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit ImageBuffer(QQuickItem *parent = nullptr);
    Renderer *createRenderer() const override;

    const Texture &image1() const;
    const Texture &image2() const;

    const bool &firstImage() const;
    const bool &updateImage1() const;
    void setUpdateImage1(const bool newflag);

public slots:
    void updateImage(std::uint64_t index,
                     std::uint64_t width,
                     std::uint64_t height,
                     std::uint8_t *data);

signals:
    void imageChanged();

private:
    Texture m_image1;
    Texture m_image2;
    bool m_updateImage1 = true;
    bool m_firstImage = false;
};

class ImageBufferRenderer
        : public QQuickFramebufferObject::Renderer,
          protected QOpenGLFunctions_4_3_Core
{
public:
    ImageBufferRenderer();

    void synchronize(QQuickFramebufferObject *item) override;
    void render() override;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

private:
    void checkCompileErrors(GLuint shader, std::string type);
    void createShader();
    void createGeometry();
    void createTexture();

private:
    unsigned int m_shader = 0;
    unsigned int m_VBO = 0;
    unsigned int m_VAO = 0;
    unsigned int m_EBO = 0;

    Texture m_texture;
    bool rendertexture1 = false;
};

#endif // IMAGEBUFFER_H
