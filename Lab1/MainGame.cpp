#include "MainGame.h"
#include "Camera.h"
#include <iostream>
#include <string>

//using namespace std;
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
	backGroundMusic = audioDevice.loadSound("..\\res\\background.wav");
	texture.load("..\\res\\bricks.jpg");
	rockMesh.loadModel("..\\res\\Rock1.obj");
	shipMesh.loadModel("..\\res\\R33.obj");
	missileMesh.loadModel("..\\res\\R33.obj");

	fogShader.init("..\\res\\fogShader.vert", "..\\res\\fogShader.frag"); //new shader
	toonShader.init("..\\res\\shaderToon.vert", "..\\res\\shaderToon.frag"); //new shader
	rimShader.init("..\\res\\shaderRim.vert", "..\\res\\shaderRim.frag");
	eMapping.init("..\\res\\shaderReflection.vert", "..\\res\\shaderReflection.frag");
	FBOShader.init("..\\res\\FBOShader.vert", "..\\res\\FBOShader.frag");

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
	/*
	
		"..\\res\\skybox\\right1.jpg",
		"..\\res\\skybox\\left1.jpg",
		"..\\res\\skybox\\top1.jpg",
		"..\\res\\skybox\\bottom1.jpg",
		"..\\res\\skybox\\front1.jpg",
		"..\\res\\skybox\\back1.jpg"
	*/
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
		processInput();
		currentCamPos = myCamera.getPos();
		drawGame();
		updateDelta();
		collision(rockMesh.getSpherePos(), rockMesh.getSphereRadius(), shipMesh.getSpherePos(), shipMesh.getSphereRadius());
		playAudio(backGroundMusic, glm::vec3(0.0f,0.0f,0.0f));
		setCameraTarget();
		/*for (int i = 0; i < sizeof(asteroid); i++)
		{
			collision(*asteroid[i].getPos(), asteroid[i].getSphereRadiusinGameObject(&rockMesh) * 3, shipMesh.getSpherePos(), shipMesh.getSphereRadius());
		}*/
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
		case SDL_KEYDOWN:
			/* Check the SDLKey values and move change the coords */
			switch (evnt.key.keysym.sym)
			{
			case SDLK_a:
				ship.yaw(-shipRotationSpeed);
				break;
			case SDLK_d:
				ship.yaw(shipRotationSpeed);
				break;
			case SDLK_w:
				ship.moveForward(shipSpeed);
				break;
			case SDLK_s:
				ship.moveForward(-shipSpeed);
				break;
		/*	case SDLK_e:
				ship.transformPositions((*ship.getPosition() + glm::vec3(0.0, 0.0, -speed) * deltaTime), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2, 0.2, 0.2));
				break;
			case SDLK_q:
				ship.transformPositions((*ship.getPosition() + glm::vec3(0.0, 0.0, speed) * deltaTime), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2, 0.2, 0.2));
				break;*/
			case SDLK_LEFT:
				myCamera.MoveLeft(150.0f*deltaTime);
				cout << myCamera.getPos().x;
				break;
			case SDLK_RIGHT:
				myCamera.MoveRight(150.0f*deltaTime);
				break;
			case SDLK_UP:
				myCamera.MoveUp(1.0f);
				break;
			case SDLK_DOWN:
				myCamera.MoveDown(1.0f);
				break;
			case SDLK_SPACE:
				missiles[missileLaunchNumber].transformPositions(*ship.getTM().GetPos() + (ship.rght() * 1.0f), *ship.getTM().GetRot(), glm::vec3(0.05, 0.05, 0.05));
				missiles[missileLaunchNumber].setActive(1);
				missileLaunchNumber++;
				break;
			case SDLK_BACKSPACE:
				//Shakes the camera
				shake = !shake;
				break;
			case SDLK_ESCAPE:
				_gameState = GameState::EXIT;
				break;
				/////////Miscellaneous///////////
				//Cycle delta time denomination
			case SDLK_TAB:
					deltaInSeconds = !deltaInSeconds;
					break;
					/////////Camera Controls///////////
//Rotate Around
			case SDLK_j:
				theta -= cameraRotationSpeed;
				break;
			case SDLK_l:
						theta += cameraRotationSpeed;
						break;
			case SDLK_c:
					{
						cameraType = 1;
						break;
					}
			case SDL_QUIT:
				_gameState = GameState::EXIT;
				break;
					/*if (keys[SDL_SCANCODE_F])
					{
						cameraType = 2;
					}
					if (keys[SDL_SCANCODE_T])
					{
						cameraType = 3;
					}
					if (keys[SDL_SCANCODE_Q])
					{
						cameraType = 4;
					}
					if (keys[SDL_SCANCODE_E])
					{
						cameraType = 5;
					}
					//Cycle Target
					if (keys[SDL_SCANCODE_X])
					{
						cout << asteroidCamTarget << "\n";
						asteroidCamTarget += 1;
					}
					if (keys[SDL_SCANCODE_Z])
					{
						cout << asteroidCamTarget << "\n";
						asteroidCamTarget -= 1;
						}
						*/
			default:
				break;
			}
		}

	}
}

void MainGame::initModels(GameObject*& asteroid)
{
	for (int i = 0; i < numberOfAsteroids; ++i)
	{
		float rX = -1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		float rY= -1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));
		float rZ = -1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0 - -1.0)));

		asteroid[i].transformPositions(glm::vec3(0.4 * i * rX, 0.4 * i * rY, 0.4 * i * rZ), glm::vec3(rX, rY, rZ), glm::vec3(asteroidScale, asteroidScale, asteroidScale));
		asteroid[i].updateAsteroidSphere(&rockMesh);
	}

	ship.transformPositions(glm::vec3(0.0, 0.0, -3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2,0.2,0.2));
	ship.update(&shipMesh);

	
	for (int i = 0; i < numberOfMissiles; i++)
	{
		missiles[i].setActive(0);
	}
}

void MainGame::drawAsteriods()
{
	texture.Bind(0);
	eMapping.Bind();
	linkEmapping();

	glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, texture.getID());

	glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getID());

	for (int i = 0; i < numberOfAsteroids; i++)
	{
		if (asteroid[i].getActive())
		{
			asteroid[i].transformPositions(glm::vec3(*asteroid[i].getTM().GetPos()), glm::vec3(asteroid[i].getTM().GetRot()->x + deltaTime, asteroid[i].getTM().GetRot()->y + deltaTime, asteroid[i].getTM().GetRot()->z + deltaTime), rockScale);
			asteroid[i].draw(&rockMesh);
			asteroid[i].update(&rockMesh);
			eMapping.Update(asteroid[i].getTM(), myCamera);
		}
	}
}

void MainGame::drawMissiles()
{
	texture.Bind(0);
	rimShader.Bind();
	linkRimLighting();

	for (int i = 0; i < numberOfMissiles; i++)
	{
		if (missiles[i].getActive())
		{
			missiles[i].draw(&missileMesh);
			missiles[i].update(&missileMesh);
			rimShader.Update(missiles[i].getTM(), myCamera);
			fireMissiles(i);
		}
	}
}

void MainGame::fireMissiles(int i) 
{
	float deg2rad = GameObject().deg2rad;
	float rotAngle = 360.0f * deg2rad;

	glm::vec3 missilePosition = glm::vec3(*missiles[i].getTM().GetPos());
	glm::vec3 missileRotation = glm::vec3(*missiles[i].getTM().GetRot());

	glm::vec3 asteroidPosition = *asteroid[i].getTM().GetPos();

	glm::vec3 targetVector = asteroidPosition - missilePosition;
	glm::vec3 targetVelocity = glm::normalize(targetVector) * missileSpeed;

	glm::vec3 norm = glm::normalize(targetVector);
	glm::vec3 rot = glm::vec3(rotAngle, rotAngle, rotAngle) * norm;

	missiles[i].transformPositions(missilePosition + targetVelocity, rot, (glm::vec3(missileScale, missileScale, missileScale)));


	if (collision(missilePosition, 0.2, asteroidPosition, 0.2))
	{
		missiles[i].setActive(0);
		asteroid[i].setActive(0);
		playAudio(whistle, missilePosition);
	}
}

void MainGame::drawShip()
{
	toonShader.Bind();
	linkToon();

	ship.draw(&shipMesh);
	ship.update(&shipMesh);
	toonShader.Update(ship.getTM(), myCamera);
}


void MainGame::drawSkyBox()
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.textureID);

	counter = counter + 0.02f;

	skybox.draw(&myCamera);

	//myCamera.setPos(currentCamPos);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnd();
}

void MainGame::setCameraTarget()
{
	glm::vec3 forwardTemp = ship.fwd();
	glm::vec3 upTemp = ship.uP();
	glm::vec3 rightTemp = ship.rght();
	glm::vec3 shipPos = *ship.getTM().GetPos();

	switch (cameraType)
	{
	case 0:
		return;
		break;
	case 1:
	{
		float distance = glm::distance(shipPos, myCamera.getPos());

		glm::vec3 posRel2Ship = glm::vec3(cameraOffset * sin(theta), 0.0f, cameraOffset * cos(theta));

		myCamera.setLook(shipPos + (2.0f * forwardTemp));
		myCamera.setPos(shipPos + posRel2Ship);
	}
	break;
	case 2:
	{
		myCamera.setForward(forwardTemp);
		myCamera.setPos(shipPos + (forwardTemp * cameraOffset / 2.0f));
	}
	break;
	case 3:
	{
		myCamera.setForward(forwardTemp);
		myCamera.setPos(shipPos - (forwardTemp * cameraOffset) + (upTemp * cameraOffset / 5.0f));
	}
	break;
	case 4:
	{
		if (asteroidCamTarget < 0)
		{
			asteroidCamTarget = numberOfAsteroids - 1;
		}
		if (asteroidCamTarget > numberOfAsteroids - 1)
		{
			asteroidCamTarget = 0;
		}
		myCamera.setLook(*asteroid[asteroidCamTarget].getTM().GetPos());
	}
	break;
	case 5:
	{
		myCamera.setPos(*missiles[missileLaunchNumber - 1].getTM().GetPos() + (forwardTemp * cameraOffset) - (upTemp * cameraOffset / 5.0f));
		myCamera.setLook(*asteroid[missileLaunchNumber - 1].getTM().GetPos());
	}
	break;
	}

	/*myCamera.setLook(*asteroid[cameraTarget].getTM().GetPos());
	myCamera.setPos(currentCamPos);*/
}

bool MainGame::collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad)
{
	float distance = glm::sqrt((m2Pos.x - m1Pos.x)*(m2Pos.x - m1Pos.x) + (m2Pos.y - m1Pos.y)*(m2Pos.y - m1Pos.y) + (m2Pos.z - m1Pos.z)*(m2Pos.z - m1Pos.z));

	if (distance < (m1Rad + m2Rad))
	{
		audioDevice.setlistener(myCamera.getPos(), m1Pos); //add bool to mesh
		//playAudio(whistle, m1Pos);
		cout << "collision";
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
	audioDevice.playSound(Source, pos);
	
	/*
	if (AL_PLAYING != state)
{
		
}*/

}

void MainGame::linkFogShader()
{
	//fogShader.setMat4("u_pm", myCamera.getProjection());
	//fogShader.setMat4("u_vm", myCamera.getProjection());
	fogShader.setFloat("maxDist", 20.0f);
	fogShader.setFloat("minDist", 0.0f);
	fogShader.setVec3("fogColor", glm::vec3(0.0f, 0.0f, 0.0f));
}

void MainGame::linkToon()
{
	toonShader.setVec3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
}

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

void MainGame::linkEmapping()
{
	eMapping.setMat4("model", asteroid[0].getModel());
	//eMapping.setVec3("cameraPos", myCamera.getPos());
}

void MainGame::updateDelta()
{
	if (deltaInSeconds)
	{
		deltaTimeString = "s";
		deltaTimeConversion = 1;
	}
	else if (!deltaInSeconds)
	{
		deltaTimeString = "ms";
		deltaTimeConversion = 1000;
	}
	LAST = NOW;
	NOW = SDL_GetPerformanceCounter();

	deltaTime = (float)((NOW - LAST) / (float)SDL_GetPerformanceFrequency());

	deltaTimeDisplay = deltaTime * deltaTimeConversion;

	cameraSpeed = cameraVelocity * deltaTime;
	cameraRotationSpeed = cameraRotationVelocity * deltaTime;

	asteroidSpeed = asteroidVelocity * deltaTime;

	shipSpeed = shipVelocity * deltaTime;
	shipRotationSpeed = shipRotationVelocity * deltaTime;

	missileSpeed = missileVelocity * deltaTime;
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

	FBOShader.Bind();
	if(shake)
	FBOShader.setFloat("time", counter); else
		FBOShader.setFloat("time", 1);
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, CBO);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void MainGame::moveCamera()
{
	myCamera.setLook(shipMesh.getSpherePos());
	myCamera.setPos(currentCamPos);
}



void MainGame::drawGame()
{
	_gameDisplay.clearDisplay(0.8f, 0.8f, 0.8f, 1.0f); //sets our background colour	

	bindFBO();

	drawAsteriods();
	drawShip();
	drawSkyBox();
	drawMissiles();
	//moveCamera();

	unbindFBO();

	renderFBO();

	glEnable(GL_DEPTH_TEST);

	drawAsteriods();
	drawShip();
	drawSkyBox();
	drawMissiles();
	
	_gameDisplay.swapBuffer();		

} 
