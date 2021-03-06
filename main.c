#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include <glib.h>
#include "structs.h"
#include "read_script.h"
#include "load_obj.h"
#include "help.h"

#define SENS_ROT	10.0
#define SENS_OBS	1.0
#define SENS_TRANSL	1.0

#define TAM 1000
#define D 100

int x_ini,y_ini,bot;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX, obsY = 200, obsZ = 400, obsX_ini, obsY_ini, obsZ_ini;
GLfloat fAspect = 1, angle = 45;

GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0};
GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};		 // "cor"
GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};// "brilho"
GLfloat posicaoLuz[4]={0.0, 30.0, 120.0, 1.0};

int toggle_help = 0;
GLfloat red = 0, green = 0, blue = 0;

GSList *actors_list = NULL;
GSList *animation_list_linear = NULL;


void plot_obj(model_t *obj, animation_t *anim) {
	face_t *face;
	val_t *v;
	GSList *aux;
	int i;

	if (anim == NULL || obj == NULL)
		return;
	//dump_animation(anim);

	glPushMatrix();

	v = anim->trans;
	if (v != NULL) {
		glTranslatef(v->x, v->y, v->z);
	}

	v = anim->scale;
	if (v != NULL) {
		glScalef(v->x, v->y, v->z);
	}

	v = anim->rot;
	if (v != NULL) {
		//glRotatef(v->y, 0, 1, 0);
		glRotatef(v->x, 1, 0, 0);
		glRotatef(v->y, 0, 1, 0);
		glRotatef(v->z, 0, 0, 1);
	}

	aux = obj->face_list;
	glBegin(GL_TRIANGLES);
	while ((aux = g_slist_next(aux)) != NULL) {

		face = (face_t *)aux->data;
		for (i = 0; i < face->fvertex_size; i++) {
			v = get_vertex(face->fvertex[i], obj);
			glVertex3f(v->x, v->y, v->z);
		}
		for (i = 0; i < face->ftexture_size; i++) {
			v = get_texture(face->ftexture[i], obj);
			glTexCoord3f(v->x, v->y, v->z);
		}
		for (i = 0; i < face->fnormal_size; i++) {
			v = get_normal(face->fnormal[i], obj);
			glNormal3f(v->x, v->y, v->z);
		}
	}
	glEnd();
	glPopMatrix();
}

void plot_actor(actor_t *a, int *frame_atual) {
	guint len;
	animation_t *anim;

	len = g_slist_length(a->animations);
	if (len > *frame_atual)
		anim = (animation_t *)g_slist_nth_data(a->animations, *frame_atual - 1);
	else
		anim = (animation_t *)g_slist_nth_data(a->animations, len - 1);

	plot_obj(a->obj, anim);
}

/**
 * trans: val_t a ser dividido
 * dividor: por quanto vai ser dividido
 * n: qual momento tu quer a transformacao
 */
val_t *divide_val(val_t *val, int divisor, int n) {
	val_t *v;

	if (val == NULL)
		return NULL;

	v = g_new0(val_t, 1);
	//uma hora n == divisor
	if (n == 0) {
		v->x = val->x;
		v->y = val->y;
		v->z = val->z;
	} else {
		v->x = val->x / ((float)n / divisor);
		v->y = val->y / ((float)n / divisor);
		v->z = val->z / ((float)n / divisor);
	}

	return v;
}

// Esta fun��o resolve o problema de um val_t estar nulo no script replicando o
// valor do val_t anterior.
void fix_empty_vals(val_t **prev, val_t **next) {
	// se n�o h� valor em prev, presume-se 0,0,0
	if (*prev == NULL) {
		*prev = g_new0(val_t, 1);
		return;
	}

	if (*next == NULL) {
		*next = g_new0(val_t, 1);
		memcpy(*next, *prev, sizeof(val_t));
	}
}

val_t *calcula_passo(val_t *prev_val, val_t *next_val, int delta) {
	val_t *passo_val;

	if (prev_val == NULL || next_val == NULL || delta < 1)
		return NULL;

	passo_val = g_new0(val_t, 1);

	// Se n�o tivermos um pr�ximo valor, o objeto deve permanecer intacto

	passo_val->x = (float)((next_val->x - prev_val->x) / (float)delta);
	passo_val->y = (float)((next_val->y - prev_val->y) / (float)delta);
	passo_val->z = (float)((next_val->z - prev_val->z) / (float)delta);

	return passo_val;
}

/* Criando os frames intermediarios para interpolar */
void delta_func(animation_t *next, animation_t **prev) {
	int c, delta;
	animation_t *anim, *aux, *prev_aux;
	val_t *passo_trans, *passo_rot, *passo_scale;

	prev_aux = *prev;

	if (prev_aux != NULL) {
		printf("INICIAL:");
		dump_animation(prev_aux);
		printf("FINAL:");
		dump_animation(next);
		printf("=========\n");

		printf("Prev: %d\n", prev_aux->frame);
		printf("Next: %d\n", next->frame);

		// Arrumando val_t nulo caso haja
		fix_empty_vals(&prev_aux->trans, &next->trans);
		fix_empty_vals(&prev_aux->rot, &next->rot);
		fix_empty_vals(&prev_aux->scale, &next->scale);

		// N�mero de frames intermediarios que devem ser criados
		delta = next->frame - prev_aux->frame;

		passo_trans = calcula_passo(prev_aux->trans, next->trans, delta);
		passo_rot = calcula_passo(prev_aux->rot, next->rot, delta);
		passo_scale = calcula_passo(prev_aux->scale, next->scale, delta);

		animation_list_linear = g_slist_append(animation_list_linear, prev_aux);
		// Come�amos o offset somando do primeiro frame
		aux = prev_aux;
		// 'c' nunca vai come�ar em 1
		for (c = prev_aux->frame + 1; c < next->frame; c++) {
			anim = g_new0(animation_t, 1);
			anim->frame = c;
			anim->trans = add_val_t(aux->trans, passo_trans);
			anim->scale = add_val_t(aux->scale, passo_scale);
			anim->rot = add_val_t(aux->rot, passo_rot);
			//dump_animation(anim);
			// Agora nosso pr�ximo offset ser� baseado no �ltimo frame criado
			aux = anim;
			animation_list_linear = g_slist_append(animation_list_linear, anim);
		}
		animation_list_linear = g_slist_append(animation_list_linear, next);

		printf("INICIAL1:");
		dump_animation(prev_aux);
		printf("FINAL1:");
		dump_animation(next);

		if (passo_trans != NULL)
			g_free(passo_trans);
		if (passo_scale != NULL)
			g_free(passo_scale);
		if (passo_rot != NULL)
			g_free(passo_rot);
	} else {
		//Essa anima��o come�a s� no frame a->frame, ent�o bota nulo nos frames
		//iniciais
		for (c = 1; c < next->frame; c++)
			animation_list_linear = g_slist_append(animation_list_linear, NULL);
	}

	*prev = next;
}

void load_obj(actor_t *a) {
	animation_t *inicial = NULL;

	printf("Loading %s\n", a->file);
	a->obj = load_new_obj(a->file);

	animation_list_linear = NULL;
	g_slist_foreach(a->animations, (GFunc)delta_func, &inicial);

	g_slist_foreach(animation_list_linear, (GFunc)dump_animation, NULL);
	a->animations = animation_list_linear;
}

void PosicionaObservador(void)
{
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	//glTranslatef(-obsX,-obsY,-obsZ); //Outra opcao de camera
	glRotatef(rotX,1,0,0);
	glRotatef(rotY,0,1,0);

	gluLookAt(obsX,obsY,obsZ, 0.0,0.0,0.0, 0.0,1.0,0.0);
}

// Fun��o usada para especificar o volume de visualiza��o
void EspecificaParametrosVisualizacao(void)
{
	// Especifica sistema de coordenadas de proje��o
	glMatrixMode(GL_PROJECTION);
	// Inicializa sistema de coordenadas de proje��o
	glLoadIdentity();

	// Especifica a proje��o perspectiva
	gluPerspective(angle,fAspect,0.4,5000);

	// Especifica posi��o do observador e do alvo
	PosicionaObservador();
}

void DesenhaChao() {
	float z, x;

	glColor3f(0, 1, 0);
	glLineWidth(1);
	glBegin(GL_LINES);

	for (z = -1000; z <= 1000; z += 10) {
		glVertex3f(-1000, -0.1f, z);
		glVertex3f(1000, -0.1f, z);
	}
	for (x = -1000; x <= 1000; x += 10) {
		glVertex3f(x, -0.1f, -1000);
		glVertex3f(x, -0.1f, 1000);
	}
	glEnd();
	glLineWidth(1);
}

void DefineIluminacao()
{
	// Capacidade de brilho do material
	GLfloat especularidade[4]={1.0,1.0,1.0,1.0};
	GLint especMaterial = 60;

	// Habilita o modelo de coloriza��o de Gouraud
	glShadeModel(GL_SMOOTH);
	//glShadeModel(GL_FLAT);

	// Define a reflet�ncia do material
	glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
	// Define a concentra��o do brilho
	glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

	// Ativa o uso da luz ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os par�metros da luz de n�mero 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );
}

void getLatestFrameIndex(actor_t *a) {
	animation_t *anim;

	anim = g_slist_nth_data(a->animations, g_slist_length(a->animations) - 1);
	if (anim && anim->frame > maxFrame)
		maxFrame = anim->frame;
}

// Fun��o callback chamada para fazer o desenho
void Desenha(void)
{
	static int framerate;

	//  0 Stopped
	//  1 Playing
	if (playing)
		frame_atual += !(++framerate % velocidade);

// -1 Playing backward try
//		frame_atual += playing * !(++framerate % velocidade);

	if (frame_atual > maxFrame) {
		if (playloop) {
			frame_atual = 1;
		} else {
			frame_atual = maxFrame;
			playing = 0;
    }
	}

	if (frame_atual < 0)
		frame_atual = maxFrame;

	// Limpa a janela e o depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DesenhaChao();

  if (toggle_help)
    HelpDisplay(800, 600);

	EspecificaParametrosVisualizacao();
	glColor3f(red, green, blue);

	DefineIluminacao();

	g_slist_foreach(actors_list, (GFunc)plot_actor, &frame_atual);

	glutSwapBuffers();
}

// Inicializa par�metros de rendering
void Inicializa (void)
{
	// Especifica que a cor de fundo da janela ser� preta
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	DefineIluminacao();

	// Habilita a defini��o da cor do material a partir da cor corrente
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de ilumina��o
	glEnable(GL_LIGHTING);
	// Habilita a luz de n�mero 0
	glEnable(GL_LIGHT0);
	// Habilita o depth-buffering
	glEnable(GL_DEPTH_TEST);
}

// Fun��o callback chamada quando o tamanho da janela � alterado
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
	// Para previnir uma divis�o por zero
	if ( h == 0 ) h = 1;

	// Especifica o tamanho da viewport
	glViewport(0, 0, w, h);

	// Calcula a corre��o de aspecto
	fAspect = (GLfloat)w/(GLfloat)h;

	EspecificaParametrosVisualizacao();
}

// Fun��o callback chamada para gerenciar eventos do mouse
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
	// play (tocar at� o final)
	// pause (pausar a anima��o)
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
		case 'r':
			red += red < 1 ? 0.1 : 0;
			break;
		case 'g':
			green += green < 1 ? 0.1 : 0;
			break;
		case 'b':
			blue += blue < 1 ? 0.1 : 0;
			break;
		case 'R':
			red -= red > 0 ? 0.1 : 0;
			break;
		case 'G':
			green -= green > 0 ? 0.1 : 0;
			break;
		case 'B':
			blue -= blue > 0 ? 0.1 : 0;
			break;
    case 'H':
    case 'h':
      toggle_help = !toggle_help;
      break;
    case 'D':
    case 'd':
      visual_debug = !visual_debug;
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
			frame_atual++;		// forward (avan�ar um frame)
			break;
		case GLUT_KEY_UP:
			posicaoLuz[1] += 2;
			velocidade -= velocidade > 5 ? 5 : 0;	// aumenta velocidade
			break;
		case GLUT_KEY_DOWN:
			posicaoLuz[1] -= 2;
			velocidade += velocidade < 75 ? 5 : 0;	// diminui velocidade
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

void dump_infos(int i) {
	printf("Playing = %s\t"
		"Looping = %s\t"
		"MaxFrame = %d\n"
		"Velocidade = %d\n"
		"Frame Atual = %d\n",
		playing ? "true" : "false",
		playloop ? "true" : "false",
		maxFrame, velocidade, frame_atual);
}

// Programa Principal
int main(int argc, char **argv) {
	if (argc < 2) {
		printf("usage: %s <script file> [glut params]\n", argv[0]);
		return 1;
	}

  // Valores default
  toggle_help = 0;
  playing = 1;
  playloop = 1;
  velocidade = 30;
  frame_atual = 0;
  visual_debug = 0;
  maxFrame = 0;

	signal(SIGUSR1, dump_infos);

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
	glutCreateWindow("Trabalho2 - Computacao Grafica");
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
