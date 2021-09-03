#include "SubView.h"  

SubView::SubView(QWidget *parent) :	QGLWidget(parent)
{
}
SubView::~SubView()
{}
void SubView::initializeGL()
{
	initializeOpenGLFunctions();
	//initializeTexture();
	meshObj = new MeshObj();

	mesh = new Tri_Mesh;
	meshObj->mesh = mesh;

	QVector<QVector3D> vertices;
	QVector<QVector3D> normals;
	QVector<QVector2D> uvs;

	meshObj->vertices << QVector3D(1, 1, -0.5)
		<< QVector3D(-1, 1, -0.5)
		<< QVector3D(-1, -1, -0.5)
		<< QVector3D(1, -1, -0.5);

	meshObj->normals << QVector3D(0, 0, 1)
		<< QVector3D(0, 0, 1)
		<< QVector3D(0, 0, 1)
		<< QVector3D(0, 0, 1);
	
	meshObj->uvs << QVector2D(1, 1)
		<< QVector2D(0, 1)
		<< QVector2D(0, 0)
		<< QVector2D(1, 0);

	meshObj->CustomInit();
}
void SubView::initializeTexture()
{
}

void SubView::paintGL()
{	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width(), height());
	glClearColor(1, 1, 1, 1);

	// Blayne prefers GL_DIFFUSE
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 0, 0, 0, -1, 1, 0, 0);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1, 1, -1, 1, 1, -1);

	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);


	if (meshObj->isInit) {
		meshObj->Begin();
		if (isTextureMap) 
			if (!Textures.isEmpty()) {
				glActiveTexture(GL_TEXTURE0);
				Textures[0]->bind();
				meshObj->shaderProgram->setUniformValue("Texture", 0);
				meshObj->shaderProgram->setUniformValue("Theta", degree2rad(rotationAngle));
				meshObj->Paint(ProjectionMatrex, ModelViewMatrex, false, isTextureMap, true, false);
			}
			else
				meshObj->Paint(ProjectionMatrex, ModelViewMatrex, false, false, true, false);		
		else {
			meshObj->Paint(ProjectionMatrex, ModelViewMatrex, false, isTextureMap, true, false);
		}				
		meshObj->End();
	}

	if (isParameterized) {
		OMT::VIter v_it;

		/* Draw all the vertices of submesh */
		glPointSize(7.0f);
		glColor3f(1.0, 0.0, 1.0);
		glBegin(GL_POINTS);
		for (v_it = submesh->vertices_begin(); v_it != submesh->vertices_end(); v_it++)
			glVertex3d(submesh->texcoord2D(v_it).data()[0] * 2 - 1, submesh->texcoord2D(v_it).data()[1] * 2 - 1, -0.99);			
		glEnd();

		/* Draw the lines */
		glColor3f(1.0, 0.0, 0.0);
		glLineWidth(4.0f);
		glBegin(GL_LINES);
		for (v_it = submesh->vertices_begin(); v_it != submesh->vertices_end(); v_it++) {
			if (submesh->is_boundary(v_it) == false)
				for (OMT::VVIter vv_it = submesh->vv_iter(v_it); vv_it; vv_it++)
				{
					glVertex3d(submesh->texcoord2D(v_it).data()[0] * 2 - 1, submesh->texcoord2D(v_it).data()[1] * 2 - 1, -0.99);
					glVertex3d(submesh->texcoord2D(vv_it).data()[0] * 2 - 1, submesh->texcoord2D(vv_it).data()[1] * 2 - 1, -0.99);
				}
		}		
		glEnd();
	}
	
}
void SubView::setSubMesh(Tri_Mesh *submesh) {
	this->submesh = new Tri_Mesh(*submesh);
	isParameterized = true;
	update();
}

void SubView::loadTexture(QString fileName) {
	if (!Textures.isEmpty())
		Textures.pop_back();
	makeCurrent();
	QOpenGLTexture* texture = new QOpenGLTexture(QImage(fileName));
	Textures.push_back(texture);	
	//isTextureMap = true;
}

void SubView::addPatch() {
	isTextureMap = false;
	if (!Textures.isEmpty()) {
		Textures[0]->release();
		Textures.pop_back();
	}
	submesh = NULL;
	isParameterized = false;
	update();
}

float SubView::degree2rad(float degree) {
	return degree * M_PI / 180;
}