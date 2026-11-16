#pragma once
// BallanceMMO x87-soft headless engine: the 2002 CK2_3D.dll x87 VxVector helpers (VX_X87_GAME builds only).
#include "VxVector.h"

float ck2_3d_x87_magnitude(const VxVector &v);
VxVector ck2_3d_x87_normalized(const VxVector &v);
VxVector ck2_3d_x87_cross(const VxVector &a, const VxVector &b);
VxVector ck2_3d_x87_scaled(const VxVector &v, float f);
