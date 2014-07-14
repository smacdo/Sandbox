#pragma once
#include <Windows.h> // todo: hate having to include Windows.h, maybe use a PIMPL pattern?
#include <vector>

#include "Frustum.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Dx3d;
class Camera;
class Model;
class Text;
class Light;
class LightShader;

class Graphics
{
public:
	Graphics();
	~Graphics();
	void Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	void Frame();		// terrible name

private:
	void Render(float rotation);
    void RenderUi();

private:
	bool mInitialized;
    Frustum mFrustum;
	Dx3d * mpD3d;
	Camera * mpCamera;
    Camera * mpUiCamera;
    Text * mpText;
    std::vector<Model *> mModels;
    LightShader * mpLightShader;
    Light * mpLight;
};

