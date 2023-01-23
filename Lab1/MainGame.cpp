#include "MainGame.h"
#include "Camera.h"
#include <iostream>
#include <string>

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
	cout << "Loading Systems... \n";
	_gameDisplay.initDisplay();

	texture.load("..\\res\\bricks.jpg");
	
	meshAsteroid.loadModel("..\\res\\Rock1.obj");
	meshSpaceship.loadModel("..\\res\\R33.obj");
	meshMissile.loadModel("..\\res\\monkey3.obj");

	shaderFog.init("..\\res\\fogShader.vert", "..\\res\\fogShader.frag");
	shaderToon.init("..\\res\\shaderToon.vert", "..\\res\\shaderToon.frag");
	shaderRim.init("..\\res\\shaderRim.vert", "..\\res\\shaderRim.frag");
	shaderReflection.init("..\\res\\shaderReflection.vert", "..\\res\\shaderReflection.frag");
	shaderFBO.init("..\\res\\FBOShader.vert", "..\\res\\FBOShader.frag");
	shaderGeo.initGeo();

	audioBackgroundMusic = audioDevice.loadSound("..\\res\\background.wav");
	audioBoom = audioDevice.loadSound("..\\res\\bang.wav");


	initModels(asteroids);

	myCamera.initCamera(glm::vec3(0, -10, -50), 70.0f, (float)_gameDisplay.getWidth()/_gameDisplay.getHeight(), 0.01f, 1000.0f);

	generateFBO(_gameDisplay.getWidth(), _gameDisplay.getHeight());

	createScreenQuad();

	counter = 1.0f;

	vector<std::string> facesAir
	{
		"..\\res\\skybox\\right.png",
		"..\\res\\skybox\\left.png",
		"..\\res\\skybox\\top.png",
		"..\\res\\skybox\\bottom.png",
		"..\\res\\skybox\\front.png",
		"..\\res\\skybox\\back.png"
	};
	vector<std::string> facesLand
	{
		"..\\res\\skybox\\right1.jpg",
		"..\\res\\skybox\\left1.jpg",
		"..\\res\\skybox\\top1.jpg",
		"..\\res\\skybox\\bottom1.jpg",
		"..\\res\\skybox\\front1.jpg",
		"..\\res\\skybox\\back1.jpg"
	};


	skybox.init(facesAir);
}

void MainGame::createScreenQuad()
{
	if (smallFilter) 
	{
		float quadVertices[] = {
			// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			 //positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f,  0.25f,  0.0f, 0.0f,
		-0.25f,  0.25f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		-0.25f,  0.25f,  1.0f, 0.0f,
		-0.25f,  1.0f,  1.0f, 1.0f

		};
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
	else
	{
		float quadVertices[] = {
			// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			 //positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f

		};
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
	// cube VAO

}

void MainGame::gameLoop()
{
	while (_gameState != GameState::EXIT)
	{
		processInput();
		currentCamPosition = myCamera.getPos();
		drawGame();
		updateDelta();
		playAudio(audioBackgroundMusic, glm::vec3(0.0f,0.0f,0.0f));
		if (first) 
		{
			firstTimeSetup();
		}
		checkGameOver();
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
			myCamera.MoveBack(evnt.wheel.y*2);
			break;
		default:
			break;
			/*
			* Asteroid Camera Mode
			* Left/Right Mouse Button - Change to next/previous asteroid
			* Middle Mouse Button - Change camera mode
			*/
		case SDL_MOUSEBUTTONDOWN:
			switch (evnt.button.button)
			{
			case SDL_BUTTON_LEFT:
				cameraAsteroidCount++;
				break;
			case SDL_BUTTON_RIGHT:
				cameraAsteroidCount--;
				break;
			case SDL_BUTTON_MIDDLE:
				if(cameraAsteroidMode){
					cameraAsteroidMode = false;
					myCamera.MoveBack(50.0f);
				}
				else {
					cameraAsteroidMode = true;
					myCamera.MoveForward(50.0f);
				}
				
				break;
			default:
				break;
			}
		case SDL_KEYDOWN:
			switch (evnt.key.keysym.sym)
			{
			/* 
			Ship Movement:
			W/S - Moving on Y axis
			A/D - Moving on X axis
			E/Q - Moving on Z axis
			*/
			case SDLK_w:
				ship.moveTo((*ship.getTPosition() + yMovement * deltaTime));
				break;
			case SDLK_s:
				ship.moveTo((*ship.getTPosition() - yMovement * deltaTime));
				break;
			case SDLK_a:
				ship.moveTo((*ship.getTPosition() + xMovement * deltaTime));
				break;
			case SDLK_d:
				ship.moveTo((*ship.getTPosition() - xMovement * deltaTime));
				break;
			case SDLK_e:
				ship.moveTo((*ship.getTPosition() + zMovement * deltaTime));
				break;
			case SDLK_q:
				ship.moveTo((*ship.getTPosition() - zMovement * deltaTime));
				break;
			/*
			Camera Movement:
			Left/Right Arrow - Moving on X axis
			Up/Down Arrow - Moving on Y axis
			Shift - Change speed
			*/
			case SDLK_LEFT:
				myCamera.MoveLeft(cameraSpeed*deltaTime);
				break;
			case SDLK_RIGHT:
				myCamera.MoveRight(cameraSpeed*deltaTime);
				break;
			case SDLK_UP:
				myCamera.MoveUp(cameraSpeed*deltaTime);
				break;
			case SDLK_DOWN:
				myCamera.MoveDown(cameraSpeed*deltaTime);
				break;
			case SDLK_LSHIFT:
				if (cameraSpeed == 10.0f) {
					cameraSpeed = 50.0f;
				}
				else 
				{
					cameraSpeed = 10.0f;
				}
				break;
			/*
			Action Keys:
			Space - Launch Missile
			Escape - Quit Game
			*/
			case SDLK_SPACE:
				if (canShoot) {
					if (missileLaunchNumber == 20) {

						SDL_ShowSimpleMessageBox(0, "Space Explorer - VICTORY", "You destroyed all rockets and survived! You got back to earth and now can travel around!", _gameDisplay.getWindow());
						
					}
					else
					{
						if (!gameOver) {
							fireMissiles();
						}
						
					}
				}
				break;
			case SDLK_ESCAPE:
				_gameState = GameState::EXIT;
				break;
			default:
				break;
			case SDL_QUIT:
				_gameState = GameState::EXIT;
				break;
			}
		}

	}
}

void MainGame::initModels(GameObject*& asteroids)
{
	for (int i = 0; i < 20; ++i)
	{
		float rX = -1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		float rY= -1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		float rZ = -1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));

		asteroids[i].moveTo(glm::vec3(5.0 * i * rX, 5.0 * i * rY, 5.0 * i * rZ));
		asteroids[i].setRotation(glm::vec3(rX, rY, rZ));
		asteroids[i].setScale(asteroidScale);
		asteroids[i].update(&meshAsteroid);
	}

	ship.moveTo(glm::vec3(0.0, 0.0, -3.0));
	ship.setRotation(zeroVector);
	ship.setScale(shipScale);
}

void MainGame::drawAsteriods()
{
	texture.Bind(0);
	shaderReflection.Bind();
	linkReflection();

	glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, texture.getID());

	glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getID());

	for (int i = 0; i < 20; ++i)
	{
		if (asteroids[i].getActive())
		{
			asteroids[i].moveTo(glm::vec3(*asteroids[i].getTM().GetPos()));
			asteroids[i].setRotation(glm::vec3(asteroids[i].getTM().GetRot()->x + deltaTime, asteroids[i].getTM().GetRot()->y + deltaTime, asteroids[i].getTM().GetRot()->z + deltaTime));
			asteroids[i].setScale(asteroidScale);
			asteroids[i].draw(&meshAsteroid);
			asteroids[i].update(&meshAsteroid);
			shaderReflection.Update(asteroids[i].getTM(), myCamera);
		}
	}
}

void MainGame::drawMissiles()
{
	texture.Bind(0);
	shaderRim.Bind();
	linkRimLighting();
	for (int i = 0; i < 20; ++i)
	{
		if (missiles[i].getActive()) {
			missiles[i].draw(&meshMissile);
			missiles[i].update(&meshMissile);
			shaderRim.Update(missiles[i].getTM(), myCamera);
		}

	}

	
}

void MainGame::fireMissiles() 
{
	missiles[missileLaunchNumber].moveTo(*ship.getTM().GetPos());
	missiles[missileLaunchNumber].setActive(1);
	canShoot = false;
	updateMissiles();
}

void MainGame::updateMissiles() 
{	
	
	while (canShoot == false)
	{
	
		glm::vec3 shootVector = *asteroids[missileLaunchNumber].getTPosition() - *missiles[missileLaunchNumber].getTPosition();
		glm::vec3 shootVel = normalize(shootVector) * missileSpeed; //* deltaTime;


		missiles[missileLaunchNumber].moveTo(*missiles[missileLaunchNumber].getTPosition()+shootVel);

		if (checkCollision(*missiles[missileLaunchNumber].getTPosition(), missileScaleFloat*0.01, *asteroids[missileLaunchNumber].getTPosition(), asteroidScaleFloat))
		{
			missiles[missileLaunchNumber].setRotation(glm::vec3(rand(), rand(), rand()));
			asteroids[missileLaunchNumber].setActive(0);
			playSound(audioBoom, *missiles[missileLaunchNumber].getTPosition());
			missileLaunchNumber++;
			if (missileLaunchNumber == 20) {

				victoryActions();
				canShoot = true;

			}
			else {
				cout << "destroyed asteroid " << missileLaunchNumber << ". \n";
				canShoot = true;
			}
						
		}
	}
}

void MainGame::victoryActions() {
	cout << "VICTORY!\n";
	smallFilter = false;
	createScreenQuad();

	for (int i = 0; i < 20; ++i)
	{
		missiles[i].setActive(false);
	}
	vector<std::string> facesLand
	{
		"..\\res\\skybox\\right1.jpg",
		"..\\res\\skybox\\left1.jpg",
		"..\\res\\skybox\\top1.jpg",
		"..\\res\\skybox\\bottom1.jpg",
		"..\\res\\skybox\\front1.jpg",
		"..\\res\\skybox\\back1.jpg"
	};


	skybox.init(facesLand);
}

void MainGame::checkGameOver()
{
	for (int i = 0; i < sizeof(asteroids); i++)
	{
		if (checkCollision(*asteroids[i].getTPosition(), asteroids[i].getSphereRadiusinGameObject(&meshAsteroid) * 3, meshSpaceship.getSpherePos(), meshSpaceship.getSphereRadius()))
		{
			if (asteroids[i].getActive()) {
				audioDevice.setlistener(myCamera.getPos(), meshSpaceship.getSpherePos());
				playAudio(audioBoom, meshSpaceship.getSpherePos());
				shakeCamera = true;
				life--;
				if (life == 0) {
					gameOver = true;
					SDL_ShowSimpleMessageBox(0, "Space Explorer - GAME OVER", "You lost the game! Press ESC to quit.", _gameDisplay.getWindow());
				}
			}

		}
	}
}

void MainGame::firstTimeSetup() {
	
		first = false;
		for (int i = 0; i < 20; ++i)
		{
			missiles[i].setActive(0);
		}
		SDL_ShowSimpleMessageBox(0, "Space Explorer", "Welcome to the game! \nDestroy all the asteroids to land on earth and win the game. \n\n* C O N T R O L S *\n\nQuit:\nEscape\n\nS H I P\n\nMovement:\nW, S, A, D, E, Q\n\nShoot Missile:\nSpace\n\n\nC A M E R A\n\nMovement:\nArrow Keys\n\nChange Speed:\nLeft Shift\n\nSwitch Between Ship and Asteroid Mode:\nMiddle Mouse Button\n\nChange Current Asteroid View (in Asteroid Camera Mode):\nLeft/Right Mouse Button", _gameDisplay.getWindow());

}

void MainGame::changeCamera()
{
	if (cameraAsteroidMode)
	{
		if (cameraAsteroidCount < 0)
		{
			cameraAsteroidCount = 19;
		}
		if (cameraAsteroidCount > 19)
		{
			cameraAsteroidCount = 0;
		}
		myCamera.setLook(*asteroids[cameraAsteroidCount].getTM().GetPos());
	}
	else
	{
		myCamera.setLook(meshSpaceship.getSpherePos());
	}

}

void MainGame::playSound(unsigned int Source, glm::vec3 pos)
{

	ALint state;
	alGetSourcei(Source, AL_SOURCE_STATE, &state);
	audioDevice.playSound(Source, pos);
}

void MainGame::drawShip()
{
	shaderToon.Bind();
	linkToon();

	ship.draw(&meshSpaceship);
	ship.update(&meshSpaceship);
	shaderToon.Update(ship.getTM(), myCamera);
}

void MainGame::drawSkyBox()
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.textureID);

	counter = counter + 0.02f;

	skybox.draw(&myCamera);

	myCamera.setPos(currentCamPosition);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnd();
}

bool MainGame::checkCollision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad)
{
	float distance = glm::sqrt((m2Pos.x - m1Pos.x)*(m2Pos.x - m1Pos.x) + (m2Pos.y - m1Pos.y)*(m2Pos.y - m1Pos.y) + (m2Pos.z - m1Pos.z)*(m2Pos.z - m1Pos.z));

	if (distance < (m1Rad + m2Rad))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void MainGame::playAudio(unsigned int Source, glm::vec3 pos)
{
	ALint state; 
	alGetSourcei(Source, AL_SOURCE_STATE, &state);
	if (AL_PLAYING != state)
	{
		audioDevice.playSound(Source, pos);
	}
}

void MainGame::linkFogShader()
{
	//fogShader.setMat4("u_pm", myCamera.getProjection());
	//fogShader.setMat4("u_vm", myCamera.getProjection());
	shaderFog.setFloat("maxDist", 20.0f);
	shaderFog.setFloat("minDist", 0.0f);
	shaderFog.setVec3("fogColor", glm::vec3(0.0f, 0.0f, 0.0f));
}

void MainGame::linkToon()
{
	shaderToon.setVec3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
}

void MainGame::linkGeo()
{
	float randX = ((float)rand() / (RAND_MAX));
	float randY = ((float)rand() / (RAND_MAX));
	float randZ = ((float)rand() / (RAND_MAX));
	// Frag: uniform float randColourX; uniform float randColourY; uniform float randColourZ;
	shaderGeo.setFloat("randColourX", randX);
	shaderGeo.setFloat("randColourY", randY);
	shaderGeo.setFloat("randColourZ", randZ);
	// Geom: uniform float time;
	shaderGeo.setFloat("time", counter);
}

void MainGame::linkRimLighting()
{
	glm::vec3 camDir;
	camDir = meshSpaceship.getSpherePos() - myCamera.getPos();
	camDir = glm::normalize(camDir);
	shaderRim.setMat4("u_pm", myCamera.getProjection());
	shaderRim.setMat4("u_vm", myCamera.getView());
	shaderRim.setMat4("model", transform.GetModel());
	shaderRim.setMat4("view", myCamera.getView());
	shaderRim.setVec3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
}

void MainGame::linkReflection()
{
	shaderReflection.setMat4("model", asteroids[0].getModel());
	//eMapping.setVec3("cameraPos", myCamera.getPos());
}

void MainGame::updateDelta()
{
	LAST = NOW;
	NOW = SDL_GetPerformanceCounter();

	deltaTime = (float)((NOW - LAST) / (float)SDL_GetPerformanceFrequency());

}

void MainGame::bindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
}

void MainGame::unbindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

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

void MainGame::renderFBO()
{
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	shaderFBO.Bind();
	if(shakeCamera)
	shaderFBO.setFloat("time", counter); else
		shaderFBO.setFloat("time", 1);
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, CBO);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void MainGame::drawGame()
{
	_gameDisplay.clearDisplay(0.8f, 0.8f, 0.8f, 1.0f); //sets our background colour	

	bindFBO();

	drawAsteriods();
	drawShip();
	drawSkyBox();
	drawMissiles();
	changeCamera();

	unbindFBO();

	renderFBO();

	glEnable(GL_DEPTH_TEST);

	drawAsteriods();
	drawShip();
	drawSkyBox();
	drawMissiles();
	
	_gameDisplay.swapBuffer();		

} 

