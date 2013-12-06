//
//
//
//

#include <GL/glut.h>
#include <portaudio.h>
#include <stdio.h>
#include "Audio.h"

#include "Point3.h"
#include "Scene.h"
#include "Box.h"

#include <math.h>
#include <unistd.h>

int windowWidth  = 1280;
int windowHeight = 960;
//extern Packet *sharedBuffer;
//PaStream* stream;
Scene *scene;
Audio *audio;

//
// Create a scene
//
void createScene(void) {
    scene = new Scene();
}

//
// Window reshape function
//
void reshape(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(10, windowWidth/windowHeight, 0.1, 1000.0);
    glutPostRedisplay();
}

//
// Initialize OpenGL components
//
void appInit(void) {
    glClearColor(0.5,0.5,0.5,0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutReshapeFunc(reshape);
    createScene();
}

//
// OpenGL redraw function
//
void redraw(void) {
    glutPostRedisplay();
    //glutTimerFunc(30, redraw, -1);    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLfloat ambient[4]  = {0.2, 0.2, 0.2, 1};
    GLfloat diffuse[4]  = {1.0, 1.0, 1.0, 1.0};
    GLfloat specular[4] = {1.0, 1.0, 1.0, 1.0};
    GLfloat location[4] = {-2.0, 1.5, -1.0, 1.0};
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_POSITION, location);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
    glEnable(GL_LIGHT1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,5,16,
              0,0,0,
              0,1,0);
    scene->redraw();
    glutSwapBuffers();
    glFlush();
}

int main(int argc, char *argv[]) {
    int window;
    //sharedBuffer = (Packet*)malloc(sizeof(Packet) * BUFFER_SIZE);

    if (argc != 2) {
        printf("No sound file\n");
        return EXIT_FAILURE;
    }

    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 150);
    window = glutCreateWindow("Visualizer");

    //Initialize audio components 
    audio = new Audio();
    audio->initialize();
    if (!audio->loadFile(argv[1])) {
        return EXIT_FAILURE;
    }
    
    glutDisplayFunc(redraw);
    appInit();

    audio->play();
    glutMainLoop();
    return EXIT_SUCCESS;
}
