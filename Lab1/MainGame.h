#pragma once
#include <SDL\SDL.h>
#include <GL/glew.h>
#include "Display.h" 
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "transform.h"
#include "Audio.h"
#include "SkyBox.h"
#include "GameObject.h"

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

	void fireMissiles(int i);

	void initModels(GameObject*& asteroid);
	void setCameraTarget();
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

	Transform transform;
	Transform transform1;
	GameObject asteroid1;
	GameObject asteroid2;
	GameObject* asteroid = new GameObject[numberOfAsteroids];
	GameObject* missiles = new GameObject[numberOfMissiles];
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
	float counter;
	bool shake = false;

	double speed = 100;
	float velocity = 10;
	glm::vec3 xMovement = glm::vec3(speed, 0.0, 0.0);
	glm::vec3 yMovement = glm::vec3(0.0, speed, 0.0);
	glm::vec3 zMovement = glm::vec3(0.0, 0.0, speed);
	glm::vec3 shipRotation = glm::vec3(0.0, 0.0, 0.0);
	float shipScaleF = 0.2f;
	//glm::vec3 shipScale = glm::vec3(0.2, 0.2, 0.2);
	glm::vec3 rockScale = glm::vec3(0.05, 0.05, 0.05);


	unsigned int whistle;
	unsigned int backGroundMusic;

	int numberOfAsteroids = 50;
	int numberOfMissiles = 20;
	int missileLaunchNumber = 0;
	float deltaTime = 0;
	float deltaTimeConversion = 1;
	float deltaTimeDisplay = 0;

	float missileVelocity = 2;
	float missileSpeed;
	float asteroidVelocity = 15;
	float asteroidSpeed;
	float shipVelocity = 20;
	float shipSpeed;
	float shipRotationVelocity = 3;
	float shipRotationSpeed;

	int cameraType = 0;
	int asteroidCamTarget = 0;
	float cameraVelocity = 15;
	float cameraSpeed;
	float cameraRotationVelocity = 20;
	float cameraRotationSpeed;
	float cameraOffset = 10;
	float theta = 0;

	float randLow = -1;
	float randHigh = 1;

	float asteroidScale = 0.05;
	float shipScale = 0.5;
	float missileScale = 0.1;

	bool deltaInSeconds = true;
	string deltaTimeString = "s";
	string controls;

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
};

