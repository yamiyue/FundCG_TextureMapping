#include <QtOpenGL/QGLWidget> 
#include <QtGui/QtGui>  
#include <QtOpenGL/QtOpenGL>  
#include <GL/GLU.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib") 
#include "ArcBallCam.h"
#include "MeshObj.h"

class MainView : public QGLWidget, protected QOpenGLFunctions_4_3_Core
{
	

public:
	explicit MainView(QWidget *parent = 0);
	~MainView();

public:
	// overrides of important window things 
	virtual void paintGL();

	// Reset the Arc ball control
	void resetArcball();

	void initializeGL();
	void initializeTexture();

	void readMesh(const char* filename);
	void readPatches(std::string filename);
	void addMulFaces(int x, int y);
	void deleteMulFace(int x, int y);
	void addAllFace();
	void drawFaces();
	void loadTexture(QString fileName);
	float degree2rad(float degree);
	void addPatch();
	void exportPatch(QString filePath);
	void AddFrame();


public:
	ArcBallCam		arcball;			// keep an ArcBall for the UI

	MeshObj* meshObj;
	MeshObj* submeshObj;
	GLfloat ProjectionMatrex[16];
	GLfloat ModelViewMatrex[16];
	QVector<QOpenGLTexture*> Textures;

	/* Store selected face handle*/
	std::vector<OMT::FHandle> select_mfh;
	QVector<QString> texfileNames;
	QVector<MeshObj*> meshObjs;
	QVector<bool> isDisplays;
	QVector<float> rotationAngles;

	/* For texture map */
	bool hasLoadTex = false;
	bool isTextureMap = false;
	int textureIndex = 0;

	/* For texture rotate */
	float rotationAngle = 0.0f;

	//
	QVector<int> Texture_pointer;


	QOpenGLTexture* cloud_tex;
	/**/
	bool showBase = true;

	float time = 0;

};  