#include "MeshObj.h"

MeshObj::MeshObj()
{
}

void MeshObj::DimensionTransformation(GLfloat source[], GLfloat target[][4])
{
	//for uniform value, transfer 1 dimension to 2 dimension
	int i = 0;
	for (int j = 0; j<4; j++)
		for (int k = 0; k<4; k++)
		{
			target[j][k] = source[i];
			i++;
		}
}

void MeshObj::Begin()
{
	shaderProgram->bind();
	vao.bind();
}

void MeshObj::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, bool isDrawWireframe, bool isTextureMap, bool isSubview, bool isCustomOffset)
{	
	/* Get MVP */
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	shaderProgram->setUniformValue("ModelViewMatrix", MV);

	vvbo.bind();
	shaderProgram->enableAttributeArray(0);
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	vvbo.release();

	nvbo.bind();
	shaderProgram->enableAttributeArray(1);
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	nvbo.release();

	uvbo.bind();
	shaderProgram->enableAttributeArray(2);
	shaderProgram->setAttributeArray(2, GL_FLOAT, 0, 2, NULL);
	uvbo.release();

	/* Determine whether Subview or not*/
	if (!isSubview) {
		shaderProgram->setUniformValue("Color", QVector3D(1, 0.96, 0.49));
		shaderProgram->setUniformValue("isTextureMap", isTextureMap);
		if (!isCustomOffset) {
			shaderProgram->setUniformValue("OffsetFactor", 0.0f);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		}
		else {		
			shaderProgram->setUniformValue("OffsetFactor", CustomOffsetFactor);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		}			
	}
	else {
		shaderProgram->setUniformValue("Color", QVector3D(1.0, 0.96, 0.49));
		shaderProgram->setUniformValue("isTextureMap", isTextureMap);		
		shaderProgram->setUniformValue("OffsetFactor", 0.0f);
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
	}
	
	/* Draw wireframe */
	if (isDrawWireframe) {
		shaderProgram->setUniformValue("Color", QVector3D(0, 0, 0));
		for (int i = 0; i < vertices.size(); i += 3) {
			shaderProgram->setUniformValue("OffsetFactor", 0.0001f);
			glDrawArrays(GL_LINE_LOOP, i, 3);
		}
	}
}

void MeshObj::End()
{
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);
	shaderProgram->disableAttributeArray(2);

	vao.release();
	shaderProgram->release();
}

void MeshObj::Init(InitMode mode)
{
	InitShader("./Shader/Triangle.vs", "./Shader/Triangle.fs");
	if (!isVAOInit) {
		InitVAO();
		isVAOInit = true;
	}
	if (mode == InitMode::Custom) {
		if (!InitVBObyCustom()) {
			isInit = false;
			printf("InitVBObyCustom fail\n");
			return;
		}else
			printf("InitVBObyCustom success\n");
	}
	else {
		if (!InitVBObyMesh()) {
			isInit = false;
			printf("InitVBObyMesh fail\n");
			return;
		}else
			printf("InitVBObyMesh success\n");
	}	
	isInit = true;
}

void MeshObj::InitVAO()
{
	vao.create();
	vao.bind();
}

bool MeshObj::InitVBObyCustom() {
	if (vertices.size() == 0) 
		return false;	

	vvbo.create();
	vvbo.bind();
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

	nvbo.create();
	nvbo.bind();
	nvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	nvbo.allocate(normals.constData(), normals.size() * sizeof(QVector3D));

	uvbo.create();
	uvbo.bind();
	uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));

	return true;
}

bool MeshObj::InitVBObyMesh()
{
	if (mesh == NULL) {
		printf("NULL\n");
		return false;
	}
		

	OMT::FIter f_it;
	OMT::FVIter fv_it;

	/* Clear the vectors */
	vertices.clear();
	normals.clear();
	uvs.clear();

	/* Check if mesh has normals or uvs*/
	mesh->request_vertex_normals();
	hasNormals = mesh->has_vertex_normals();

	mesh->request_vertex_texcoords2D();
	hasUvs = mesh->has_vertex_texcoords2D();

	/* Put mesh vertices to VBO vertices vector */
	for (f_it = mesh->faces_begin(); f_it != mesh->faces_end(); f_it++) {
		for (fv_it = mesh->fv_iter(f_it); fv_it; fv_it++) {

			/* Set each vertex's position */
			double* vp = mesh->point(fv_it.handle()).data();
			vertices << QVector3D(vp[0], vp[1], vp[2]);

			/* Set each vertex's normal */
			if (hasNormals) {
				const double* np = mesh->normal(fv_it.handle()).data();
				normals << QVector3D(np[0], np[1], np[2]);
			}
				
			/* Set each vertex's uv */
			if (hasUvs) {
				const float* up = mesh->texcoord2D(fv_it.handle()).data();
				uvs << QVector2D(up[0], up[1]);
			}
				
		}
	}
	mesh->release_vertex_normals();
	mesh->release_vertex_texcoords2D();

	vvbo.create();
	vvbo.bind();
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

	nvbo.create();
	nvbo.bind();
	nvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	nvbo.allocate(normals.constData(), normals.size() * sizeof(QVector3D));

	uvbo.create();
	uvbo.bind();
	uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));
	return true;
}

void MeshObj::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
{
	// Create Shader
	shaderProgram = new QOpenGLShaderProgram();
	QFileInfo  vertexShaderFile(vertexShaderPath);
	if (vertexShaderFile.exists())
	{
		vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
		if (vertexShader->compileSourceFile(vertexShaderPath))
			shaderProgram->addShader(vertexShader);
		else
			qWarning() << "Vertex Shader Error " << vertexShader->log();
	}
	else
		qDebug() << vertexShaderFile.filePath() << " can't be found";

	QFileInfo  fragmentShaderFile(fragmentShaderPath);
	if (fragmentShaderFile.exists())
	{
		fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
		if (fragmentShader->compileSourceFile(fragmentShaderPath))
			shaderProgram->addShader(fragmentShader);
		else
			qWarning() << "fragment Shader Error " << fragmentShader->log();
	}
	else
		qDebug() << fragmentShaderFile.filePath() << " can't be found";
	shaderProgram->link();
}

bool MeshObj::ReadObj(const char* filename) {
	if (mesh != NULL)
		delete mesh;

	mesh = new Tri_Mesh;

	if (ReadFile(filename, mesh)) {
		std::cout << "Load Obj " << filename << " success." << std::endl;
		Init(InitMode::Mesh);
	}
	else {
		std::cout << "Load Obj " << filename << " fail." << std::endl;
		return false;
	}
	return true;
}

void MeshObj::CustomInit() {
	Init(InitMode::Custom);
}

void MeshObj::setCustomOffsetFactor(float factor) {
	CustomOffsetFactor = factor;
}