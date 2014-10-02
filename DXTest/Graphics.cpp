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

using namespace DirectX::SimpleMath;

Graphics::Graphics()
: mInitialized(false),
  mFrustum(),
  mpD3d(nullptr),
  mpCamera(nullptr),
  mpUiCamera(nullptr),
  mpText(nullptr),
  mModels(),
  mpLightShader(nullptr),
  mpLight(nullptr)
{
}

Graphics::~Graphics()
{
	Shutdown();
}

void Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	if (mInitialized)
	{
		return;
	}

	// Initialize DirectX D3D.
	mpD3d = new Dx3d();
	mpD3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

	// Initialize the camera.
	mpCamera = new Camera();
	mpCamera->SetPosition(Vector3(0.0f, 0.0f, -10.0f));

    // Set up a base view matrix for 2d user interface rendering.
    //  TODO: Rename referneces to "Base view matrix" to "2d ui view matrix" or "ui view matrix".
    mpUiCamera = new Camera();
    
    mpUiCamera->SetPosition(Vector3(0.0f, 0.0f, -1.0f));
    mpUiCamera->Render();
    
    Matrix baseViewMatrix = mpUiCamera->ViewMatrix();

	// Create the text manager class.
	mpText = new Text();
    mpText->Initialize(mpD3d->GetDevice(),
                       mpD3d->GetDeviceContext(),
                       screenWidth, screenHeight,
                       baseViewMatrix);

    // Initialize and load models.
    for (auto i : MakeRange(0, 25))
    {
        Model *pModel = new Model();
        pModel->Initialize(mpD3d->GetDevice(), ".\\Models\\cube.model", ".\\Textures\\seafloor.dds");

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
    mpLightShader = new LightShader();
    mpLightShader->Initialize(mpD3d->GetDevice());

    mpLight = new Light();

    mpLight->SetAmbientColor(Vector4(0.15f, 0.15f, 0.15f, 1.0f));
    mpLight->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mpLight->SetSpecularColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mpLight->SetSpecularPower(32.0f);

    mpLight->SetDirection(Vector3(0.0f, 0.0f, 1.0f));

	mInitialized = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Shutdown the graphics system.
///////////////////////////////////////////////////////////////////////////////////////////////////
void Graphics::Shutdown()
{
	if (mInitialized)
	{
        SafeDelete(mpLight);
        SafeDelete(mpLightShader);
        SafeDeleteContainer(mModels);
		SafeDelete(mpText);
		SafeDelete(mpCamera);
        SafeDelete(mpUiCamera);

		mpD3d->Shutdown();
		SafeDelete(mpD3d);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Render the current graphics frame. Draws the current 3d scene and UI.
///////////////////////////////////////////////////////////////////////////////////////////////////
void Graphics::Frame()
{
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
	AssertNotNull(mpD3d);
    AssertNotNull(mpCamera);

	// Clear graphics buffers before beginning scene rendering.
	mpD3d->BeginScene(0.0f, 0.0f, 0.25f, 1.0f);

	// Generate the view matrix based on the camera's position. Grab the world, view and projection matrices from the
	// camera and d3d objects.
	mpCamera->Render();

	Matrix viewMatrix = mpCamera->ViewMatrix();
	Matrix worldMatrix = mpD3d->GetWorldMatrix();
	Matrix projectionMatrix = mpD3d->GetProjectionMatrix();

    // Rotate the world a little bit to show off.
//    worldMatrix = Matrix::CreateRotationY(rotation) * worldMatrix;

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
        pModel->Render(mpD3d->GetDeviceContext());

        // Render the model using the color shader.
        mpLightShader->Render(
            mpD3d->GetDeviceContext(),
            pModel->GetIndexCount(),
            objectWorldMatrix,
            viewMatrix,
            projectionMatrix,
            pModel->GetTexture(),
            *mpCamera,
            *mpLight);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Render current UI.
///////////////////////////////////////////////////////////////////////////////////////////////////
void Graphics::RenderUi()
{
    Matrix worldMatrix = mpD3d->GetWorldMatrix();
    Matrix orthoMatrix = mpD3d->GetOrthoMatrix();

    mpD3d->EnableZBuffer(false);
    mpD3d->EnableAlphaBlending(true);

    mpText->Render(mpD3d->GetDeviceContext(), worldMatrix, orthoMatrix);

    mpD3d->EnableZBuffer(true);
    mpD3d->EnableAlphaBlending(false);

    mpD3d->EndScene();
}