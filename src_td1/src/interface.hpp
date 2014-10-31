/***************************************************************************
                          interface.hpp  -  description
                             -------------------
    begin                : Tue 28 Jul 2010
    copyright            : (C) 2010 by Biri
    email                : biri@univ-mlv.fr
 ***************************************************************************/
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
#define FLAG_FICHIER	1
#define PAS_FICHIER		1
#define FLAG_TCOMPTE	1
#define TIME_LIMIT		1000

#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GL/glut.h>

#include "../tools/basic_mesh.hpp"
#include "../tools/texture.hpp"
#include "../tools/shaders.hpp"
#include "TimerManager.hpp"

using namespace std;
using namespace STP3D;

//////////////////////////////////////////////////////////////////////////////
/// variables globales
//////////////////////////////////////////////////////////////////////////////

/// L'ecran
extern unsigned int width_ecran;
extern unsigned int height_ecran;

/// Flag pour le dessin
extern uint flag_dessin;
extern bool flag_svg_fic;
extern bool flag_test;
extern bool flag_shaders;
extern bool flag_timer;
extern unsigned char *image_save;
/// Variable indiquant le test réalisé
/// 0 : no test
/// 1 : RGB => BGR
/// 2 : Luminance
/// 3 : Sepia personnalisé ...
extern unsigned int which_test;

/// La scene
extern char nomFIC[60];
extern Texture2D* image_base;
extern Texture2D* image_gpu;
extern Texture2D* image_cpu;
extern unsigned int fbo_handler;
extern TimerManager* timeManager;
extern GLuint visuShader;
extern GLuint treatmentShader;
extern StandardMesh* screen_quad;
extern float mouse[2];
extern float sepiaColor[3];

/// ///////////////////////////////////////////////////////////////////////////
///				Prototypes
/// ///////////////////////////////////////////////////////////////////////////
void init();
void initGL();
void reshapeFunc(int,int);
void kbdFunc(unsigned char,int,int);
void kbdSpFunc(int,int,int);
void clickMouse(int button,int state,int x,int y);
void motionFunc(int x,int y);
void help(char *);
void print();
void idleFunc();

/// ///////////////////////////////////////////////////////////////////////////
/// Fonctions utilitaires
/// ///////////////////////////////////////////////////////////////////////////
void saveFromGL(int w,int h,const char *nom_dest);
