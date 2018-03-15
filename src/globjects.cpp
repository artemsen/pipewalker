/************************************************************************** 
 *  PipeWalker - simple puzzle game                                       * 
 *  Copyright (C) 2007-2008 by Artem A. Senichev <artemsen@gmail.com>     * 
 *                                                                        * 
 *  This program is free software: you can redistribute it and/or modify  * 
 *  it under the terms of the GNU General Public License as published by  * 
 *  the Free Software Foundation, either version 3 of the License, or     * 
 *  (at your option) any later version.                                   * 
 *                                                                        * 
 *  This program is distributed in the hope that it will be useful,       * 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         * 
 *  GNU General Public License for more details.                          * 
 *                                                                        * 
 *  You should have received a copy of the GNU General Public License     * 
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. * 
 **************************************************************************/

#include "globjects.h"
#include "tgaloader.h"


//! Types and files of textures
struct TextFiles {
	CGLObjects::Texture	Type;
	const char*			FileName;
};

static const TextFiles TextureFiles[] = {
	{ CGLObjects::TxrCell,			"textures/cell_bkgr.tga" },
	{ CGLObjects::TxrLock,			"textures/cell_lock.tga" },
	{ CGLObjects::TxrActiveTube,	"textures/tube_act.tga" },
	{ CGLObjects::TxrPassiveTube,	"textures/tube_psv.tga" },
	{ CGLObjects::TxrSender,		"textures/sender.tga" },
	{ CGLObjects::TxrRcvActive,		"textures/rcv_dsp_act.tga" },
	{ CGLObjects::TxrRcvPassive,	"textures/rcv_dsp_psv.tga" },
	{ CGLObjects::TxrRcvBack,		"textures/rcv_static.tga" },
	{ CGLObjects::TxrButtonRad,		"textures/btn_radio.tga" },
	{ CGLObjects::TxrButtonNew,		"textures/btn_new.tga" },
	{ CGLObjects::TxrButtonCust,	"textures/btn_custom.tga" },
	{ CGLObjects::TxrButtonRset,	"textures/btn_reset.tga" },
	{ CGLObjects::TxrButtonOk,		"textures/btn_ok.tga" },
	{ CGLObjects::TxrButtonCncl,	"textures/btn_cancel.tga" },
	{ CGLObjects::TxrWndInfo,		"textures/wnd_info.tga" },
	{ CGLObjects::TxrWndCustom,		"textures/wnd_custom.tga" },
	{ CGLObjects::TxrHexNum,		"textures/hexnum.tga"},
	{ CGLObjects::TxrTitle,			"textures/env_title.tga" },
	{ CGLObjects::TxrEnvironment,	"textures/env_bkgr.tga" }
};


CGLObjects::CGLObjects(void)
{
	m_pszLastErrorMessage[0] = 0;
}


CGLObjects::~CGLObjects()
{
	//TODO: Destroy textures and display lists
	//glDeleteLists(0, m_aDispList[ObjCounter]);
}


bool CGLObjects::Initialize(void)
{
	unsigned int i;	//Counter

	//Load the textures from files
	for (i = 0; i < sizeof(TextureFiles) / sizeof(TextureFiles[0]); i++) {
		if (!CTGALoader::LoadTexture(&m_aTextures[TextureFiles[i].Type], TextureFiles[i].FileName, m_pszLastErrorMessage)) {
			return false;
		}
	}

	//Create hex numbers display list
	m_aDispList[ObjHexNum] = glGenLists(16);	//16 hex numbers 0...F
	const GLfloat dHexNumSize = 0.5f;
	for (i = 0; i < 16; i++) {
		GLfloat dX = static_cast<GLfloat>(i) / 16.0f;
		glNewList(m_aDispList[ObjHexNum] + i, GL_COMPILE);				
			glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrHexNum]);	
			glBegin(GL_QUADS);									
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(dX,                  0.0f); glVertex2f(0.0f,               0.0f);
				glTexCoord2f(dX + (1.0f / 16.0f), 0.0f); glVertex2f(dHexNumSize / 1.2f, 0.0f);
				glTexCoord2f(dX + (1.0f / 16.0f), 1.0f); glVertex2f(dHexNumSize / 1.2f, dHexNumSize);
				glTexCoord2f(dX,                  1.0f); glVertex2f(0.0f,               dHexNumSize);
			glEnd();											
		glEndList();						
	}
	
	//Text "Map ID:"
	Image hMapIdText;
	if (!CTGALoader::LoadImage(&hMapIdText, "textures/mapid.tga", m_pszLastErrorMessage))
		return false;
	m_aDispList[ObjMapId] = glGenLists(1);
	glNewList(m_aDispList[ObjMapId], GL_COMPILE);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
			glDrawPixels(hMapIdText.Width, hMapIdText.Height, hMapIdText.Format, GL_UNSIGNED_BYTE, hMapIdText.Data);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	glEndList();

	//Radio button
	m_aDispList[ObjButtonRadio] = glGenLists(2);	//2 button: on and off
	const GLfloat dRadioButtonSize = 0.6f;
	for (i = 0; i < 2; i++) {
		GLfloat dX = static_cast<GLfloat>(i) / 2.0f;
		glNewList(m_aDispList[ObjButtonRadio] + i, GL_COMPILE);				
			glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrButtonRad]);	
			glBegin(GL_QUADS);									
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(dX,        0.0f); glVertex2f(0.0f,             0.0f);
				glTexCoord2f(dX + 0.5f, 0.0f); glVertex2f(dRadioButtonSize, 0.0f);
				glTexCoord2f(dX + 0.5f, 1.0f); glVertex2f(dRadioButtonSize, dRadioButtonSize);
				glTexCoord2f(dX,        1.0f); glVertex2f(0.0f,             dRadioButtonSize);
			glEnd();											
		glEndList();						
	}

	//Cell
	m_aDispList[ObjCell] = glGenLists(1);
	glNewList(m_aDispList[ObjCell], GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrCell]);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, 0.5f, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f,-0.5f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f,-0.5f, 0.0f);
		glEnd();
	glEndList();

	//Tube
	static GLfloat TubeVert[] = {
		-0.07f, 0.0f, 0.10f,	// 0
		-0.07f, 0.5f, 0.10f,	// 1
		-0.07f, 0.0f, 0.15f,	// 2
		-0.07f, 0.5f, 0.15f,	// 3
		-0.02f, 0.0f, 0.20f,	// 4
		-0.02f, 0.5f, 0.20f,	// 5
		 0.02f, 0.0f, 0.20f,	// 6
		 0.02f, 0.5f, 0.20f,	// 7
		 0.07f, 0.0f, 0.15f,	// 8
		 0.07f, 0.5f, 0.15f,	// 9
		 0.07f, 0.0f, 0.10f,	//10
		 0.07f, 0.5f, 0.10f,	//11
		 0.07f, 0.0f, 0.05f,	//12
		 0.07f, 0.5f, 0.05f,	//13
		 0.02f, 0.0f, 0.00f,	//14
		 0.02f, 0.5f, 0.00f,	//15
		-0.02f, 0.0f, 0.00f,	//16
		-0.02f, 0.5f, 0.00f,	//17
		-0.07f, 0.0f, 0.05f,	//18
		-0.07f, 0.5f, 0.05f,	//19
		-0.07f, 0.0f, 0.10f,	//20
		-0.07f, 0.5f, 0.10f		//21
	};
	static GLushort TubeInd[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21
	};
	static GLshort TubeTex[] = {
		0, 1, 1, 1, 1, 0, 0, 0,
		0, 1, 1, 1, 1, 0, 0, 0,
		0, 1, 1, 1, 1, 0, 0, 0,
		0, 1, 1, 1, 1, 0, 0, 0,
		0, 1, 1, 1, 1, 0, 0, 0,
		0, 1, 1, 1, 1, 0, 0, 0
	};
	static GLfloat TubeNorm[] = {
		-1.0f, 0.0f, 0.0f,		// 0
		-1.0f, 0.0f, 0.0f,		// 1
		-1.0f, 0.0f, 1.0f,		// 2
		-1.0f, 0.0f, 1.0f,		// 3
		 0.0f, 0.0f, 1.0f,		// 4
		 0.0f, 0.0f, 1.0f,		// 5
		 0.0f, 0.0f, 1.0f,		// 6
		 0.0f, 0.0f, 1.0f,		// 7
		 1.0f, 0.0f, 1.0f,		// 8
		 1.0f, 0.0f, 1.0f,		// 9
		 1.0f, 0.0f, 0.0f,		//10
		 1.0f, 0.0f, 0.0f,		//11
		 1.0f, 0.0f, 1.0f,		//12
		 1.0f, 0.0f, 1.0f,		//13
		 0.0f, 0.0f, 1.0f,		//14
		 0.0f, 0.0f, 1.0f,		//15
		 0.0f, 0.0f, 1.0f,		//16
		 0.0f, 0.0f, 1.0f,		//17
		-1.0f, 0.0f, 1.0f,		//18
		-1.0f, 0.0f, 1.0f,		//19
		-1.0f, 0.0f, 0.0f,		//20
		-1.0f, 0.0f, 0.0f		//21
	};
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	m_aDispList[ObjTube] = glGenLists(1);
	glNewList(m_aDispList[ObjTube], GL_COMPILE);
		glTexCoordPointer(2, GL_SHORT, 0, TubeTex);
		glNormalPointer(GL_FLOAT, 0, TubeNorm);
		glVertexPointer(3, GL_FLOAT, 0, TubeVert);
		glDrawElements(GL_QUAD_STRIP, sizeof(TubeInd) / sizeof(TubeInd[0]), GL_UNSIGNED_SHORT, TubeInd);
	glEndList();
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	//Tube joiner
	m_aDispList[ObjTubeJoiner] = glGenLists(1);
	glNewList(m_aDispList[ObjTubeJoiner], GL_COMPILE);
		DrawBox(-0.13f, 0.13f, 0.20f, 0.26f, 0.26f, 0.20f);
	glEndList();
	
	//Sender
	m_aDispList[ObjSender] = glGenLists(1);
	glNewList(m_aDispList[ObjSender], GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrSender]);
		DrawBox(-0.35f, 0.4f, 0.4f, 0.7f, 0.8f, 0.4f);
	glEndList();

	//Receiver (system block)
	m_aDispList[ObjReceiverSb] = glGenLists(1);
	glNewList(m_aDispList[ObjReceiverSb], GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrRcvBack]);
		DrawBox(-0.4f, -0.17f, 0.3f, 0.8f, 0.23f, 0.3f);
	glEndList();

	//Receiver (monitor)
	m_aDispList[ObjReceiverMon] = glGenLists(1);
	glNewList(m_aDispList[ObjReceiverMon], GL_COMPILE);
		DrawBox(-0.4f, 0.4f, 0.25f, 0.8f, 0.7f, 0.18f);
	glEndList();

	//Lock
	m_aDispList[ObjLock] = glGenLists(1);
	glNewList(m_aDispList[ObjLock], GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrLock]);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2i(0, 1); glVertex3f(-0.3f,  0.3f, 0.0f);
			glTexCoord2i(1, 1); glVertex3f( 0.3f,  0.3f, 0.0f);
			glTexCoord2i(1, 0); glVertex3f( 0.3f, -0.3f, 0.0f);
			glTexCoord2i(0, 0); glVertex3f(-0.3f, -0.3f, 0.0f);
		glEnd();
	glEndList();

	//Button
	m_aDispList[ObjButton] = glGenLists(1);
	glNewList(m_aDispList[ObjButton], GL_COMPILE);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.7f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(2.8f, 0.7f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(2.8f, 0.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();
	glEndList();

	//Spin
	m_aDispList[ObjSpin] = glGenLists(1);
	glNewList(m_aDispList[ObjSpin], GL_COMPILE);
		BindTexture(CGLObjects::TxrActiveTube);
		glBegin(GL_TRIANGLES);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.0f, 0.0f, 0.05f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.2f,-0.4f, 0.05f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.2f,-0.4f, 0.05f);
		glEnd();
	glEndList();
	
	//Environment
	static GLfloat EnvVert[] = {
		-5.0f, 5.0f, 0.0f,	// 0
		-5.0f, 5.0f, 0.3f,	// 1
		 5.0f, 5.0f, 0.0f,	// 2
		 5.0f, 5.0f, 0.3f,	// 3
		 5.0f,-5.0f, 0.0f,	// 4
		 5.0f,-5.0f, 0.3f,	// 5
		-5.0f,-5.0f, 0.0f,	// 6
		-5.0f,-5.0f, 0.3f	// 7
	};
	static GLushort EnvInd[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 0, 1
	};
	static GLshort EnvTex[] = {
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0
	};
	static GLfloat EnvNorm[] = {
		 1.0f,-1.0f, 0.0f,		// 0
		 1.0f,-1.0f, 0.0f,		// 1
		-1.0f,-1.0f, 0.0f,		// 2
		-1.0f,-1.0f, 0.0f,		// 3
		-1.0f, 1.0f, 0.0f,		// 4
		-1.0f, 1.0f, 0.0f,		// 5
		 1.0f, 1.0f, 0.0f,		// 6
		 1.0f, 1.0f, 0.0f,		// 7
		 1.0f,-1.0f, 0.0f,		// 8
		 1.0f,-1.0f, 0.0f		// 9
	};
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	m_aDispList[ObjEnvironment] = glGenLists(1);
	glNewList(m_aDispList[ObjEnvironment], GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, m_aTextures[TxrEnvironment]);
		glTexCoordPointer(2, GL_SHORT, 0, EnvTex);
		glNormalPointer(GL_FLOAT, 0, EnvNorm);
		glVertexPointer(3, GL_FLOAT, 0, EnvVert);
		glDrawElements(GL_QUAD_STRIP, sizeof(EnvInd) / sizeof(EnvInd[0]), GL_UNSIGNED_SHORT, EnvInd);
		//Top front side (main background)
		glBegin(GL_QUADS);
			glNormal3b(0, 0, 1);
			glTexCoord2i(0, 0); glVertex3f(-8.0f, 9.0f, 0.0f);
			glTexCoord2i(0, 1); glVertex3f( 8.0f, 9.0f, 0.0f);
			glTexCoord2i(1, 1); glVertex3f( 8.0f, 5.0f, 0.0f);
			glTexCoord2i(1, 0); glVertex3f(-8.0f, 5.0f, 0.0f);

			glTexCoord2i(0, 0); glVertex3f(-8.0f,-5.0f, 0.0f);
			glTexCoord2i(0, 1); glVertex3f( 8.0f,-5.0f, 0.0f);
			glTexCoord2i(1, 1); glVertex3f( 8.0f,-9.0f, 0.0f);
			glTexCoord2i(1, 0); glVertex3f(-8.0f,-9.0f, 0.0f);

			glTexCoord2i(0, 0); glVertex3f(-8.0f, 5.0f, 0.0f);
			glTexCoord2i(0, 1); glVertex3f(-5.0f, 5.0f, 0.0f);
			glTexCoord2i(1, 1); glVertex3f(-5.0f,-5.0f, 0.0f);
			glTexCoord2i(1, 0); glVertex3f(-8.0f,-5.0f, 0.0f);

			glTexCoord2i(0, 0); glVertex3f( 5.0f, 5.0f, 0.0f);
			glTexCoord2i(0, 1); glVertex3f( 8.0f, 5.0f, 0.0f);
			glTexCoord2i(1, 1); glVertex3f( 8.0f,-5.0f, 0.0f);
			glTexCoord2i(1, 0); glVertex3f( 5.0f,-5.0f, 0.0f);

			glTexCoord2i(0, 0); glVertex3f( 8.0f,  9.0f, -10.5f);
			glTexCoord2i(0, 1); glVertex3f(-8.0f,  9.0f, -10.5f);
			glTexCoord2i(1, 1); glVertex3f(-8.0f, -9.0f, -10.5f);
			glTexCoord2i(1, 0); glVertex3f( 8.0f, -9.0f, -10.5f);
		glEnd();
	glEndList();
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
 
	return true;
}


void CGLObjects::DrawBox(GLfloat dX1, GLfloat dY1, GLfloat dZ1, GLfloat dWidth, GLfloat dHeight, GLfloat dDepth) const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	GLfloat Vert[] = {
		dX1,          dY1,           dZ1,			//0
		dX1 + dWidth, dY1,           dZ1,			//1
		dX1 + dWidth, dY1 - dHeight, dZ1,			//2
		dX1,          dY1 - dHeight, dZ1,			//3
		dX1,          dY1,           dZ1 - dDepth,	//4
		dX1 + dWidth, dY1,           dZ1 - dDepth,	//5
		dX1 + dWidth, dY1 - dHeight, dZ1 - dDepth,	//6
		dX1,          dY1 - dHeight, dZ1 - dDepth	//7
	};

	static GLushort Ind[] = {
		0, 1, 2, 3,				//Front
		0, 4, 5, 1,				//Top
		3, 7, 6, 2,				//Bottom
		0, 4, 7, 3,				//Left
		1, 5, 6, 2				//Right
	  //4, 5, 6, 7				//Back
	};
	static GLfloat Tex[] = {
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,	//Front
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,	//Top
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,	//Bottom
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,	//Left
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f	//Right
	};
	static GLfloat Norm[] = {
		 0.0f, 0.0f, 1.0f,		//Front
		 0.0f, 0.0f, 1.0f,		//Front
		 0.0f, 0.0f, 1.0f,		//Front
		 0.0f, 0.0f, 1.0f,		//Front
		 0.0f, 1.0f, 0.0f,		//Top
		 0.0f, 1.0f, 0.0f,		//Top
		 0.0f, 1.0f, 0.0f,		//Top
		 0.0f, 1.0f, 0.0f,		//Top
		 0.0f,-1.0f, 0.0f,		//Bottom
		 0.0f,-1.0f, 0.0f,		//Bottom
		 0.0f,-1.0f, 0.0f,		//Bottom
		 0.0f,-1.0f, 0.0f,		//Bottom
		-1.0f, 0.0f, 0.0f,		//Left
		-1.0f, 0.0f, 0.0f,		//Left
		-1.0f, 0.0f, 0.0f,		//Left
		-1.0f, 0.0f, 0.0f,		//Left
		 1.0f, 0.0f, 0.0f,		//Right
		 1.0f, 0.0f, 0.0f,		//Right
		 1.0f, 0.0f, 0.0f,		//Right
		 1.0f, 0.0f, 0.0f		//Right
	};

	glTexCoordPointer(2, GL_FLOAT, 0, Tex);
	glNormalPointer(GL_FLOAT, 0, Norm);
	glVertexPointer(3, GL_FLOAT, 0, Vert);
	glDrawElements(GL_QUADS, sizeof(Ind) / sizeof(Ind[0]), GL_UNSIGNED_SHORT, Ind);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}


void CGLObjects::DrawStatusBar(const unsigned int nMapId) const
{
	glTranslatef(0.0f, -0.1f, 0.0f);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glPushAttrib(GL_LIST_BIT);
		glRasterPos2f(0.0f, 0.0f);
		glCallList(m_aDispList[ObjMapId]);
	glPopAttrib();
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glTranslatef(1.5f, -0.1f, 0.0f);
	for (int i = 0; i < 8; i++) {
		unsigned short nNumPart = (nMapId & (0xF0000000 >> (i * sizeof(unsigned int)))) >> (sizeof(unsigned int) * 8 - sizeof(unsigned int) - i * sizeof(unsigned int));
		glTranslatef(0.5f, 0.0f, 0.0f);
		PrintHexNumber(nNumPart);
	}
}
