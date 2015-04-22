

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
class QLabel;
class QMenu;
class QScrollArea;
class QSlider;
QT_END_NAMESPACE

class GLWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void renderIntoPixmap();
    void grabFrameBuffer();
    void clearPixmap();
    void about();
	void refresh();
	void start();

private:
    void createActions();
    void createMenus();
    QSlider *createSlider(const char *changedSignal, const char *setterSlot);
    void setPixmap(const QPixmap &pixmap);
    QSize getSize();
	RGBQUAD * Getm_pBackgroundRGBX(){return glWidget->kdata->Getm_pBackgroundRGBX();}
	RGBQUAD * GetColorRGBX(){return glWidget->kdata->GetColorRGBX();}
	RGBQUAD*  GetOutputRGBX(){return glWidget->kdata->GetOutputRGBX();}
	UINT16*   GetDepthBuffer(){return glWidget->kdata->GetDepthBuffer();}
	BYTE*     GetpBodyIndexBuffer(){return glWidget->kdata->GetpBodyIndexBuffer();}

    QWidget *centralWidget;
    QScrollArea *glWidgetArea;
    QScrollArea *pixmapLabelArea;
    GLWidget *glWidget;
    QLabel *pixmapLabel;
    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *grabFrameBufferAct;
    QAction *renderIntoPixmapAct;
    QAction *clearPixmapAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

	bool  started;

};

#endif // MAINWINDOW_H
