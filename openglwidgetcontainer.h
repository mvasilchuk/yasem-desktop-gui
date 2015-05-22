#ifndef OPENGLWIDGETCONTAINER_H
#define OPENGLWIDGETCONTAINER_H

#include <QOpenGLWidget>

namespace yasem {

class OpenGLWidgetContainer: public QOpenGLWidget
{
public:
    OpenGLWidgetContainer(QWidget* parent = 0, Qt::WindowFlags f = 0);

    // QOpenGLWidget interface
protected:
    void paintGL();

};

}

#endif // OPENGLWIDGETCONTAINER_H
