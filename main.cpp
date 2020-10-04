#define GLEW_STATIC
#define FPS 30

#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "include/Engine.cpp"
#include "include/spacetime.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

OpenGL::Shader shader;
GUI mainMenu;

GUI targetGui;
GUI planetDetails[10];
GUI spaceshipTypes[4];
GUI velocity_meter;
GUI velocity_pointer;

OpenGL::Shader GUI::shader;

Renderer renderer;
Tools::BloomPostProcessor bloomPostProcessor;
SkyBox skybox;

Planet sun;
Planet mercury;
Planet venus;
Planet earth;
Planet mars;
Planet jupiter;
Planet saturn;
Planet uranus;
Planet neptune;

Planet moon;

Billboard billboard[10];
OpenGL::Shader Billboard::shader;

Spaceship ship;
Tools::Camera camera;
glm::mat4 projectionMatrix;

enum {MENU, SAILING}currentState;

Tools::MousePicker picker;
int screenWidth,screenHeight;
bool initial = true;
bool showPointers = true;

Tools::ShadowLoader shadow;
Tools::FPSManager fpsManager(FPS);

bool keyBuffer[128];
bool mouseBuffer[2];

Spacetime spacetime;
Vector3 pos;
double radius_scale = 27.0/695842e3;//6e-6;
double position_scale = 90e-11;//15e-11;

int physicsInitiate(){
	Matter Mercury;
	Matter Earth;
	Matter Mars;
	Matter Venus;
	Matter Jupiter;
	Matter Saturn;
	Matter Uranus;
	Matter Neptune;
	Matter Pluto;
    Matter Sun;
    Sun.setMass(1.9e30);
    Sun.setRadius(695842e3);
    Sun.setTemperature(5800);
    Sun.setBlackBody(true);

    Mercury.setMass(3.3011e23);
    Mercury.setPosition(Vector3::getVector(46e9,0,0));
    Mercury.setVelocity(Vector3::getVector(0,0,1).scale(57.59e3));
    Mercury.setRadius(2439.7e3);

	Venus.setMass(4.8675e24);
	Venus.setPosition(Vector3::getVector(107.48e9, 0, 0));
	Venus.setVelocity(Vector3::getVector(0, 0, 1).scale(34.3e3));
	Venus.setRadius(6052e3);

    Earth.setMass(5.97237e24);
    Earth.setPosition(Vector3::getVector(147.09e9,0,0));
    Earth.setVelocity(Vector3::getVector(0,0,1).scale(29.5e3));
    Earth.setRadius(6357e3);

        Matter Moon;
        Moon.setMass(7.342e22);
        Moon.setPosition(Vector3::getVector(0.3633e9+147.09e9,0,0));
        Moon.setVelocity(Vector3::getVector(0,0,1).scale(1.0823e3+29.5e3));
        Moon.setRadius(1734.4e3);

    Mars.setMass(6.4171e23);
    Mars.setPosition(Vector3::getVector(206.62e9,0,0));
    Mars.setVelocity(Vector3::getVector(0,0,1).scale(25.80e3));
    Mars.setRadius(3390e3);

	Jupiter.setMass(1898.19e24);
	Jupiter.setPosition(Vector3::getVector(740.52e9, 0, 0));
	Jupiter.setVelocity(Vector3::getVector(0, 0, 1).scale(13.23e3));
	Jupiter.setRadius(69911e3);

	Saturn.setMass(568.34e24);
	Saturn.setPosition(Vector3::getVector(1352.55e9, 0, 0));
	Saturn.setVelocity(Vector3::getVector(0, 0, 1).scale(10.18e3));
	Saturn.setRadius(58232e3);

	Uranus.setMass(86.813e24);
	Uranus.setPosition(Vector3::getVector(2741.30e9, 0, 0));
	Uranus.setVelocity(Vector3::getVector(0, 0, 1).scale(7.11e3));
    Uranus.setRadius(25362e3);

	Neptune.setMass(102.413e24);
	Neptune.setPosition(Vector3::getVector(4444.45e9, 0, 0));
	Neptune.setVelocity(Vector3::getVector(0, 0, 1).scale(6.10e3));
    Neptune.setRadius(24622e3);

	spacetime.addMatter(0,Sun);
    spacetime.addMatter(1,Mercury);
	spacetime.addMatter(2, Venus);
    spacetime.addMatter(3,Earth);
        spacetime.addMatter(31,Moon);
    spacetime.addMatter(4,Mars);
    spacetime.addMatter(5, Jupiter);
	spacetime.addMatter(6, Saturn);
	spacetime.addMatter(7, Uranus);
	spacetime.addMatter(8, Neptune);
    return 0;
}

void resetPointer(){
    glutWarpPointer(screenWidth/2, screenHeight/2);
    camera.setYaw(0);
    camera.setPitch(10);
}

void init(){
    currentState = MENU;
    physicsInitiate();

    shader.loadShader("res/shaders/Basic.shader");
    shader.bind();

    int i[] = {0,1};
    shader.addUniform1iv("u_Textures", i);

    {
        float view[]= {0.0f, 0.0f, 0.0f};
        float d_position[] = {0.0f, 0.0f, 0.0f};
        float a_color[] = {1.0f, 1.0f, 1.0f};
        float d_color[] = {1.0f, 1.0f, 1.0f};
        float s_color[] = {1.0f, 1.0f, 1.0f};

        shader.addUniform3f("u_viewPos", view);
        shader.addUniform3f("light.position", d_position);
        shader.addUniform3f("light.ambient", a_color);
        shader.addUniform3f("light.diffuse", d_color);
        shader.addUniform3f("light.specular", s_color);
    }

    {
        float d_position[] = {5.0f, 0.0f, 0.0f};
        float a_color[] = {1.0f, 1.0f, 1.0f};
        float d_color[] = {1.0f, 1.0f, 1.0f};
        float s_color[] = {1.0f, 1.0f, 1.0f};
        float constant = 0.5f;
        float linear = 0.045f;
        float quadratic = 0.016f;

        shader.addUniform3f("plight.position", d_position);
        shader.addUniform3f("plight.ambient", a_color);
        shader.addUniform3f("plight.diffuse", d_color);
        shader.addUniform3f("plight.specular", s_color);
        shader.addUniform1f("plight.constant", constant);
        shader.addUniform1f("plight.linear", linear);
        shader.addUniform1f("plight.quadratic", quadratic);
    }

    {
        float direction[] = {-1.0f, 0.0f, 0.0f};
        float a_color[] = {1.0f, 1.0f, 1.0f};
        float d_color[] = {1.0f, 1.0f, 1.0f};
        float s_color[] = {1.0f, 1.0f, 1.0f};

        shader.addUniform3f("dlight.direction", direction);
        shader.addUniform3f("dlight.ambient", a_color);
        shader.addUniform3f("dlight.diffuse", d_color);
        shader.addUniform3f("dlight.specular", s_color);
    }

    {
        float shininess = 16.0f;
        float color[] = {1.0f, 1.0f, 1.0f};
        float a_color[] = {0.1f, 0.1f, 0.1f};
        float d_color[] = {1.0f, 1.0f, 1.0f};
        float s_color[] = {0.0f, 0.0f, 0.0f};

        shader.addUniform3f("u_color", color);
        shader.addUniform1f("material.shininess", 32.0f);
        shader.addUniform3f("material.ambient", a_color);
        shader.addUniform3f("material.diffuse", d_color);
        shader.addUniform3f("material.specular", s_color);
        shader.addUniform1f("material.shininess", shininess);
    }

    {
        shader.addUniform1i("u_useLighting", 1);
        shader.addUniform1i("u_lightType", 0);
        shader.addUniform1i("u_textureMap", 0);
        shader.addUniform1i("u_texSlot", 0);
    }

    mainMenu.load(glm::vec2(0.0), glm::vec2(2.0, 2.0), "res/textures/main_menu.png");
    GUI::specifyShader("res/shaders/GUI.shader");

    sun.loadPlanet("res/textures/sun.png", radius_scale*spacetime.getMatter(0).getRadius(), 16);

    mercury.loadPlanet("res/textures/mercury.png", radius_scale*spacetime.getMatter(1).getRadius(), 8);
    venus.loadPlanet("res/textures/venus.png", radius_scale*spacetime.getMatter(2).getRadius(), 8);
    earth.loadPlanet("res/textures/earth.png", radius_scale*spacetime.getMatter(3).getRadius(), 8);
        moon.loadPlanet("res/textures/moon.png", radius_scale*spacetime.getMatter(31).getRadius(), 8);
    mars.loadPlanet("res/textures/mars.png", radius_scale*spacetime.getMatter(4).getRadius(), 8);
    jupiter.loadPlanet("res/textures/jupiter.png", radius_scale*spacetime.getMatter(5).getRadius(), 8);
    saturn.loadPlanet("res/textures/saturn.png", radius_scale*spacetime.getMatter(6).getRadius(), 8);
    uranus.loadPlanet("res/textures/uranus.png", radius_scale*spacetime.getMatter(7).getRadius(), 8);
    neptune.loadPlanet("res/textures/neptune.png", radius_scale*spacetime.getMatter(8).getRadius(), 8);

    ship.loadFiles("res/models/SpaceShip.obj", "res/textures/spaceship_texture.png", 0.125);
    ship.setPosition(glm::vec3(12.0, 0.0, 0.0));
    ship.setDirection(glm::vec3(1.0,0.0,1.0));

    camera.setFov(60);
    std::vector<std::string> locations;
    locations.push_back("res/textures/Milky Way/Positive_X.png");
    locations.push_back("res/textures/Milky Way/Negative_X.png");
    locations.push_back("res/textures/Milky Way/Positive_Y.png");
    locations.push_back("res/textures/Milky Way/Negative_Y.png");
    locations.push_back("res/textures/Milky Way/Positive_Z.png");
    locations.push_back("res/textures/Milky Way/Negative_Z.png");
    skybox.loadSkyBox(locations, "res/shaders/Skybox.shader", 10000.0);
    locations.clear();

    std::vector<std::string> processes;
    processes.push_back("res/shaders/Dummy.shader");
    processes.push_back("res/shaders/BrightFilter.shader");
    processes.push_back("res/shaders/HorizotalBlur.shader");
    processes.push_back("res/shaders/VerticalBlur.shader");
    processes.push_back("res/shaders/Bloom.shader");
    bloomPostProcessor.load(800, 600, screenWidth, screenHeight, processes, 0.963);

    velocity_meter.load(glm::vec2(0.0, -0.9), glm::vec2(1.0, 0.15), "res/textures/velocity_meter.png");
    velocity_pointer.load(glm::vec2(-0.47, -0.85), glm::vec2(0.03, 0.04), "res/textures/velocity_pointer.png");

    planetDetails[0].load(glm::vec2(0.625, 0.625), glm::vec2(0.625, 0.625), "res/textures/sun_details.png");
    planetDetails[1].load(glm::vec2(0.625, 0.625), glm::vec2(0.625, 0.625), "res/textures/mercury_details.png");
    planetDetails[2].load(glm::vec2(0.625, 0.625), glm::vec2(0.625, 0.625), "res/textures/venus_details.png");
    planetDetails[3].load(glm::vec2(0.625, 0.625), glm::vec2(0.625, 0.625), "res/textures/earth_details.png");
    planetDetails[4].load(glm::vec2(0.625, 0.625), glm::vec2(0.625, 0.625), "res/textures/mars_details.png");
    planetDetails[5].load(glm::vec2(0.625, 0.625), glm::vec2(0.625, 0.625), "res/textures/jupiter_details.png");
    planetDetails[6].load(glm::vec2(0.625, 0.625), glm::vec2(0.625, 0.625), "res/textures/saturn_details.png");
    planetDetails[7].load(glm::vec2(0.625, 0.625), glm::vec2(0.625, 0.625), "res/textures/uranus_details.png");
    planetDetails[8].load(glm::vec2(0.625, 0.625), glm::vec2(0.625, 0.625), "res/textures/neptune_details.png");
    planetDetails[9].load(glm::vec2(0.625, 0.625), glm::vec2(0.625, 0.625), "res/textures/moon_details.png");

    targetGui.load(glm::vec2(0.0, 0.0), glm::vec2(0.05, 0.065), "res/textures/center.png");
    billboard[0].loadData(glm::vec3(0.0),glm::vec2(0.125, 0.125), "res/textures/sun_pointer.png");
    billboard[1].loadData(glm::vec3(0.0),glm::vec2(0.125, 0.125), "res/textures/mercury_pointer.png");
    billboard[2].loadData(glm::vec3(0.0),glm::vec2(0.125, 0.125), "res/textures/venus_pointer.png");
    billboard[3].loadData(glm::vec3(0.0),glm::vec2(0.125, 0.125), "res/textures/earth_pointer.png");
    billboard[4].loadData(glm::vec3(0.0),glm::vec2(0.125, 0.125), "res/textures/mars_pointer.png");
    billboard[5].loadData(glm::vec3(0.0),glm::vec2(0.125, 0.125), "res/textures/jupiter_pointer.png");
    billboard[6].loadData(glm::vec3(0.0),glm::vec2(0.125, 0.125), "res/textures/saturn_pointer.png");
    billboard[7].loadData(glm::vec3(0.0),glm::vec2(0.125, 0.125), "res/textures/uranus_pointer.png");
    billboard[8].loadData(glm::vec3(0.0),glm::vec2(0.125, 0.125), "res/textures/neptune_pointer.png");
    billboard[9].loadData(glm::vec3(0.0),glm::vec2(0.125, 0.125), "res/textures/moon_pointer.png");
    Billboard::specifyShader("res/shaders/Billboard.shader");

    spaceshipTypes[0].load(glm::vec2(-0.65, 0.9), glm::vec2(0.6, 0.075), "res/textures/spaceship0.png");
    spaceshipTypes[1].load(glm::vec2(-0.65, 0.9), glm::vec2(0.6, 0.075), "res/textures/spaceship1.png");
    spaceshipTypes[2].load(glm::vec2(-0.65, 0.9), glm::vec2(0.6, 0.075), "res/textures/spaceship2.png");
    spaceshipTypes[3].load(glm::vec2(-0.65, 0.9), glm::vec2(0.6, 0.075), "res/textures/spaceship3.png");
}

static void resize_display(int width, int height){
    screenHeight = height;
    screenWidth = width;
    glViewport(0, 0, width, height);
}

static void closeWindow(){
    shader.deleteShader();

    ship.cleanUP();
    skybox.cleanUP();

    mainMenu.cleanUP();
    GUI::deleteShader();

    sun.cleanUp();
    mercury.cleanUp();
    venus.cleanUp();
    earth.cleanUp();
        moon.cleanUp();
    mars.cleanUp();
    jupiter.cleanUp();
    saturn.cleanUp();
    uranus.cleanUp();
    neptune.cleanUp();

    exit(0);
}

static void handleKey(){
    if(keyBuffer[27]){
        if(currentState == MENU){
            closeWindow();
            return;
        }
        else if(currentState == SAILING){
            currentState = MENU;
            glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
            keyBuffer[27] = false;
            return;
        }
    }

    bool accelerated = false;
    if(keyBuffer['1']){
        spacetime.setSpaceShipType(0);
        keyBuffer['1'] = false;
    }
    if(keyBuffer['2']){
        spacetime.setSpaceShipType(1);
        keyBuffer['2'] = false;
    }
    if(keyBuffer['3']){
        spacetime.setSpaceShipType(2);
        keyBuffer['3'] = false;
    }
    if(keyBuffer['4']){
        spacetime.setSpaceShipType(3);
        keyBuffer['4'] = false;
    }

    if(keyBuffer[' ']){
        spacetime.toggleLock();
        keyBuffer[' '] = false;
    }
    if(keyBuffer['f']){
        spacetime.forceSynchronize();
        keyBuffer['f'] = false;
    }
    if(keyBuffer['z']){
        showPointers = !showPointers;
        keyBuffer['z'] = false;
    }
    if(keyBuffer['w']){
        ship.moveForward();
        accelerated = true;
    }
    if(keyBuffer['a']){
        ship.moveLeft();
        accelerated = true;
    }
    if(keyBuffer['s']){
        ship.moveBackward();
        accelerated = true;
    }
    if(keyBuffer['d']){
        ship.moveRight();
        accelerated = true;
    }
    if(accelerated){
        ship.setDirection(-camera.getDirection());
        spacetime.accelerate(ship.getAcceleration());
    }
}

static void handleMouse(){
    if(mouseBuffer[0]){
        ship.setDirection(-camera.getDirection());
    }
    else if(mouseBuffer[1]){
        spacetime.forceShipToStop();
    }
}

static void display(void){
    fpsManager.initiateTimer();
    fpsManager.updateCounter();

    handleKey();
    handleMouse();

    if(currentState == MENU){
        renderer.clear();
        mainMenu.draw();
    }
    else if(currentState == SAILING){
        spacetime.update();
        double dt = spacetime.getDt();

        ship.setPosition(spacetime.getPosition(position_scale));
        camera.follow(ship);
        glm::mat4 view = camera.getViewMatrix();
        projectionMatrix = glm::perspective(glm::radians(45.0f+float((179.0-45.0)*spacetime.getVelRatioWithC())), (float)screenWidth/screenHeight, 0.01f, 10000.0f);
        glm::mat4 viewProj = projectionMatrix*view;

        shader.bind();
        glm::vec3 cam = camera.getPosition();
        shader.addUniform3f("u_viewPos", &cam[0]);

        shader.unbind();
        bloomPostProcessor.startSampling();
            renderer.clear();

            shader.bind();
            shader.addUniform1i("u_useLighting", 0);
            float getVelocityNormalized = glm::dot(ship.getDirection(), spacetime.getDirection());
            if(getVelocityNormalized>0){
                shader.addUniform1f("u_velocity", spacetime.getVelRatioWithC());
            }
            else{
                shader.addUniform1f("u_velocity", -spacetime.getVelRatioWithC());
            }
            pos=spacetime.getMatter(0).getPosition();
            pos=pos.scale(position_scale);
            sun.setPosition(glm::vec3(pos.getValue1(), pos.getValue2(), pos.getValue3()));
            sun.increaseRotation(360.0*dt/(25.31*24.0*60.0*60.0));
            shader.addUniform1f("u_wavelength", float(spacetime.getMatter(0).getRadiationWaveLength()));
            sun.drawPlanet(shader, viewProj);
            shader.addUniform1i("u_useLighting", 1);

            pos=spacetime.getMatter(1).getPosition();
            pos=pos.scale(position_scale);
            mercury.setPosition(glm::vec3(pos.getValue1(), pos.getValue2(), pos.getValue3()));
            mercury.increaseRotation(360.0*dt/(58.80*24.0*60.0*60.0));
            shader.addUniform1f("u_wavelength", float(spacetime.getMatter(1).getRadiationWaveLength()));
            mercury.drawPlanet(shader, viewProj);

            pos=spacetime.getMatter(2).getPosition();
            pos=pos.scale(position_scale);
            venus.setPosition(glm::vec3(pos.getValue1(), pos.getValue2(), pos.getValue3()));
            venus.increaseRotation(-360.0*dt/(244.0*24.0*60.0*60.0));
            shader.addUniform1f("u_wavelength", float(spacetime.getMatter(2).getRadiationWaveLength()));
            venus.drawPlanet(shader, viewProj);

            pos=spacetime.getMatter(3).getPosition();
            pos=pos.scale(position_scale);
            earth.setPosition(glm::vec3(pos.getValue1(), pos.getValue2(), pos.getValue3()));
            earth.increaseRotation(360.0*dt/(24.0*60.0*60.0));
            shader.addUniform1f("u_wavelength", float(spacetime.getMatter(3).getRadiationWaveLength()));
            earth.drawPlanet(shader, viewProj);

                pos=spacetime.getMatter(31).getPosition();
                pos=pos.scale(position_scale);
                moon.setPosition(glm::vec3(pos.getValue1(), pos.getValue2(), pos.getValue3()));
                moon.increaseRotation(360.0*dt/(27.4*24.0*60.0*60.0));
                shader.addUniform1f("u_wavelength", float(spacetime.getMatter(31).getRadiationWaveLength()));
                moon.drawPlanet(shader, viewProj);

            pos=spacetime.getMatter(4).getPosition();
            pos=pos.scale(position_scale);
            mars.setPosition(glm::vec3(pos.getValue1(), pos.getValue2(), pos.getValue3()));
            mars.increaseRotation(360.0*dt/(1.04*24.0*60.0*60.0));
            shader.addUniform1f("u_wavelength", float(spacetime.getMatter(4).getRadiationWaveLength()));
            mars.drawPlanet(shader, viewProj);

            pos = spacetime.getMatter(5).getPosition();
            pos = pos.scale(position_scale);
            jupiter.setPosition(glm::vec3(pos.getValue1(), pos.getValue2(), pos.getValue3()));
            jupiter.increaseRotation(360.0*dt/(0.415*24.0*60.0*60.0));
            shader.addUniform1f("u_wavelength", float(spacetime.getMatter(5).getRadiationWaveLength()));
            jupiter.drawPlanet(shader, viewProj);

            pos = spacetime.getMatter(6).getPosition();
            pos = pos.scale(position_scale);
            saturn.setPosition(glm::vec3(pos.getValue1(), pos.getValue2(), pos.getValue3()));
            saturn.increaseRotation(360.0*dt/(0.445*24.0*60.0*60.0));
            shader.addUniform1f("u_wavelength", float(spacetime.getMatter(6).getRadiationWaveLength()));
            saturn.drawPlanet(shader, viewProj);

            pos = spacetime.getMatter(7).getPosition();
            pos = pos.scale(position_scale);
            uranus.setPosition(glm::vec3(pos.getValue1(), pos.getValue2(), pos.getValue3()));
            uranus.increaseRotation(-360.0*dt/(0.72*24.0*60.0*60.0));
            shader.addUniform1f("u_wavelength", float(spacetime.getMatter(7).getRadiationWaveLength()));
            uranus.drawPlanet(shader, viewProj);

            pos = spacetime.getMatter(8).getPosition();
            pos = pos.scale(position_scale);
            neptune.setPosition(glm::vec3(pos.getValue1(), pos.getValue2(), pos.getValue3()));
            neptune.increaseRotation(360.0*dt/(0.673*24.0*60.0*60.0));
            shader.addUniform1f("u_wavelength", float(spacetime.getMatter(8).getRadiationWaveLength()));
            neptune.drawPlanet(shader, viewProj);

            skybox.increaseRotation();
            skybox.render(view, projectionMatrix);
        bloomPostProcessor.stopSampling(screenWidth, screenHeight);

        glDisable(GL_DEPTH_TEST);
        bloomPostProcessor.drawProcessed(screenWidth, screenHeight);
        projectionMatrix = glm::perspective(glm::radians(45.0f), (float)screenWidth/screenHeight, 0.01f, 10000.0f);
        glm::mat4 new_viewProj = projectionMatrix*view;
        shader.bind();
        shader.addUniform1i("u_useLighting", 0);
        shader.addUniform1f("u_velocity", 0);
        ship.draw(shader, new_viewProj);
        if(spacetime.isLocked()){
            int id = spacetime.getLockedId();
            if(id == 31){id = 9;}
            planetDetails[id].draw();
        }
        targetGui.draw();

        if(showPointers){
            for(unsigned int i=0; i<9; i++){
                pos = spacetime.getMatter(i).getPosition();
                pos = pos.scale(position_scale)+ Vector3::getVector(0.0, spacetime.getMatter(i).getRadius(), 0.0).scale(radius_scale);
                billboard[i].setPosition(glm::vec3(pos.getValue1(), pos.getValue2(), pos.getValue3()));
                billboard[i].draw(viewProj);
            }
            pos = spacetime.getMatter(31).getPosition();
            pos = pos.scale(position_scale)+ Vector3::getVector(0.0, spacetime.getMatter(31).getRadius(), 0.0).scale(radius_scale);
            billboard[9].setPosition(glm::vec3(pos.getValue1(), pos.getValue2(), pos.getValue3()));
            billboard[9].draw(viewProj);
        }

        velocity_meter.draw();
        velocity_pointer.draw(glm::vec2(0.94*spacetime.getVelRatioWithC(), 0.0));

        spaceshipTypes[spacetime.getSpaceShipType()].draw();
        glEnable(GL_DEPTH_TEST);
    }

    /*
    //Works currently
    picker.setMatrices(view, proj);
    glm::vec3 ray = picker.calculateRay(0, 0);
    std::cout << "Ray = " << ray.x << " " << ray.y << " " << ray.z <<std::endl;*/

    //Limiting FPS to targeted
    fpsManager.limitFPS();
    glutSwapBuffers();
    glutPostRedisplay();
}

static void key(unsigned char key, int x, int y){
    keyBuffer[key] = true;
    glutPostRedisplay();
}

static void keyUp(unsigned char key, int x, int y){
    keyBuffer[key] = false;
    glutPostRedisplay();
}

void mouseMoved(int x,int y){
    if(currentState == SAILING){
        if(initial){
            resetPointer();
            initial = false;
            return;
        }

        static bool just_warped = false;
        int dx = x - screenWidth/2;
        int dy = y - screenHeight/2;

        if(just_warped) {
            just_warped = false;
            return;
        }

        camera.increaseYaw(-dx);
        camera.increasePitch(dy);
        glutWarpPointer(screenWidth/2, screenHeight/2);
        just_warped = true;
    }
}

void mouseFunc(int button, int state, int x, int y){
    if(currentState == MENU){
        if(button == GLUT_LEFT_BUTTON){
            float normal_x = float(x)/screenWidth;
            float normal_y = float(y)/screenHeight;

            if(normal_x>0.10 && normal_x<0.40 && normal_y>0.27 && normal_y<0.90){
                glutSetCursor(GLUT_CURSOR_NONE);
                //glutSetCursor(GLUT_CURSOR_CROSSHAIR);
                currentState = SAILING;
                if(!spacetime.isLocked()){
                    spacetime.toggleLock();
                }
                camera.setYawPitch(-90.0, 0.0);
                glutWarpPointer(screenWidth/2, screenHeight/2);
            }
        }
    }

    if(currentState == SAILING){
        if(button == GLUT_LEFT_BUTTON){
            if(state == GLUT_DOWN){
                mouseBuffer[0] = true;
            }
            else if(state == GLUT_UP){
                mouseBuffer[0] = false;
            }
        }

        else if(button == GLUT_RIGHT_BUTTON){
            if(state == GLUT_DOWN){
                mouseBuffer[1] = true;
            }
            else if(state == GLUT_UP){
                mouseBuffer[1] = false;
            }
        }
    }
}

int main(int argc, char *argv[]){
    Window::createGlutWindow(argc, argv,
                        "Solar Sailer",
                        glm::vec2(250, 75),
                        glm::vec2(800, 600));
    Window::initializeGLEW(false);
    Window::enableBackfaceCulling();
    Window::enableDepthTest();

    init();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glutReshapeFunc(resize_display);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(keyUp);
    glutMotionFunc(mouseMoved);
    glutMouseFunc(mouseFunc);
	glutPassiveMotionFunc(mouseMoved);

    glutMainLoop();
    return EXIT_SUCCESS;
}
