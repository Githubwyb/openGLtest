#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib,"glew32.lib")

using namespace std;

GLfloat lightPosition[3] = { 30.0,30.0,30.0 };
GLfloat ambient[4] = { 0.0 , 0.0 , 1.0 , 1.0 };
GLfloat lightcolor[4] = { 1.0 , 1.0 , 1.0 , 1.0 };
GLfloat eyeposition[3] = { 0.0 , 10.0 , 30.0 };
GLfloat Ns = 30;
GLfloat attenuation = 0.01;
GLfloat objectSize = 15.0;
GLuint programHandle;
GLuint vShader, fShader;

//读入字符流
char *textFileRead(const char *fn)
{
    FILE *fp;
    char *content = NULL;
    int count = 0;
    if (fn != NULL)
    {
        fp = fopen(fn, "rt");
        if (fp != NULL)
        {
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);
            if (count > 0)
            {
                content = (char *)malloc(sizeof(char) * (count + 1));
                count = fread(content, sizeof(char), count, fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

void initShader(const char *VShaderFile, const char *FShaderFile)
{
    //1、查看显卡、GLSL和OpenGL的信息
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    cout << "显卡供应商   : " << vendor << endl;
    cout << "显卡型号     : " << renderer << endl;
    cout << "OpenGL版本   : " << version << endl;
    cout << "GLSL版本     : " << glslVersion << endl;
    //2、编译着色器
    //创建着色器对象：顶点着色器
    vShader = glCreateShader(GL_VERTEX_SHADER);
    //错误检测
    if (0 == vShader)
    {
        cerr << "ERROR : Create vertex shader failed" << endl;
        exit(1);
    }
    //把着色器源代码和着色器对象相关联
    const GLchar *vShaderCode = textFileRead(VShaderFile);
    const GLchar *vCodeArray[1] = { vShaderCode };

    //将字符数组绑定到对应的着色器对象上
    glShaderSource(vShader, 1, vCodeArray, NULL);

    //编译着色器对象
    glCompileShader(vShader);

    //检查编译是否成功
    GLint compileResult;
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &compileResult);
    if (GL_FALSE == compileResult)
    {
        GLint logLen;
        //得到编译日志长度
        glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0)
        {
            char *log = (char *)malloc(logLen);
            GLsizei written;
            //得到日志信息并输出
            glGetShaderInfoLog(vShader, logLen, &written, log);
            cerr << "vertex shader compile log : " << endl;
            cerr << log << endl;
            free(log);//释放空间
        }
    }

    //创建着色器对象：片断着色器
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    //错误检测
    if (0 == fShader)
    {
        cerr << "ERROR : Create fragment shader failed" << endl;
        exit(1);
    }

    //把着色器源代码和着色器对象相关联
    const GLchar *fShaderCode = textFileRead(FShaderFile);
    const GLchar *fCodeArray[1] = { fShaderCode };
    glShaderSource(fShader, 1, fCodeArray, NULL);

    //编译着色器对象
    glCompileShader(fShader);

    //检查编译是否成功
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &compileResult);
    if (GL_FALSE == compileResult)
    {
        GLint logLen;
        //得到编译日志长度
        glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0)
        {
            char *log = (char *)malloc(logLen);
            GLsizei written;
            //得到日志信息并输出
            glGetShaderInfoLog(fShader, logLen, &written, log);
            cerr << "fragment shader compile log : " << endl;
            cerr << log << endl;
            free(log);//释放空间
        }
    }
    //3、链接着色器对象
    //创建着色器程序
    programHandle = glCreateProgram();
    if (!programHandle)
    {
        cerr << "ERROR : create program failed" << endl;
        exit(1);
    }
    //将着色器程序链接到所创建的程序中
    glAttachShader(programHandle, vShader);
    glAttachShader(programHandle, fShader);
    //将这些对象链接成一个可执行程序
    glLinkProgram(programHandle);
    //查询链接的结果
    GLint linkStatus;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
    if (GL_FALSE == linkStatus)
    {
        cerr << "ERROR : link shader program failed" << endl;
        GLint logLen;
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH,
            &logLen);
        if (logLen > 0)
        {
            char *log = (char *)malloc(logLen);
            GLsizei written;
            glGetProgramInfoLog(programHandle, logLen,
                &written, log);
            cerr << "Program log : " << endl;
            cerr << log << endl;
        }
    }
}

//完成glew初始化和加载顶点、片段着色器
void init()
{
    //初始化glew扩展库
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        cout << "Error initializing GLEW: " << glewGetErrorString(err) << endl;
    }
    glEnable(GL_DEPTH_TEST);
    //加载顶点和片段着色器对象并链接到一个程序对象上
    initShader("toon.vert", "toon.frag");

    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void Reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, 1, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeposition[0], eyeposition[1], eyeposition[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glUseProgram(programHandle);
    glUniform3f(glGetUniformLocation(programHandle, "lightposition"), lightPosition[0], lightPosition[1], lightPosition[2]);
    glUniform3f(glGetUniformLocation(programHandle, "eyeposition"), eyeposition[0], eyeposition[1], eyeposition[2]);
    glUniform4f(glGetUniformLocation(programHandle, "ambient"), ambient[0], ambient[1], ambient[2], ambient[3]);
    glUniform4f(glGetUniformLocation(programHandle, "lightcolor"), lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3]);
    glUniform1f(glGetUniformLocation(programHandle, "Ns"), Ns);
    glUniform1f(glGetUniformLocation(programHandle, "attenuation"), attenuation);

    glutSolidTeapot(objectSize);
    //glutSolidSphere(objectSize-3, 100, 100);
    glutSwapBuffers();
}

void SpecialKey(GLint key, GLint x, GLint y)
{
    if (key == GLUT_KEY_UP)
    {
        //do something
    }
    display();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Hello GLSL");
    init();
    glutReshapeFunc(Reshape);
    glutDisplayFunc(display);
    glutSpecialFunc(SpecialKey);
    glutMainLoop();
    return 0;
}