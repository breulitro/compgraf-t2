#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include "t2.h"

#define SENS_ROT	10.0
#define SENS_OBS	1.0
#define SENS_TRANSL	1.0

#define TAM 1000
#define D 100

int x_ini,y_ini,bot;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX, obsY=200, obsZ=400, obsX_ini, obsY_ini, obsZ_ini;
GLfloat fAspect = 1, angle = 45;

GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0};
GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};		 // "cor"
GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};// "brilho"
GLfloat posicaoLuz[4]={0.0, 30.0, 120.0, 1.0};

int playing = 1;
int playloop = 1;
int velocidade = 30;
int frame_atual = 0;

void PosicionaObservador(void)
{
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	//glTranslatef(-obsX,-obsY,-obsZ); //Outra opcao de camera
	glRotatef(rotX,1,0,0);
	glRotatef(rotY,0,1,0);

	gluLookAt(obsX,obsY,obsZ, 0.0,0.0,0.0, 0.0,1.0,0.0);
}

// Função usada para especificar o volume de visualização
void EspecificaParametrosVisualizacao(void)
{
	// Especifica sistema de coordenadas de projeção
	glMatrixMode(GL_PROJECTION);
	// Inicializa sistema de coordenadas de projeção
	glLoadIdentity();

	// Especifica a projeção perspectiva
	gluPerspective(angle,fAspect,0.4,5000);

	// Especifica posição do observador e do alvo
	PosicionaObservador();
}
#if 0
void DesenhaChao()
{
	//Flags para determinar a cord de cada quadrado
	int flagx, flagz;
	//Define a normal apontando para cima
	glNormal3f(0,1,0);

	glBegin(GL_QUADS);
	flagx = 0;
	//X varia de -TAM a TAM, de D em D
	for(float x=-TAM; x<TAM; x+=D)
	{
		//Flagx determina a cor inicial
		if(flagx) flagz = 0;
		else flagz = 1;
		//Z varia de -TAM a TAM, de D em D
		for (float z=-TAM;z<TAM;z+=D)
		{
			//Escolhe cor
			if(flagz)
				glColor3f(0.4,0.4,0.4);
			else
				glColor3f(1,1,1);
			//E desenha o quadrado
			glVertex3f(x,-60,z);
			glVertex3f(x+D,-60,z);
			glVertex3f(x+D,-60,z+D);
			glVertex3f(x,-60,z+D);
			//Alterna cor
			flagz = !flagz;
		}
		//A cada coluna, alterna cor inicial
		flagx = !flagx;
	}
	glEnd();
}
#else
void DesenhaChao() {
	float z, x;

	glColor3f(0, 0, 1);
	glLineWidth(1);
	glBegin(GL_LINES);

	for(z = 1000; z <= 1000; z += 10) {
		glVertex3f(1000, 0.1f, z);
		glVertex3f(1000, 0.1f, z);
	}
	for(x = 1000; x <= 1000; x += 10) {
		glVertex3f(x, 0.1f, 1000);
		glVertex3f(x, 0.1f, 1000);
	}
	glEnd();
	glLineWidth(1);
}
#endif
void DefineIluminacao()
{
	// Capacidade de brilho do material
	GLfloat especularidade[4]={1.0,1.0,1.0,1.0};
	GLint especMaterial = 60;

	// Habilita o modelo de colorização de Gouraud
	glShadeModel(GL_SMOOTH);
	//glShadeModel(GL_FLAT);

	// Define a refletância do material
	glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
	// Define a concentração do brilho
	glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

	// Ativa o uso da luz ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os parâmetros da luz de número 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );

#if 0
	//Desabilita iluminacao para desenhar a esfera
	glDisable(GL_LIGHTING);
	//Desenha esfera na posição da fonte de luz
	glPushMatrix();
	glTranslatef(posicaoLuz[0]+60, posicaoLuz[1], posicaoLuz[2]);
	glColor3f(1.0f, 0, 0);
	glutSolidSphere(5, 50, 50);
	glTranslatef(posicaoLuz[0], posicaoLuz[1], posicaoLuz[2]);
	glColor3f(1.0f, 1.0f, 0.0f);
	glutSolidSphere(5, 50, 50);
	glPopMatrix();
	glEnable(GL_LIGHTING);
#endif
}

int maxFrame = 0;

void getLatestFrameIndex(actor_t *a) {
	animation_t *anim;

	anim = g_slist_nth_data(a->animations, g_slist_length(a->animations) - 1);
	if (anim && anim->frame > maxFrame)
		maxFrame = anim->frame;
}

// Função callback chamada para fazer o desenho
void Desenha(void)
{
	static int framerate;

	//FIXME: Ultimo frame nao esta sendo exibido
	//  0 Stopped
	//  1 Playing
	if (playing)
		frame_atual += !(++framerate % velocidade);
// -1 Playing backward try
//		frame_atual += playing * !(++framerate % velocidade);
	if (frame_atual > maxFrame + 1) {
		if (playloop)
			frame_atual = 0;
		else
			playing = 0;
	}
	if (frame_atual < 0)
		frame_atual = maxFrame + 1;
	// Limpa a janela e o depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	EspecificaParametrosVisualizacao();
	glColor3f(0.0f, 0.0f, 1.0f);
	DesenhaChao();

	DefineIluminacao();

	g_slist_foreach(actors_list, (GFunc)plot_actor, &frame_atual);

	glutSwapBuffers();
}

// Inicializa parâmetros de rendering
void Inicializa (void)
{
	// Especifica que a cor de fundo da janela será preta
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	DefineIluminacao();

	// Habilita a definição da cor do material a partir da cor corrente
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de iluminação
	glEnable(GL_LIGHTING);
	// Habilita a luz de número 0
	glEnable(GL_LIGHT0);
	// Habilita o depth-buffering
	glEnable(GL_DEPTH_TEST);
}

// Função callback chamada quando o tamanho da janela é alterado
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
	// Para previnir uma divisão por zero
	if ( h == 0 ) h = 1;

	// Especifica o tamanho da viewport
	glViewport(0, 0, w, h);

	// Calcula a correção de aspecto
	fAspect = (GLfloat)w/(GLfloat)h;

	EspecificaParametrosVisualizacao();
}

// Função callback chamada para gerenciar eventos do mouse
void GerenciaMouse(int button, int state, int x, int y)
{
	if(state==GLUT_DOWN)
	{
		x_ini = x;
		y_ini = y;
		obsX_ini = obsX;
		obsY_ini = obsY;
		obsZ_ini = obsZ;
		rotX_ini = rotX;
		rotY_ini = rotY;
		bot = button;
	}
	else bot = -1;
}

void GerenciaMovim(int x, int y)
{
	if(bot==GLUT_LEFT_BUTTON)
	{
		int deltax = x_ini - x;
		int deltay = y_ini - y;

		rotY = rotY_ini - deltax/SENS_ROT;
		rotX = rotX_ini - deltay/SENS_ROT;
	}

	else if(bot==GLUT_RIGHT_BUTTON)
	{
		int deltaz = y_ini - y;

		obsZ = obsZ_ini + deltaz/SENS_OBS;
	}

	else if(bot==GLUT_MIDDLE_BUTTON)
	{
		int deltax = x_ini - x;
		int deltay = y_ini - y;

		obsX = obsX_ini + deltax/SENS_TRANSL;
		obsY = obsY_ini - deltay/SENS_TRANSL;
	}
	PosicionaObservador();
	glutPostRedisplay();
}

void GerenciaTeclado(unsigned char key,int a,int b)
{
	// play (tocar até o final)
	// pause (pausar a animação)
	if (key == 13) playing = !playing;
	if (key == 27) exit(0);
	switch (key)
	{
		case 'l':
			playloop = !playloop;
			if (frame_atual > maxFrame)
				frame_atual = 0;
			if (!playing)
				playing = 1;
			break;
		case 'q':
			luzAmbiente[0] -= .1;
			luzAmbiente[1] -= .1;
			luzAmbiente[2] -= .1;
			break;
		case 'w':
			luzAmbiente[0] += .1;
			luzAmbiente[1] += .1;
			luzAmbiente[2] += .1;
			break;
		case 'a':
			luzDifusa[0] -= .1;
			luzDifusa[1] -= .1;
			luzDifusa[2] -= .1;
			break;
		case 's':
			luzDifusa[0] += .1;
			luzDifusa[1] += .1;
			luzDifusa[2] += .1;
			break;
		case 'z':
			luzEspecular[0] -= .1;
			luzEspecular[1] -= .1;
			luzEspecular[2] -= .1;
			break;
		case 'x':
			luzEspecular[0] += .1;
			luzEspecular[1] += .1;
			luzEspecular[2] += .1;
			break;

	}
	//glutPostRedisplay(); idle() does that
}

void GerenciaTecladoEspecial(int key, int x,int y)
{

	switch (key)
	{
		case GLUT_KEY_LEFT:
			posicaoLuz[0] -= 2;
			frame_atual--;		// rewind (voltar um frame)
			break;
		case GLUT_KEY_RIGHT:
			posicaoLuz[0] += 2;
			frame_atual++;		// forward (avançar um frame)
			break;
		case GLUT_KEY_UP:
			posicaoLuz[1] += 2;
			velocidade -= 10;	// aumenta velocidade
			if (velocidade <= 0)
				velocidade = 1;
			break;
		case GLUT_KEY_DOWN:
			posicaoLuz[1] -= 2;
			velocidade += 10;	// diminui velocidade
			if (velocidade >= 60);
				velocidade = 59;
			break;
		case GLUT_KEY_PAGE_UP:
			posicaoLuz[2] -= 2;
			break;
		case GLUT_KEY_PAGE_DOWN:
			posicaoLuz[2] += 2;
			break;
	}

	//glutPostRedisplay();
}

void idle() {
	static char flag = 0;

	if (flag)
		return;

	flag++;
	Desenha();
	flag--;
}

// Programa Principal
int main(int argc, char **argv) {
	if (argc < 2) {
		printf("usage: %s <script file> [glut params]\n", argv[0]);
		return 1;
	}

	printf("DEBUG: %s\n", argv[0]);
	printf("DEBUG: %s\n", argv[1]);

	actors_list = read_script(argv[1]);

	// XXX: review
	argv[1] = argv[0];
	argv++;
	argc--;

	printf("script loaded\n");

	g_slist_foreach(actors_list, (GFunc)getLatestFrameIndex, NULL);
	g_slist_foreach(actors_list, (GFunc)load_obj, NULL);
	glutInit(&argc,argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800,600);
	glutCreateWindow("Exercicio de Iluminacao");
	glutDisplayFunc(Desenha);
	glutReshapeFunc(AlteraTamanhoJanela);
	glutMotionFunc(GerenciaMovim);
	glutMouseFunc(GerenciaMouse);
	glutKeyboardFunc(GerenciaTeclado);
	glutSpecialFunc(GerenciaTecladoEspecial);

	glutIdleFunc(idle);

	Inicializa();
	glutMainLoop();


	return 0;
}
