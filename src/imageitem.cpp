#include "imageitem.h"

#include <QtQuick/qquickwindow.h>
#include <QOpenGLContext>
#include <QtCore/QRunnable>
#include <QQuickOpenGLUtils>

#include <iostream>

ImageItem::ImageItem()
    :m_renderer(nullptr)
{
    setFlag(QQuickItem::ItemHasContents, true);
    connect(this, &QQuickItem::windowChanged, this, &ImageItem::handleWindowChanged);
}

const Texture &ImageItem::image() const
{
    return m_image;
}

void ImageItem::setImage(const Texture &newImage)
{
    m_image = newImage;
    emit imageChanged();
}

void ImageItem::sync()
{
    if (!m_renderer) {
        m_renderer = new ImageItemRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &ImageItemRenderer::init, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, m_renderer, &ImageItemRenderer::paint, Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setTexture(m_image.width,
                           m_image.height,
                           m_image.data);
    m_renderer->setWindow(window());
}

void ImageItem::updateImage(uint64_t width,
                            uint64_t height,
                            uint8_t *data)
{
    m_image.width = width;
    m_image.height = height;
    m_image.data = data;
    emit imageChanged();

    update();
//    if (window())
//        window()->update();
}

void ImageItem::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &ImageItem::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &ImageItem::cleanup, Qt::DirectConnection);
        // Ensure we start with cleared to black. The squircle's blend mode relies on this.
        win->setColor(Qt::black);
    }
}

void ImageItem::cleanup()
{
    delete m_renderer;
    m_renderer = nullptr;
}

class CleanupJob : public QRunnable
{
public:
    CleanupJob(ImageItemRenderer *renderer) : m_renderer(renderer) { }
    void run() override { delete m_renderer; }
private:
    ImageItemRenderer *m_renderer;
};

void ImageItem::releaseResources()
{
    window()->scheduleRenderJob(new CleanupJob(m_renderer), QQuickWindow::BeforeSynchronizingStage);
    m_renderer = nullptr;
}

ImageItemRenderer::~ImageItemRenderer()
{
//    delete m_program;
}

void ImageItemRenderer::setViewportSize(const QSize &size) { m_viewportSize = size; }

void ImageItemRenderer::setWindow(QQuickWindow *window) { m_window = window; }

void ImageItemRenderer::setTexture(uint64_t width, uint64_t height, uint8_t *data)
{
    m_texture.width = width;
    m_texture.height = height;
    m_texture.data = data;
}

void ImageItemRenderer::init()
{
    QSGRendererInterface *rif = m_window->rendererInterface();
    Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL);

    initializeOpenGLFunctions();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // create Shader
    createShader();

    // create Geometry
    createGeometry();

    // createTexture
    createTexture();

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    glUseProgram(m_shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(m_shader, "tex"), 0);
}

void ImageItemRenderer::paint()
{
    m_window->beginExternalCommands();
    // bind textures on corresponding texture unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture.ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
                 m_texture.width, m_texture.height,
                 0,  GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 m_texture.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use Shader Program
    glUseProgram(m_shader);

    // Bind VertexArrays
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    // Draw Call
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    QQuickOpenGLUtils::resetOpenGLState();

    m_window->endExternalCommands();
}

void ImageItemRenderer::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if(type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void ImageItemRenderer::createShader()
{
    const char* vShaderCode =
        "#version 430 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "\n"
        "out vec2 TexCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos, 1.0);\n"
        "    TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
        "}\n";

    const char *fShaderCode =
        "#version 430 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "in vec2 TexCoord;\n"
        "\n"
        "// texture samplers\n"
        "uniform sampler2D tex;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(tex, TexCoord);\n"
        "}\n";

    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    m_shader = glCreateProgram();
    glAttachShader(m_shader, vertex);
    glAttachShader(m_shader, fragment);
    glLinkProgram(m_shader);
    checkCompileErrors(m_shader, "PROGRAM");
    // delete the shaders as they're linked into our program now
    // and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void ImageItemRenderer::createGeometry()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void ImageItemRenderer::createTexture()
{
    // Texture 1
    glGenTextures(1, &m_texture.ID);
    glBindTexture(GL_TEXTURE_2D, m_texture.ID);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

