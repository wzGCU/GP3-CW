#pragma once
#include <SDL\SDL.h>
#include <SDL/SDL_mixer.h>
#include <GL/glew.h>
#include "Display.h" 
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "transform.h"
#include "SkyBox.h"
#include "GameObject.h"
#include "SDLAudio.h"
#include "Audio.h"

enum class GameState{PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:

	void initSystems();
	void processInput();
	void gameLoop();
	void drawGame();
	void linkFogShader();
	void linkToon();
	void linkRimLighting();
	void linkGeo();
	void linkEmapping();

	void drawSkyBox();
	void drawAsteriods();
	void drawShip();
	void drawMissiles();

	void fireMissiles();

	void initModels(GameObject*& asteroid);
	bool collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad);

	void moveCamera();
	void updateDelta();

	void createScreenQuad();

	void generateFBO(float w, float h);
	void bindFBO();
	void unbindFBO();
	void renderFBO();

	void playAudio(unsigned int Source, glm::vec3 pos);

	Display _gameDisplay;
	GameState _gameState;
	Mesh rockMesh;
	Mesh shipMesh;
	Mesh missileMesh;
	Camera myCamera;
	Shader fogShader;
	Shader toonShader;
	Shader rimShader;
	Shader geoShader;
	Shader shaderSkybox;
	Shader eMapping;
	Shader FBOShader;
	SDLAudio gameAudio;

	Transform transform;
	GameObject* asteroid = new GameObject[20];
	GameObject* missiles = new GameObject[20];
	GameObject ship;
	Texture texture;

	GLuint FBO;
	GLuint RBO;
	GLuint CBO;

	GLuint quadVAO;
	GLuint quadVBO;


	glm::vec3 currentCamPos;

	Skybox skybox;

	vector<std::string> faces;
	
	Audio audioDevice;
	bool look = true;
	bool shake = false;
	unsigned int whistle;
	double speed = 100;
	float counter;
	float velocity = 10;
	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	float deltaTime = 0;
};

