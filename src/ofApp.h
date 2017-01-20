// magicLab 2016
// http://www.magiclab.nyc
// author: enrico viola
// license: GPL v.3

#pragma once

#include "ofMain.h"
#include "ofxMotive.h"
#include "ofxXmlSettings.h"
#include "NPTrackingTools.h"
#include "ofxUI.h"

class ofApp : public ofBaseApp{

	public:
		enum AppScreen {
			SCREEN_SETUP,
			SCREEN_VIZ
		};

		enum DrawMode {
			DRAW_VOXELS,
			DRAW_HULL
		};

		void setup();
		void update();
		void draw();
		void drawViz();

		void setupGui();
		void onGui(ofxUIEventArgs & e);
		void onVizGui(ofxUIEventArgs & e);
		void reload();
		void save();
		void getTTPPath() { getPath(pthTTP, "set ttp path", laTTP); }
		void getCALPath() { getPath(pthCAL, "set cal path", laCAL); }
		void getPath(string & _pth, string title, ofxUILabel * la);
		void export3DHull();
		void addDronePlaceHolders(ofVboMesh & msh);
		void addBox(ofVboMesh & msh, float sz, ofVec3f pos);

		void calc();
		void goViz();
		void goVoxels();
		void goHull();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
		ofxUICanvas * gui;
		ofxUICanvas * guiViz;
		ofxUINumberDialer * nuVoxelSz;
		ofxUINumberDialer * nuMinCams;
		ofxUILabelButton * butCalc;
		ofxUILabelButton * butLoad;
		ofxUILabelButton * butSave;
		ofxUILabelButton * butTTP;
		ofxUILabelButton * butCAL;
		ofxUILabel * laTTP;
		ofxUILabel * laCAL;

		ofxUILabelButton * butHull;
		ofxUILabelButton * butBack;
		ofxUILabelButton * butSaveHull;

		ofEasyCam cam;
		ofxMotive motive;
		AppScreen curScreen;
		DrawMode curDrawMode;
		ofVec3f phOrder;
		vector<ofVec4f> exportRots;

		string pthTTP;
		string pthCAL;
};
