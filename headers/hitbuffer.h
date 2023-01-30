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
//    Q_PROPERTY(int row READ row WRITE setRow NOTIFY rowChanged)
//    Q_PROPERTY(int col READ col WRITE setCol NOTIFY colChanged)
    QML_ELEMENT

public:
    explicit HitBuffer(QQuickItem *parent = nullptr);
    Renderer *createRenderer() const override;
    const std::array<Textureh, ROW * COL> &images() const;
    const bool &firstImage() const;

//    int row() const;
//    void setRow(int newRow);

//    int col() const;
//    void setCol(int newCol);

public slots:
    void updateImage(std::uint64_t index,
                     std::uint64_t width,
                     std::uint64_t height,
                     std::uint8_t *data);

//signals:

//    void rowChanged();

//    void colChanged();

private:
    std::array<Textureh, ROW * COL> m_images;
    bool m_firstImage = false;
//    int m_row = ROW;
//    int m_col = COL;
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

//    int row() const;
//    void setRow(int newRow);

//    int col() const;
//    void setCol(int newCol);

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
//    int m_row = ROW;
//    int m_col = COL;
};


#endif // HITBUFFER_H
