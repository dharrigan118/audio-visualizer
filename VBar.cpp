#include "VBar.h"

// Constructor
//----------------------------------------------------
VBar::VBar() {
    xLoc = yLoc = zLoc = 0;
    xSize = ySize = zSize = 0;
    opacity = 1;
    color = new Color(0,0,0);
    childCount = 0;
}


// Destructor
//----------------------------------------------------
VBar::~VBar() {

}


// Public Methods
//----------------------------------------------------
void VBar::setLocation(float x, float y, float z) {
    xLoc = x; 
    yLoc = y; 
    zLoc = z;
}

void VBar::setSize(float xs, float ys, float zs) {
    xSize = xs;
    ySize = ys;
    zSize = zs;
}

void VBar::setHeight(float freq) {
    int tempCount = 0;
    ySize = freq;
    if (ySize > 1) ySize = 1; //limit the max height

    // decide color
    if (freq < 0.01) {
        ySize = 0;
        color->set(0.2, 0.1, 0.8);
        tempCount = 0;
    }
    else if(freq < 0.3) {
        color->set(1.0 * freq * 4, 0.1, 1.0);
        tempCount = 2;
    }
    else if(freq < 0.5) {
        color->set(0.4 * freq * 1.2, 
                   1.0 * freq * 2, 
                   1.0);
        tempCount = 8;
    }
    else if(freq < 0.6) {
        color->set(0.4 * freq * 1.2, 
                   1.0 * freq * 2, 
                   1.0);
        tempCount = 14;
    }
    else {
        color->set(0.7 * freq, 0.3 * freq, 0.0);
        tempCount = 20;
    }

    if (children.size() > 0)
        childCount = tempCount;

    int i;
    float offset = 0.01 * freq;
    for (i=1, iter = children.begin(); i < childCount+1; iter++, i++)
       (*iter)->setHeight(freq - (float)(offset*i)*(i+1.2));
}

void VBar::moveUp() {
    zLoc -= zSize;
    opacity -= 0.001;

    int i;
    for (i=0, iter = children.begin(); i < childCount; i++, iter++)
       (*iter)->moveUp(zSize);
}

void VBar::moveUp(float step) {
    zLoc -= step;
    opacity -= 0.001;
}

void VBar::reset() {
    zLoc = 2;
    opacity = 1;

    for (iter = children.begin(); iter != children.end(); iter++)
       (*iter)->reset();
}

void VBar::redraw() {
    glPushMatrix();
        glColor4f(color->r, color->g, color->b, opacity);
        glTranslatef(xLoc, yLoc + (ySize/2), zLoc);
        glScalef(xSize, ySize, zSize);
        glutSolidCube(1);
    glPopMatrix();
  
    int i;
    for (i=0, iter = children.begin(); i < childCount; i++, iter++)
       (*iter)->redraw();
}

void VBar::createChildren(int maxChildren) {
    for (int i=1; i<maxChildren+1; i++) {
        VBar *v = new VBar();
        v->setLocation(xLoc, yLoc, zLoc);
        v->setSize(xSize+(xSize*(i+1)*0.75), ySize, zSize*(i+1)*0.75);
        children.push_back(v);
    }
}
