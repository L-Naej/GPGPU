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

#include "interface.hpp"
#include "display.hpp"
#include "rtfbo.hpp"
#include "imageComputation.hpp"
#include <iostream>


/// ///////////////////////////////////////////////////////////////////////////
/// variables globales
/// ///////////////////////////////////////////////////////////////////////////

/// ///////////////////////////////////////////////////////////////////////////
/// Corps des fonctions de dessin
/// ///////////////////////////////////////////////////////////////////////////

void initDisplay() {
	flag_dessin = 0;

	sepiaColor[0] = 0.2;
	sepiaColor[1] = 0.1;
	sepiaColor[2] = 0;

	/// INITIALISATION DES TEXTURES ...
	initTexture();

	/// INITIALISATION DES SHADERS ...
	if(!ShaderManager::areShadersSupported(true)) {
		std::cerr<<"[In initDisplay] : Shaders are not supported..."<<std::endl;
		exit(5);
	}
	visuShader = ShaderManager::loadShader("./shaders/visuShader.vert","./shaders/visuShader.frag",true);
	treatmentShader = ShaderManager::loadShader("./shaders/treatmentShader.vert","./shaders/treatmentShader.frag",true);

	screen_quad = basicRect(1.0,1.0);
	screen_quad->createVAO();

	/// INITIALISATION DES FBOS ...
	if(initFBO()==false){
		cerr<<"FBO not supported ! Exiting"<<endl;
		exit(5);
	}

	glGenFramebuffersEXT(1, &fbo_handler);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fbo_handler);
	GLuint depth_rb;
	glGenRenderbuffersEXT(1, &depth_rb);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_DEPTH_COMPONENT,image_base->tex_w,image_base->tex_h);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, image_base->gl_id_tex , 0);
	checkFramebufferStatus();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

	/// INITIALISATION CLASSIQUE OPENGL ...
	glDisable(GL_DEPTH_TEST);
	glShadeModel( GL_SMOOTH );
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	CHECK_GL;

	glUseProgram(0);
}

void initTexture() {
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	image_base->initTexture();
	image_base->setTextureWraping(GL_CLAMP,GL_CLAMP);
	image_cpu->initTexture();
	image_cpu->setTextureWraping(GL_CLAMP,GL_CLAMP);
	image_gpu->initTexture();
	image_gpu->setTextureWraping(GL_CLAMP,GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D,0);
}

void drawScene(void) {


	// ********************************** CLEARING ***********************************
	glClearColor(0.0,0.0,0.5,0.0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// *************************** TRAITEMENT D'IMAGE ********************************
	timeManager->startOneExecution(0,true);
	switch(which_test) {
		case (0) :
			// Nothing to do...
			break;
		case (1) :
			if (flag_shaders) makeColorSwitchGPU(image_base,image_gpu);
			else  makeColorSwitch(image_base,image_cpu);
			break;
		case (2) :
			if (flag_shaders) makeLuminanceGPU(image_base,image_gpu);
			else  makeLuminance(image_base,image_cpu);
			break;
		case (3) :
			if (flag_shaders) makeSepiaGPU(image_base,image_gpu,sepiaColor);
			else  makeSepia(image_base,image_cpu,sepiaColor);
			break;
		case (4) :
			if (flag_shaders) makeBlurGPU(image_base,image_gpu);
			else  makeBlur(image_base,image_cpu);
			break;
		default :
			std::cerr<<"Cas non traité (encore)"<<std::endl;
	}
	timeManager->stopOneExecution(0,true);

	/// DESSIN DES TEXTURES
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glColor3f(1.0,1.0,1.0);
  if (flag_dessin==0)
    image_base->loadTexture();
	if (flag_dessin==1) {
		image_cpu->loadTexture();
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,image_cpu->tex_w,image_cpu->tex_h,
									 0,GL_RGB,GL_UNSIGNED_BYTE,image_cpu->getTab());
		glFinish();
	}
  if (flag_dessin==2)
    image_gpu->loadTexture();
	if (flag_dessin==3) {
		image_cpu->loadTexture(GL_TEXTURE0);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,image_cpu->tex_w,image_cpu->tex_h,
									 0,GL_RGB,GL_UNSIGNED_BYTE,image_cpu->getTab());
		glFinish();
		image_gpu->loadTexture(GL_TEXTURE1);
	}

	glUseProgram(visuShader);
	glUniform1i(glGetUniformLocation(visuShader,"visuMode"),flag_dessin);
	glUniform1i(glGetUniformLocation(visuShader,"basetex"),0);
	glUniform1i(glGetUniformLocation(visuShader,"suptex"),1);
	glUniform2f(glGetUniformLocation(visuShader,"mouseCoord"),mouse[0],mouse[1]);
	screen_quad->draw();

	
	if (flag_dessin==0) image_base->unloadTexture();
	if (flag_dessin==1) image_cpu->unloadTexture();
	if (flag_dessin==2) image_gpu->unloadTexture();
	if (flag_dessin==3) {
		image_gpu->unloadTexture();
		image_cpu->unloadTexture();
	}
	glUseProgram(0);


	// AFFICHAGES TEXTUELS
	std::string cpumsg("TRAITEMENT ");
	cpumsg += intToString(which_test);
	if (flag_shaders) cpumsg += string(" en GPU");
	else cpumsg += string(" en CPU");
	writeString(10,30,(char*)cpumsg.c_str());

	std::string srcmsg("");
	if (flag_dessin < 3) {
		srcmsg += std::string("IMAGE ");
		if (flag_dessin==0) srcmsg += string("SOURCE");
		else if (flag_dessin==1) srcmsg += string("CPU");
		else if (flag_dessin==2) srcmsg += string("GPU");
	}
	else {
		srcmsg += std::string("CPU | GPU");
	}
	writeString(10,50,(char*)srcmsg.c_str());

	std::string fpsmsg("TIME : ");
	fpsmsg += floatToString(timeManager->getLastTime(0));
	writeString(10,10,(char*)fpsmsg.c_str());

	CHECK_GL;

	glFinish();
	glutSwapBuffers();
}

/// ///////////////////////////////////////////////////////////////////////////
/// Corps des fonctions utilitaires
/// ///////////////////////////////////////////////////////////////////////////
void writeString(float x, float y,  char* s)
{
	char* c;
	glRasterPos2f(x, y);                // set position to start drawing fonts
	glDisable(GL_TEXTURE_2D);
	for (c=s; *c != '\0'; c++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c ); // draw the character to the screen
	glEnable(GL_TEXTURE_2D);
}


