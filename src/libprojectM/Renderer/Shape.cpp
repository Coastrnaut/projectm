#include "Shape.hpp"

#include <glm/gtc/type_ptr.hpp>

Shape::Shape()
    : RenderItem()
{
    glGenVertexArrays(1, &m_vaoID_texture);
    glGenBuffers(1, &m_vboID_texture);

    glGenVertexArrays(1, &m_vaoID_not_texture);
    glGenBuffers(1, &m_vboID_not_texture);

    glBindVertexArray(m_vaoID_texture);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID_texture);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          sizeof(ShapeVertexShaderData), nullptr); // Positions
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                          sizeof(ShapeVertexShaderData), reinterpret_cast<void*>(sizeof(float) * 2)); // Colors
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          sizeof(ShapeVertexShaderData), reinterpret_cast<void*>(sizeof(float) * 6)); // Textures

    glBindVertexArray(m_vaoID_not_texture);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID_not_texture);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          sizeof(ShapeVertexShaderData), nullptr); // Points
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                          sizeof(ShapeVertexShaderData), reinterpret_cast<void*>(sizeof(float) * 2)); // Colors

    RenderItem::Init();
}

Shape::~Shape()
{
    glDeleteBuffers(1, &m_vboID_texture);
    glDeleteVertexArrays(1, &m_vaoID_texture);

    glDeleteBuffers(1, &m_vboID_not_texture);
    glDeleteVertexArrays(1, &m_vaoID_not_texture);
}

void Shape::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);   // points
    glDisableVertexAttribArray(1);
}

void Shape::Draw(RenderContext& context)
{
    float xval{ x };
    float yval{ -(y - 1) };
    float t;

    float temp_radius{ radius * (.707f * .707f * .707f * 1.04f) };

    // Additive Drawing or Overwrite
    if (additive == 0)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

    auto vertexData = new ShapeVertexShaderData[sides + 2];

    if (textured)
    {
        if (!imageUrl.empty())
        {
            TextureSamplerDesc tex = context.textureManager->getTexture(imageUrl, GL_CLAMP_TO_EDGE, GL_LINEAR);
            if (tex.first != NULL)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex.first->texID);
                glBindSampler(0, tex.second->samplerID);

                context.aspectRatio = 1.0;
            }
        }
        else
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, context.textureManager->getMainTexture()->texID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        // Define the center point of the shape
        vertexData[0].color_r = r;
        vertexData[0].color_g = g;
        vertexData[0].color_b = b;
        vertexData[0].color_a = a * masterAlpha;
        vertexData[0].tex_x = 0.5;
        vertexData[0].tex_y = 0.5;
        vertexData[0].point_x = xval;
        vertexData[0].point_y = yval;

        for (int i = 1; i < sides + 2; i++)
        {
            vertexData[i].color_r = r2;
            vertexData[i].color_g = g2;
            vertexData[i].color_b = b2;
            vertexData[i].color_a = a2 * masterAlpha;

            t = static_cast<float>(i - 1) / static_cast<float>(sides);
            vertexData[i].tex_x = 0.5f + 0.5f * cosf(t * 3.1415927f * 2 + tex_ang + 3.1415927f * 0.25f) *
                                         (context.aspectCorrect ? context.aspectRatio : 1.0) / tex_zoom;
            vertexData[i].tex_y = 0.5f + 0.5f * sinf(t * 3.1415927f * 2 + tex_ang + 3.1415927f * 0.25f) / tex_zoom;
            vertexData[i].point_x = temp_radius * cosf(t * 3.1415927f * 2 + ang + 3.1415927f * 0.25f) *
                                    (context.aspectCorrect ? context.aspectRatio : 1.0) + xval;
            vertexData[i].point_y = temp_radius * sinf(t * 3.1415927f * 2 + ang + 3.1415927f * 0.25f) + yval;
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_vboID_texture);

        glBufferData(GL_ARRAY_BUFFER, sizeof(ShapeVertexShaderData) * (sides + 2), NULL, GL_DYNAMIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ShapeVertexShaderData) * (sides + 2), vertexData, GL_DYNAMIC_DRAW);

        glUseProgram(context.programID_v2f_c4f_t2f);

        glUniformMatrix4fv(context.uniform_v2f_c4f_t2f_vertex_transformation, 1, GL_FALSE,
                           glm::value_ptr(context.mat_ortho));
        glUniform1i(context.uniform_v2f_c4f_t2f_frag_texture_sampler, 0);

        glBindVertexArray(m_vaoID_texture);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sides + 2);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindSampler(0, 0);
    }
    else
    {//Untextured (use color values)

        //Define the center point of the shape
        vertexData[0].color_r = r;
        vertexData[0].color_g = g;
        vertexData[0].color_b = b;
        vertexData[0].color_a = a * masterAlpha;
        vertexData[0].point_x = xval;
        vertexData[0].point_y = yval;

        for (int i = 1; i < sides + 2; i++)
        {
            vertexData[i].color_r = r2;
            vertexData[i].color_g = g2;
            vertexData[i].color_b = b2;
            vertexData[i].color_a = a2 * masterAlpha;
            t = static_cast<float>(i - 1) / static_cast<float>(sides);
            vertexData[i].point_x = temp_radius * cosf(t * 3.1415927f * 2 + ang + 3.1415927f * 0.25f) *
                                    (context.aspectCorrect ? context.aspectRatio : 1.0) + xval;
            vertexData[i].point_y = temp_radius * sinf(t * 3.1415927f * 2 + ang + 3.1415927f * 0.25f) + yval;
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_vboID_not_texture);

        glBufferData(GL_ARRAY_BUFFER, sizeof(ShapeVertexShaderData) * (sides + 2), NULL, GL_DYNAMIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ShapeVertexShaderData) * (sides + 2), vertexData, GL_DYNAMIC_DRAW);

        glUseProgram(context.programID_v2f_c4f);

        glUniformMatrix4fv(context.uniform_v2f_c4f_vertex_transformation, 1, GL_FALSE,
                           glm::value_ptr(context.mat_ortho));

        glBindVertexArray(m_vaoID_not_texture);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sides + 2);
        glBindVertexArray(0);
    }

    if (border_a > 0.0f)
    {
        auto points = new float[sides + 1][2];

        for (int i = 0; i < sides; i++)
        {
            t = (i - 1) / (float) sides;
            points[i][0] = temp_radius * cosf(t * 3.1415927f * 2 + ang + 3.1415927f * 0.25f) *
                           (context.aspectCorrect ? context.aspectRatio : 1.0) + xval;
            points[i][1] = temp_radius * sinf(t * 3.1415927f * 2 + ang + 3.1415927f * 0.25f) + yval;
        }

        glUseProgram(context.programID_v2f_c4f);

        glUniformMatrix4fv(context.uniform_v2f_c4f_vertex_transformation, 1, GL_FALSE,
                           glm::value_ptr(context.mat_ortho));
        glVertexAttrib4f(1, border_r, border_g, border_b, border_a * masterAlpha);
        glLineWidth(1);
        glEnable(GL_LINE_SMOOTH);

        glBindVertexArray(m_vaoID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

        auto iterations = thickOutline ? 4 : 1;

        // Need to use +/- 1.0 here instead of 2.0 used in Milkdrop to achieve the same rendering result.
        auto incrementX = 1.0f / static_cast<float>(context.viewportSizeX);
        auto incrementY = 1.0f / static_cast<float>(context.viewportSizeY);

        // If thick outline is used, draw the shape four times with slight offsets
        // (top left, top right, bottom right, bottom left).
        for (auto iteration = 0; iteration < iterations; iteration++)
        {
            switch (iteration)
            {
                case 0:
                default:
                    break;

                case 1:
                    for (auto j = 0; j < sides + 1; j++)
                    {
                        points[j][0] += incrementX;
                    }
                    break;

                case 2:
                    for (auto j = 0; j < sides + 1; j++)
                    {
                        points[j][1] += incrementY;
                    }
                    break;

                case 3:
                    for (auto j = 0; j < sides + 1; j++)
                    {
                        points[j][0] -= incrementX;
                    }
                    break;
            }

            glBufferData(GL_ARRAY_BUFFER, sizeof(floatPair) * (sides), points, GL_DYNAMIC_DRAW);
            glDrawArrays(GL_LINE_LOOP, 0, sides);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        delete[] points;
    }

    glDisable(GL_LINE_SMOOTH);
    glUseProgram(0);

    delete[] vertexData;
}