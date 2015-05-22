#include "openglwidgetcontainer.h"

#include <QOpenGLContext>
#include <QSurface>
#include <QSurfaceFormat>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

using namespace yasem;

OpenGLWidgetContainer::OpenGLWidgetContainer(QWidget* parent, Qt::WindowFlags f):
    QOpenGLWidget(parent, f)
{
    //setAttribute(Qt::WA_AlwaysStackOnTop);
}

void OpenGLWidgetContainer::paintGL()
{
    QOpenGLWidget::paintGL();
}

