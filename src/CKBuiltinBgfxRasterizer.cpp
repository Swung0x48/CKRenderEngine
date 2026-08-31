#include "CKRasterizer.h"

// This adapter belongs only to the regular static graphics target.  Keeping
// the bgfx symbol out of CK2_3D.cpp lets CK2_3DHeadlessStatic use the same
// complete RCK implementation without acquiring a mandatory graphics backend.
extern void CKBgfxRasterizerGetInfo(CKRasterizerInfo *info);

void CKREGetBuiltInBgfxRasterizerInfo(CKRasterizerInfo *info)
{
    CKBgfxRasterizerGetInfo(info);
}
