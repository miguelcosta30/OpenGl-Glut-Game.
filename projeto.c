
#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#else
#include <GL/glut.h>
#include <Windows.h>
#include <Mmsystem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#endif
#include "glm.h"

/**************************************
************* CONSTANTE PI ************
**************************************/

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

/**************************************
* AUXILIARES CONVERSÃO GRAUS-RADIANOS *
**************************************/

#define rtd(x) (180 * (x) / M_PI)
#define dtr(x) (M_PI * (x) / 180)
#define LADO 100
#define DEBUG 1
#define QTD_TEXT 13

/**************************************
***** INICIALIZACAO DE ESTRUTURAS *****
**************************************/
typedef struct
{
        GLboolean up, down, left, right, a, d;

} Teclas;

typedef struct
{
        GLfloat x, y, z, angle;

} Objeto;

typedef struct
{
        GLuint textura[QTD_TEXT];
} Modelo;

Modelo modelo;
Teclas teclas;
Objeto lua, sol;

/* Número de objetos */
int num_obj = 100;        //Num de nuvens a cada 10k
int num_arvores = 250;    //Numero de arvores a cada 10k
int num_powerups = 10;    //Num powerups a cada 100k
int num_pedras = 1000;    //Num pedras a cada 10k
int num_montanhas = 150;  //Num de montanhas laterais a cada 10k
int num_obstaculos = 150; //Num de obstaculos a cada 100k
bool dia = true;          //Para definir o ceu
int ceu = -20000;         //Intervalo de tempo em que o seu muda de cor

/* Variaveis de auxilio de power_ups*/

int vida = 3;                //Num de vidas
int invencivel = 0;          //Flag de powerup de invencibilidade
int super_salto = 0;         //Flag de powerup de invencibilidade
int counter_super_salto = 5; //Num de super saltos
float redesenhar = 0;        //Aux para redesenhar o cenario
float redesenhar_pista = 0;  //Aux para redesenhar a pista

clock_t start, end;      //Timer para o powerup da invencibilidade e
double tempo_invencivel; //Tempo invencivel
int frame = 0, times, timebase = 0, fps;   //Variaveis auxiliares para contar os FPS
    

/*Variaveis de controlo de menus , displays 2D , auxiliares para o salto ,etc */

bool fullscreen = false;       //Flag para fullscreen
bool paused = false;           //Flag para pausa
bool menu_principal = true;    //Flag para menu  inicial
bool angle_left = true;        //Aux para animacao de menu inicial
bool angle_right = false;      //Aux para animacao de menu inicial
int mx = 0, my = 0;            //Cordenadas do rato
int click_left = 0;            //Clica esquerdo do rato
int WIDTH = 800, HEIGHT = 600; //Resoluçao inicial da janela
int JUMP_UP, JUMP_DOWN;        //Aux para salto
int JUMP_LIMITE = 150;         //Limite do salto
bool fim_salto = true;


/*Vetores que possuem as coordenadas de diferentes tipos de objetos */

GLfloat random_obj[250][3];
GLfloat random_nuvem[250][3];
GLfloat random_pedras[1000][3];
GLfloat random_lateral[100][6];
GLfloat random_obstaculos[150][7];
GLfloat random_powerups[100][4];
GLfloat grades[150][4];

/* Ângulo para a direção da camera */
float angle = 0.0;
/* Representação da direção da camera */
float lx = 0.0f, lz = -1.0f, ly = 1.0f;
/* Posição da camera */
float x = -0.0f, z = 1.0f, y = 70.0f;

/*Melhoria de performance*/

GLuint theObject;
void arvores();
void nuvens();
void pedras();


/* Função para posicionar a camera */
void pos_camera()
{
        gluLookAt(x, y, z, x + lx, y - 0.15, z + lz, 0.0f, 1.0f, 0.0f);
}

/* Função para mover a camera */
void move_camera()
{
        if (invencivel == 1 && tempo_invencivel <= 11) //Se estiver invencivel
        {
                end = clock();
                tempo_invencivel = ((double)(end - start)) / CLOCKS_PER_SEC;
                z -= 90;
        }
        else //Se não estiver
        {
                tempo_invencivel = 0;
                invencivel = 0;
                z -= 30.0;
        }
}

/*Geração de todas as texturas*/ 

void gerarTexturas(void)
{
        unsigned char *image = NULL;
        int w, h;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(QTD_TEXT, modelo.textura);

        image = glmReadPPM("textures/pedra.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        image = glmReadPPM("textures/wood.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        image = glmReadPPM("textures/woodinside.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[2]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        image = glmReadPPM("textures/cerca.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[3]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        image = glmReadPPM("textures/heart.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[4]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        image = glmReadPPM("textures/boost.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[5]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        image = glmReadPPM("textures/estrela.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[6]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        image = glmReadPPM("textures/quit.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[7]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        image = glmReadPPM("textures/customize.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[8]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        image = glmReadPPM("textures/back_to_game_button.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[9]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        image = glmReadPPM("textures/play.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[10]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        image = glmReadPPM("textures/done.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[11]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        image = glmReadPPM("textures/playagain.ppm", &w, &h);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[12]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void fog()
{
        float fog_color[4];
        if (!dia)
        {

                fog_color[0] = 0.0;
                fog_color[1] = 0.0;
                fog_color[2] = 0.0;
                fog_color[3] = 1.0;
        }
        else
        {
                fog_color[0] = 0.0;
                fog_color[1] = 0.6;
                fog_color[2] = 0.8;
                fog_color[3] = 1.0;
        }

        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, 5000);                            
        glFogf(GL_FOG_END, 6000);
        glFogf(GL_FOG_DENSITY, 1.0);
        glFogfv(GL_FOG_COLOR, fog_color);
}

void lighting()
{
        glEnable(GL_COLOR_MATERIAL);
        GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
        GLfloat diffuseLight[] = {0.8f, 0.8f, 0.8, 1.0f+z};
        GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0+z};
        GLfloat mat_shininess[] = {50.0};
        if (dia)
        {
                GLfloat light_position[] = {sol.x + x, sol.y + y, sol.z + z, 3.0}; //Ponto de luz = sol
                glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        }
        else
        {
                GLfloat light_position[] = {lua.x + x, lua.y + y, lua.z + z, 1.0}; //Ponto de luz = Lua
                glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        }
        glLightfv(GL_LIGHT1, GL_POSITION, diffuseLight);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glShadeModel(GL_SMOOTH);
        glMaterialfv(GL_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_DEPTH_TEST);
}

void init(void)
{
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        theObject = glGenLists(3);
        glNewList(theObject, GL_COMPILE);
        arvores();
        glEndList();
        glNewList(theObject + 1, GL_COMPILE);
        nuvens();
        glEndList();
        glNewList(theObject + 2, GL_COMPILE);
        pedras();
        glEndList();
        gerarTexturas();
}

/* Função as coordenadas das laterais (Montanhas e cercas)*/

void laterais()
{
        int pos = 0;
        for (int i = 0; i < num_obj; i++)
        {
                if (i % 2 == 0)
                {
                        random_lateral[i][0] = -1000; //x
                        grades[i][0] = -270;
                }

                else
                {
                        random_lateral[i][0] = 1000; //x
                        grades[i][0] = 270;
                }

                random_lateral[i][1] = -40.0; //y
                grades[i][1] = -40.0;
                random_lateral[i][2] = (pos - 200); //z
                grades[i][2] = (pos - 100);
                pos = pos - 200;
                random_lateral[i][3] = (rand() % (400 - 200 + 1)) + 200;  //base
                random_lateral[i][4] = (rand() % (1000 - 300 + 1)) + 300; //altura
                random_lateral[i][5] = (rand() % (2 - 1 + 1)) + 1;        //cor
        }
}

/*Coordanadas das arvores*/

void random_arvores()
{

        for (int i = 0; i < num_arvores; i++)
        {
                if (i % 2 == 0)
                        random_obj[i][0] = (rand() % (900 - 300 + 1)) + 300; //900
                else
                        random_obj[i][0] = -(rand() % (900 - 300 + 1)) - 300; //-900

                random_obj[i][1] = 5.0;

                random_obj[i][2] = -((rand() % 10000));
        }
}

void random_nuvens()
{

        for (int i = 0; i < num_arvores; i++)
        {
                if (i % 2 == 0)
                        random_nuvem[i][0] = (rand() % 1500); //1500
                else
                        random_nuvem[i][0] = -(rand() % 1500); //-1500

                random_nuvem[i][1] = 5.0;

                random_nuvem[i][2] = -((rand() % 15000));
        }
}

void r_pedras()
{

        for (int i = 0; i < num_pedras; i++)
        {
                if (i % 2 == 0)
                        random_pedras[i][0] = (rand() % (300 - (-300) + 1)) + (-300); // 300
                else
                        random_pedras[i][0] = -(rand() % (300 - (-300) + 1)) - (-300); //-300

                random_pedras[i][1] = 2.5;

                random_pedras[i][2] = -((rand() % 15000));
        }
}

void r_powerups(float rZ)
{
        srand(time(0));
        for (int i = 0; i < num_powerups; i++)
        {
                if (i % 2 == 0)
                        random_powerups[i][0] = (rand() % (260 - (-260) + 1)) + (-260); // 300
                else
                        random_powerups[i][0] = -(rand() % (260 - (-260) + 1)) - (-260); //-300

                random_powerups[i][1] = 50.0;
                float k = ((float)rand()) / RAND_MAX * -100000 + 1;
                int j = k;
                random_powerups[i][2] = j + rZ;
                random_powerups[i][3] = (rand() % (3 - 1 + 1)) + 1; //cor;       //Tipo do power up
        }
}
void random_obstaculo(float Rz)
{
        srand(time(0));
        int pos = -500;
        for (int i = 0; i < num_obstaculos; i++)
        {
                random_obstaculos[i][1] = 5.0;
                random_obstaculos[i][2] = (pos) + Rz; //z

                if (random_obstaculos[i][2] > -2000) //Para ter obstaculos perto do spawn
                {
                        random_obstaculos[i][2] = (pos)-1000; //z
                }
                pos = pos - 700;
                random_obstaculos[i][3] = 1; //Tipo de obstaculo
                if (random_obstaculos[i][3] == 1)
                {
                        random_obstaculos[i][4] = (rand() % (30 - 10 + 1)) + 10;  //Altura
                        random_obstaculos[i][5] = (rand() % (150 - 50 + 1)) + 50; //comprimento
                }
                random_obstaculos[i][0] = (rand() % (270 - (-270) + 1)) + (-270);

                /*Verificação se os obstaculos não têm coordenadas que possam mais tarde ser fora da pista*/

                if (i % 3 == 0) //Se for uma parede
                {
                        if (random_obstaculos[i][0] + 80 > 260)
                        {
                                random_obstaculos[i][0] = 260 - 80;
                        }
                        if (random_obstaculos[i][0] - 80 < -260)
                        {
                                random_obstaculos[i][0] = -260 + 80;
                        }
                }
                else //Se for um tronco
                {
                        if (random_obstaculos[i][0] + random_obstaculos[i][5] > 260)
                        {
                                random_obstaculos[i][0] = 260 - random_obstaculos[i][5];
                        }
                        if (random_obstaculos[i][0] - random_obstaculos[i][5] < -260)
                        {
                                random_obstaculos[i][0] = -260 + random_obstaculos[i][5];
                        }
                }
        }
}

/* Função para desenhar solo */

void desenhar_solo()
{
        glDisable(GL_LIGHTING);
        glColor3f(0.9f, 0.9f, 0.9f);
        glBegin(GL_QUADS);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(-10000.0f, 0.0f, -10000.0f + z);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(10000.0f, 0.0f, -10000.0f + z);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(10000.0f, 0.0f, 100.0f + z);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(-10000.0f, 0.0f, 100.0f + z);
        glEnd();
        glColor3f(0.9f, 0.9f, 0.9f);
        glBegin(GL_QUADS);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(-10000.0f, 0.0f, +10000.0f + z);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(10000.0f, 0.0f, +10000.0f + z);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(10000.0f, 0.0f, 100.0f + z);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(-10000.0f, 0.0f, 100.0f + z);
        glEnd();
        glEnable(GL_LIGHTING);
}

void montanhas(float base, float height, int cor)
{
        GLUquadricObj *p = gluNewQuadric();
        gluQuadricDrawStyle(p, GL_LINES);
        glPushMatrix();
        if (cor == 1)
        {
                glColor3ub(141, 75, 0);
        }
        if (cor == 2)
        {
                glColor3ub(80, 57, 30);
        }
        glTranslatef(0, 0, -50);
        glRotatef(-90, 1, 0, 0);
        glTranslatef(-5.0, -20.0, 40.0);
        gluCylinder(p, base, 50, height, 20, 20);
        glPopMatrix();
}

void desenhar_montanhas_laterais(float rZ)
{
        for (int i = 0; i < num_obj; i++)
        {
                glPushMatrix();
                glTranslatef(random_lateral[i][0], random_lateral[i][1], random_lateral[i][2] + rZ);
                montanhas(random_lateral[i][3], random_lateral[i][4], random_lateral[i][5]);
                glPopMatrix();
        }
}

void sol_lua()
{
        lua.x = 300;
        sol.x = 300;
        lua.y = 700;
        sol.y = 700;
        lua.z = -5000;
        sol.z = -5000;
        lua.angle = 0.00f;
        sol.angle = 0.00f;
}

void checkColisionPowerUp()
{
        for (int i = 0; i < num_powerups; i++)
        {
                if (random_powerups[i][3] == 1)
                {
                        if ((z <= random_powerups[i][2] + 25 && z >= random_powerups[i][2] - 25) && (y <= random_powerups[i][1] + 35 && y>=random_powerups[i][1] - 35 ) && (x >= random_powerups[i][0] - 25 && x <= (random_powerups[i][0] + 25)))
                        {
                                z = (random_powerups[i][2] - 25) - 1; //Para nao bater vais vezes
                                if (vida <= 10)
                                {
                                        vida++;
                                }
                                PlaySound("sounds/vida.wav", NULL, SND_FILENAME | SND_ASYNC);
                                return;
                        }
                }
                if (random_powerups[i][3] == 2)
                {
                        if ((z <= random_powerups[i][2] + 25 && z >= random_powerups[i][2] - 25) && (y <= random_powerups[i][1] + 35 && y>=random_powerups[i][1] - 35) && (x >= random_powerups[i][0] - 25 && x <= (random_powerups[i][0] + 25)))
                        {
                                z = (random_powerups[i][2] - 25) - 1; //Para nao bater vais vezes
                                invencivel = 1;
                                PlaySound("sounds/invencivel.wav", NULL, SND_FILENAME | SND_ASYNC);
                                start = clock();
                                return;
                        }
                }
                if (random_powerups[i][3] == 3)
                {
                        if ((z <= random_powerups[i][2] + 25 && z >= random_powerups[i][2] - 25) && (y <= random_powerups[i][1] + 35 && y>=random_powerups[i][1] - 35) && (x >= random_powerups[i][0] - 25 && x <= (random_powerups[i][0] + 25)))
                        {
                                z = (random_powerups[i][2] - 25) - 1; //Para nao bater vais vezes
                                super_salto = 1;
                                return;
                        }
                }
        }
}
void desenhar_powerups(float rZ)
{

        for (int i = 0; i < num_powerups; i++)
        {
                if (random_powerups[i][3] == 1)
                {
                        glColor3f(225, 0, 0); //Vermelho -> +1 Vida
                }
                if (random_powerups[i][3] == 2)
                {
                        glColor3f(225, 225, 0); //Amarelo -> Invencivel durante 10s
                }
                if (random_powerups[i][3] == 3)
                {
                        glColor3f(0, 0, 225); //Azul -> 10 Super Pulos
                }
                glPushMatrix();
                glTranslatef(random_powerups[i][0], random_powerups[i][1], random_powerups[i][2]);
                glutSolidSphere(30, 50, 50);
                glPopMatrix();
                checkColisionPowerUp();
        }
}

int checkColision(int altura, int comprimento, int largura)
{

        for (int i = 0; i < num_obstaculos; i++)
        {
                if (i % 3 == 0)
                {
      if ((z <= random_obstaculos[i][2] + largura / 2 && z >= random_obstaculos[i][2] - largura / 2) && (y <= random_obstaculos[i][1] + altura) && (x >= random_obstaculos[i][0] - comprimento / 2 && x <= (random_obstaculos[i][0] + comprimento / 2)) && invencivel == 0)
                        {
                                z = (random_obstaculos[i][2] - largura) - 1; //Para nao bater vais vezes
                                PlaySound("sounds/dano.wav", NULL, SND_ASYNC);
                                return 1;
                        }
                }
                else
                {
       if ((z <= random_obstaculos[i][2] + largura / 2 && z >= random_obstaculos[i][2] - largura / 2) && (y <= random_obstaculos[i][1] + altura / 2) && (x >= random_obstaculos[i][0] && x <= random_obstaculos[i][0] + random_obstaculos[i][5]) && invencivel == 0) //hitbox com ajustes
                        {
                                z = (random_obstaculos[i][2] - largura) - 1; //Para nao bater vais vezes
                                PlaySound("sounds/dano.wav", NULL, SND_ASYNC);
                                return 1;
                        }
                }
        }
        return 0;
}

void retangulo2d(int x1, int y1, int x2, int y2, int textura_id)
{
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[textura_id]);
        glBegin(GL_QUADS);

        if (textura_id < 0) //Pinta de preto
        {
                glColor3f(0, 0, 0);
        }
        else
        {
                glColor3f(1, 1, 1);
        }

        glTexCoord2f(0.0, 1.0);
        glVertex2d(x1, y1);
        glTexCoord2f(1.0, 1.0);
        glVertex2d(x2, y1);
        glTexCoord2f(1.0, 0.0);
        glVertex2d(x2, y2);
        glTexCoord2f(0.0, 0.0);
        glVertex2d(x1, y2);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
}

void quadrado2d(double x1, double y1, double sidelength, int textura_id, int trans)
{
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, modelo.textura[textura_id]);
        double halfside = sidelength / 2;
        glBegin(GL_POLYGON);

        if (trans == 1)
        {
                glColor4f(1, 1, 1, 0.0); //Aplica transparencia
        }
        else
        {
                glColor3f(1, 1, 1);
        }

        glTexCoord2f(0.0, 0.0);
        glVertex2d(x1 + halfside, y1 + halfside);
        glTexCoord2f(0.0, 1.0);
        glVertex2d(x1 + halfside, y1 - halfside);
        glTexCoord2f(1.0, 1.0);
        glVertex2d(x1 - halfside, y1 - halfside);
        glTexCoord2f(1.0, 0.0);
        glVertex2d(x1 - halfside, y1 + halfside);
        glEnd();
        glDisable(GL_TEXTURE_2D);
}

void desenhaPoligono(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat d[], GLfloat cor[], int replica)
{

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glBegin(GL_QUADS);

        glColor3fv(cor);
        glTexCoord2f(0.0, 0.0);
        glNormal3f(1.0, 0, 0);
        glVertex3fv(a);
        glTexCoord2f(replica, 0.0);
        glNormal3f(1.0, 0, 0);
        glVertex3fv(b);
        glTexCoord2f(replica, replica);
        glNormal3f(1.0, 0, 0);
        glVertex3fv(c);
        glTexCoord2f(0.0, replica);
        glNormal3f(1.0, 0, 0);
        glVertex3fv(d);
        glEnd();
}

void desenhaCubo(int textura_id, int replica, int transparencia)
{

        GLfloat cores[3];
        glEnable(GL_TEXTURE_2D);

        GLfloat vertices[][3] = {
            {-0.5, -0.5, -0.5},
            {0.5, -0.5, -0.5},
            {0.5, 0.5, -0.5},
            {-0.5, 0.5, -0.5},
            {-0.5, -0.5, 0.5},
            {0.5, -0.5, 0.5},
            {0.5, 0.5, 0.5},
            {-0.5, 0.5, 0.5}};

        if (textura_id >= 0)
        {
                cores[0] = 255;
                cores[1] = 255;
                cores[2] = 255;
        }
        else
        {
                cores[0] = 1;
                cores[1] = 1;
                cores[2] = 1;
        }

        glBindTexture(GL_TEXTURE_2D, modelo.textura[textura_id]);
        desenhaPoligono(vertices[1], vertices[0], vertices[3], vertices[2], cores, replica);
        desenhaPoligono(vertices[2], vertices[3], vertices[7], vertices[6], cores, replica);
        desenhaPoligono(vertices[3], vertices[0], vertices[4], vertices[7], cores, replica);
        desenhaPoligono(vertices[6], vertices[5], vertices[1], vertices[2], cores, replica);
        desenhaPoligono(vertices[4], vertices[5], vertices[6], vertices[7], cores, replica);
        desenhaPoligono(vertices[5], vertices[4], vertices[0], vertices[1], cores, replica);
        glDisable(GL_TEXTURE_2D);
}

void desenhar_cercas(float rZ)
{
        for (int i = 0; i < num_obj; i++)
        {
                glColor3ub(153, 76, 0);

                glPushMatrix();
                glTranslatef(grades[i][0], 0, grades[i][2] + rZ);                       //Pau para cima
                glScaled(14, 100, 14);
                desenhaCubo(1, 2, 0);
                glPopMatrix();

                glPushMatrix();
                glTranslatef(grades[i][0], 35, grades[i][2] + rZ);                      //Pau de cima deitado
                glScaled(12, 12, 400);
                desenhaCubo(3, 10, 0);
                glPopMatrix();

                glPushMatrix();
                glTranslatef(grades[i][0], 17.5, grades[i][2] + rZ);                    //Pau de baixo deitado
                glScaled(12, 12, 400);
                desenhaCubo(3, 10, 0);
                glPopMatrix();
        }
}

void desenhar_obstaculos(float rZ)
{

        for (int i = 0; i < num_obstaculos; i++)
        {
                int altura, comprimento, largura;
                GLUquadricObj *quadratic;
                quadratic = gluNewQuadric();

                if (i % 3 != 0)
                {

                        glPushMatrix();
                        glColor3f(255, 255, 255);
                        glTranslatef(random_obstaculos[i][0], 0, random_obstaculos[i][2] + 100);                
                        glRotatef(90, 0, 1, 0);
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, modelo.textura[1]);
                        gluQuadricDrawStyle(quadratic, GLU_FILL);
                        gluQuadricNormals(quadratic, GLU_SMOOTH);
                        gluQuadricTexture(quadratic, GL_TRUE);
                        gluCylinder(quadratic, random_obstaculos[i][4], random_obstaculos[i][4], random_obstaculos[i][5], 32, 32);       //Desenha tronco de madeira deitado
                        glDisable(GL_TEXTURE_2D);
                        
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, modelo.textura[2]);
                        gluQuadricDrawStyle(quadratic, GLU_FILL);
                        gluQuadricNormals(quadratic, GLU_SMOOTH);
                        gluQuadricTexture(quadratic, GL_TRUE);
                        gluDisk(quadratic, 0, random_obstaculos[i][4], 32, 32);                                                         //Desenha disco do lado esquerdo
                        glDisable(GL_TEXTURE_2D);
                        glPopMatrix();

                        glPushMatrix();
                        glColor3f(255, 255, 255);
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, modelo.textura[2]);
                        gluQuadricDrawStyle(quadratic, GLU_FILL);
                        gluQuadricNormals(quadratic, GLU_SMOOTH);
                        gluQuadricTexture(quadratic, GL_TRUE);
                        glTranslatef(random_obstaculos[i][0]+random_obstaculos[i][5], 0, random_obstaculos[i][2] + 100);
                        glRotatef(90, 0, 1, 0);
                        gluDisk(quadratic, 0, random_obstaculos[i][4], 32, 32);                                                      //Desenha disco do lado direito
                        glDisable(GL_TEXTURE_2D);
                        glPopMatrix();

                        altura = random_obstaculos[i][4];
                        comprimento = random_obstaculos[i][5];
                        largura = random_obstaculos[i][4];
                }
                else                                                            //Desenha parede
                {
                        glPushMatrix();
                        glTranslatef(random_obstaculos[i][0], 40, random_obstaculos[i][2]);
                        glScaled(160, 80, 30);
                        desenhaCubo(0, 5, 0);
                        altura = 160;
                        comprimento = 160;
                        largura = 30;
                        glPopMatrix();
                }
                if (checkColision(altura, comprimento, largura) == 1)
                {
                        vida--;
                }
        }
}

/* Função para desenhar o sol */
void desenhar_sol()
{
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 0.0f);
        glTranslatef(sol.x + x, sol.y + y, sol.z + z);
        sol.angle += 0.05f;
        glutSolidSphere(50, 30, 20);
        glPopMatrix();
        glEnable(GL_LIGHTING);
}

void desenhar_lua()
{
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(lua.x + x, lua.y + y, lua.z + z);
        glutSolidSphere(50, 30, 20);
        glPopMatrix();
        glPushMatrix();
        glColor3f(0.0f, 0.0f, 0.0f);
        glTranslatef(300 - 50 + x, 700 + y, -5000 + z + 100);
        glutSolidSphere(50, 30, 20);
        glPopMatrix();
        glEnable(GL_LIGHTING);
}

/* Funções para desenhar nuvens */

void nuvens()
{
        glPushMatrix();

        glColor3ub(224, 224, 224);
        glTranslatef(0, 20, -40);
        glutSolidSphere(10, 20, 20);
        glPopMatrix();

        glPushMatrix();
        glColor3ub(224, 224, 224);
        glTranslatef(0, 20, -30);
        glutSolidSphere(5, 20, 20);
        glPopMatrix();

        glPushMatrix();
        glColor3ub(224, 224, 224);
        glTranslatef(-10, 30, -30);
        glutSolidSphere(10, 30, 20);
        glPopMatrix();

        glPushMatrix();
        glColor3ub(224, 224, 224);
        glTranslatef(0, 25, -30);
        glutSolidSphere(10, 20, 20);
        glPopMatrix();

        glPushMatrix();
        glColor3ub(224, 224, 224);
        glTranslatef(0, 10, -30);
        glutSolidSphere(10, 20, 20);
        glPopMatrix();

        glPushMatrix();
        glColor3ub(224, 224, 224);
        glTranslatef(0, 20, -50);
        glutSolidSphere(10, 20, 20);
        glPopMatrix();

        glPushMatrix();
        glColor3ub(224, 224, 224);
        glTranslatef(-20, 20, -50);
        glutSolidSphere(20, 20, 20);
        glPopMatrix();

        glPushMatrix();
        glColor3ub(224, 224, 224);
        glTranslatef(-15, 20, -10);
        glutSolidSphere(20, 20, 20);
        glPopMatrix();
}
void desenhar_nuvens(float rZ)
{
        float mcolor[] = {1.0f, 0.0f, 0.0f, 1.0f};
        GLfloat qaWhite[] = {1.0, 1.0, 1.0, 1.0};
        GLfloat qaRed[] = {1.0, 0.0, 0.0, 1.0};
        glMaterialfv(GL_LEFT, GL_AMBIENT_AND_DIFFUSE, mcolor);
        glMaterialfv(GL_LEFT, GL_SPECULAR, qaWhite);
        glMaterialf(GL_LEFT, GL_SHININESS, 20);

        for (int i = 0; i < num_obj; i++)
        {
                glPushMatrix();
                glColor3f(0.255, 0.255, 0.255);
                glTranslatef(random_nuvem[i][0] + 50, 1100, random_nuvem[i][2] + 100 + rZ);
                glScalef(3, 3, 3);
                glCallList(theObject + 1);
                glPopMatrix();
        }
}

void pedras()
{
        glPushMatrix();
        glColor3f(0.658824, 0.658824, 0.658824);
        glutSolidDodecahedron();
        glTranslatef(0, -20.0, 40.0 + z);
        glPopMatrix();
}

void arvores()
{
        glPushMatrix();
        glColor3f(1.0, 0.0, 0.0);
        glTranslatef(0.0, 20.0, -40.0);
        glRotatef(90, 1, 0, 0);
        glutSolidCone(3, 40, 40, 20);
        glTranslatef(0.0, -20.0, 40.0);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0.0, 0.8, 0.0);
        glTranslatef(0.0, 20.0, -40.0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(7, 10, 40, 20);
        glTranslatef(0.0, -20.0, 40.0);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0.0, 0.7, 0.0);
        glTranslatef(0.0, 13.0, -40.0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(10, 10, 40, 20);
        glTranslatef(0.0, -20.0, 40.0);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0.0, 0.6, 0.0);
        glTranslatef(0.0, 4.0, -40.0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(15, 12, 40, 20);
        glTranslatef(0.0, -20.0, 40.0);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0.0, 0.6, 0.0);
        glTranslatef(0.0, -5.0, -40.0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(20, 16, 5, 5);
        glTranslatef(0.0, -20.0, 40.0);
        glPopMatrix();
}
void desenhar_arvores(float rZ)
{
        for (int i = 0; i < num_arvores; i++)
        {
                glPushMatrix();
                glTranslatef(random_obj[i][0] + 50, 45, random_obj[i][2] + 100 + rZ);
                glScaled(4, 4, 4);
                glCallList(theObject);
                glPopMatrix();
        }
}

void desenhar_pedras(float rZ)
{
        for (int i = 0; i < num_pedras; i++)
        {
                glPushMatrix();
                glTranslatef(random_pedras[i][0] + 50, random_pedras[i][1], random_pedras[i][2] + 300 + rZ);
                glScaled(3, 3, 3);
                glCallList(theObject + 2);
                glPopMatrix();
        }
}

/* Callback para redimensionar janela */

void reshape(int width, int height)
{
        if (height == 0)
                height = 1;
        float ratio = width * 1.0 / height;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, width, height);
        gluPerspective(45.0f, ratio, 0.1f, 6000.0f);

        glMatrixMode(GL_MODELVIEW);
}

void salto()
{

        if (super_salto == 0) //Animação para cima
        {
                if (JUMP_UP == 1)
                {
                        y += 10.0;

                        if (y > JUMP_LIMITE)
                        {
                                JUMP_UP = 0;
                                JUMP_DOWN = 1;
                        }
                }
                else if (JUMP_DOWN == 1)
                {
                        y -= 10.0;
                        if (y < 75)
                        {
                                fim_salto = !fim_salto;
                                JUMP_DOWN = 0;
                        }
                }
        }
        else //Animação para baixo
        {
                if (JUMP_UP == 1)
                {
                        y += 10.0;

                        if (y > 300)
                        {
                                JUMP_UP = 0;
                                JUMP_DOWN = 1;
                        }
                }
                else if (JUMP_DOWN == 1)
                {
                        y -= 20.0;
                        if (y < 75)
                        {
                                fim_salto = !fim_salto;

                                JUMP_DOWN = 0;
                        }
                }
        }
}

/*Função responsavel pela cor do ceu e pela iluminação feita atravez dos pontos de luz (sol e lua)*/

void cor_ceu()
{
        if (dia)
        {
                glClearColor(0.0f, 0.6f, 0.8f, 0.0f); //Ceu azul
                desenhar_sol();
                if (z < ceu)
                {
                        dia = !dia; //Noite
                        lighting(); //Atualiza a fonte de luz
                        ceu += -20000;
                }
        }
        else
        {
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Ceu preto
                desenhar_lua();
                if (z < ceu)
                {
                        dia = !dia; //Dia
                        lighting(); //Atualiza a fonte de luz
                        ceu += -20000;
                }
        }
}

void writeString(char *string, int Rx, int Ry, void *font)
{
        glColor3f(255, 255, 255);
        glRasterPos2i(Rx, Ry);
        int len = strlen(string);
        for (int i = 0; i < len; i++)
        {
                glutBitmapCharacter(font, string[i]);
        }
}

/*Inicia projeção 2d no ecra*/

void Start2D()
{
        glDisable(GL_LIGHTING);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, WIDTH, 0, HEIGHT);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
}

/*Termina projeção 2d no ecra*/

void End2D()
{
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glEnable(GL_LIGHTING);
}

/*Função responsavel por imprimir todo o tipo de estatistica no ecra*/

void counter_fps()
{
        char fps_name[] = "FPS:";
        char vida_name[] = "VIDAS:";
        char distance_name[100000];
        char sfps[3];
        char svida[3];
        frame++;
        times = glutGet(GLUT_ELAPSED_TIME);
        if (times - timebase > 1000)
        {
                fps = frame * 1000.0 / (times - timebase);
                timebase = times;
                frame = 0;
        }

        float distancia = -z;
        sprintf(sfps, "%d", fps);
        sprintf(distance_name, "%0.0f", distancia);
        sprintf(svida, "%d", vida);

        Start2D();
        if (!fullscreen)
        {
                writeString(fps_name, 720, 580, GLUT_BITMAP_TIMES_ROMAN_24);
                writeString(sfps, 770, 580, GLUT_BITMAP_TIMES_ROMAN_24);
                writeString(distance_name, 720, 560, GLUT_BITMAP_TIMES_ROMAN_24);
        }
        else
        {
                writeString(fps_name, 1830, 1020, GLUT_BITMAP_TIMES_ROMAN_24);
                writeString(sfps, 1880, 1020, GLUT_BITMAP_TIMES_ROMAN_24);
                writeString(distance_name, 1830, 1000, GLUT_BITMAP_TIMES_ROMAN_24);
        }
        End2D();

        if (super_salto == 1 && counter_super_salto != 0)
        {
                char saltos[2];
                sprintf(saltos, "%d", counter_super_salto);

                Start2D();
                if (!fullscreen)
                {
                        writeString(saltos, 50, 530, GLUT_BITMAP_TIMES_ROMAN_24);
                        quadrado2d(20, 540, 40, 5, 1);
                }
                else
                {
                        writeString(saltos, 50, 970, GLUT_BITMAP_TIMES_ROMAN_24);
                        quadrado2d(20, 980, 50, 5, 1);
                }
                End2D();
        }

        if (invencivel == 1)
        {
                char timer[10];
                double reverse_cpu_time = 11.0 - tempo_invencivel;
                sprintf(timer, "%0.2f", reverse_cpu_time);

                Start2D();
                if (!fullscreen)
                {
                        quadrado2d(20, 500, 40, 6, 1);
                        writeString(timer, 50, 490, GLUT_BITMAP_TIMES_ROMAN_24);
                }
                else
                {
                        quadrado2d(20, 930, 50, 6, 1);
                        writeString(timer, 50, 920, GLUT_BITMAP_TIMES_ROMAN_24);
                }

                End2D();
        }

        int aux = 0;
        for (int j = 0; j < vida; j++)
        {
                Start2D();
                if (!fullscreen)
                {
                        quadrado2d(20 + aux, 580, 40, 4, 1);
                        aux += 40;
                }
                else
                {
                        quadrado2d(20 + aux, 1060, 50, 4, 1);
                        aux += 50;
                }

                End2D();
        }
}

void reload()
{
        /*Redesenhar pista*/
        if (redesenhar_pista == 0)
        {
                desenhar_obstaculos(0);
                desenhar_powerups(0);

                if (z < -105000)
                {
                        redesenhar_pista++;
                        random_obstaculo(-105000 * redesenhar_pista);
                        r_powerups(-105000 * redesenhar_pista);
                }
        }
        else
        {
                desenhar_obstaculos(-105000 * redesenhar_pista);
                desenhar_powerups(-105000 * redesenhar_pista);

                if (z < -105000 * (redesenhar_pista + 1))
                {
                        redesenhar_pista++;
                        random_obstaculo(-105000 * redesenhar_pista);
                        r_powerups(-105000 * redesenhar_pista);
                }
        }

        /*Desenhar o ambiente */
        if (redesenhar == 0)
        {
                desenhar_montanhas_laterais(0);
                desenhar_nuvens(0);
                desenhar_cercas(0);
                desenhar_arvores(0);
                desenhar_pedras(0);

                if (z < -8000)
                {
                        redesenhar++;
                }
        }
        else
        {
                desenhar_cercas(-8000 * redesenhar);
                desenhar_arvores(-8000 * redesenhar);
                desenhar_pedras(-8000 * redesenhar);
                desenhar_nuvens(-8000 * redesenhar);
                desenhar_montanhas_laterais(-8000 * redesenhar);

                if (z < -8000 * (redesenhar + 1))
                {
                        redesenhar++;
                }
        }
}

void desenha_menu()
{
        Start2D();
        if (!fullscreen)
        {
                retangulo2d(200, 400, 600, 450, 9); //Back to the game
                retangulo2d(200, 200, 600, 250, 7); //Quit

                if (mx >= 200 && mx <= 600 && my <= HEIGHT - 400 && my >= HEIGHT - 450)
                {
                        retangulo2d(190, 390, 610, 460, 9); //Back to the game
                        if (click_left == 1)
                        {
                                click_left == 0;
                                paused = !paused;
                        }
                }

                if (mx >= 200 && mx <= 600 && my <= HEIGHT - 200 && my >= HEIGHT - 250)
                {
                        retangulo2d(190, 190, 610, 260, 7); //Quit
                        if (click_left == 1)
                        {
                                click_left == 0;
                                exit(0);
                        }
                }
        }
        else
        {
                retangulo2d(660, 800, 1260, 850, 9); //Back to the game
                retangulo2d(660, 400, 1260, 450, 7); //Quit

                if (mx >= 660 && mx <= 1260 && my <= HEIGHT - 800 && my >= HEIGHT - 850)
                {
                        retangulo2d(650, 790, 1270, 860, 9); //Back to the game
                        if (click_left == 1)
                        {
                                click_left == 0;
                                paused = !paused;
                        }
                }

                if (mx >= 660 && mx <= 1260 && my <= HEIGHT - 400 && my >= HEIGHT - 450)
                {
                        retangulo2d(650, 390, 1270, 460, 7); //Quit
                        if (click_left == 1)
                        {
                                click_left == 0;
                                exit(0);
                        }
                }
        }

        End2D();
}

void menu_inicial()

{
        Start2D();
        if (!fullscreen)
        {
                retangulo2d(200, 400, 600, 450, 10); //Play
                retangulo2d(200, 200, 600, 250, 7);  //Quit

                if (mx >= 200 && mx <= 600 && my <= HEIGHT - 400 && my >= HEIGHT - 450)
                {
                        retangulo2d(190, 390, 610, 460, 10); //Play
                        if (click_left == 1)
                        {
                                click_left == 0;
                                angle = 0.0f;
                                menu_principal = !menu_principal;
                        }
                }

                if (mx >= 200 && mx <= 600 && my <= HEIGHT - 200 && my >= HEIGHT - 250)
                {
                        retangulo2d(190, 190, 610, 260, 7); //Quit
                        if (click_left == 1)
                        {
                                click_left == 0;
                                exit(0);
                        }
                }
        }
        else
        {
                retangulo2d(660, 700, 1260, 750, 10); //Play
                retangulo2d(660, 300, 1260, 350, 7);  //Quit

                if (mx >= 660 && mx <= 1260 && my <= HEIGHT - 700 && my >= HEIGHT - 750)
                {
                        retangulo2d(650, 690, 1270, 760, 10); //Play
                        if (click_left == 1)
                        {
                                click_left == 0;
                                angle = 0.0f;
                                menu_principal = !menu_principal;
                        }
                }

                if (mx >= 660 && mx <= 1260 && my <= HEIGHT - 300 && my >= HEIGHT - 350)
                {
                        retangulo2d(650, 290, 1270, 360, 7); //Quit
                        if (click_left == 1)
                        {
                                click_left == 0;
                                exit(0);
                        }
                }
        }
        End2D();
}

void menu_playagain()

{
        Start2D();
        if (!fullscreen)
        {
                retangulo2d(200, 400, 600, 450, 12); //Play Again
                retangulo2d(200, 200, 600, 250, 7);  //Quit

                if (mx >= 200 && mx <= 600 && my <= HEIGHT - 400 && my >= HEIGHT - 450)
                {
                        retangulo2d(190, 390, 610, 460, 12); //Play Again
                        if (click_left == 1)
                        {
                                click_left == 0;
                                angle = 0.0f;
                                //Reseta a todas as variaveis
                                dia = true;
                                ceu = -20000;
                                vida = 3;
                                redesenhar_pista = 0;
                                redesenhar = 0;
                                super_salto = 0;
                                random_obstaculo(0);
                                r_powerups(0);
                                z = 0;
                        }
                }

                if (mx >= 200 && mx <= 600 && my <= HEIGHT - 200 && my >= HEIGHT - 250)
                {
                        retangulo2d(190, 190, 610, 260, 7); //Quit
                        if (click_left == 1)
                        {
                                click_left == 0;
                                exit(0);
                        }
                }
        }
        else
        {
                retangulo2d(660, 700, 1260, 750, 12); //Play Again
                retangulo2d(660, 300, 1260, 350, 7);  //Quit

                if (mx >= 660 && mx <= 1260 && my <= HEIGHT - 700 && my >= HEIGHT - 750)
                {
                        retangulo2d(650, 690, 1270, 760, 12); //Play Again
                        if (click_left == 1)
                        {
                                click_left == 0;
                                angle = 0.0f;
                                //Reseta a todas as variaveis
                                dia = true;
                                ceu = -20000;
                                vida = 3;
                                redesenhar_pista = 0;
                                redesenhar = 0;
                                super_salto = 0;
                                random_obstaculo(0);
                                r_powerups(0);
                                z = 0;
                        }
                }

                if (mx >= 660 && mx <= 1260 && my <= HEIGHT - 300 && my >= HEIGHT - 350)
                {
                        retangulo2d(650, 290, 1270, 360, 7); //Quit
                        if (click_left == 1)
                        {
                                click_left == 0;
                                exit(0);
                        }
                }
        }
        End2D();
}

void move_animation()
{
        if (angle_right) //Animação para a direita
        {
                angle -= 0.005f;
                lx = sin(angle);
                lz = -cos(angle);
                if (angle < -0.50f)
                {
                        angle_right = !angle_right;
                        angle_left = !angle_left;
                }
        }
        else //Animacao para a esquerda
        {
                angle += 0.005f;
                lx = sin(angle);
                lz = -cos(angle);
                if (angle > 0.50f)
                {
                        angle_right = !angle_right;
                        angle_left = !angle_left;
                }
        }
}

void display(void)
{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        if (menu_principal) //Se tivermos no menu inicial
        {
                menu_inicial();
                pos_camera();
                fog();
                desenhar_solo();
                cor_ceu();
                reload();
                move_animation();
                z = z;
        }
        else //Se não tivermos no menu inicial
        {
                if (!paused && vida > 0) //Quando jogo não está pausado (movimento da cara e salto abilitados)
                {
                        move_camera();
                        salto();
                }
                else if (vida > 0) //Quando o jogo está pausado mas o player ainda tem vidas (apenas desenha um menu)
                {
                        z = z;
                        desenha_menu();
                }
                if (vida < 1) //Quando as vidas acabam(apenas desenha um menu)
                {
                        menu_playagain();
                        z = z;
                }

                pos_camera();
                cor_ceu();
                fog();
                reload();
                desenhar_solo();
                counter_fps();
        }
        glutSwapBuffers();
}

/* Callback Idle */
void idle(void)
{
        /* Ações a tomar quando o GLUT está idle */
        /* Redesenhar o ecrã */
        // glutPostRedisplay();
}

/* Callback de temporizador */

void timer(int value)
{
        glutTimerFunc(10, timer, 0);
        if (teclas.a)
        {
                if (!paused)
                {
                        x -= 10.0;
                        if (x < -260)
                        {
                                x = -260;
                        }
                }
        }
        if (teclas.d)
        {
                if (!paused)
                {
                        x += 10.0;
                        if (x > 260)
                        {
                                x = 260;
                        }
                }
        }

        /* Redesenhar o ecrã (invoca o callback de desenho) */
        glutPostRedisplay();
}

/* Callback para interação via teclado (carregar na tecla) */

void processNormalKeys(unsigned char key, int xx, int yy)
{
        switch (key)
        {
        case 13: // Tecla Enter
                if (!fullscreen)
                {
                        glutFullScreen();
                        HEIGHT = 1080;
                        WIDTH = 1920;
                        fullscreen = !fullscreen;
                }
                else
                {
                        WIDTH = 800;
                        HEIGHT = 600;
                        glutPositionWindow(0, 0);
                        glutReshapeWindow(800, 600);
                        fullscreen = !fullscreen;
                }
                break;
        case 27: // Tecla Esc
                paused = !paused;
                break;
        case 'a':
        case 'A':
                teclas.a = GL_TRUE;

                break;
        case 'D':
        case 'd':
                teclas.d = GL_TRUE;

                break;
        case ' ':

                if (!paused)
                {
                        if (fim_salto)
                        {
                                fim_salto = !fim_salto;                                                 //Correção do bug do salto
                                JUMP_UP = 1;
                                JUMP_DOWN = 0;
                                if (super_salto == 1 && counter_super_salto != 0)
                                {
                                        PlaySound("sounds/superjump.wav", NULL, SND_FILENAME | SND_ASYNC);
                                        counter_super_salto--;
                                }
                                else
                                {
                                        super_salto = 0;
                                        counter_super_salto = 5;
                                }
                        }
                }
                break;
        }
}

void processNormalKeysUp(unsigned char key, int xx, int yy)
{
        switch (key)
        {
        case 'a':
        case 'A':
                teclas.a = GL_FALSE;
                break;
        case 'D':
        case 'd':
                teclas.d = GL_FALSE;
                break;
        }
}

void moveMouse(int Mx, int My)

{
        mx = Mx;
        my = My;
        click_left = 0;
}

void clickMouse(int button, int x, int y, int yy)
{
        switch (button)
        {
        case GLUT_LEFT_BUTTON:
                click_left = 1;
                break;
        case GLUT_MIDDLE_BUTTON:

                break;
        case GLUT_RIGHT_BUTTON:

                break;
        }
}

int main(int argc, char **argv)
{

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

        glutGameModeString("1920x1080:60@144"); // 1º teste
        if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
        {
                glutEnterGameMode();
        }
        else
        {
                glutGameModeString("1920x1080:60@144"); // 2º teste
                if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
                {
                        glutEnterGameMode();
                }
                else
                {
                        glutInitWindowPosition(0, 0);
                        glutInitWindowSize(800, 600);
                        if ((glutCreateWindow("Corrida do Estevao")) == GL_FALSE)
                                exit(1);
                }
        }

        /* Inicialização */
        init();
        random_arvores();
        sol_lua();
        random_nuvens();
        laterais();
        r_pedras();
        random_obstaculo(0);
        r_powerups(0);

        /*Mouse Movement */
        glutPassiveMotionFunc(moveMouse);
        glutMouseFunc(clickMouse);

        /* callbacks de janelas/desenho */
        glutReshapeFunc(reshape);
        glutDisplayFunc(display);

        /* Callbacks de teclado */
        glutKeyboardUpFunc(processNormalKeysUp);
        glutKeyboardFunc(processNormalKeys);

        /* Callbacks timer/idle */
        glutTimerFunc(10, timer, 0);

        /* Ligar luzes */
        lighting();

        /* Começar loop */
        glutMainLoop();

        return 0;
}
