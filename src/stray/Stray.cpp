#include "Math.h"
#include "Kernels.h"

#include <embree3/rtcore.h>

#include <iostream>
#include <vector>

namespace Stray
{

struct Quad
{
    int v0, v1, v2, v3;
};

} // namespace Stray

using namespace Stray;

static const int SCREEN_WIDTH  = 640;
static const int SCREEN_HEIGHT = 360;

int main(const int argc, const char* argv[])
{
    try
    {
        // Dummy ISPC command
        int N = 16;
        float v[N];
        render(v, N);

        // Make a Quad
        std::vector<Vec3f> vertexBuffer{{ 0.5f, -0.5f, -1.0f},
                                        { 0.5f,  0.5f, -1.0f},
                                        {-0.5f,  0.5f, -1.0f},
                                        {-0.5f, -0.25, -1.0f}};
        std::vector<int> indexBuffer{0, 1, 2 ,3};

        // Init RTCDevice, RTCScene
        RTCDevice device = rtcNewDevice(0x0);
        RTCScene scene   = rtcNewScene(device);

        // Create quad mesh
        RTCGeometry geom = rtcNewGeometry(device,
                                          RTC_GEOMETRY_TYPE_QUAD);


        // Share geometry buffers
        rtcSetSharedGeometryBuffer(geom,
                                   RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
                                   vertexBuffer.data(), 0, sizeof(Vec3f), 4);
        rtcSetSharedGeometryBuffer(geom,
                                   RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT4,
                                   indexBuffer.data(), 0, sizeof(Quad), 4);

        // Commit RTCGeometry
        rtcCommitGeometry(geom);

        // Attach RTCGeometry to RTCScene
        rtcAttachGeometryByID(scene, geom, 0);

        // Commit RTCScene
        rtcCommitScene(scene);

        // Trace a ray!
        RTCIntersectContext rctx;
        rtcInitIntersectContext(&rctx);

        RTCRayHit query;
        query.ray.org_x  = 0.12f;
        query.ray.org_y  = 0.03f;
        query.ray.org_z  = 0.01f;
        query.ray.dir_x  = -0.03f;
        query.ray.dir_y  = 0.05f;
        query.ray.dir_z  = -0.94f;
        query.ray.tnear  = EPS;
        query.ray.tfar   = MAX;
        query.ray.time   = 0.0f;
        query.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        query.hit.geomID = RTC_INVALID_GEOMETRY_ID;

        rtcIntersect1(scene,
                      &rctx,
                      &query);
        if (query.hit.geomID == RTC_INVALID_GEOMETRY_ID)
        {
            std::cout << "Did not hit the Quad! :(" << std::endl;
        }
        else
        {
            std::cout << "Hit the Quad! :)\n"
                      << "\tu: " << query.hit.u << "\n"
                      << "\tv: " << query.hit.v << "\n"
                      << "\tt: " << query.ray.tfar << std::endl;
        }

        // Free RTCScene, RTCDevice
        rtcReleaseScene(scene);
        rtcReleaseDevice(device);
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
