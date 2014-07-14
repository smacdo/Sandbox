#pragma once

/// XXX: Get rid of these heavy weight headers.
#include <SimpleMath.h>
#include <string>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
class Texture;

// TODO: TERRIBLE TERRIBLE
// TODO: Remove loading of texture in this class, pass in Texture object.
class Model
{
public:
	Model();
	virtual ~Model();

	void Initialize(ID3D11Device* pDevice,
                    const std::string& modelFile,
                    const std::string& textureFile);
	void Shutdown();
	void Render(ID3D11DeviceContext* pContext);

	int GetIndexCount() const;
	ID3D11ShaderResourceView * GetTexture();

    DirectX::SimpleMath::Vector3 Position() const { return mPosition; }
    void SetPosition(const DirectX::SimpleMath::Vector3& position) { mPosition = position; }

    DirectX::SimpleMath::Vector4 Color() const { return mColor; }
    void SetColor(const DirectX::SimpleMath::Vector4& color) { mColor = color; }

    bool Enabled() const { return mEnabled; }
    void SetEnabled(bool isEnabled) { mEnabled = isEnabled; }

    DirectX::SimpleMath::Vector3 BoundingSphereCenter() const { return mPosition; }
    float BoundingSphereRadius() const { return mBoundingSphereRadius; }

private:
	void InitializeBuffers(ID3D11Device *pDevice);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext *pContext);
	void LoadTexture(ID3D11Device *pDevice, const std::string& filepath);
	void ReleaseTexture();
    void LoadModel(const std::string& filepath);
    void LoadTxtModelv1(const std::string& filepath);
    void LoadTxtModelv2(const std::string& filepath);
    void ReleaseModel();

private:
	Model(const Model&);
	Model& operator= (const Model&);

private:
    // in-memory software mesh format.
    struct s_mesh_vertex_t
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

private:
	bool mInitialized;
    bool mEnabled;
	ID3D11Buffer *mpVertexBuffer;
	ID3D11Buffer *mpIndexBuffer;
	int mVertexCount;
	int mIndexCount;
    s_mesh_vertex_t * mpMesh;
    int * mpMeshIndices;
	Texture *mpTexture;
    
    DirectX::SimpleMath::Vector3 mPosition;
    DirectX::SimpleMath::Vector4 mColor;
    float mBoundingSphereRadius;
};

