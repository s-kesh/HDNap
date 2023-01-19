#ifndef HITBUFFER_H
#define HITBUFFER_H
#define ROW 2
#define COL 3


#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_3_Core>
#include <array>

struct Textureh {
    unsigned int ID;
    std::uint64_t width;
    std::uint64_t height;
    std::uint8_t *data;
};

class HitBuffer : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit HitBuffer(QQuickItem *parent = nullptr);
    Renderer *createRenderer() const override;
    const std::array<Textureh, ROW * COL> &images() const;
    const bool &firstImage() const;

public slots:
    void updateImage(std::uint64_t index,
                     std::uint64_t width,
                     std::uint64_t height,
                     std::uint8_t *data);

signals:

private:
    std::array<Textureh, ROW * COL> m_images;
    bool m_firstImage = false;
};

class HitBufferRenderer
        : public QQuickFramebufferObject::Renderer,
          protected QOpenGLFunctions_4_3_Core
{
public:
    HitBufferRenderer();

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

    std::array<Textureh, ROW * COL> m_textures;
    bool rendertexture1 = false;
};


#endif // HITBUFFER_H
