

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "KinectData.h"
struct Daxis3{
	int x;
	int y;
	int z;
};
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
	KinectData * kdata;
	void UpdateKinectData();

public slots:
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *);
	void getKinectData();
	RGBQUAD* GLWidget::GetKinectRGB();
    
private slots:

private:
    void normalizeAngle(int *angle);
	void DrawCoordinateAxis();
	void DrawPointClould();
	void Drawtest();
	void CameraSet();
	RGBQUAD * Getm_pBackgroundRGBX(){return kdata->Getm_pBackgroundRGBX();}
	RGBQUAD * GetColorRGBX(){return kdata->GetColorRGBX();}
	RGBQUAD*  GetOutputRGBX(){return kdata->GetOutputRGBX();}
	UINT16*   GetDepthBuffer(){return kdata->GetDepthBuffer();}
	BYTE*     GetpBodyIndexBuffer(){return kdata->GetpBodyIndexBuffer();}
	
	int distance;
	float red,green,blue;
    QPoint lastPos;
	RGBQUAD   Colort ;
	Daxis3 CameraSite,CameraFace,CameraRotate;


// for test
	int xRotation() const { return xRot; }
	int yRotation() const { return yRot; }
	int zRotation() const { return zRot; }

	public slots:
		void setXRotation(int angle);
		void setYRotation(int angle);
		void setZRotation(int angle);

signals:
		void xRotationChanged(int angle);
		void yRotationChanged(int angle);
		void zRotationChanged(int angle);

protected:
	private slots:
		void advanceGears();

private:
	GLuint makeGear(const GLfloat *reflectance, GLdouble innerRadius,
		GLdouble outerRadius, GLdouble thickness,
		GLdouble toothSize, GLint toothCount);
	void drawGear(GLuint gear, GLdouble dx, GLdouble dy, GLdouble dz,
		GLdouble angle);
	GLuint gear1;
	GLuint gear2;
	GLuint gear3;
	int xRot;
	int yRot;
	int zRot;
	int gear1Rot;
};

#endif // GLWIDGET_H
