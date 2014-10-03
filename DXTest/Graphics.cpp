#include "Graphics.h"
#include "DXSandbox.h"
#include "Utils.h"
#include "Dx3d.h"
#include "Range.h"

#include "camera.h"
#include "Model.h"
#include "LightShader.h"
#include "Light.h"
#include "Text.h"
#include "SimpleMath.h"
#include "size.h"

using namespace DirectX::SimpleMath;

Graphics::Graphics()
: mFrustum(),
  mD3d(),
  mCamera(),
  mUiCamera(),
  mText(),
  mModels(),
  mLightShader(),
  mLight()
{
}

Graphics::~Graphics()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Shutdown the graphics system.
///////////////////////////////////////////////////////////////////////////////////////////////////
void Graphics::Initialize(const Size& screenSize, HWND hwnd)
{
    if (IsInitialized()) { return; }

	// Initialize DirectX D3D.
    mD3d.reset(new Dx3d());
    mD3d->Initialize(
        screenSize,
        hwnd,
        VSYNC_ENABLED,
        FULL_SCREEN,
        SCREEN_DEPTH,
        SCREEN_NEAR);

    mD3d->SetBackgroundColor(Color(0.0f, 0.0f, 0.25f));

	// Initialize the camera.
	mCamera.reset(new Camera(screenSize, SCREEN_NEAR, SCREEN_DEPTH));
	mCamera->SetPosition(Vector3(0.0f, 0.0f, -10.0f));

    // Set up a base view matrix for 2d user interface rendering.
    //  TODO: Rename referneces to "Base view matrix" to "2d ui view matrix" or "ui view matrix".
    mUiCamera.reset(new Camera(screenSize, SCREEN_NEAR, SCREEN_DEPTH));
    
    mUiCamera->SetPosition(Vector3(0.0f, 0.0f, -1.0f));
    mUiCamera->Render();

	// Create the text manager class.
	mText.reset(new Text());
    mText->Initialize(mD3d->GetDevice(),
                      mD3d->GetDeviceContext(),
                      screenSize.width, screenSize.height,
                      mUiCamera->ViewMatrix());

    // Initialize and load models.
    for (auto i : MakeRange(0, 25))
    {
        Model *pModel = new Model();
        pModel->Initialize(mD3d->GetDevice(), L".\\Models\\cube.model", L".\\Textures\\seafloor.dds");

        // Assign random position and color.
        Vector4 color(Utils::RandFloat(), Utils::RandFloat(), Utils::RandFloat(), 1.0f);
        Vector3 position(Utils::RandFloat(0.0f, 6.0f), Utils::RandFloat(2.0f, 8.0f), Utils::RandFloat(-3.0f, -1.0f));
//        Vector3 position(Utils::RandFloat(-10.0f, 0.0f), Utils::RandFloat(-10.0f, 0.0f), Utils::RandFloat(-6.0f, -1.0f));
//        Vector3 position(0.0f, 0.0f, -1);

  //      position = Vector3(0.0f, -3.0f, 0.0f);

        pModel->SetColor(color);
        pModel->SetPosition(position);

        // Store for later rendering.
        mModels.push_back(pModel);
    }

    // Create a light and a light shader for the model.
    mLightShader.reset(new LightShader());
    mLightShader->Initialize(mD3d->GetDevice());

    mLight.reset(new Light());

    mLight->SetAmbientColor(Vector4(0.15f, 0.15f, 0.15f, 1.0f));
    mLight->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mLight->SetSpecularColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mLight->SetSpecularPower(32.0f);

    mLight->SetDirection(Vector3(0.0f, 0.0f, 1.0f));

    SetInitialized();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Shutdown the graphics system.
///////////////////////////////////////////////////////////////////////////////////////////////////
void Graphics::OnShutdown()
{
    SafeDeleteContainer(mModels);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Render the current graphics frame. Draws the current 3d scene and UI.
///////////////////////////////////////////////////////////////////////////////////////////////////
void Graphics::Frame()
{
    if (!IsInitialized()) { return; }

    const float pi = 3.14159f;
    const float halfPi = 3.14159f / 2.0f;
    static float rotation = 0.0f;
    static bool isForward = true;

    rotation += halfPi * 0.01f * (isForward ? 1.0f : -1.0f);

    if (rotation > pi) { rotation = pi; isForward = false; }
    else if (rotation < 0) { rotation = 0.0f; isForward = true; }

	Render(rotation);
    RenderUi();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Render current scene.
///////////////////////////////////////////////////////////////////////////////////////////////////
void Graphics::Render(float rotation)
{
    if (!IsInitialized()) { return; }

	// Clear graphics buffers before beginning scene rendering.
	mD3d->BeginScene();

	// Generate the view matrix based on the camera's position. Grab the world, view and projection matrices from the
	// camera and d3d objects.
	mCamera->Render();

	Matrix viewMatrix = mCamera->ViewMatrix();
    Matrix worldMatrix = DirectX::XMMatrixIdentity();
	Matrix projectionMatrix = mCamera->ProjectionMatrix();

    // Rotate the world a little bit to show off.
    worldMatrix = Matrix::CreateRotationY(rotation) * worldMatrix;

    // Update camera view frustum before proceeding with rendering.
    mFrustum.Update(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	//// Put the model's vertex and index buffers on the graphics pipeline to prepare them for drawing.
    for (Model *pModel : mModels)
    {
        AssertNotNull(pModel);
        Model& model = *pModel;

        // Don't render the model if it is disabled.
        if (!model.Enabled())
        {
            continue;
        }

        // Don't render the model if it is not visible to the camera frustum.
 /*       if (!mFrustum.CheckSphere(model.BoundingSphereCenter(), model.BoundingSphereRadius()))
        {
            continue;
        }*/

        // Move the object to the correct location for rendering.
        //  TODO: Does this belong somewhere else? Honestly all this terrible rendering code from rasterk needs to
        //        be burned in a fire and refactored.
        Matrix objectWorldMatrix = Matrix::CreateTranslation(pModel->Position()) * worldMatrix;

        // This is really a "bind buffers for rendering" method call.
        pModel->Render(mD3d->GetDeviceContext());

        // Render the model using the color shader.
        mLightShader->Render(
            mD3d->GetDeviceContext(),
            pModel->IndexCount(),
            objectWorldMatrix,
            viewMatrix,
            projectionMatrix,
            pModel->GetTexture(),
            *mCamera,
            *mLight);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Render current UI.
///////////////////////////////////////////////////////////////////////////////////////////////////
void Graphics::RenderUi()
{
    Matrix worldMatrix = DirectX::XMMatrixIdentity();
    Matrix orthoMatrix = mUiCamera->OrthoMatrix();

    mD3d->SetZBufferEnabled(false);
    mD3d->SetAlphaBlendingEnabled(true);

    mText->Render(mD3d->GetDeviceContext(), worldMatrix, orthoMatrix);

    mD3d->SetZBufferEnabled(true);
    mD3d->SetAlphaBlendingEnabled(false);

    mD3d->EndScene();
}