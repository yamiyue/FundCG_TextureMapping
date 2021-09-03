#ifndef APPMAIN_H
#define APPMAIN_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QFileDialog>
#include "ui_AppMain.h"
#include <QtOpenGL/qgl.h>
#include "MainView.h" 
#include "SubView.h"

class AppMain : public QMainWindow 
{
	Q_OBJECT

public:
	AppMain(QWidget *parent = 0);
	~AppMain();

	static AppMain *getInstance();
	static AppMain *Instance;

public:


public:

	// the widgets that make up the Window
	MainView* mainview;
	SubView* subview;
	
	bool g_bLeftMouseDown = false;
	bool g_bRightMouseDown = false;
	bool g_bAltButtonDown = false;
	QTimer *myTimer;

	clock_t procstart = 0;
	clock_t procend = 0;

	enum SelectMode {
		Add,Delete
	};

	SelectMode selectMode = SelectMode::Add;

private:	
	Ui::AppMainClass ui;

	private slots:
		void LoadObjPath();
		void LoadPatchPath();
		void ExitApp();

		void ChangetoAddFaceMode();
		void ChangetoDeleteFaceMode();

		void SelectAllFace();
		void Parameterazation();
		void LoadTexture();
		void TextureMap();
		void ClearPatch();
		void RotatePatch();
		void AddPatch();
		void DeletePatch();
		void ExportPatch();

		void onListItemClicked(QListWidgetItem*);
		void addframe();

protected:
	bool eventFilter(QObject *watched, QEvent *e); 
};

#endif // APPMAIN_H
