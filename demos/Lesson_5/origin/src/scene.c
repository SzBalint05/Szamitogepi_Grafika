#include "scene.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/gl.h>

void init_scene(Scene* scene)
{
}

void update_scene(Scene* scene)
{
}

void render_scene(const Scene* scene)
{
    draw_origin();
    draw_chessboard();
    draw_colorful_triangle();
    draw_tessellated_sphere(scene->sphere_rotation);
    draw_cylinder();
    draw_cone();

    glBegin(GL_QUADS);

    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(1, 0, 1);
    glVertex3f(1, 0, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(1, 0, 1);
    glVertex3f(1, 0, 0);

    glEnd();

}

void draw_origin()
{
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(1, 0, 0);
    glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 1, 0);
    glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1);
    glEnd();
}

void draw_colorful_triangle()
{
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(0.2f, 0.2f, 0.5f);
    
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.8f, 0.2f, 0.5f);
    
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(0.5f, 0.8f, 0.5f);
    glEnd();
}

void draw_chessboard()
{
    glBegin(GL_QUADS);
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if ((i + j) % 2 == 0) {
                glColor3f(1.0f, 1.0f, 1.0f);
            } else {
                glColor3f(0.2f, 0.2f, 0.2f);
            }
            float x0 = i / 8.0f;
            float y0 = j / 8.0f;
            float x1 = (i + 1) / 8.0f;
            float y1 = (j + 1) / 8.0f;

            glVertex3f(x0, y0, 0.0f);
            glVertex3f(x1, y0, 0.0f);
            glVertex3f(x1, y1, 0.0f);
            glVertex3f(x0, y1, 0.0f);
        }
    }
    glEnd();
}

void draw_tessellated_sphere(double rotation)
{
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 0.5f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glColor3f(0.6f, 0.6f, 0.6f);
    
    int stacks = 8;
    int slices = 8;
    double radius = 0.2;
    
    for (int i = 0; i < stacks; ++i) {
        double lat0 = M_PI * (-0.5 + (double)(i) / stacks);
        double z0  = radius * sin(lat0);
        double zr0 = radius * cos(lat0);

        double lat1 = M_PI * (-0.5 + (double)(i+1) / stacks);
        double z1 = radius * sin(lat1);
        double zr1 = radius * cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            double lng = 2 * M_PI * (double)(j) / slices;
            double x = cos(lng);
            double y = sin(lng);

            glVertex3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
    glPopMatrix();
}

void draw_cylinder()
{
    int slices = 12;
    double radius = 0.1;
    double height = 0.3;
    
    glPushMatrix();
    glTranslatef(0.2f, 0.8f, 0.0f);
    glColor3f(0.2f, 0.8f, 0.2f);
    
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= slices; ++i) {
        double angle = 2 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        
        glVertex3f(x, y, 0.0f);
        glVertex3f(x, y, height);
    }
    glEnd();
    glPopMatrix();
}

void draw_cone()
{
    int slices = 12;
    double radius = 0.1;
    double height = 0.3;
    
    glPushMatrix();
    glTranslatef(0.8f, 0.8f, 0.0f);
    glColor3f(0.8f, 0.2f, 0.2f);
    
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, height);
    for (int i = 0; i <= slices; ++i) {
        double angle = 2 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glPopMatrix();
}
