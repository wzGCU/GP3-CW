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

	//Shader Functions
	void linkFogShader();
	void linkToon();
	void linkRimLighting();
	void linkGeo();
	void linkReflection();

	//FBO Functions
	void generateFBO(float w, float h);
	void bindFBO();
	void unbindFBO();
	void renderFBO();
	void createScreenQuad();

	//Model Functions
	void drawSkyBox();
	void drawAsteriods();
	void drawShip();
	void drawMissiles();
	void initModels(GameObject*& asteroids);

	//Audio Functions
	void playAudio(unsigned int Source, glm::vec3 pos);
	void playSound(unsigned int Source, glm::vec3 pos);

	//Game Functions
	void initSystems();
	void processInput();
	void gameLoop();
	void drawGame();
	void updateDelta();
	bool checkCollision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad);
	void changeCamera();
	void firstTimeSetup();
	void victoryActions();
	void checkGameOver();

	//Missiles Functions
	void fireMissiles();
	void updateMissiles();

	//System variables
	Display _gameDisplay;
	GameState _gameState;
	Camera myCamera;
	Shader shaderFog;
	Shader shaderToon;
	Shader shaderRim;
	Shader shaderGeo;
	Shader shaderSkybox;
	Shader shaderReflection;
	Shader shaderFBO;

	GLuint FBO;
	GLuint RBO;
	GLuint CBO;
	GLuint quadVAO;
	GLuint quadVBO;

	Skybox skybox;

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	float deltaTime = 0;

	Transform transform;
	Texture texture;

	Audio audioDevice;
	float counter;

	glm::vec3 currentCamPosition;
	glm::vec3 zeroVector = glm::vec3(0.0, 0.0, 0.0);

	unsigned int audioBoom;
	unsigned int audioBackgroundMusic;

	//Meshes and GameObjects
	Mesh meshAsteroid;
	Mesh meshMissile;
	Mesh meshSpaceship;

	GameObject* asteroids = new GameObject[20];
	GameObject* missiles = new GameObject[20];
	GameObject ship;

	//Game Objects Variables
	float asteroidScaleFloat = 0.05f;
	glm::vec3 asteroidScale = glm::vec3(asteroidScaleFloat, asteroidScaleFloat, asteroidScaleFloat);

	float missileSpeed = 0.01f;
	float missileScaleFloat = 0.1f;
	glm::vec3 missileScale = glm::vec3(missileScaleFloat, missileScaleFloat, missileScaleFloat);

	float shipSpeed = 20;
	glm::vec3 shipScale = glm::vec3(0.2, 0.2, 0.2);

	glm::vec3 xMovement = glm::vec3(shipSpeed, 0.0, 0.0);
	glm::vec3 yMovement = glm::vec3(0.0, shipSpeed, 0.0);
	glm::vec3 zMovement = glm::vec3(0.0, 0.0, shipSpeed);

	float cameraSpeed = 10.0f;
	//Game Variables
	bool gameOver;//if gameOver does not allow for shoot
	int missileLaunchNumber = 0;	//which missile to send
	bool canShoot = true;			//if can shoot
	bool shakeCamera = false;		//if the camera will shake (disabled due to possible accessibility issues, can be enabled by changing to true
	bool smallFilter = true;		//if the FBO filter will be small or big, needs to be changed manually

	bool cameraAsteroidMode = false; //camera mode variable (space vs asteroid focused)
	int cameraAsteroidCount; //which asteroid should camera focus

	bool first = true; //a boolean used to show the welcome message (could not make it less intrusive way sorry xD)

	int life = 1;//if player hits asteroid the  game  is over

};

