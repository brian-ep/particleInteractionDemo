#include "POP.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
POP::POP() : textureLocation(0), width(0), height(0) {}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void POP::setup(unsigned particleCount) {

	//	Set texture size
	width  = sqrt(particleCount);
	height = width;

	//	Allocate fbos
	ofFbo::Settings s = getFboSettings(NUM_TEXTURES);
	pp.src->allocate(s);
	pp.dst->allocate(s);

	//	Setup POP mesh
	popMesh.clear();
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			popMesh.addVertex(glm::vec3(0, 0, 0));
			popMesh.addTexCoord(glm::vec2(x, y));
		}
	}
	popMesh.setMode(OF_PRIMITIVE_POINTS);

	//	Setup screen quad mesh
	quadMesh.addVertex(glm::vec3(-1.0f, -1.0f, 0.0f));
	quadMesh.addVertex(glm::vec3( 1.0f, -1.0f, 0.0f));
	quadMesh.addVertex(glm::vec3( 1.0f,  1.0f, 0.0f));
	quadMesh.addVertex(glm::vec3(-1.0f,  1.0f, 0.0f));

	quadMesh.addTexCoord(glm::vec2( 0.0f,   0.0f));
	quadMesh.addTexCoord(glm::vec2(width,   0.0f));
	quadMesh.addTexCoord(glm::vec2(width, height));
	quadMesh.addTexCoord(glm::vec2( 0.0f, height));

	quadMesh.addIndex(0);
	quadMesh.addIndex(1);
	quadMesh.addIndex(2);
	quadMesh.addIndex(0);
	quadMesh.addIndex(2);
	quadMesh.addIndex(3);

	quadMesh.setMode(OF_PRIMITIVE_TRIANGLES);

	//	Initialize position texture
	float* posData = new float[width * height * 4];
	for (unsigned y = 0; y < height; ++y) {
		for (unsigned x = 0; x < width; ++x) {
			unsigned idx = y * width + x;
			posData[idx * 4 + 0] = ofRandom(0, ofGetWidth());
			posData[idx * 4 + 1] = ofRandom(0, ofGetHeight());
			posData[idx * 4 + 2] = 0;
			posData[idx * 4 + 3] = 1;
		}
	}
	loadTextureData(POSITION, posData);
	delete[] posData;

	//	Initialize velocity data
	float* velData = new float[width * height * 4];
	for (unsigned y = 0; y < height; ++y) {
		for (unsigned x = 0; x < width; ++x) {
			unsigned idx = y * width + x;
			velData[idx * 4 + 0] = 0;
			velData[idx * 4 + 1] = 0;
			velData[idx * 4 + 2] = 0;
			velData[idx * 4 + 3] = ofRandom(0.4, 1.0);
		}
	}
	loadTextureData(VELOCITY, velData);
	delete[] velData;

	//	Initialize color data
	float* colData = new float[width * height * 4];
	for (unsigned y = 0; y < height; ++y) {
		for (unsigned x = 0; x < width; ++x) {
			unsigned idx = y * width + x;

			float blue = ofRandom(0.1, 0.7);
			float red = ofRandom(0.7 * blue, 0.9 * blue);
			float green = ofRandom(0.2 * red, 0.5 * red);

			colData[idx * 4 + 0] = red;
			colData[idx * 4 + 1] = green;
			colData[idx * 4 + 2] = blue;
			colData[idx * 4 + 3] = 0.0;
		}
	}
	loadTextureData(COLOR, colData);
	delete[] colData;

	//	Initialize state
	float* staData = new float[width * height * 4];
	for (unsigned y = 0; y < height; ++y) {
		for (unsigned x = 0; x < width; ++x) {
			unsigned idx = y * width + x;

			staData[idx * 4 + 0] = 1.0;
			staData[idx * 4 + 1] = 0.0;
			staData[idx * 4 + 2] = 0.0;
			staData[idx * 4 + 3] = 0.0;
		}
	}
	loadTextureData(STATE, staData);
	delete[] staData;

	//	Load shaders
	updateShader.load("shaders/updatePOP");
	drawShader.load("shaders/drawPOP");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void POP::update() {
	
	pp.dst->begin();
	
	//	Set draw state
	ofPushStyle();
	ofSetColor(255, 255, 255);
	pp.dst->activateAllDrawBuffers();

	glPushAttrib(GL_ENABLE_BIT);
	glViewport(0, 0, width, height);
	glDisable(GL_BLEND);

	//	Execute shader
	updateShader.begin();
	updateShader.setUniformTexture("posIn", pp.src->getTexture(POSITION), POSITION);
	updateShader.setUniformTexture("velIn", pp.src->getTexture(VELOCITY), VELOCITY);
	updateShader.setUniformTexture("colIn", pp.src->getTexture(COLOR), COLOR);
	updateShader.setUniformTexture("stateIn", pp.src->getTexture(STATE), STATE);
	updateShader.setUniformTexture("kData", kTex, NUM_TEXTURES);
	updateShader.setUniform1f("elapsed", ofGetLastFrameTime());
	updateShader.setUniform2f("res", ofGetWidth(), ofGetHeight());
	updateShader.setUniform2f("mouse", ofGetMouseX(), ofGetMouseY());
	quadMesh.draw();
	updateShader.end();

	//	Reset draw state
	ofPopStyle();

	pp.dst->end();
	pp.swap();

	glPopAttrib();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void POP::draw() {

	//	Set GL settings
	glPointSize(6);

	//	Execute shader
	drawShader.begin();
	drawShader.setUniformTexture("posIn", pp.src->getTexture(POSITION), POSITION);
	drawShader.setUniformTexture("velIn", pp.src->getTexture(VELOCITY), VELOCITY);
	drawShader.setUniformTexture("colIn", pp.src->getTexture(COLOR), COLOR);
	drawShader.setUniformTexture("stateIn", pp.src->getTexture(STATE), STATE);
	drawShader.setUniform2f("res", ofGetWidth(), ofGetHeight());
	popMesh.draw();
	drawShader.end();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void POP::loadTextureData(unsigned idx, float * data, unsigned x, unsigned y, unsigned width, unsigned height) {

	if (idx < pp.src->getNumTextures()) {
		if (!width)  width	= this->width;
		if (!height) height = this->height;

		pp.src->getTexture(idx).bind();
		glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, x, y, width, height, GL_RGBA, GL_FLOAT, data);
		pp.src->getTexture(idx).unbind();
	}
	else ofLogError("POP::loadTextureData") << "Trying to load data into a non-existent buffer...";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void POP::zeroTextureData(unsigned idx, unsigned x, unsigned y, unsigned width, unsigned height) {

	if (idx < pp.src->getNumTextures()) {
		if (!width)  width = this->width;
		if (!height) height = this->height;

		float* zeros = new float[width * height * 4];
		memset(zeros, 0, sizeof(float) * width * height * 4);
		loadTextureData(idx, zeros, x, y, width, height);
		delete[] zeros;
	}
	else ofLogError("POP::loadTextureData") << "Trying to load data into a non-existent buffer...";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void POP::updateKinectData(ofTexture& tex) {
	kTex = tex;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void POP::loadShaders() {

	updateShader.unload();
	updateShader.load("shaders/updatePOP");

	drawShader.unload();
	drawShader.load("shaders/drawPOP");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ofFbo::Settings POP::getFboSettings(unsigned numBuffers) const {
	
	//	Check that setup function has been called
	if (!width || !height) {
		ofLogFatalError("POP::getFboSettings") << "setup function needs to be called first";
		return ofFbo::Settings();
	}

	//	Create fbo settings
	ofFbo::Settings s;
	s.internalformat		= GL_RGBA32F_ARB;
	s.textureTarget			= GL_TEXTURE_RECTANGLE_ARB;
	s.minFilter				= GL_NEAREST;
	s.maxFilter				= GL_NEAREST;
	s.wrapModeHorizontal	= GL_CLAMP;
	s.wrapModeVertical		= GL_CLAMP;
	s.width					= width;
	s.height				= height;
	s.numColorbuffers		= numBuffers;
	return s;
}