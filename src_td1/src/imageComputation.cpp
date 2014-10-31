/***************************************************************************
                      imageComputation.cpp  -  description
                             -------------------
    begin                : Aug 21 2010
    copyright            : (C) 2010 by Biri
    email                : biri@univ-mlv.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "imageComputation.hpp"
#include "interface.hpp"
#include "../tools/gl_tools.hpp"

/// /////////////////////////////////////////////////////////////////////////////////////
/// Corps des fonctions utilitaires
/// /////////////////////////////////////////////////////////////////////////////////////

void setPx(Texture2D* img,uint i,uint j,uint canal, char value) {
	unsigned char* tab = img->getTab();
	tab[j*img->tex_w*3+i*3+canal] = value;
}

unsigned char getPx(Texture2D* img,uint i,uint j,uint canal) {
	return (img->getTab())[j*img->tex_w*3+i*3+canal];
}


void drawOneFrontQuad() {
	screen_quad->draw();
}

void startFBOComputation(Texture2D* img_src,Texture2D* img_res) {
	// FIXATION OFFLINE RENDERING
	glBindFramebuffer(GL_FRAMEBUFFER,fbo_handler);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,img_res->gl_id_tex,0);
	glClearColor(1.0,1.0,0.0,0.0);
	glClear( GL_COLOR_BUFFER_BIT );

	// CALCUL DE LA PROJECTION
	glViewport( 0, 0, (GLint)img_src->tex_w, (GLint)img_src->tex_h );

	// FIXATION DE QQ PARAMETRE GL
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	// CHARGEMENT DU VERTEX & FRAGMENT SHADER
	glUseProgram(treatmentShader);
}

void endFBOComputation() {
	// DECHARGEMENT VERTEX & FRAGMENT SHADER
	glUseProgram(0);

	// RETOUR FBO CLASSIQUE
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

/// /////////////////////////////////////////////////////////////////////////////////////
/// Corps des fonctions de calcul
/// /////////////////////////////////////////////////////////////////////////////////////

void makeColorSwitch(Texture2D* img_src,Texture2D* img_res) {

  int decal = 0;
  if (img_src->typetext == TEX_TYPE_LUM)
    assert (false && "Cannot make color switch on grayscale image.");
  if (img_src->typetext == TEX_TYPE_RVB) decal = 3;
  if (img_src->typetext == TEX_TYPE_RVBA) decal = 4;

  for (unsigned int i = 0; i < img_src->tex_h; ++i)
  {
    for (unsigned int j = 0; j < img_src->tex_w * decal; j += decal)
    {
      int red = 0, green = 2, blue = 1;
      img_res->getTab()[i*decal*img_src->tex_h + j + red] = img_src->getTab()[i*decal*img_src->tex_h + j + blue];
      img_res->getTab()[i*decal*img_src->tex_h + j + blue] = img_src->getTab()[i*decal*img_src->tex_h + j + green];
      img_res->getTab()[i*decal*img_src->tex_h + j + green] = img_src->getTab()[i*decal*img_src->tex_h + j + red];
    }
  }
}

void makeColorSwitchGPU(Texture2D* img_src,Texture2D* img_res) {
	// Activation du FBO...
	startFBOComputation(img_src,img_res);

	// DO SOME WORK...
  //std::cerr<<"makeColorSwitch (GPU) non implemente"<<std::endl;

	// CHARGEMENT DE LA TEXTURE SOURCE...
	img_src->loadTexture();

	// FIXATION DES PARAMETRES SHADERS
  glUniform1i(glGetUniformLocation(treatmentShader, "basetex"), 0);
  glUniform1i(glGetUniformLocation(treatmentShader, "treatment"), 1);

  CHECK_GL

	drawOneFrontQuad();

	// DECHARGEMENT DE LA TEXTURE SOURCE...
	img_src->unloadTexture();

	endFBOComputation();
}

void makeLuminance(Texture2D* img_src,Texture2D* img_res) {
	// L = 0.3 R + 0.59 G + 0.11 B
  int decal = 0;
  if (img_src->typetext == TEX_TYPE_LUM)
    assert (false && "Already a grayscale image");
  if (img_src->typetext == TEX_TYPE_RVB) decal = 3;
  if (img_src->typetext == TEX_TYPE_RVBA) decal = 4;

  for (unsigned int i = 0; i < img_src->tex_h; ++i)
  {
    for (unsigned int j = 0; j < img_src->tex_w * decal; j += decal)
    {
      int red = 0, green = 2, blue = 1;
      unsigned char luminance = (unsigned char) (0.3f * img_src->getTab()[i*decal*img_src->tex_h + j + red]
                           + 0.59f * img_src->getTab()[i*decal*img_src->tex_h + j + green]
                           + 0.11 * img_src->getTab()[i*decal*img_src->tex_h + j + blue]);
      img_res->getTab()[i*decal*img_src->tex_h + j + red] = luminance;
      img_res->getTab()[i*decal*img_src->tex_h + j + blue] = luminance;
      img_res->getTab()[i*decal*img_src->tex_h + j + green] = luminance;
    }
  }

}

void makeLuminanceGPU(Texture2D* img_src,Texture2D* img_res) {
	// Activation du FBO...
	startFBOComputation(img_src,img_res);

	// CHARGEMENT DE LA TEXTURE SOURCE...
	img_src->loadTexture();

	// FIXATION DES PARAMETRES SHADERS
  glUniform1i(glGetUniformLocation(treatmentShader, "basetex"), 0);
  glUniform1i(glGetUniformLocation(treatmentShader, "treatment"), 2);
  CHECK_GL
	
	drawOneFrontQuad();

	// DECHARGEMENT DE LA TEXTURE SOURCE...
	img_src->unloadTexture();

	endFBOComputation();
}

void makeSepia(Texture2D* img_src,Texture2D* img_res,float* sepiaColor) {
	// L = 0.3 R + 0.59 G + 0.11 B
	// R = (1 + S_r) * L
	// G = (1 + S_g) * L
	// G = (1 + S_b) * L

  int decal = 0;
  if (img_src->typetext == TEX_TYPE_LUM)
    assert (false && "Already a grayscale image");
  if (img_src->typetext == TEX_TYPE_RVB) decal = 3;
  if (img_src->typetext == TEX_TYPE_RVBA) decal = 4;

  for (unsigned int i = 0; i < img_src->tex_h; ++i)
  {
    for (unsigned int j = 0; j < img_src->tex_w * decal; j += decal)
    {
      int red = 0, green = 2, blue = 1;
      unsigned char luminance = (unsigned char) (0.3f * img_src->getTab()[i*decal*img_src->tex_h + j + red]
                           + 0.59f * img_src->getTab()[i*decal*img_src->tex_h + j + green]
                           + 0.11 * img_src->getTab()[i*decal*img_src->tex_h + j + blue]);
      img_res->getTab()[i*decal*img_src->tex_h + j + red] = (sepiaColor[0]) * luminance;
      img_res->getTab()[i*decal*img_src->tex_h + j + blue] = (sepiaColor[1]) * luminance;
      img_res->getTab()[i*decal*img_src->tex_h + j + green] = (sepiaColor[2]) * luminance;
    }
  }

}

void makeSepiaGPU(Texture2D* img_src,Texture2D* img_res,float* sepiaColor) {
	// Activation du FBO...
	startFBOComputation(img_src,img_res);

	// DO SOME WORK...
	std::cerr<<"makeSepia (GPU) non implemente"<<std::endl;

	// CHARGEMENT DE LA TEXTURE SOURCE...
	img_src->loadTexture();

	// FIXATION DES PARAMETRES SHADERS
  glUniform1i(glGetUniformLocation(treatmentShader, "basetex"), 0);
  glUniform1i(glGetUniformLocation(treatmentShader, "treatment"), 3);
  glUniform3f(glGetUniformLocation(treatmentShader, "sepiaColor"), sepiaColor[0],
                                                                   sepiaColor[1],
                                                                   sepiaColor[2]);

  CHECK_GL
	
	drawOneFrontQuad();

	// DECHARGEMENT DE LA TEXTURE SOURCE...
	img_src->unloadTexture();

	endFBOComputation();
}

void makeBlur(Texture2D* img_src,Texture2D* img_res) {
	// Simple flou : on additionne au pixel ses 8 pixels voisins (et on divise par 9 bien sûr)

  int decal = 0;
  if (img_src->typetext == TEX_TYPE_LUM)
    assert (false && "Already a grayscale image");
  if (img_src->typetext == TEX_TYPE_RVB) decal = 3;
  if (img_src->typetext == TEX_TYPE_RVBA) decal = 4;

  for (unsigned int i = 0; i < img_src->tex_h; ++i)
  {
    for (unsigned int j = 0; j < img_src->tex_w * decal; j += decal)
    {
      int red = 0, green = 2, blue = 1;
      unsigned int finalRed = img_res->getTab()[i*decal*img_src->tex_h + j + red];
      unsigned int finalGreen = img_res->getTab()[i*decal*img_src->tex_h + j + green];
      unsigned int finalBlue = img_res->getTab()[i*decal*img_src->tex_h + j + blue];

      //Haut
      if (i > 0)
      {
        finalRed += img_src->getTab()[(i-1)*decal*img_src->tex_h + j + red];
        finalBlue += img_src->getTab()[(i-1)*decal*img_src->tex_h + j + blue];
        finalGreen += img_src->getTab()[(i-1)*decal*img_src->tex_h + j + green];
      }

      //Bas
      if (i < img_src->tex_h - 2)
      {
        finalRed += img_src->getTab()[(i+1)*decal*img_src->tex_h + j + red];
        finalRed += img_src->getTab()[(i+1)*decal*img_src->tex_h + j + green];
        finalBlue += img_src->getTab()[(i+1)*decal*img_src->tex_h + j + blue];
      }

      //Gauche
      if (j > 0)
      {
        finalRed += img_src->getTab()[i*decal*img_src->tex_h + j - decal + red];
        finalGreen += img_src->getTab()[i*decal*img_src->tex_h + j - decal + green];
        finalBlue += img_src->getTab()[i*decal*img_src->tex_h + j - decal + blue];
      }

      //Droite
      if (j < img_src->tex_w* decal - 2)
      {
        finalRed += img_src->getTab()[i*decal*img_src->tex_h + j + decal + red];
        finalGreen += img_src->getTab()[i*decal*img_src->tex_h + j + decal + green];
        finalBlue += img_src->getTab()[i*decal*img_src->tex_h + j + decal + blue];
      }

      //Diagonale haut gauche
      if (i > 0 && j > 0)
      {
       finalRed += img_src->getTab()[(i-1)*decal*img_src->tex_h + j - decal + red];
        finalGreen += img_src->getTab()[(i-1)*decal*img_src->tex_h + j - decal + green];
        finalBlue += img_src->getTab()[(i-1)*decal*img_src->tex_h + j - decal + blue];
      }

      //Diagonale haut droite
      if (i > 0 && j < img_src->tex_w * decal - 2)
      {
        finalRed += img_src->getTab()[(i-1)*decal*img_src->tex_h + j + decal + red];
        finalGreen += img_src->getTab()[(i-1)*decal*img_src->tex_h + j + decal + green];
        finalBlue += img_src->getTab()[(i-1)*decal*img_src->tex_h + j + decal + blue];
      }

      //Diagonale bas gauche
      if (i < img_src->tex_h - 2 && j > 0 )
      {
        finalRed += img_src->getTab()[(i+1)*decal*img_src->tex_h + j - decal + red];
        finalGreen += img_src->getTab()[(i+1)*decal*img_src->tex_h + j - decal + green];
        finalBlue += img_src->getTab()[(i+1)*decal*img_src->tex_h + j - decal + blue];
      }

      //Diagonale bas droite
      if (i < img_src->tex_h - 2 && j < img_src->tex_w * decal - 2)
      {
        finalRed += img_src->getTab()[(i+1)*decal*img_src->tex_h + j + decal + red];
        finalGreen += img_src->getTab()[(i+1)*decal*img_src->tex_h + j + decal + green];
        finalBlue += img_src->getTab()[(i+1)*decal*img_src->tex_h + j + decal + blue];
      }

      //Division par 9
      finalRed /= 9;
      finalGreen /= 9;
      finalBlue /= 9;

      img_res->getTab()[i*decal*img_src->tex_h + j + red] = finalRed;
      img_res->getTab()[i*decal*img_src->tex_h + j + green] = finalGreen;
      img_res->getTab()[i*decal*img_src->tex_h + j + blue] = finalBlue;

    }
  }
}

void makeBlurGPU(Texture2D* img_src,Texture2D* img_res) {
	// Activation du FBO...
	startFBOComputation(img_src,img_res);

	// DO SOME WORK...
	std::cerr<<"makeBlur (GPU) non implemente"<<std::endl;

	// CHARGEMENT DE LA TEXTURE SOURCE...
	img_src->loadTexture();

	// FIXATION DES PARAMETRES SHADERS
	//...
	
	drawOneFrontQuad();

	// DECHARGEMENT DE LA TEXTURE SOURCE...
	img_src->unloadTexture();

	endFBOComputation();
}

