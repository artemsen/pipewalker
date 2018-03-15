/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2009 by Artem A. Senichev <artemsen@gmail.com>     *
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

#include "texture.h"
#include "image.h"

//! Types and files of textures
struct TexFile {
	CTextureBank::TextureType	Type;
	const char*					FileName;
};

static const TexFile TextureFiles[] = {
	{ CTextureBank::TexEnvBkgr,			"env_bkgr.tga"		},
	{ CTextureBank::TexEnvTitle,		"env_title.tga"		},
	{ CTextureBank::TexEnvInfo,			"env_info.tga"		},
	{ CTextureBank::TexCellBackground,	"cell_bkg.tga"		},
	{ CTextureBank::TexSender,			"cell_sender.tga"	},
	{ CTextureBank::TexReceiverActive,	"cell_rcv_a.tga"	},
	{ CTextureBank::TexReceiverPassive,	"cell_rcv_p.tga"	},
	{ CTextureBank::TexLock,			"cell_lock.tga"		},
	{ CTextureBank::TexTubeHalfPassive,	"cell_ph_p.tga"		},
	{ CTextureBank::TexTubeHalfActive,	"cell_ph_a.tga"		},
	{ CTextureBank::TexTubeCrvPassive,	"cell_pc_p.tga"		},
	{ CTextureBank::TexTubeCrvActive,	"cell_pc_a.tga"		},	
	{ CTextureBank::TexTubeStrPassive,	"cell_ps_p.tga"		},
	{ CTextureBank::TexTubeStrActive,	"cell_ps_a.tga"		},
	{ CTextureBank::TexTubeJnrPassive,	"cell_pj_p.tga"		},
	{ CTextureBank::TexTubeJnrActive,	"cell_pj_a.tga"		},
	{ CTextureBank::TexExplosionPart,	"expl_part.tga"		},
	{ CTextureBank::TexNum0,			"num_0.tga"			},
	{ CTextureBank::TexNum1,			"num_1.tga"			},
	{ CTextureBank::TexNum2,			"num_2.tga"			},
	{ CTextureBank::TexNum3,			"num_3.tga"			},
	{ CTextureBank::TexNum4,			"num_4.tga"			},
	{ CTextureBank::TexNum5,			"num_5.tga"			},
	{ CTextureBank::TexNum6,			"num_6.tga"			},
	{ CTextureBank::TexNum7,			"num_7.tga"			},
	{ CTextureBank::TexNum8,			"num_8.tga"			},
	{ CTextureBank::TexNum9,			"num_9.tga"			},
	{ CTextureBank::TexButtonNext,		"btn_next.tga"		},
	{ CTextureBank::TexButtonPrev,		"btn_prev.tga"		},
	{ CTextureBank::TexButtonReset,		"btn_reset.tga"		},
	{ CTextureBank::TexButtonInfo,		"btn_info.tga"		},
	{ CTextureBank::TexButtonOK,		"btn_ok.tga"		},
};


CTexture CTextureBank::m_Texture[CTextureBank::TexCounter];


void CTexture::Free()
{
	if (m_Id)
		glDeleteTextures(1, &m_Id);
	m_Id = 0;
}


void CTexture::Load(const char* fileName)
{
	assert(fileName);

	Free();

	CImage img;
	img.Load(fileName);

	glGenTextures(1, &m_Id);

	glBindTexture(GL_TEXTURE_2D, m_Id);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

#if 0	//best
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, img.GetMode(), img.GetWidth(), img.GetHeight(), img.GetMode(), GL_UNSIGNED_BYTE, img.GetData());
#else	//worst, but fast
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, img.GetMode(), img.GetWidth(), img.GetHeight(), 0, img.GetMode(), GL_UNSIGNED_BYTE, img.GetData());
#endif
}


void CTextureBank::Initialize()
{
	assert(TexCounter == sizeof(TextureFiles) / sizeof(TextureFiles[0]));	//Don't forget sync counter and files!

	Free();

	//Load the textures from files
	for (size_t i = 0; i < TexCounter; ++i) {
		char fileName[256];
		strcpy(fileName, DIR_TEXTURES);
		strcat(fileName, TextureFiles[i].FileName);

		m_Texture[i].Load(fileName);
	}
}


GLuint CTextureBank::Get(const TextureType type)
{
	assert(type >= 0 && type < TexCounter);
	return m_Texture[type].GetId();
}


void CTextureBank::Free()
{
	for (size_t i = 0; i < TexCounter; ++i)
		m_Texture[i].Free();
}
