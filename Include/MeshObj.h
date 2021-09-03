#pragma once
#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QVector>
#include <QVector3D>
#include <QFileInfo>
#include <QDebug>
#include <QString>
#include <GUA_OM.h>

class MeshObj 
{
public:
	Tri_Mesh* mesh = NULL;
	QOpenGLShaderProgram * shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QVector<QVector3D> vertices;
	QVector<QVector3D> normals;
	QVector<QVector2D> uvs;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer nvbo;
	QOpenGLBuffer uvbo;

	bool isInit = false;
	bool isVAOInit = false;
	bool hasNormals = false;
	bool hasUvs = false;

	float CustomOffsetFactor = 0.0005f;

	enum InitMode {
		Custom, Mesh
	};

public:
	MeshObj();
	void Init(InitMode mode);
	void InitVAO();
	bool InitVBObyCustom();
	bool InitVBObyMesh();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void Begin();
	void Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, bool isDrawWireframe, bool isTextureMap, bool isSubview, bool isCustomOffset);
	void End();
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	bool ReadObj(const char* filename);
	void CustomInit();
	void setCustomOffsetFactor(float factor);
};