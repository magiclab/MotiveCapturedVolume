// magicLab 2016
// http://www.magiclab.nyc
// author: enrico viola
// license: GPL v.3

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofSetWindowTitle("[Magic.Lab]MotiveTool");

	curScreen = SCREEN_SETUP;

	cam.setNearClip(0.01);
	cam.setDistance(15);
	
	//cam.enableOrtho();

	phOrder.set(1,2,0);
	exportRots.clear();
	ofxXmlSettings xmlCS;
	if (xmlCS.load("xml/coord_spaces.xml")) {
		if (xmlCS.tagExists("coords")) {
			xmlCS.pushTag("coords");
			if (xmlCS.tagExists("placeholders")) {
				xmlCS.pushTag("placeholders");
				phOrder.x = xmlCS.getValue("x",0);
				phOrder.y = xmlCS.getValue("y", 1);
				phOrder.z = xmlCS.getValue("z", 2);
				xmlCS.popTag();
			}
			int numRot = xmlCS.getNumTags("rot");
			for (int i = 0; i < numRot; i++) {
				ofVec4f r;
				r.w = xmlCS.getAttribute("rot", "angle", 90.0, i);
				r.x = 0;
				r.y = 0;
				r.z = 0;
				int aId = xmlCS.getAttribute("rot","axis",0,i);
				r[aId] = 1;
				exportRots.push_back(r);
			}
			xmlCS.popTag();
		}
		else {
			ofLogError("XML error:", "malformed misc.xml");
		}
	}
	else {
		ofLogError("XML error:", "cannot load misc.xml");
	}

	setupGui();
	int volumeMode = 0; //0 -> rays 1-> voxels
	ofxXmlSettings xml;
	if (xml.load("xml/misc.xml")) {
		if (xml.tagExists("misc")) {
			xml.pushTag("misc");
			nuVoxelSz->setValue(xml.getValue("voxel", 0.3));
			nuMinCams->setValue(xml.getValue("min_cameras", 1));
			pthTTP = xml.getValue("ttp", ofFilePath::getAbsolutePath("motive/magicLab.ttp"));
			pthCAL = xml.getValue("ttp", ofFilePath::getAbsolutePath("motive/magicLab.cal"));
			//motive.setVoxelSize(xml.getValue("voxel", 0.3));
			//motive.setMinNumCameras(xml.getValue("min_cameras",1));
			//motive.loadRayLengths(xml);
			volumeMode = xml.getValue("volume_mode",0);
			xml.popTag();
		}
		else {
			ofLogError("XML error:", "malformed misc.xml");
		}
	}
	else {
		ofLogError("XML error:","cannot load misc.xml");
	}
	reload();
	/*
	motive.setup(ofFilePath::getAbsolutePath("motive/magicLab.ttp"), ofFilePath::getAbsolutePath("motive/magicLab.cal"));
	motive.setupRayCasting();
	motive.raycastAndFlagVoxels();
	motive.saveVoxels();
	*/
	//motive.bDrawVoxels = false;

}

void ofApp::setupGui() {
	gui = new ofxUICanvas(0,0,  ofGetWidth(), ofGetHeight());
	gui->addWidgetDown(new ofxUILabel("Magic.Lab", OFX_UI_FONT_LARGE));
	gui->addWidgetDown(new ofxUILabel("-Motive Tool-", OFX_UI_FONT_MEDIUM));

	guiViz = new ofxUICanvas(0,0,200,100);

	float ww = 180;
	float hw = ww / 2;

	butCalc = new ofxUILabelButton("compute volume", false, ww);
	butLoad = new ofxUILabelButton("reload", false, hw);
	butSave = new ofxUILabelButton("save", false, hw);
	nuVoxelSz = new ofxUINumberDialer(0.1, 1.0, 0.3, 2, "m", OFX_UI_FONT_MEDIUM);
	nuMinCams = new ofxUINumberDialer(1,6,1,0,"", OFX_UI_FONT_MEDIUM);
	butCAL = new ofxUILabelButton("change CAL",false,ww);
	butTTP = new ofxUILabelButton("change TTP",false,ww);

	laTTP = new ofxUILabel(ofFilePath::getAbsolutePath("motive/magicLab.ttp"), OFX_UI_FONT_SMALL);
	laCAL = new ofxUILabel(ofFilePath::getAbsolutePath("motive/magicLab.cal"), OFX_UI_FONT_SMALL);

	butHull = new ofxUILabelButton("compute hull",false);
	butBack = new ofxUILabelButton("back",false);
	butSaveHull = new ofxUILabelButton("export model",false);

	gui->addWidgetDown(butLoad);
	gui->addWidgetRight(butSave);
	gui->addWidgetDown(new ofxUILabel("Path TTP:", OFX_UI_FONT_MEDIUM));
	gui->addWidgetDown(laTTP);
	gui->addWidgetDown(butTTP);
	gui->addWidgetDown(new ofxUILabel("Path CAL:", OFX_UI_FONT_MEDIUM));
	gui->addWidgetDown(laCAL);
	gui->addWidgetDown(butCAL);
	gui->addWidgetDown(new ofxUILabel("Voxel:", OFX_UI_FONT_MEDIUM));
	gui->addWidgetRight(nuVoxelSz);
	gui->addWidgetDown(new ofxUILabel("Min cams:", OFX_UI_FONT_MEDIUM));
	gui->addWidgetRight(nuMinCams);
	gui->addWidgetDown(butCalc);

	ofAddListener(gui->newGUIEvent, this, &ofApp::onGui);
	ofAddListener(guiViz->newGUIEvent, this, &ofApp::onVizGui);

	guiViz->setVisible(false);
}

void ofApp::reload() {
	ofxXmlSettings xml;
	if (xml.load("xml/misc.xml")) {
		if (xml.tagExists("misc")) {
			xml.pushTag("misc");
			nuVoxelSz->setValue(xml.getValue("voxel",0.3));
			nuMinCams->setValue(xml.getValue("min_cameras", 1));
			pthTTP = xml.getValue("ttp", ofFilePath::getAbsolutePath("motive/magicLab.ttp"));
			pthCAL = xml.getValue("cal", ofFilePath::getAbsolutePath("motive/magicLab.cal"));
			laCAL->setLabel(pthCAL);
			laTTP->setLabel(pthTTP);
			xml.popTag();
		}
		else {
			ofLogError("XML error:", "malformed misc.xml");
		}
	}
	else{
		ofLogError("XML error:", "cannot load misc.xml");
	}
}

void ofApp::save() {
	ofxXmlSettings xml;
	if (xml.load("xml/misc.xml")) {
		if (xml.tagExists("misc")) {
			xml.pushTag("misc");
			xml.setValue("voxel", nuVoxelSz->getValue());
			xml.setValue("min_cameras", int(nuMinCams->getValue()));
			xml.setValue("ttp" , pthTTP);
			xml.setValue("cal", pthCAL);
			xml.popTag();
		}
		else {
			ofLogError("XML error:", "malformed misc.xml");
		}

		xml.save("xml/misc.xml");
	}
	else {
		ofLogError("XML error:", "cannot load misc.xml");
	}
}

void ofApp::addBox(ofVboMesh & msh, float sz, ofVec3f pos) {
	float hSz = sz / 2;
	float minX = pos.x-hSz;
	float maxX = pos.x+hSz;
	float minY = pos.y-hSz;
	float maxY = pos.y+hSz;
	float minZ = pos.z-hSz;
	float maxZ = pos.z+hSz;

	//bottom
	msh.addVertex(ofVec3f(minX, minY,minZ));
	msh.addVertex(ofVec3f(maxX, minY, minZ));
	msh.addVertex(ofVec3f(maxX, minY, maxZ));

	msh.addVertex(ofVec3f(maxX, minY, maxZ));
	msh.addVertex(ofVec3f(minX, minY, maxZ));
	msh.addVertex(ofVec3f(minX, minY, minZ));

	//left
	msh.addVertex(ofVec3f(minX, maxY, minZ));
	msh.addVertex(ofVec3f(minX, minY, minZ));
	msh.addVertex(ofVec3f(minX, minY, maxZ));

	msh.addVertex(ofVec3f(minX, minY, maxZ));
	msh.addVertex(ofVec3f(minX, maxY, maxZ));
	msh.addVertex(ofVec3f(minX, maxY, minZ));

	//right
	msh.addVertex(ofVec3f(maxX, maxY, minZ));
	msh.addVertex(ofVec3f(maxX, minY, minZ));
	msh.addVertex(ofVec3f(maxX, minY, maxZ));

	msh.addVertex(ofVec3f(maxX, minY, maxZ));
	msh.addVertex(ofVec3f(maxX, maxY, maxZ));
	msh.addVertex(ofVec3f(maxX, maxY, minZ));

	//front
	msh.addVertex(ofVec3f(minX, maxY, maxZ));
	msh.addVertex(ofVec3f(minX, minY, maxZ));
	msh.addVertex(ofVec3f(maxX, minY, maxZ));

	msh.addVertex(ofVec3f(maxX, minY, maxZ));
	msh.addVertex(ofVec3f(maxX, maxY, maxZ));
	msh.addVertex(ofVec3f(minX, maxY, maxZ));

	//back
	msh.addVertex(ofVec3f(minX, maxY, maxZ));
	msh.addVertex(ofVec3f(minX, minY, maxZ));
	msh.addVertex(ofVec3f(maxX, minY, maxZ));

	msh.addVertex(ofVec3f(maxX, minY, minZ));
	msh.addVertex(ofVec3f(maxX, maxY, minZ));
	msh.addVertex(ofVec3f(minX, maxY, minZ));
}

void ofApp::addDronePlaceHolders(ofVboMesh & msh) {
	//blender x,y,z -> y,z,x
	float sz = 0.3;
	float hSz = sz / 2;
	addBox(msh, sz, ofVec3f(1.0, -hSz, -0.5));
	addBox(msh, sz, ofVec3f(-1.0, -hSz, -0.5));
	addBox(msh, sz, ofVec3f(0.0, -hSz, 0.5));
}

void ofApp::export3DHull() {
	ofFileDialogResult dr = ofSystemSaveDialog("hull.ply", "Export Volume as...");
	if (dr.bSuccess) {
		ofMesh msh;
		vector<ofVec3f> vtx = motive.getVboVolume().getVertices();
		if (exportRots.size() > 0) {
			cout << "Applying " << exportRots.size() << " rotations" << endl;
			for (int i = 0; i < exportRots.size(); i++) {
				for (int j = 0; j < vtx.size(); j++) {
					vtx[j].rotate(exportRots[i].w, ofVec3f(exportRots[i].x, exportRots[i].y, exportRots[i].z));
				}
			}
		}
		msh.addVertices(vtx);
		msh.save(dr.getPath());
		//motive.getVboVolume().save(dr.getPath());
	}
}

void ofApp::getPath(string & _pth, string title, ofxUILabel * la) {
	ofFileDialogResult dr = ofSystemLoadDialog(title, false, _pth);
	if (dr.bSuccess) {
		_pth = dr.getPath();
		la->setLabel(_pth);
	}
}

void ofApp::calc() {
	
	motive.setVoxelSize(nuVoxelSz->getValue());
	motive.setMinNumCameras(int(nuMinCams->getValue()));
	ofxXmlSettings xml;
	if (xml.load("xml/misc.xml")) {
		if (xml.tagExists("misc")) {
			xml.pushTag("misc");
			motive.loadRayLengths(xml);
			xml.popTag();
		}
	}
	motive.setup(pthTTP, pthCAL);
	motive.setupRayCasting();
	motive.raycastAndFlagVoxels();
	motive.saveVoxels();
	goViz();
}

void ofApp::goViz() {
	gui->setVisible(false);
	goVoxels();
	curScreen = SCREEN_VIZ;
}

void ofApp::goVoxels() {
	guiViz->setVisible(true);
	guiViz->clearWidgets();
	guiViz->addWidgetDown(new ofxUILabel("Voxels", OFX_UI_FONT_LARGE));
	guiViz->addWidgetDown(butHull);
	curDrawMode = DRAW_VOXELS;
}

void ofApp::goHull() {
	motive.computeVolumeHull();
	addDronePlaceHolders(motive.getVboVolume());
	guiViz->setVisible(true);
	guiViz->clearWidgets();
	guiViz->addWidgetDown(new ofxUILabel("Convex Hull", OFX_UI_FONT_LARGE));
	guiViz->addWidgetDown(butSaveHull);
	guiViz->addWidgetDown(butBack);
	curDrawMode = DRAW_HULL;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw() {
	if (curScreen == SCREEN_VIZ) {
		drawViz();
	}
}

void ofApp::drawViz(){
	cam.begin();
	ofDrawAxis(0.5);
	motive.drawCameras();
	motive.drawSpace();
	motive.drawRays();
	switch (curDrawMode)
	{
	case DRAW_VOXELS:
		motive.drawVoxels();
	case DRAW_HULL:
		motive.drawVolumeHull();
	default:
		break;
	}
	cam.end();

	ofPushStyle();
	ofSetColor(ofColor::white);
	ofDrawBitmapString(ofToString(motive.getNumCameras())+" cameras\nVoxel size: "+ofToString(motive.getVoxelSize(),2), 20, ofGetHeight()-40);
	ofPopStyle();
}

void ofApp::onGui(ofxUIEventArgs & e) {
	if (e.widget == butSave) {
		if (butSave->getValue()) {
			save();
		}
	}
	else if (e.widget == butLoad) {
		if (butLoad->getValue()) {
			reload();
		}
	}
	else if (e.widget == butTTP) {
		if (butTTP->getValue()) {
			getTTPPath();
		}
	}
	else if (e.widget == butCAL) {
		if (butCAL->getValue()) {
			getCALPath();
		}
	}
	else if (e.widget == butCalc) {
		if (butCalc->getValue()) {
			calc();
		}
	}
	else if (e.widget == nuVoxelSz) {
	
	}
	else if (e.widget == nuMinCams) {
	}
}

void ofApp::onVizGui(ofxUIEventArgs & e) {
	if (e.widget == butHull) {
		if (butHull->getValue()) {
			goHull();
		}
	}
	else if (e.widget == butBack) {
		if (butBack->getValue()) {
			goVoxels();
		}
	}
	else if (e.widget == butSaveHull) {
		if (butSaveHull->getValue()) {
			export3DHull();
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
