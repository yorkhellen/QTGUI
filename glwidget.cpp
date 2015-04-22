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


    glEnable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_NORMALIZE);
	glClearColor(0.0f,0.0f,0.0f,1.0f);

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
	gluPerspective(45,900/600,0.1,10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(600,500,4000+distance,0,0,0,0,1,0);
	//gluLookAt(50,50,100,0,0,0,0,1,0);
    //glRotated(0, 0, 400, 1700);

	glPointSize(1);
	DrawCoordinateAxis();
	glBegin(GL_POINTS);
	{
      int i = 0 ;
	  for (int w  = 0 ; w < kdata->getnColorWidth() ; w ++ )
		  for (int h = 0 ; h < kdata->getnColorHeight() ; h++)
		  {   
			   Colort = GetKinectRGB()[i++];
			   red =Colort.rgbRed ;
			   green =Colort.rgbGreen;
			   blue = Colort.rgbBlue;
			   if ( red >255 || green >255  ||  blue > 255)
			   {
				   red = 0;
				   green=0;
				   blue = 0;
			   }
			   red/=255;
			   green/=255;
			   blue/=255;
		     glColor3f( blue,green,	red	 );
	         glVertex3f((float)w,(float)h,0);
		  }
	       
	}
	glEnd();
	
    glPopMatrix();
	swapBuffers();
}

void GLWidget::resizeGL(int width, int height)
{
   // int side = qMin(width, height);
    //glViewport((width - side) / 2, (height - side) / 2, side, side);
	glViewport(0, 0, width, height);
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
    	distance+=event->delta()/20;
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
	int coordinate ;
	for (coordinate = 0 ; coordinate < kdata->getDepthHeight() *kdata->getnDepthWidth() ; coordinate ++)
	{
		/*
		const RGBQUAD* pSrc = m_pBackgroundRGBX + colorIndex;

		DepthSpacePoint p = m_pDepthCoordinates[colorIndex];

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
		*/
	}
}
void GLWidget::Drawtest()
{

}