#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "CReader.h"

GLuint v, f, p;
float lpos[4] = {1, 0.5, 1, 0};
float a = 0;

void changeSize(int w, int h) {
    // 防止窗口太矮而造成分数过大
    if (h == 0) h = 1;
    float ratio = 1.0 * w / h;

    // 在更改之前重设坐标系
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 设置视口为整个窗口
    glViewport(0, 0, w, h);

    // 设置正确的透视投影方法
    gluPerspective(45, ratio, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, -1.0,
              0.0f, 1.0f, 0.0f);
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    glRotatef(a, 0, 1, 1);
    glutSolidTeapot(1);
    a += 0.1;
    glutSwapBuffers();
}

void setShaders() {                                  //一：比普通opengl编程多出的部分
    char *vs = NULL, *fs = NULL;
    v = glCreateShader(GL_VERTEX_SHADER);              //建立着色器对象
    f = glCreateShader(GL_FRAGMENT_SHADER);

    CReader reader;                                  //着色器读入内存
    vs = reader.textFileRead("toon.vert");
    fs = reader.textFileRead("toon.frag");

    const char *vv = vs;

    const char *ff = fs;

    glShaderSource(v, 1, &vv, NULL);                  //加载到着色器对象
    glShaderSource(f, 1, &ff, NULL);

    free(vs);
    free(fs);
    glCompileShader(v);                              //编译
    glCompileShader(f);

    p = glCreateProgram();                             //建立空程序对象
    glAttachShader(p, v);                             //glAttachShader(程序对象，着色器对象)，将着色器对象添加到程序对象中
    glAttachShader(p, f);
    glLinkProgram(p);                                //对程序对象进行连接
    glUseProgram(p);                                 //选择想要使用的程序对象
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(320, 320);
    glutCreateWindow("GPGPU Tutorial");
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_CULL_FACE);
    glewInit();

    setShaders();                                                             //二：比普通opengl编程多出的部分

    glutMainLoop();

    return 0;
}