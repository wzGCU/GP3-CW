#include "MainGame.h"
#include "Camera.h"
#include <iostream>
#include <string>

using namespace std;
MainGame::MainGame()
{
	_gameState = GameState::PLAY;
	Display* _gameDisplay = new Display(); //new display
}

MainGame::~MainGame()
{
}

void MainGame::run()
{
	initSystems(); 
	gameLoop();
}

void MainGame::initSystems()
{
	_gameDisplay.initDisplay(); 
	whistle = audioDevice.loadSound("..\\res\\bang.wav");
	//backGroundMusic = audioDevice.loadSound("..\\res\\background.wav");
	texture.load("..\\res\\bricks.jpg");
	rockMesh.loadModel("..\\res\\Rock1.obj");
	shipMesh.loadModel("..\\res\\R33.obj");
	missileMesh.loadModel("..\\res\\R33.obj");

	fogShader.init("..\\res\\fogShader.vert", "..\\res\\fogShader.frag"); //new shader
	toonShader.init("..\\res\\shaderToon.vert", "..\\res\\shaderToon.frag"); //new shader
	rimShader.init("..\\res\\shaderRim.vert", "..\\res\\shaderRim.frag");
	eMapping.init("..\\res\\shaderReflection.vert", "..\\res\\shaderReflection.frag");
	FBOShader.init("..\\res\\FBOShader.vert", "..\\res\\FBOShader.frag");
	
	gameAudio.addAudioTrack("..\\res\\background.wav");

	initModels(asteroid);

	geoShader.initGeo();

	myCamera.initCamera(glm::vec3(0, 0, -50), 70.0f, (float)_gameDisplay.getWidth()/_gameDisplay.getHeight(), 0.01f, 1000.0f);

	generateFBO(_gameDisplay.getWidth(), _gameDisplay.getHeight());

	createScreenQuad();

	counter = 1.0f;

	vector<std::string> faces
	{
		"..\\res\\skybox\\right.png",
		"..\\res\\skybox\\left.png",
		"..\\res\\skybox\\top.png",
		"..\\res\\skybox\\bottom.png",
		"..\\res\\skybox\\front.png",
		"..\\res\\skybox\\back.png"
	};

	skybox.init(faces);
}

void MainGame::createScreenQuad()
{
	float quadVertices[] = { 
		// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		 //positions   // texCoords
		//-1.0f,  1.0f,  0.0f, 1.0f,
		//-1.0f, -1.0f,  0.0f, 0.0f,
		// 1.0f, -1.0f,  1.0f, 0.0f,

		//-1.0f,  1.0f,  0.0f, 1.0f,
		// 1.0f, -1.0f,  1.0f, 0.0f,
		// 1.0f,  1.0f,  1.0f, 1.0f

		//// vertex attributes for a quad that fills the half of the screen
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f,  0.25f,  0.0f, 0.0f,
		-0.25f,  0.25f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		-0.25f,  0.25f,  1.0f, 0.0f,
		-0.25f,  1.0f,  1.0f, 1.0f
	};
	// cube VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //stride offset example
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

}


void MainGame::gameLoop()
{
	while (_gameState != GameState::EXIT)
	{
		gameAudio.playMusic();
		processInput();
		currentCamPos = myCamera.getPos();
		drawGame();
		updateDelta();
		for (int i = 0; i < sizeof(asteroid); i++)
		{
			collision(*asteroid[i].getPos(), asteroid[i].getSphereRadiusinGameObject(&rockMesh) * 3, shipMesh.getSpherePos(), shipMesh.getSphereRadius());
		}
		//playAudio(backGroundMusic, glm::vec3(0.0f,0.0f,0.0f));
		//cout << "x: " << myCamera.getPos().x << "y: " << myCamera.getPos().y << "z: " << myCamera.getPos().z << "\n";
	}
}

void MainGame::processInput()
{
	SDL_Event evnt;

	while (SDL_PollEvent(&evnt)) //get and process events
	{
		switch (evnt.type)
		{
		case SDL_MOUSEWHEEL:
			myCamera.MoveBack(evnt.wheel.y);
			break;
		default:
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (evnt.button.button)
			{
			case SDL_BUTTON_LEFT:
				//SDL_ShowSimpleMessageBox(0, "Mouse", "Left button was pressed!", _gameDisplay.getWindow());
				break;
			case SDL_BUTTON_RIGHT:
				//SDL_ShowSimpleMessageBox(0, "Mouse", "Right button was pressed!", _gameDisplay.getWindow());
				break;
			case SDL_BUTTON_MIDDLE:
				break;
			default:
				//SDL_ShowSimpleMessageBox(0, "Mouse", "Some other button was pressed!", window);
				break;
			}
		case SDL_KEYDOWN:
			/* Check the SDLKey values and move change the coords */
			switch (evnt.key.keysym.sym)
			{
			case SDLK_a:			
				ship.transformPositions((*ship.getPosition() + glm::vec3(speed,0.0,0.0) * deltaTime), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2, 0.2, 0.2));	
				//When A is pressed it moves the ship left
				break;
			case SDLK_w:
				ship.transformPositions((*ship.getPosition() + glm::vec3(0.0, speed, 0.0) * deltaTime), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2, 0.2, 0.2));	
				//When W is pressed it moves the ship up
				break;
			case SDLK_s:
				ship.transformPositions((*ship.getPosition() + glm::vec3(0.0, -speed, 0.0) * deltaTime), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2, 0.2, 0.2));
				//When S is pressed it moves the ship down
				break;
			case SDLK_d:
				ship.transformPositions((*ship.getPosition() + glm::vec3(-speed, 0.0, 0.0) * deltaTime), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2, 0.2, 0.2));
				//When D is pressed it moves the ship right
				break;
			case SDLK_q:
				ship.transformPositions((*ship.getPosition() + glm::vec3(0.0, 0.0, speed) * deltaTime), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2, 0.2, 0.2));
				break;
				//When Q is pressed it moves the ship further on the z axis
			case SDLK_e:
				ship.transformPositions((*ship.getPosition() + glm::vec3(0.0, 0.0, -speed) * deltaTime), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2, 0.2, 0.2));
				//When e is pressed it moves the ship backwards on the z axis
				break;
			case SDLK_LEFT:
				myCamera.MoveLeft(100.0f*deltaTime);
				//Moves the camera to the left
				break;
			case SDLK_RIGHT:
				myCamera.MoveRight(100.0f*deltaTime);
				//Moves the camera to the right
				break;
			case SDLK_UP:
				myCamera.MoveUp(1.0f);
				//Moves the camera up
				break;
			case SDLK_DOWN:
				myCamera.MoveDown(1.0f);
				//Moves the camera down
				break;

			case SDLK_BACKSPACE:
				//Shakes the camera
				if (shake)
					shake = false;
				else
					shake = true;
				break;
			default:
				break;
			case SDL_QUIT:
				_gameState = GameState::EXIT;
				break;
			}
		}

	}
	//ip.transformPositions(*transform.GetPos(), *ship.getTM().GetRot(), *ship.getTM().GetScale());
}

//Initialize the asteroids, the ship and the missiles.
void MainGame::initModels(GameObject*& asteroid)
{
	for (int i = 0; i < 20; ++i)
	{
		float rX = -1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		float rY= -1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		float rZ = -1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));

		asteroid[i].transformPositions(glm::vec3(2.0 * i * rX, 2.0 * i * rY, 2.0 * i * rX), glm::vec3(rX, rY, rZ), glm::vec3(1.1, 1.1, 1.1));
		asteroid[i].updateAsteroidSphere(&rockMesh);		
	}

	ship.transformPositions(glm::vec3(0.0, 0.0, -3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2,0.2,0.2));
	//Sets the starting position of the ship
	for (int i = 0; i < 20; ++i)
	{
		missiles[i].setActive(0);
	}
}
// Draws the asteroids
void MainGame::drawAsteriods()
{
	texture.Bind(0);
	eMapping.Bind();
	linkEmapping();

	glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, texture.getID());

	glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getID());

	for (int i = 0; i < 20; ++i)
	{
		asteroid[i].transformPositions(glm::vec3(*asteroid[i].getTM().GetPos()), glm::vec3(asteroid[i].getTM().GetRot()->x + deltaTime, asteroid[i].getTM().GetRot()->y + deltaTime, asteroid[i].getTM().GetRot()->z + deltaTime), glm::vec3(0.1, 0.1, 0.1));
		asteroid[i].draw(&rockMesh);
		asteroid[i].updateAsteroidSphere(&rockMesh);
		eMapping.Update(asteroid[i].getTM(), myCamera);
	}
}
//Draws the missiles.
void MainGame::drawMissiles()
{
	texture.Bind(0);
	rimShader.Bind();
	linkRimLighting();

	for (int i = 0; i < 20; ++i)
	{
		if (missiles[i].getActive())
		{
			missiles[i].draw(&missileMesh);
			missiles[i].update(&missileMesh);
			rimShader.Update(missiles[i].getTM(), myCamera);
		}
	}
}

void MainGame::fireMissiles() 
{
	/** CALL THIS FROM processInput()
	* Set the missle transform to the ship transform ONCE (initial conditions)
	* Set the missle to active (check it is not already active)
	* Update the tranform to move the missle along its forward vector(more advanced, use seek to target)
	* check for asteroid collision
	* handle asteroid collision
	*/
}
//Draws the ship
void MainGame::drawShip()
{
	toonShader.Bind();
	linkToon();

	ship.draw(&shipMesh);
	ship.update(&shipMesh);
	toonShader.Update(ship.getTM(), myCamera);
}

//Draws the skybox
void MainGame::drawSkyBox()
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.textureID);

	counter = counter + 0.02f;

	skybox.draw(&myCamera);

	myCamera.setPos(currentCamPos);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnd();
}

//Function to detect collision between two objects.
bool MainGame::collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad)
{
	float distance = glm::sqrt((m2Pos.x - m1Pos.x)*(m2Pos.x - m1Pos.x) + (m2Pos.y - m1Pos.y)*(m2Pos.y - m1Pos.y) + (m2Pos.z - m1Pos.z)*(m2Pos.z - m1Pos.z));

	if (distance  < (m1Rad + m2Rad))
	{
		audioDevice.setlistener(myCamera.getPos(), m1Pos); //add bool to mesh
		playAudio(whistle, m1Pos);
		cout << "collison";
		return true;
	}
	else
	{
		return false;
	}
}
//Plays audio
void MainGame::playAudio(unsigned int Source, glm::vec3 pos)
{
	
	ALint state; 
	alGetSourcei(Source, AL_SOURCE_STATE, &state);
	/*
	Possible values of state
	AL_INITIAL
	AL_STOPPED
	AL_PLAYING
	AL_PAUSED
	*/
	if (AL_PLAYING != state)
	{
		audioDevice.playSound(Source, pos);
	}
}
//Linking the fog shader
void MainGame::linkFogShader()
{
	//fogShader.setMat4("u_pm", myCamera.getProjection());
	//fogShader.setMat4("u_vm", myCamera.getProjection());
	fogShader.setFloat("maxDist", 20.0f);
	fogShader.setFloat("minDist", 0.0f);
	fogShader.setVec3("fogColor", glm::vec3(0.0f, 0.0f, 0.0f));
}
//Linking the toonShader
void MainGame::linkToon()
{
	toonShader.setVec3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
}
//Linknig the geoShader
void MainGame::linkGeo()
{
	float randX = ((float)rand() / (RAND_MAX));
	float randY = ((float)rand() / (RAND_MAX));
	float randZ = ((float)rand() / (RAND_MAX));
	// Frag: uniform float randColourX; uniform float randColourY; uniform float randColourZ;
	geoShader.setFloat("randColourX", randX);
	geoShader.setFloat("randColourY", randY);
	geoShader.setFloat("randColourZ", randZ);
	// Geom: uniform float time;
	geoShader.setFloat("time", counter);
}
//Linking Rim Lightning
void MainGame::linkRimLighting()
{
	glm::vec3 camDir;
	camDir = shipMesh.getSpherePos() - myCamera.getPos();
	camDir = glm::normalize(camDir);
	rimShader.setMat4("u_pm", myCamera.getProjection());
	rimShader.setMat4("u_vm", myCamera.getView());
	rimShader.setMat4("model", transform.GetModel());
	rimShader.setMat4("view", myCamera.getView());
	rimShader.setVec3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
}
//Linking Emapping
void MainGame::linkEmapping()
{
	eMapping.setMat4("model", asteroid[0].getModel());
	//eMapping.setVec3("cameraPos", myCamera.getPos());
}
//Updating the delta time.
void MainGame::updateDelta()
{
	LAST = NOW;
	NOW = SDL_GetPerformanceCounter();

	deltaTime = (float)((NOW - LAST) / (float)SDL_GetPerformanceFrequency());
}
//Binding FBO
void MainGame::bindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
}
//Unbinding FBO
void MainGame::unbindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//Generating FBO
void MainGame::generateFBO(float w, float h)
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// create a colorbuffer for attachment texture
	glGenTextures(1, &CBO);
	glBindTexture(GL_TEXTURE_2D, CBO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CBO, 0);

	// create a renderbuffer object for depth and stencil attachment 
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h); // use a single renderbuffer object for both a depth AND stencil buffer.
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it


// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		cout << "FRAMEBUFFER:: Framebuffer is complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//Rendering FBO
void MainGame::renderFBO()
{
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	FBOShader.Bind();
	if(shake)
	FBOShader.setFloat("time", counter);
	else
	FBOShader.setFloat("time", 1);
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, CBO);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
//Moving the camera to follow the player
void MainGame::moveCamera()
{
	myCamera.setLook(shipMesh.getSpherePos());
	myCamera.setPos(currentCamPos);
}
//Drawing the game
void MainGame::drawGame()
{
	_gameDisplay.clearDisplay(0.8f, 0.8f, 0.8f, 1.0f); //sets our background colour	

	bindFBO();

	drawAsteriods();
	drawShip();
	drawSkyBox();
	drawMissiles();

	unbindFBO();

	renderFBO();

	glEnable(GL_DEPTH_TEST);

	drawAsteriods();
	drawShip();
	drawSkyBox();
	drawMissiles();
	moveCamera();
		
	
	_gameDisplay.swapBuffer();		

} 

