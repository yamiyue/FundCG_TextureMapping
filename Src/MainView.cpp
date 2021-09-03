#include "MainView.h"  

MainView::MainView(QWidget *parent) :
QGLWidget(parent)  
{  
	resetArcball();
	setMouseTracking(true);
}  

MainView::~MainView()
{}  

void MainView::initializeGL()
{
	initializeOpenGLFunctions();

	//Create a MeshObj object
	meshObj = new MeshObj;
	submeshObj = new MeshObj;	
	initializeTexture();
}

void MainView::initializeTexture()
{
	cloud_tex = new QOpenGLTexture(QImage("./Textures/cloud.png").mirrored());
	cloud_tex->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	cloud_tex->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
	cloud_tex->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
	cloud_tex->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);	
}

void MainView:: resetArcball()
{
	arcball.setup(this, 40, 10, .2f, .4f, 0);
}

void MainView::paintGL()
{			
	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glViewport(0, 0, width(), height());
	glClearColor(1,1,1,1);
	
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	
	// Blayne prefers GL_DIFFUSE
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	arcball.setProjection(true);

	/* Draw select face */

	if (select_mfh.size() != 0)
		drawFaces();

	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	                               
#pragma region Draw Texture Mapping Mesh
	if (submeshObj->isInit) {
		if (isTextureMap) {
			submeshObj->Begin();
			glActiveTexture(GL_TEXTURE0 + textureIndex);
			Textures[textureIndex]->bind();
			submeshObj->shaderProgram->setUniformValue("Texture", textureIndex);
			submeshObj->shaderProgram->setUniformValue("Theta", degree2rad(rotationAngle));
			submeshObj->setCustomOffsetFactor(0.0005f + textureIndex * 0.0001f);
			submeshObj->Paint(ProjectionMatrex, ModelViewMatrex, false, isTextureMap, false, true);
			submeshObj->End();
		}
	}
#pragma endregion

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#pragma region Draw Texture Mapped Mesh	
	for (int i = 0; i < meshObjs.size(); i++) {
		MeshObj* tempObj = meshObjs[i];
		if (isDisplays[i]) {
			if (i == 33 || i == 34 || i == 39) {
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			tempObj->Begin();
			glActiveTexture(GL_TEXTURE0 + Textures[Texture_pointer[i]]->textureId());
			Textures[Texture_pointer[i]]->bind();
			tempObj->shaderProgram->setUniformValue("Texture", Textures[Texture_pointer[i]]->textureId());
			if (i == 35 || i == 36 || i == 37 || i == 38 || i == 40) {
				glActiveTexture(GL_TEXTURE0 + cloud_tex->textureId());
				cloud_tex->bind();
				tempObj->shaderProgram->setUniformValue("Texture1", cloud_tex->textureId());
				tempObj->shaderProgram->setUniformValue("isLava", true);
				tempObj->shaderProgram->setUniformValue("time", time);
			}else
				tempObj->shaderProgram->setUniformValue("isLava", false);
			tempObj->shaderProgram->setUniformValue("Theta", degree2rad(rotationAngles[i]));
			tempObj->setCustomOffsetFactor(0.0005f + i * 0.0001f);
			tempObj->Paint(ProjectionMatrex, ModelViewMatrex, false, true, false, true);
			tempObj->End();
			if (i == 33 || i == 34 || i == 39) {
				glDisable(GL_BLEND);
			}
		}
	}
#pragma endregion
	
	glDisable(GL_BLEND);

#pragma region Draw meshObj base obj
	if (meshObj->isInit && showBase) {
		meshObj->Begin();
		meshObj->Paint(ProjectionMatrex, ModelViewMatrex, true, false, false, false);
		meshObj->End();
	}
#pragma endregion

}

void MainView::readMesh(const char* filename)
{
	if (meshObj->ReadObj(filename)) 
		update();
}

void MainView::readPatches(std::string filename) {
	makeCurrent();
	MeshObj* meshObj = new MeshObj;
	if(meshObj->ReadObj(filename.data()))
		update();

	bool isDisplay = true;
	isDisplays.push_back(isDisplay);

	int index = filename.find(".obj");
	filename.replace(index, index + 3, ".txt");
	//std::cout << filename << std::endl;
	std::string buffer;
	std::ifstream fileRead(filename);

	std::getline(fileRead, buffer);
	std::cout << "texfileName:" << buffer << std::endl;
	bool dup = false;
	int ind = 0;
	for (int i = 0; i < texfileNames.size(); i++) {
		QString str = texfileNames.at(i);
		if (str == QString::fromStdString(buffer)) {
			dup = true;
			ind = i;
			printf("dup copy:%d\n",ind);
			break;
		}			
	}
	if (dup) {
		Texture_pointer.push_back(ind);
	}
	else {
		texfileNames.push_back(QString::fromStdString(buffer));
		QOpenGLTexture* texture = new QOpenGLTexture(QImage(buffer.data()).mirrored());
		texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
		texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
		texture->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
		texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
		Textures.push_back(texture);
		Texture_pointer.push_back(Textures.size() - 1);
	}
		

	
	
	float rotationAngle;
	std::getline(fileRead, buffer);
	rotationAngle = std::stof(buffer);
	//std::cout << "temp_angle:" << rotationAngle << std::endl;
	rotationAngles.push_back(rotationAngle);

	meshObjs.push_back(meshObj);
}

void MainView::addMulFaces(int CursorX, int CursorY) {
	GLdouble modelview[16]; //var to hold the modelview info
	GLdouble projection[16]; //var to hold the projection matrix info
	GLint viewport[4]; //var to hold the viewport info
	GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
	GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //get the modelview info
	glGetDoublev(GL_PROJECTION_MATRIX, projection); //get the projection matrix info
	glGetIntegerv(GL_VIEWPORT, viewport); //get the viewport info

	winX = (GLfloat)(CursorX);
	winY = (GLfloat)(viewport[3] - CursorY);
	glReadPixels((GLint)winX, (GLint)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	if (winZ != 1) {
		gluUnProject(winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);


		QMatrix4x4 a, b;
		a = a * b;
		OMT::FHandle fh_temp;
		float sum = 0;
		float bigger1 = 100;
		float CenterX = 0, CenterY = 0, CenterZ = 0;

		double  error = DBL_MAX;

		/* Get mesh Instance */
		Tri_Mesh* mesh = meshObj->mesh;
		if (mesh != NULL) {
			/* Find face by Barycentric */
			for (OMT::FIter f_it = mesh->faces_begin(); f_it != mesh->faces_end(); f_it++) {
				for (OMT::FVIter fv_it = mesh->fv_iter(f_it); fv_it; ++fv_it) {
					CenterX = CenterX + mesh->point(fv_it).data()[0];
					CenterY = CenterY + mesh->point(fv_it).data()[1];
					CenterZ = CenterZ + mesh->point(fv_it).data()[2];
				}
				sum = (CenterX / 3 - worldX)*(CenterX / 3 - worldX) + (CenterY / 3 - worldY)*(CenterY / 3 - worldY) + (CenterZ / 3 - worldZ)*(CenterZ / 3 - worldZ);
				if (sum < bigger1) {
					bigger1 = sum;
					fh_temp = f_it.handle();
				}
				CenterX = 0;
				CenterY = 0;
				CenterZ = 0;
			}//end for

			/* Find that whether this face is in the select_mfh */
			bool dup = false;
			for (int i = 0; i < select_mfh.size(); i++) {
				if (fh_temp == select_mfh.at(i)) {
					dup = true;
					break;
				}
			}
			if (dup == false)
				select_mfh.push_back(fh_temp);
		}
	}

	update();
}

void MainView::deleteMulFace(int CursorX, int CursorY) {
	GLdouble modelview[16]; //var to hold the modelview info
	GLdouble projection[16]; //var to hold the projection matrix info
	GLint viewport[4]; //var to hold the viewport info
	GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
	GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //get the modelview info
	glGetDoublev(GL_PROJECTION_MATRIX, projection); //get the projection matrix info
	glGetIntegerv(GL_VIEWPORT, viewport); //get the viewport info

	winX = (GLfloat)(CursorX);
	winY = (GLfloat)(viewport[3] - CursorY);
	glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	if (winZ != 1) {
		gluUnProject(winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);	

		OMT::FHandle fh_temp;
		float sum = 0;
		float bigger1 = 100;
		float CenterX = 0, CenterY = 0, CenterZ = 0;

		double  error = DBL_MAX;

		/* Get mesh Instance */
		Tri_Mesh* mesh = meshObj->mesh;
		if (mesh != NULL) {
			/* Find face by Barycentric */
			for (OMT::FIter f_it = mesh->faces_begin(); f_it != mesh->faces_end(); f_it++) {
				for (OMT::FVIter fv_it = mesh->fv_iter(f_it); fv_it; ++fv_it) {
					CenterX = CenterX + mesh->point(fv_it).data()[0];
					CenterY = CenterY + mesh->point(fv_it).data()[1];
					CenterZ = CenterZ + mesh->point(fv_it).data()[2];
				}
				sum = (CenterX / 3 - worldX)*(CenterX / 3 - worldX) + (CenterY / 3 - worldY)*(CenterY / 3 - worldY) + (CenterZ / 3 - worldZ)*(CenterZ / 3 - worldZ);
				if (sum < bigger1) {
					bigger1 = sum;
					fh_temp = f_it.handle();
				}
				CenterX = 0;
				CenterY = 0;
				CenterZ = 0;
			}//end for

			/* Find that whether this face is in the select_mfh and delete it*/
			for (std::vector<OMT::FHandle>::iterator mfh_it = select_mfh.begin(); mfh_it != select_mfh.end(); ++mfh_it) {
				if (fh_temp == *mfh_it) {
					select_mfh.erase(mfh_it);
					break;
				}
			}
		}
	}

	update();
}

void MainView::addAllFace() {

	OMT::FHandle fh_temp;

	/*	Clear Vector */
	select_mfh.clear();

	/* Get mesh Instance */
	Tri_Mesh* mesh = meshObj->mesh;

	for (OMT::FIter f_it = mesh->faces_begin(); f_it != mesh->faces_end(); f_it++) {
		fh_temp = f_it.handle();
		select_mfh.push_back(fh_temp);
	}
	update();
}

void MainView::drawFaces() {
	OMT::FVIter fv_it;

	/* Get mesh */
	Tri_Mesh* mesh = meshObj->mesh;

	/* Draw face */
	glPushMatrix();	
	glBegin(GL_TRIANGLES);
	glColor3f(0.0, 1.0, 0.0);
	mesh->request_vertex_normals();
	for (int i = 0; i < select_mfh.size(); i++) {
		const double* np = mesh->normal(select_mfh[i]).data();
		for (fv_it = mesh->fv_iter(select_mfh[i]); fv_it; fv_it++) {
			double* vp = mesh->point(fv_it.handle()).data();
			double offsetFactor = 0.0001f;
			glVertex3d(vp[0] + np[0] * offsetFactor, vp[1] + np[1] * offsetFactor, vp[2] + np[2] * offsetFactor);
		}
	}
	mesh->release_vertex_normals();
	glEnd();
	glPopMatrix();
}

void MainView::loadTexture(QString fileName) {
	if (!Textures.isEmpty() && hasLoadTex) {
		Textures.pop_back();
		texfileNames.pop_back();
	}
		
	makeCurrent();
	QOpenGLTexture* texture = new QOpenGLTexture(QImage(fileName).mirrored());
	texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
	texture->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
	texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	Textures.push_back(texture);
	texfileNames.push_back(fileName);
	hasLoadTex = true;
}

float MainView::degree2rad(float degree) {
	return degree * M_PI / 180;
}

void MainView::addPatch() {
	select_mfh.clear();
	submeshObj->isInit = false;
	hasLoadTex = false;
	isTextureMap = false;
	textureIndex++;
	bool isDisplay = false;
	rotationAngles.push_back(rotationAngle);
	isDisplays.push_back(isDisplay);
	meshObjs.push_back(submeshObj);
	submeshObj = new MeshObj;
	update();
}

void  MainView::exportPatch(QString filePath) {
	std::string data = filePath.toStdString();

	for (int i = 0; i < meshObjs.size(); i++) {
		MeshObj* meshObj = meshObjs[i];
		Tri_Mesh* mesh = meshObj->mesh;

		std::stringstream ss;
		ss << i;
		std::string index = ss.str();
		std::string filename = data + "/" + index + ".obj";

		if (SaveFile(filename, mesh)) {
			FILE* fp;
			int index = filename.find(".obj");
			filename.replace(index, index + 3, ".txt");
			//std::cout << filename << std::endl;

			fp = fopen(filename.data() , "w");

			if (fp) {
				std::string pngpath = texfileNames[i].toStdString();
				float rotationAngle = rotationAngles[i];
				fprintf(fp, "%s\n", pngpath.data());
				fprintf(fp, "%f\n", rotationAngle);
			}
			else
				printf("txt failed\n");

			fclose(fp);
		}
		else
			printf("SaveFile Failed\n");
	}
}

void MainView::AddFrame() {
	time += 0.03f;
	update();
}