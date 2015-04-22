

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
	
	int distance;
	float red,green,blue;
    QPoint lastPos;
	RGBQUAD   Colort ;
	Daxis3 CameraSite,CameraFace,CameraRotate;
};

#endif // GLWIDGET_H
