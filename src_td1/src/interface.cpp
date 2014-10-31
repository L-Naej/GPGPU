/***************************************************************************
 *   Copyright (C) 2010 by Venceslas BIRI                                  *
 *   biri@univ-mlv.fr                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "display.hpp"
#include "interface.hpp"

/// ///////////////////////////////////////////////////////////////////////////
/// variables globales
/// ///////////////////////////////////////////////////////////////////////////

/// L'ecran
unsigned int width_ecran = 800;
unsigned int height_ecran = 600;

/// Position de la camera ! VARIATION DE -100 a 100
unsigned int mytime;
unsigned int cptidletime;

/// Flag pour le dessin
uint flag_dessin;
bool flag_svg_fic;
bool flag_test;
bool flag_shaders;
unsigned char *image_save;
bool modified_tampon = true;
unsigned int which_test;
TimerManager* timeManager;
float mouse[2];

/// La scene
char nomFIC[60] = {'\0'};
Texture2D* image_base = NULL;
Texture2D* image_gpu = NULL;
Texture2D* image_cpu = NULL;
unsigned int fbo_handler;
GLuint visuShader;
GLuint treatmentShader;
StandardMesh* screen_quad;
float sepiaColor[3];

void init()
{
	cptidletime = 0;

	image_save=NULL;
	flag_svg_fic = false;
	flag_test = true;
	flag_shaders = false;
	which_test = 0;
	modified_tampon = false;
	timeManager = new TimerManager();
	timeManager->addOneTimer(1);

	image_base = new Texture2D(nomFIC);
	image_gpu = new Texture2D(image_base,true);
	image_cpu = new Texture2D(image_base,true);
}

void initGL()
{
	initDisplay();
}



/// ///////////////////////////////////////////////////////////////////////////
/// Corps des fonctions IHM
/// ///////////////////////////////////////////////////////////////////////////

/// ///////////////////////////////////////////////////////////////////////////
/// fonction associée aux interruptions clavier
/// - c : caractère saisi
/// - x,y : coordonnée du curseur dans la fenêtre
void kbdFunc(unsigned char c, int , int )
{
 switch(c) {
		case 27: // quit
			exit(0);
			break;
		case '1': // print
		case '2': // print
		case '3': // print
		case '4': // print
		case '5': // print
      which_test = (int)(c-'0');
			std::cerr<<"Fixe test sur "<<which_test<<std::endl;
			break;
		case 'i': // print
			print();
			break;
		case '<': // aide
			help((char*)"./aSimpleGLSLProg");
			break;
		case 'o': //
			sepiaColor[0] -= 0.05;
			std::cerr<<"Sepia red : "<<sepiaColor[0]<<std::endl;
			break;
		case 'p': //
			sepiaColor[0] += 0.05;
			std::cerr<<"Sepia red : "<<sepiaColor[0]<<std::endl;
			break;
		case 'l': //
			sepiaColor[1] -= 0.05;
			std::cerr<<"Sepia green : "<<sepiaColor[1]<<std::endl;
			break;
		case 'm': //
			sepiaColor[1] += 0.05;
			std::cerr<<"Sepia green : "<<sepiaColor[1]<<std::endl;
			break;
		case 'x': // GPU On / Off
			cerr<<"Switch GPU ";
			flag_shaders = !flag_shaders;
			if (flag_shaders) cout<<" ON"<<endl;
			else  cout<<" OFF"<<endl;
			break;
		case 't': // FPS computation On / Off
			cerr<<"Switch idle compute "<<!flag_test<<endl;
			flag_test = !flag_test;
			break;
		case 's': // Print image
			saveFromGL(width_ecran,height_ecran,"./essai.ppm");
			break;
		case ' ': // DO NOTHING...
			break;
		default :
			cerr<<"Touche non fonctionnelle"<<endl;
	}
	glutPostRedisplay();
}

void affiche_flag_dessin() {
	cout<<"FLAG AFFICHAGE : "<<flag_dessin<<std::endl;
}

/// ///////////////////////////////////////////////////////////////////////////
/// fonction associée aux interruptions clavier speciales
/// - c : caractère saisi
/// - x,y : coordonnée du curseur dans la fenêtre
void kbdSpFunc(int c, int , int )
{
 switch(c)
    {
    case GLUT_KEY_F1 :
    case GLUT_KEY_F2 :
    case GLUT_KEY_F3 :
    case GLUT_KEY_F4 :
    case GLUT_KEY_F5 :
    case GLUT_KEY_F6 :
    case GLUT_KEY_F7 :
    case GLUT_KEY_F8 :
    case GLUT_KEY_F9 :
    case GLUT_KEY_F10 :
    case GLUT_KEY_F11 :
    case GLUT_KEY_F12 :
			flag_dessin = c - GLUT_KEY_F1;
			affiche_flag_dessin();
			break;
	case GLUT_KEY_UP :
			break;
    case GLUT_KEY_DOWN :
			break;
    case GLUT_KEY_LEFT :
			break;
    case GLUT_KEY_RIGHT :
			break;
    case GLUT_KEY_PAGE_UP :
			break;
    case GLUT_KEY_PAGE_DOWN :
			break;
		default :
			cerr<<"Touche speciale non fonctionnelle"<<endl;
	}
	glutPostRedisplay();
}

/// ///////////////////////////////////////////////////////////////////////////
/// fonction associée aux evenements souris
/// - button,state : état du bouton cliqué
/// - x,y : coordonnée de la souris dans la fenêtre
void clickMouse(int button,int state,int /*x*/,int /*y*/)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		//int h = glutGet(GLUT_WINDOW_HEIGHT);
		//int w = glutGet(GLUT_WINDOW_WIDTH);

		glutPostRedisplay();
	}
}

/// ///////////////////////////////////////////////////////////////////////////
/// fonction associée aux evenements souris
/// - x,y : coordonnée de la souris dans la fenêtre
void motionFunc(int x,int y)
{
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	int w = glutGet(GLUT_WINDOW_WIDTH);

	// Fit to space [-1,1] x [-1,1]
	mouse[0] = x/(float)w;
	mouse[1] = y/(float)h;

	//std::cerr<<"Mouse "<<mouse[0]<<" / "<<mouse[1]<<std::endl;

	glutPostRedisplay();
}

/// ///////////////////////////////////////////////////////////////////////////
/// Corps des fonctions de rendu
/// ///////////////////////////////////////////////////////////////////////////

/// ///////////////////////////////////////////////////////////////////////////
/// fonction de changement de dimension de la fenetre paramêtres :
/// - width  : largeur (x) de la zone de visualisation
/// - height : hauteur (y) de la zone de visualisation
void reshapeFunc(int width,int height)
{
	width_ecran = width;
	height_ecran = height;

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D(0,width,0,height);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glViewport( 0, 0, (GLint)width, (GLint)height );
}

/// ///////////////////////////////////////////////////////////////////////////
/// fonction d'exectution de process en attendant un evenement
void idleFunc(void)
{
	if (flag_test) {
		glutPostRedisplay();
	}
}

void help(char *cmd)
{
	cout <<endl;
	cout <<"**************************************"<<endl;
	cout <<"Usage: "<<cmd<<" [-h] -f <nom_fic> &"<<endl;
	cout <<"  -f <nom_fic> image à traiter"<<endl;
	cout <<"  -h   ce message "<<endl;
	cout <<" "<<endl;
	cout <<" "<<endl;
	cout <<"**************************************"<<endl;
}

void print()
{
	cout <<"\n****************************************"<<endl;
	cout <<"                INFORMATIONS            "<<endl;
	affiche_flag_dessin();
	cout <<"**************************************"<<endl;
}

/// ///////////////////////////////////////////////////////////////////////////
/// Fonctions utilitaires
/// ///////////////////////////////////////////////////////////////////////////
void saveFromGL(int w,int h,const char *nom_dest) {
	fstream fout;

	// Calculer deja la taille de l'image.
	fout.open(nom_dest,ios::out);
	if(!fout.is_open()) {
		cout << "Erreur lors de l'ouverture du fichier " << nom_dest << "\n";
	}
	else {
		//cout << "Sauver fichier : "<<nom_dest;
		//cout << "Width : "<<w<<" height : "<<h<<endl;
		fout<<"P6"<<endl;
		fout<<"# CREATOR GLSL simple prog ";
		fout<<"(biri@univ-mlv.fr)"<<endl;
		fout<<w<<" "<<h<<endl;
		fout<<"255"<<endl;

		unsigned char *image = new unsigned char[h*w*3];
		if (image==NULL) exit(5);
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glReadPixels(0,0,(GLint)w,(GLint)h,GL_RGB,GL_UNSIGNED_BYTE,image);
		for(int i=h-1;i>=0;i--) {
			fout.write((char*)(image+i*w*3),w*3);
		}
		delete(image);
	}
	fout.close();
}

