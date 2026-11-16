// BallanceMMO x87-soft headless engine (VX_X87_GAME): the 2002 CK2_3D.dll's VxVector helpers (an unoptimised build:
// every operation is its own x87 function) as they run on the game thread, on the IVP x87 model at the game thread's
// precision (IVP_X87_Control::current(): 24 bits before the process's first hull, 64 after).
#if defined(VX_X87_GAME)
#include "CK2_3DX87.h"
#include <ivu_x87.hxx>

typedef IVP_U_Float80 F;

// 0xBF50 SquareMagnitude ((x*x + y*y) + z*z in the registers), 0xBF10 stores it to a float and returns
// 0xBF30's msvcrt sqrt of it (the root rounded to a double by msvcrt's exit), stored to a float by the caller.
float ck2_3d_x87_magnitude(const VxVector &v)
{
    IVP_X87_Arithmetic x(IVP_X87_Control::current());
    const F a = x.load(v.x), b = x.load(v.y), c = x.load(v.z);
    const float square = x.store_float(x.add(x.add(x.multiply(a, a), x.multiply(b, b)), x.multiply(c, c)));
    return x.store_float(x.load(x.store_double(x.square_root(x.load(square)))));
}

// 0xBEC0 Normalize(const VxVector&): 1.0f divided by 0xBF10's root (unrounded beyond the double) stored to a float
// argument, then 0xBFF0: each component times it, stored to a float.
VxVector ck2_3d_x87_normalized(const VxVector &v)
{
    IVP_X87_Arithmetic x(IVP_X87_Control::current());
    const F a = x.load(v.x), b = x.load(v.y), c = x.load(v.z);
    const float square = x.store_float(x.add(x.add(x.multiply(a, a), x.multiply(b, b)), x.multiply(c, c)));
    const F root = x.load(x.store_double(x.square_root(x.load(square))));
    const float inverse = x.store_float(x.divide(x.load(1.0f), root));
    return VxVector(x.store_float(x.multiply(x.load(inverse), a)), x.store_float(x.multiply(x.load(inverse), b)),
                    x.store_float(x.multiply(x.load(inverse), c)));
}

// 0xBF80 CrossProduct(a, b): each component a product minus a product in the registers, stored once.
VxVector ck2_3d_x87_cross(const VxVector &a, const VxVector &b)
{
    IVP_X87_Arithmetic x(IVP_X87_Control::current());
    auto L = [&](float f) { return x.load(f); };
    const float rz = x.store_float(x.subtract(x.multiply(L(a.x), L(b.y)), x.multiply(L(a.y), L(b.x))));
    const float ry = x.store_float(x.subtract(x.multiply(L(a.z), L(b.x)), x.multiply(L(a.x), L(b.z))));
    const float rx = x.store_float(x.subtract(x.multiply(L(a.y), L(b.z)), x.multiply(L(a.z), L(b.y))));
    return VxVector(rx, ry, rz);
}

// 0xBFF0 operator*(const VxVector&, float).
VxVector ck2_3d_x87_scaled(const VxVector &v, float f)
{
    IVP_X87_Arithmetic x(IVP_X87_Control::current());
    return VxVector(x.store_float(x.multiply(x.load(f), x.load(v.x))), x.store_float(x.multiply(x.load(f), x.load(v.y))),
                    x.store_float(x.multiply(x.load(f), x.load(v.z))));
}
#endif
