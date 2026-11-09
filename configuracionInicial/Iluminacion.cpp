// Práctica Unificada: Iluminación Orbital y Animación de Agua
// Estudiante: Iván Daniel Cortés Alvarado
// commit de prueba
//agua modificada

#include <iostream>
#include <string>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "SOIL2/SOIL2.h"
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "agua.h"	
//#include <iostream>
#include <fstream>



// Dimensiones de ventana
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH = WIDTH, SCREEN_HEIGHT = HEIGHT;

// Prototipos
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void DoMovement(GLFWwindow* window);
void Animation();

// Cámara
//Camera camera(glm::vec3(0.0f, 2.0f, 10.0f));
bool keys[1024];
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool firstMouse = true;

//Camera
Camera camera(glm::vec3(0.0f, 10.0f, 3.0f));

float cameraDistance = 12.0f;
float cameraHeight = 5.0f;
float cameraYaw = 0.0f;
float cameraPitch = 1.0f;

float mouseSensitivity = 0.4f;

float MovementSpeed = 0.2f;

//Timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
//double	deltaTime = 0.0f,
//lastFrame = 0.0f;

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f,
rotX = 0.0f;

// Atributos de Luz y Cielo
glm::vec3 lightPos1(0.0f, 10.0f, 10.0f);
glm::vec3 lightPos2(0.0f, -10.0f, -10.0f);
glm::vec3 skyColor(0.1f, 0.1f, 0.12f); // Declaración única de skyColor

// DeltaTime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// AGUA Objeto que controla el agua animada
AguaAnimada aguaAnimada;


//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
posY = 1.0f,
posZ = 50.0f,
rotRodIzq = 0.0f,
giroMonito = 180.0f,
rotBrazoDer = 0.0f,
rotBrazoIzq = 0.0f,
rotPiernaDer = 0.0f,
rotPiernaIzq = 0.0f,
rotCabezaSi = 0.0f,
rotCabezaNo = 0.0f;

float tiempoCaminata = 0.0f;
//float velocidadCaminata = 0.02f;
float anguloMaximo = 80.0f;
float anguloCaminata = 0.0f;
float velocidadCaminata = 160.0f;
float direccionCaminata = 1.0f;

float	incX = 0.0f,
incY = 0.0f,
incZ = 0.0f,
rotRodIzqInc = 0.0f,
giroMonitoInc = 0.0f,
rotBrazoDerInc = 0.0f,
rotBrazoIzqInc = 0.0f,
rotPiernaDerInc = 0.0f,
rotPiernaIzqInc = 0.0f,
rotCabezaSiInc = 0.0f,
rotCabezaNoInc = 0.0f;

bool perspect = false;

int main()
{
    glfwInit();

    // Obtener resolución del monitor
    const GLFWvidmode* modo = glfwGetVideoMode(glfwGetPrimaryMonitor());

    SCREEN_WIDTH = modo->width;
    SCREEN_HEIGHT = modo->height;

    // Configuración de OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Crear ventana con resolución del monitor
    GLFWwindow* window = glfwCreateWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "Proyecto final. Computacion grafica",
        glfwGetPrimaryMonitor(),
        nullptr
    );

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSetWindowPos(window, 0, 30);
    glfwMakeContextCurrent(window);
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    //glfwSetKeyCallback(window, DoMovement);

    glfwMakeContextCurrent(window);

    // Capturar el mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    std::cout << "Ancho: " << SCREEN_WIDTH << std::endl;
    std::cout << "Alto: " << SCREEN_HEIGHT << std::endl;

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    

    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Opciones globales OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Shaders
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
    Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
    AguaAnimada::crearArchivosShader("Shader");						
    Shader aguaShader("Shader/agua.vs", "Shader/agua.fs");		

    // Modelos
    Model escena_principal((char*)"Models/escenaPrincipal.obj");
    // ANIMACIONES 
    Model agua((char*)"Models/agua.obj"); 
    Model sol((char*)"Models/modeloSol.obj");
    Model luna((char*)"Models/modeloLuna.obj");

    Model CabezaAquaman((char*)"ModelosCarlos/Aquaman/Cabeza.obj");
    Model CuerpoAquaman((char*)"ModelosCarlos/Aquaman/Cuerpo.obj");
    Model BrazoIzqAquaman((char*)"ModelosCarlos/Aquaman/BrazoIzq.obj");
    Model BrazoDerAquaman((char*)"ModelosCarlos/Aquaman/BrazoDer.obj");
    Model PiernaIzqAquaman((char*)"ModelosCarlos/Aquaman/PiernaIzq.obj");
    Model PiernaDerAquaman((char*)"ModelosCarlos/Aquaman/PiernaDer.obj");
    Model BotaIzqAquaman((char*)"ModelosCarlos/Aquaman/BotaIzq.obj");
    Model BotaDerAquaman((char*)"ModelosCarlos/Aquaman/BotaDer.obj");
    Model Traja((char*)"ModelosCarlos/traja1/trajinera1.obj");


    // create transformations and Projection
    glm::mat4 modelOp = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
    glm::mat4 viewOp = glm::mat4(1.0f);		//Use this matrix for ALL models
    glm::mat4 projectionOp = glm::mat4(1.0f);	//This matrix is for Projection

    // AGUA Parámetros de las olas. 
    //   amplitud   = qué tan altas son las olas
    //   frecuencia = qué tan juntas están (más chico = olas más anchas)
    //   velocidad  = qué tan rápido se mueven
    aguaAnimada.amplitud = 0.08f;
    aguaAnimada.frecuencia = 1.0f;
    aguaAnimada.velocidad = 1.8f;
    aguaAnimada.transparencia = 0.85f;
    aguaAnimada.texturaDelModelo = true;	
    // aguaAnimada.colorAgua = glm::vec3(0.05f, 0.35f, 0.55f);

    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        DoMovement(window);
        Animation();

        // Limpiado de pantalla con el color dinámico del cielo
        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();

        // ====================================================================
        // DIBUJO DE ESCENA PRINCIPAL
        // ====================================================================
        lightingShader.Use();

        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // --- 1. LUZ DIRECCIONAL (SOL) ---
        glm::vec3 dirSol = glm::normalize(-lightPos1);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), dirSol.x, dirSol.y, dirSol.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.55f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 1.2f, 1.15f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

        // --- 2. LUZ PUNTUAL (LUNA) ---
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), lightPos2.x, lightPos2.y, lightPos2.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.2f, 0.2f, 0.35f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 0.6f, 0.7f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.007f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.0002f);

        // --- 3. SPOTLIGHT (DESACTIVADA) ---
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

        // --------------------------------------------------------------------
        // A1. DIBUJAR ESCENA PRINCIPAL (ESTÁTICA)
        // --------------------------------------------------------------------
        glUniform1i(glGetUniformLocation(lightingShader.Program, "isWater"), 0);
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);

        //glm::mat4 modelEscena(1.0f);
        glm::mat4 modelEscena = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f)
        );

        modelEscena = glm::scale(
            modelEscena,
            glm::vec3(3.0f,3.0f,4.0f)
        );
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelEscena));
        escena_principal.Draw(lightingShader);

        // (El agua ya NO se dibuja aquí: ahora se dibuja al final, con su propio shader)

        // ====================================================================
        // DIBUJO DE LÁMPARAS (SOL Y LUNA)
        // ====================================================================
        lampShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Sol
        glm::mat4 modeloSol(1.0f);
        modeloSol = glm::translate(modeloSol, lightPos1);
        modeloSol = glm::rotate(modeloSol, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        modeloSol = glm::scale(modeloSol, glm::vec3(1.5f, 1.5f, 1.5f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modeloSol));
        glUniform3f(glGetUniformLocation(lampShader.Program, "lightColor"), 1.0f, 0.7f, 0.0f);
        sol.Draw(lampShader);

        // Luna
        glm::mat4 modeloLuna(1.0f);
        modeloLuna = glm::translate(modeloLuna, lightPos2);
        modeloLuna = glm::rotate(modeloLuna, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        modeloLuna = glm::scale(modeloLuna, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modeloLuna));
        glUniform3f(glGetUniformLocation(lampShader.Program, "lightColor"), 0.7f, 0.8f, 1.0f);
        luna.Draw(lampShader);

        // ====================================================================
        // [AGUA] AGUA ANIMADA (se dibuja al final porque es transparente)
        // ====================================================================
        // La luz del agua sigue al sol: de día brilla, de noche se oscurece
        aguaAnimada.luzDir = dirSol;
        float intensidadSol = glm::clamp(lightPos1.y / 35.0f, 0.15f, 1.0f);
        aguaAnimada.luzColor = glm::vec3(1.0f, 0.97f, 0.9f) * intensidadSol;

        //glm::mat4 modelAgua(1.0f);	// si necesitas mover o escalar el agua, hazlo aquí
        glm::mat4 modelAgua = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f)
        );
        modelAgua = glm::scale(
            modelAgua,
            glm::vec3(3.0f,3.0f,4.0f)
        );
        aguaAnimada.drawModelo(aguaShader, agua, modelAgua, view, projection,
            camera.GetPosition(), (float)glfwGetTime());

        


        //-----------------------------------------------------------------------------
        //Aquaman
        //-----------------------------------------------------------------------------
        /*staticShader.use();
        staticShader.setMat4("projection", projectionOp);
        staticShader.setMat4("view", viewOp);*/

        

        // Matriz de referencia del cuerpo de Aquaman
        glm::mat4 refCuerpoAquaman = glm::mat4(0.5f);

        //refCuerpoAquaman = modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
        //refCuerpoAquaman = modelOp = glm::rotate(modelOp, glm::radians(giroMonito), glm::vec3(0.0f, 1.0f, 0.0f));

        

        // Activar shader de iluminación
        lightingShader.Use();

        // CUERPO
        // Posición inicial del personaje
        modelOp = refCuerpoAquaman = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(posX, posY, posZ)
        );

        modelOp = refCuerpoAquaman = glm::rotate(
            refCuerpoAquaman,
            glm::radians(giroMonito),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Escala de todo Aquaman
        modelOp = refCuerpoAquaman = glm::scale(
            refCuerpoAquaman,
            glm::vec3(0.4f)
        );


        glUniformMatrix4fv(
            glGetUniformLocation(lightingShader.Program, "model"),
            1, GL_FALSE, glm::value_ptr(modelOp)
        );

        CuerpoAquaman.Draw(lightingShader);


        // CABEZA
        modelOp = glm::translate(
            refCuerpoAquaman,
            glm::vec3(0.0f, 1.5f, 0.0f)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(lightingShader.Program, "model"),
            1, GL_FALSE, glm::value_ptr(modelOp)
        );

        CabezaAquaman.Draw(lightingShader);


        // BRAZO DERECHO
        modelOp = glm::translate(
            refCuerpoAquaman,
            glm::vec3(-0.75f, 1.5f, 0.0f)
        );

        modelOp = glm::rotate(modelOp, glm::radians(rotBrazoDer), glm::vec3(1.0f, 0.0f, 0.0f));

        glUniformMatrix4fv(
            glGetUniformLocation(lightingShader.Program, "model"),
            1, GL_FALSE, glm::value_ptr(modelOp)
        );

        BrazoDerAquaman.Draw(lightingShader);


        // BRAZO IZQUIERDO
        modelOp = glm::translate(
            refCuerpoAquaman,
            glm::vec3(0.75f, 1.5f, 0.0f)
        );

        modelOp = glm::rotate(modelOp, glm::radians(rotBrazoIzq), glm::vec3(1.0f, 0.0f, 0.0f));

        glUniformMatrix4fv(
            glGetUniformLocation(lightingShader.Program, "model"),
            1, GL_FALSE, glm::value_ptr(modelOp)
        );

        BrazoIzqAquaman.Draw(lightingShader);


        // PIERNA DERECHA
        glm::mat4 refPiernaDer = modelOp = glm::translate(
            refCuerpoAquaman,
            glm::vec3(-0.5f, 0.0f, -0.1f)
        );

        refPiernaDer = modelOp = glm::rotate(modelOp, glm::radians(rotPiernaDer), glm::vec3(1.0f, 0.0f, 0.0f));

        glUniformMatrix4fv(
            glGetUniformLocation(lightingShader.Program, "model"),
            1, GL_FALSE, glm::value_ptr(modelOp)
        );

        PiernaDerAquaman.Draw(lightingShader);


        // PIERNA IZQUIERDA
        glm::mat4 refPiernaIzq = modelOp = glm::translate(
            refCuerpoAquaman,
            glm::vec3(0.5f, 0.0f, -0.1f)
        );

        refPiernaIzq = modelOp = glm::rotate(modelOp, glm::radians(rotPiernaIzq), glm::vec3(1.0f, 0.0f, 0.0f));

        glUniformMatrix4fv(
            glGetUniformLocation(lightingShader.Program, "model"),
            1, GL_FALSE, glm::value_ptr(modelOp)
        );

        PiernaIzqAquaman.Draw(lightingShader);


        // BOTA DERECHA
        /*modelOp = glm::translate(
            refCuerpoAquaman,
            glm::vec3(-0.5f, -0.9f, 0.0f)
        );*/

        modelOp = glm::translate(
            refPiernaDer, 
            glm::vec3(0.0f, -0.9f, 0.0f)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(lightingShader.Program, "model"),
            1, GL_FALSE, glm::value_ptr(modelOp)
        );

        BotaDerAquaman.Draw(lightingShader);


        // BOTA IZQUIERDA
        /*modelOp = glm::translate(
            refCuerpoAquaman,
            glm::vec3(0.5f, -0.9f, 0.0f)
        );*/

        modelOp = glm::translate(
            refPiernaIzq, 
            glm::vec3(0.0f, -0.9f, 0.0f)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(lightingShader.Program, "model"),
            1, GL_FALSE, glm::value_ptr(modelOp)
        );

        BotaIzqAquaman.Draw(lightingShader);

        //Trajinera
        glm::mat4 modelTrajinera = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(-35.0f, 0.0f, -20.0f)
        );

        modelTrajinera = glm::rotate(
            modelTrajinera,
            glm::radians(20.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        modelTrajinera = glm::scale(
            modelTrajinera,
            glm::vec3(0.18f)
        );
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelTrajinera));
        Traja.Draw(lightingShader);


        glfwSwapBuffers(window);

    }


    glfwTerminate();
    return 0;
}

void Animation()
{
    // ------------------------------------------------------------------------
    // 1. Órbita Combinada (Circular/Translacional + Movimiento Vertical)
    // ------------------------------------------------------------------------
    float radiusX = 40.0f; // Radio de la órbita en el eje X
    float radiusZ = 30.0f; // Radio de la órbita en el eje Z (profundidad)
    float maxHeight = 35.0f; // Altura máxima en Y (Cénit del mediodía)

    float speed = 0.01f;    // Velocidad del ciclo día/noche
    float angle = (float)glfwGetTime() * speed;

    // El movimiento orbital traslada el objeto en X y Z mientras asciende/desciende en Y
    float solX = cos(angle) * radiusX;
    float solY = sin(angle) * maxHeight;
    float solZ = sin(angle) * radiusZ;

    // Posición del Sol
    lightPos1 = glm::vec3(solX, solY, solZ);

    // La Luna se coloca exactamente en el extremo opuesto de la órbita 3D
    lightPos2 = glm::vec3(-solX, -solY, -solZ);

    // ------------------------------------------------------------------------
    // 2. Transición dinámica del color del cielo (Día / Atardecer / Noche)
    // ------------------------------------------------------------------------
    // Normalizamos la altura vertical del Sol (solY) de [-maxHeight, maxHeight] a [0.0, 1.0]
    float factorDia = glm::clamp(solY / maxHeight, 0.0f, 1.0f);

    glm::vec3 colorNoche = glm::vec3(0.02f, 0.02f, 0.08f); // Azul noche oscuro
    glm::vec3 colorDia = glm::vec3(0.45f, 0.65f, 0.95f); // Azul cielo claro

    // Interpolación suave del color según el progreso diurno del Sol
    skyColor = glm::mix(colorNoche, colorDia, factorDia);


    if (perspect == true)
    {
        float angle = glm::radians(giroMonito);

        glm::vec3 camPos(
            posX - sin(angle) * cameraDistance,
            posY + cameraHeight,
            posZ - cos(angle) * cameraDistance
        );

        // Actualizar posición de la cámara
        camera.SetPosition(camPos);

        // Punto al que debe mirar la cámara
        glm::vec3 target(
            posX,
            posY + cameraPitch*0.1f,
            posZ
        );

        // Actualizar dirección de la cámara
        camera.SetFront(target - camPos);

        // ANIMACIÓN DE CAMINAR

        // Actualizar ángulo
        anguloCaminata += direccionCaminata * velocidadCaminata * deltaTime;

        // Limitar el movimiento entre -60 y 60
        if (anguloCaminata >= 60.0f) {
            anguloCaminata = 60.0f;
            direccionCaminata = -1.0f;
        }
        else if (anguloCaminata <= -60.0f) {
            anguloCaminata = -60.0f;
            direccionCaminata = 1.0f;
        }

        // Alternar brazos
        rotBrazoDer = anguloCaminata;
        rotBrazoIzq = -anguloCaminata;

        // Alternar piernas
        rotPiernaDer = -anguloCaminata;
        rotPiernaIzq = anguloCaminata;

        perspect = false;
    }
    else {
        float angle = glm::radians(giroMonito);

        glm::vec3 camPos(
            posX - sin(angle) * cameraDistance,
            posY + cameraHeight,
            posZ - cos(angle) * cameraDistance
        );

        // Actualizar posición de la cámara
        camera.SetPosition(camPos);

        // Punto al que debe mirar la cámara
        glm::vec3 target(
            posX,
            posY + cameraPitch*0.1f,
            posZ
        );

        // Actualizar dirección de la cámara
        camera.SetFront(target - camPos);

    }


}

void DoMovement(GLFWwindow* window)
{
    /*if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);

    if (keys[GLFW_KEY_I]) lightPos1.y += 0.05f;
    if (keys[GLFW_KEY_K]) lightPos1.y -= 0.05f;
    if (keys[GLFW_KEY_J]) lightPos1.x -= 0.05f;
    if (keys[GLFW_KEY_L]) lightPos1.x += 0.05f;*/

    float velocidad = 8.0f * deltaTime;

    // Dirección hacia donde está orientado Aquaman
    float angulo = glm::radians(giroMonito);

    glm::vec3 direccion(
        sin(angulo),
        0.0f,
        cos(angulo)
    );

    // Movimiento hacia adelante
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        posX += direccion.x * velocidad;
        posZ += direccion.z * velocidad;
        perspect = true;
    }

    // Movimiento hacia atrás
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        posX -= direccion.x * velocidad;
        posZ -= direccion.z * velocidad;
        perspect = true;
    }

    // Movimiento hacia la derecha
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        posX -= direccion.z * velocidad;
        posZ += direccion.x * velocidad;
        perspect = true;
    }

    // Movimiento hacia la izquierda
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        posX += direccion.z * velocidad;
        posZ -= direccion.x * velocidad;
        perspect = true;
    }

    // Rotación del personaje
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        giroMonito -= 1.0f;

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        giroMonito += 1.0f;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }


    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = (GLfloat)xPos;
        lastY = (GLfloat)yPos;
        firstMouse = false;
    }

    GLfloat xOffset = (GLfloat)xPos - lastX;
    GLfloat yOffset = lastY - (GLfloat)yPos;

    lastX = (GLfloat)xPos;
    lastY = (GLfloat)yPos;

    //camera.ProcessMouseMovement(xOffset, yOffset);
    //camera.ProcessMouseMovement(xoffset, yoffset);

    //giroMonito += (float)xoffset * 0.1;
    giroMonito -= (float)xOffset * mouseSensitivity;

    // El movimiento vertical inclina solamente la cámara
    cameraPitch += yOffset * mouseSensitivity * 3;

    // Limitar la inclinación para evitar que la cámara dé vueltas
    cameraPitch = glm::clamp(cameraPitch, -5.0f, 55.0f);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}