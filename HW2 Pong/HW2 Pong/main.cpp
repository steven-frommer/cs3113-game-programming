#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Entity.h"
#include <math.h>

SDL_Window* displayWindow;

Entity* pongBall; 
Entity* rightPaddle1; 
Entity* leftPaddle1;
Entity* rightWins1;
Entity* leftWins1;

float lastFrameTicks;
const Uint8* keys = SDL_GetKeyboardState(NULL);
float displayWinTimer = 0;
bool displayingWin = false;
float elapsed = 0.0f;

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	if (!surface){
		return -1;
	}
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	SDL_FreeSurface(surface);

	return textureID;
}

void Setup(){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("HW2: Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	GLuint ball = LoadTexture("pong_ball.png");
	GLuint paddle = LoadTexture("paddle.png");
	GLuint leftWins = LoadTexture("left_wins.png");
	GLuint rightWins = LoadTexture("right_wins.png");

	pongBall = new Entity(ball, 0.0f, 0.0f, 0.0f, 0.1f, 0.2f);
	leftPaddle1 = new Entity(paddle,-1.6f,0.0f,0.0f,0.7f,0.3f);
	rightPaddle1 = new Entity(paddle,1.6f, 0.0f, 0.0f, 0.7f, 0.3f);
	leftWins1 = new Entity(leftWins, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	rightWins1 = new Entity(rightWins, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	pongBall->speed = 1.0f;
	pongBall->direction_y = 0.8f;
	pongBall->direction_x = 0.8f;

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
}

void Update(){
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	
	if (!displayingWin){
		pongBall->rotation += elapsed*200.0f;
		pongBall->x += (pongBall->speed)*(pongBall->direction_x)*elapsed;
		pongBall->y += (pongBall->speed)*(pongBall->direction_y)*elapsed;
	}
	
	if (keys[SDL_SCANCODE_UP]){rightPaddle1->y += 1.0f*elapsed;}
	else if (keys[SDL_SCANCODE_DOWN]){rightPaddle1->y -= 1.0f*elapsed;}

	if (keys[SDL_SCANCODE_W]){leftPaddle1->y += 1.0f*elapsed;}
	else if (keys[SDL_SCANCODE_S]){leftPaddle1->y -= 1.0f*elapsed;}

	if (leftPaddle1->y > 0.9f){leftPaddle1->y = 0.9f;}
	else if (leftPaddle1->y < -0.9f){leftPaddle1->y = -0.9f;}
	if (rightPaddle1->y > 0.9f){rightPaddle1->y = 0.9f;}
	else if (rightPaddle1->y < -0.9f){rightPaddle1->y = -0.9f;}
	
	if (pongBall->y > .9f){pongBall->direction_y=-abs(pongBall->direction_y);}
	else if (pongBall->y < -.9f){pongBall->direction_y=abs(pongBall->direction_y);}

	if (pongBall->x < -1.2f) {
		if (pongBall->y < leftPaddle1->y + (leftPaddle1->width*0.5f) && pongBall->y > leftPaddle1->y - (leftPaddle1->width*0.5f)){
			pongBall->direction_x = abs(pongBall->direction_x);
		}
	}
	else if (pongBall->x > 1.2f) {
		if (pongBall->y < rightPaddle1->y + (rightPaddle1->width*0.5f) && pongBall->y > rightPaddle1->y - (rightPaddle1->width*0.5f)) {
			pongBall->direction_x = -abs(pongBall->direction_x);
		}
	}
}

void Render(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	leftPaddle1->Draw();
	rightPaddle1->Draw();
	if (pongBall->x > 1.385){
		if (!displayingWin){
			displayingWin = true;
			displayWinTimer = 1.5f;
		}
		displayWinTimer -= elapsed;
		if (displayWinTimer <= 0){
			displayingWin = false;
			pongBall->x = 0;
			pongBall->direction_x = -abs(pongBall->direction_x);
		}
		leftWins1->Draw();
	} 
	else if (pongBall->x < -1.385){
		if (!displayingWin){
			displayingWin = true;
			displayWinTimer = 1.5f;
		}
		displayWinTimer -= elapsed;
		if (displayWinTimer <= 0){
			displayingWin = false;
			pongBall->x = 0;
			pongBall->direction_x = abs(pongBall->direction_x);
		}
		rightWins1->Draw();
	} 
	else pongBall->Draw();

	SDL_GL_SwapWindow(displayWindow);
}

void Cleanup(){
	delete pongBall;
	delete rightPaddle1;
	delete leftPaddle1;
}

bool ProcessEvents(){
	SDL_Event event;
	bool done = false;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				return done;
			} 
			else if(event.type == SDL_MOUSEMOTION) {
			//float unitX = (((float)event.motion.x / 800.0f) * 2.666f ) - 1.333f;
			rightPaddle1->y= (((float)(600-event.motion.y) / 600.0f) * 2.0f ) - 1.0f;
			}
		}
	return (!done);
}

int main(int argc, char *argv[]){
	Setup();
	while(ProcessEvents()){
		Update();
		Render();
	}
	Cleanup();	return 0;}