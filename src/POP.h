#pragma once

#include "ofMain.h"
#include "PingPong.h"

class POP
{
public:

	POP();

	void setup(unsigned particleCount);
	void update();
	void draw();

	void loadTextureData(unsigned idx, float* data, unsigned x = 0, unsigned y = 0, unsigned width = 0, unsigned height = 0);
	void zeroTextureData(unsigned idx, unsigned x = 0, unsigned y = 0, unsigned width = 0, unsigned height = 0);

	void updateKinectData(ofTexture& tex);
	
	void loadShaders();

	ofFbo::Settings getFboSettings(unsigned numBuffers = 1) const;

	enum DataTextureIndex
	{
		POSITION,
		VELOCITY,
		COLOR,
		STATE,
		NUM_TEXTURES
	};

private:

	unsigned textureLocation;
	unsigned width, height;

	PingPong pp;

	ofVboMesh popMesh;
	ofVboMesh quadMesh;

	ofShader updateShader;
	ofShader drawShader;

	ofTexture kTex;
};