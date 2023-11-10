#include "Sky.h"

Sky::Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler, Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	geometryMesh = mesh;
	sampleState = sampler;
	this->device = device;
	
}

Sky::~Sky()
{
}
