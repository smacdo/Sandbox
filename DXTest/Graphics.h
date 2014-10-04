#pragma once
#include <Windows.h> // todo: hate having to include Windows.h, maybe use a PIMPL pattern?
#include <vector>
#include <memory>

#include "Frustum.h"
#include "IInitializable.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Dx3d;
class Camera;
class Model;
class UiTextRenderer;
class Light;
class LightShader;
class Size;

class Graphics : public IInitializable
{
public:
	Graphics();
    Graphics(const Graphics&) = delete;
	virtual ~Graphics();

    Graphics& operator = (const Graphics&) = delete;

    void Initialize(const Size& screenSize, HWND hwnd);
	void Frame();		// terrible name

protected:
    virtual void OnShutdown() override;

private:
	void Render(float rotation);
    void RenderUi();

private:
    Frustum mFrustum;
	std::unique_ptr<Dx3d> mD3d;
	std::unique_ptr<Camera> mCamera;
    std::unique_ptr<Camera> mUiCamera;
    std::unique_ptr<UiTextRenderer> mUiTextRenderer;
    std::vector<Model *> mModels;       // TODO: Make use unique_ptr or something.
    std::unique_ptr<LightShader> mLightShader;
    std::unique_ptr<Light> mLight;
};

