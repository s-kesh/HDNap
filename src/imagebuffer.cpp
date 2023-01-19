#include "imagebuffer.h"
#include <iostream>
#include <QOpenGLFramebufferObjectFormat>
#include <QQuickOpenGLUtils>

ImageBuffer::ImageBuffer(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
{
    setFlag(QQuickItem::ItemHasContents);
}

QQuickFramebufferObject::Renderer *ImageBuffer::createRenderer() const
{
    return new ImageBufferRenderer();
}


const Texture &ImageBuffer::image1() const
{
    return m_image1;
}

const Texture &ImageBuffer::image2() const
{
    return m_image2;
}

const bool &ImageBuffer::firstImage() const
{
    return m_firstImage;
}

const bool &ImageBuffer::updateImage1() const
{
    return m_updateImage1;
}

void ImageBuffer::setUpdateImage1(const bool newflag)
{
    m_updateImage1 = newflag;
}

void ImageBuffer::updateImage(std::uint64_t index,
                              std::uint64_t width,
                            std::uint64_t height,
                            std::uint8_t *data)
{

    if (m_updateImage1) {
        m_image1.width = width;
        m_image1.height = height;
        m_image1.data = data;
    }
    else {
        m_image2.width = width;
        m_image2.height = height;
        m_image2.data = data;
    }

    if (index > 1) {
        m_firstImage = true;
    }
    else {
        m_firstImage = false;
    }
    update();
}

ImageBufferRenderer::ImageBufferRenderer()
{
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
    glUniform1i(glGetUniformLocation(m_shader, "tex1"), 0);

}

void ImageBufferRenderer::synchronize(QQuickFramebufferObject *item)
{
    ImageBuffer* obj = static_cast<ImageBuffer*>(item);
    Q_UNUSED(obj);

    // sync values ...
    if (obj->updateImage1())
        m_texture = obj->image1();
    else
        m_texture = obj->image2();

    obj->setUpdateImage1(~obj->updateImage1());

    if (obj->firstImage())
        rendertexture1 = true;
    else
        rendertexture1 = false;
}

void ImageBufferRenderer::render()
{

    // bind textures on corresponding texture unit
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, m_texture.ID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
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
}

QOpenGLFramebufferObject *ImageBufferRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setSamples(4);
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void ImageBufferRenderer::checkCompileErrors(GLuint shader, std::string type)
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

void ImageBufferRenderer::createShader()
{
    const char* vShaderCode =
        "#version 430 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "layout (location = 2) in float aColor;\n"
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
        "in float Color;\n"
        "\n"
        "// texture samplers\n"
        "uniform sampler2D tex1;\n"
        "uniform sampler2D tex2;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(tex1, TexCoord);\n"
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

void ImageBufferRenderer::createGeometry()
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
        1, 2, 3,  // second triangle

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

void ImageBufferRenderer::createTexture()
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
