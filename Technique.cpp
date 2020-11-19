#include "Technique.h"
#include "Shader.h"
#include "Renderer.h"

Technique::Technique()
	:shader(nullptr)
{
}

Technique::~Technique()
{
}

void Technique::Use()
{
	shader->Use();
}
