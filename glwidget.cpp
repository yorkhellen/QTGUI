#include "glwidget.h"
#include "KinectData.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>

#include <gl/GLU.h>
#include <gl/GL.h>
#include <math.h>

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{

	distance=0;
	kdata = new KinectData();
	kdata->InitKinect();
	kdata->Update();
	//init 
	// set camerasite at 100 ,100 ,100
	CameraSite.x= 100;
	CameraSite.y=100 ;
	CameraSite.z =100;
	// set camera face 0,0,0

	CameraFace.x = 0;
	CameraFace.y= 0 ; 
	CameraFace.z= 0;
	// set camera rotate 1,0,0
	CameraRotate.x = 1;
	CameraRotate.y = 0;
	CameraRotate.z = 0;
}
GLWidget::~GLWidget()
{

}
void GLWidget::initializeGL()
{
	static const GLfloat lightPos[4] = { 5.0f, 5.0f, 10.0f, 1.0f };
	static const GLfloat reflectance1[4] = { 0.8f, 0.1f, 0.0f, 1.0f };
	static const GLfloat reflectance2[4] = { 0.0f, 0.8f, 0.2f, 1.0f };
	static const GLfloat reflectance3[4] = { 0.2f, 0.2f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}
RGBQUAD* GLWidget::GetKinectRGB()
{
	return kdata->getColorRGBX();
}
void GLWidget::paintGL()
{
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	glPushMatrix();

 	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
 	gluPerspective(45,1000/800,0.1,10000);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	CameraSet();
	DrawCoordinateAxis();
	Drawtest();


	glPopMatrix();
	swapBuffers();
	
}

void GLWidget::resizeGL(int width, int height)
{
	int side = qMin(width, height);
	glViewport((width - side) / 2, (height - side) / 2, side, side);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, +1.0, -1.0, 1.0, 5.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -40.0);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
		

    } else if (event->buttons() & Qt::RightButton) {

    }
    lastPos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent * event)
{
    	distance+=event->delta()/120;
		updateGL();
}
void GLWidget::normalizeAngle(int *angle)
{
    while (*angle < 0)
        *angle += 360 * 16;
    while (*angle > 360 * 16)
        *angle -= 360 * 16;
}
void GLWidget::UpdateKinectData()
{
	this->kdata->Update();
	updateGL();
}
void GLWidget::DrawCoordinateAxis()
{
	//X axis
	glColor3f(1.0f,0.0f,0.0f);
	glBegin(GL_LINE);

	glVertex3f(0,0,0);
	glVertex3f(10,0,0);
	glEnd();
	glBegin(GL_LINE);
	glVertex3f(10,0,0);
	glVertex3f(9,-1,0);
	glEnd();
	glBegin(GL_LINE);
	glVertex3f(10,0,0);
	glVertex3f(9,1,0);
	glEnd();
	//y axis
	glColor3f(0.0f,1.0f,0.0f);
	glBegin(GL_LINE);
	glVertex3f(0,0,0);
	glVertex3f(0,10,0);
	glEnd();
	glBegin(GL_LINE);
	glVertex3f(0,10,0);
	glVertex3f(0,9,-1);
	glEnd();
	glBegin(GL_LINE);
	glVertex3f(0,10,0);
	glVertex3f(0,9,1);
	glEnd();
	//z axis
	glColor3f(0.0f,0.0f,1.0f);
	glBegin(GL_LINE);
	glVertex3f(0,0,0);
	glVertex3f(0,0,10);
	glEnd();
	glBegin(GL_LINE);
	glVertex3f(0,0,10);
	glVertex3f(-1,0,9);
	glEnd();
	glBegin(GL_LINE);
	glVertex3f(0,0,10);
	glVertex3f(1,0,9);
	glEnd();
}

void GLWidget::DrawPointClould()
{
	int colorIndex ;
	for (colorIndex = 0 ; colorIndex < kdata->getDepthHeight() *kdata->getnDepthWidth() ; colorIndex ++)
	{
		
		const RGBQUAD* pSrc = Getm_pBackgroundRGBX() + colorIndex;

		DepthSpacePoint p = GetDepthBuffer()[colorIndex];

		// Values that are negative infinity means it is an invalid color to depth mapping so we
		// skip processing for this pixel
		if (p.X != -std::numeric_limits<float>::infinity() && p.Y != -std::numeric_limits<float>::infinity())
		{
			int depthX = static_cast<int>(p.X + 0.5f);
			int depthY = static_cast<int>(p.Y + 0.5f);

			if ((depthX >= 0 && depthX < nDepthWidth) && (depthY >= 0 && depthY < nDepthHeight))
			{
				BYTE player = pBodyIndexBuffer[depthX + (depthY * cDepthWidth)];

				// if we're tracking a play current pixel, draw from the color camera
				if (player != 0xff)
				{
					// set source for copy to the color pixel
					pSrc = m_pColorRGBX + colorIndex;
				}
			}
		}
		// write output
		m_pOutputRGBX[colorIndex] = *pSrc;
		
	}
}
void GLWidget::Drawtest()
{
	glColor3f(1,0,0);
	glBegin(GL_TRIANGLES);
	glVertex3f(25,0,0);
	glVertex3f(0,25,0);
	glVertex3f(0,0,25);
	glEnd();
}
void GLWidget::CameraSet()
{

	gluLookAt(100,100,100,-100,-100,-100,1,0,0);
	glPointSize(1);	

}