#include <ImathVec.h>
#include <ImathMatrix.h>
#include <ImathMath.h>

namespace Stray
{

using Vec2f = Imath::Vec2<float>;
using Vec3f = Imath::Vec3<float>;
using Vec4f = Imath::Vec4<float>;
using Mat3f = Imath::Matrix33<float>;
using Mat4f = Imath::Matrix44<float>;

static const float EPS = Imath::limits<float>::epsilon();
static const float MIN = Imath::limits<float>::min();
static const float MAX = Imath::limits<float>::max();

} // namespace Stray
