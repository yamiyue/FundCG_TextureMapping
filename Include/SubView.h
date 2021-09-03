#include <QtOpenGL/QGLWidget> 
#include <QtGui/QtGui>  
#include <QtOpenGL/QtOpenGL>  
#include <GL/GLU.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib") 
#include "MeshObj.h"

class SubView : public QGLWidget, protected QOpenGLFunctions_4_3_Core
{
	Q_OBJECT
public:
	explicit SubView(QWidget *parent = 0);
	~SubView();

public:
	// overrides of important window things 
	virtual void paintGL();
	void initializeGL();
	void initializeTexture();
	void setSubMesh(Tri_Mesh *submesh);
	void loadTexture(QString fileName);
	void addPatch();
	float degree2rad(float degree);

public:

	MeshObj * meshObj;
	Tri_Mesh* mesh;
	Tri_Mesh* submesh;
	GLfloat ProjectionMatrex[16];
	GLfloat ModelViewMatrex[16];
	QVector<QOpenGLTexture*> Textures;

	bool isTextureMap = false;
	bool isParameterized = false;

	float rotationAngle = 0.0f;
	
};