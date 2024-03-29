/***************************************************************************
                      imageComputation.hpp  -  description
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
#ifndef ___IMAGE_COMPUTATION_H___
#define ___IMAGE_COMPUTATION_H___
#include <GL/glew.h>
#include <GL/glut.h>
#include "../tools/gl_tools.hpp"
#include "../tools/texture.hpp"

using namespace STP3D;

void makeColorSwitch(Texture2D* img_src,Texture2D* img_res);

void makeColorSwitchGPU(Texture2D* img_src,Texture2D* img_res);

void makeLuminance(Texture2D* img_src,Texture2D* img_res);

void makeLuminanceGPU(Texture2D* img_src,Texture2D* img_res);

void makeSepia(Texture2D* img_src,Texture2D* img_res,float* sepiaColor);

void makeSepiaGPU(Texture2D* img_src,Texture2D* img_res,float* sepiaColor);

void makeBlur(Texture2D* img_src,Texture2D* img_res);

void makeBlurGPU(Texture2D* img_src,Texture2D* img_res);

#endif