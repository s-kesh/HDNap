#include "hitbuffer.h"
#include <iostream>
#include <QOpenGLFramebufferObjectFormat>
#include <QQuickOpenGLUtils>

std::vector<float> find_vertices(int rows, int cols) {
    std::vector<float> vertices;
    float index = 0;
    float rstepsize = 2.0 / rows;
    float cstepsize = 2.0 / cols;
    double i = -1;
    double j = -1;
    for (i = -1; i < 1; i += rstepsize) {
        for (j = -1; j < 1; j += cstepsize) {
                vertices.push_back(static_cast<float>(i));
                vertices.push_back(static_cast<float>(j));
                vertices.push_back(0.0);
                vertices.push_back(0.0);
                vertices.push_back(0.0);
                vertices.push_back(index);
                vertices.push_back(static_cast<float>(i));
                vertices.push_back(static_cast<float>(j+cstepsize));
                vertices.push_back(0.0);
                vertices.push_back(0.0);
                vertices.push_back(1.0);
                vertices.push_back(index);
                vertices.push_back(static_cast<float>(i+rstepsize));
                vertices.push_back(static_cast<float>(j+cstepsize));
                vertices.push_back(0.0);
                vertices.push_back(1.0);
                vertices.push_back(1.0);
                vertices.push_back(index);
                vertices.push_back(static_cast<float>(i+rstepsize));
                vertices.push_back(static_cast<float>(j));
                vertices.push_back(0.0);
                vertices.push_back(1.0);
                vertices.push_back(0.0);
                vertices.push_back(index);
                ++index;
        }
    }
    return vertices;
}

HitBuffer::HitBuffer(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
{
    setFlag(QQuickItem::ItemHasContents);
}

QQuickFramebufferObject::Renderer *HitBuffer::createRenderer() const
{
    return new HitBufferRenderer();
}

const std::array<Textureh, ROW * COL> &HitBuffer::images() const
{
    return m_images;
}

const bool &HitBuffer::firstImage() const
{
    return m_firstImage;
}

void HitBuffer::updateImage(uint64_t index,
                            uint64_t width,
                            uint64_t height,
                            uint8_t *data)
{
    m_images[index % ( ROW * COL )].width = width;
    m_images[index % ( ROW * COL )].height = height;
    m_images[index % ( ROW * COL )].data = data;

    if (index > 1) {
        m_firstImage = true;
    }
    else {
        m_firstImage = false;
    }
    update();
}

HitBufferRenderer::HitBufferRenderer()
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
    glUseProgram(m_shader);
    auto loc = glGetUniformLocation(m_shader, "tex");
    int sampler[4] = {0, 1, 2, 3};
    // don't forget to activate/use the shader before setting uniforms!
    glUniform1iv(loc, 4, sampler);

}

void HitBufferRenderer::synchronize(QQuickFramebufferObject *item)
{
    HitBuffer* obj = static_cast<HitBuffer*>(item);
    Q_UNUSED(obj);

    // sync values ...
    m_textures = obj->images();

    if (obj->firstImage())
        rendertexture1 = true;
    else
        rendertexture1 = false;
}

void HitBufferRenderer::render()
{
    // bind textures on corresponding texture unit
    for (int i = 0; i < ( ROW * COL ); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].ID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                     m_textures[i].width, m_textures[i].height,
                     0,  GL_LUMINANCE, GL_UNSIGNED_BYTE,
                     m_textures[i].data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use Shader Program
    glUseProgram(m_shader);

    // Bind VertexArrays
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    // Draw Call
    glDrawElements(GL_TRIANGLES, 6 * ( ROW * COL )
                   , GL_UNSIGNED_INT, 0);
    QQuickOpenGLUtils::resetOpenGLState();
}

QOpenGLFramebufferObject *HitBufferRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setSamples(4);
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void HitBufferRenderer::checkCompileErrors(GLuint shader, std::string type)
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

void HitBufferRenderer::createShader()
{
    const char* vShaderCode =
        "#version 430 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "layout (location = 2) in float aTexID;\n"
        "\n"
        "out vec2 TexCoord;\n"
        "out float TexID;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos, 1.0);\n"
        "    TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
        "    TexID = aTexID;\n"
        "}\n";

    const char *fShaderCode =
        "#version 430 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "in vec2 TexCoord;\n"
        "in float TexID;\n"
        "\n"
        "// texture samplers\n"
        "uniform sampler2D tex[4];\n"
        "\n"
        "void main()\n"
        "{\n"
        "   int index = int(TexID);\n"
        "   FragColor = texture(tex[index], TexCoord);\n"
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

void HitBufferRenderer::createGeometry()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[( ROW * COL ) * 4 * 6];
    unsigned int indices[2 * ( ROW * COL ) * 3];


    std::vector<float> tver = find_vertices(ROW, COL);

    for (int i = 0; i < tver.size(); ++i) {
        vertices[i] = tver[i];
    }

    for (int index = 0; index < ( ROW * COL ); ++index) {
        indices[index * 6 + 0] = 4 * index + 0;
        indices[index * 6 + 1] = 4 * index + 1;
        indices[index * 6 + 2] = 4 * index + 3;
        indices[index * 6 + 3] = 4 * index + 1;
        indices[index * 6 + 4] = 4 * index + 2;
        indices[index * 6 + 5] = 4 * index + 3;
    }

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture ID attribute
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void HitBufferRenderer::createTexture()
{
    for (int i = 0; i < ( ROW * COL ); ++i) {
        // Texture
        glGenTextures(1, &m_textures[i].ID);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].ID);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}
