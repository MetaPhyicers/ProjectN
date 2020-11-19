#include "Engine.h"
#include "Windows.h"
#include "Renderer.h" 
#include "Camera.h"
#include "Resources.h" 
#include "Model.h"

#include "Pannel.hpp"


Engine engine;
Graphic::Renderer* renderer;
Camera camera;

Pannel* pannel;
Model* model;

bool InitObject(Window& window)
{
	pannel = new Pannel(&window);
	model = Model::LoadModel(L"nanosuit/nanosuit.obj", renderer);
	if (model == nullptr) {
		return false;
	}

	// init camera
	camera.InitSpeed(0.1f, 0.2f);

	return true;
}

void ReleaseObject()
{
	SafeDelete(pannel);
	SafeDelete(model);
}

void Update(float dt)
{
	pannel->Update(dt);

	static glm::mat4 perspective = glm::perspective(glm::radians(45.f), 1280.f / 720.f, 0.1f, 100.f);
	glm::mat4 projectionView = perspective * camera.GetViewMatrix(dt);

	glm::mat4 modelMat(1.f);
	model->SetProjectionView(projectionView);
	model->SetModel(modelMat);

}

void KeyCallback(int key, int action)
{
	switch (key)
	{
	case GLFW_KEY_UP:
		camera.GoForward();
		break;

	case GLFW_KEY_DOWN:
		camera.GoBack();
		break;

	case GLFW_KEY_LEFT:
		camera.GoLeft();
		break;

	case GLFW_KEY_RIGHT:
		camera.GoRight();
		break;

	default:
		break;
	}
}

void mouseCallBack(int button, int action, double xpos, double ypos, double scrollXOffset, double scrollYOffset)
{
	static double lastXPos = 1280.0 / 2.0;
	static double lastYPos = 720.0 / 2.0;

	double xOffset = xpos - lastXPos;
	double yOffset = lastYPos - ypos;

	lastXPos = xpos;
	lastYPos = ypos;

	camera.YawAndPitch(xOffset, yOffset);

	pannel->gui->MouseInputGLFW(button, action, static_cast<long>(xpos), static_cast<long>(ypos),
		static_cast<long>(xOffset), static_cast<long>(yOffset));
}

int main() {
	Window* window = engine.CreateWindowX(1280, 720);
	window->SetFullScreen(false);

	window->SetKeyBoradFunc(KeyCallback);
	window->SetMouseFunc(mouseCallBack);

	///< initialize renderer
	renderer = window->GetRenderer();
	renderer->SetUpdateCallBack(Update);


	///< initialize render list
	if (!InitObject(*window)) {
		return -1;
	}

	engine.Running();

	ReleaseObject();

	return 0;
}