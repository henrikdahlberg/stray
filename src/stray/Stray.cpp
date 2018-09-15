#include "Math.h"
#include "Kernels.h"

#include <embree3/rtcore.h>

#include <iostream>
#include <fstream>
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

static void
debugPrintHitResult(RTCRayHit& query)
{
    if (query.hit.geomID == RTC_INVALID_GEOMETRY_ID)
    {
        std::cout << "Did not hit the Quad! :(" << std::endl;
    }
    else
    {
        std::cout << "Hit Quad " << query.hit.primID << "! :)\n"
                  << "\tu:      "  << query.hit.u << "\n"
                  << "\tv:      "  << query.hit.v << "\n"
                  << "\tgeomID: "  << query.hit.geomID << "\n"
                  << "\tprimID: "  << query.hit.primID << "\n";
        if (query.hit.instID)
        {
        std::cout << "\tinstID: "  << query.hit.instID << "\n";
        }
        std::cout << "\tNg:     (" << query.hit.Ng_x << ", "
                                   << query.hit.Ng_y << ", "
                                   << query.hit.Ng_z << ")\n"
                  << "\tt:      "  << query.ray.tfar << std::endl;
    }
}

static void
initRTCRayHit(RTCRayHit& query,
              Vec3f&     org,
              Vec3f&     dir)
{
    query.ray.org_x  = org.x;
    query.ray.org_y  = org.y;
    query.ray.org_z  = org.z;
    query.ray.dir_x  = dir.x;
    query.ray.dir_y  = dir.y;
    query.ray.dir_z  = dir.z;
    query.ray.tnear  = EPS;
    query.ray.tfar   = MAX;
    query.ray.time   = 0.0f;
    query.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    query.hit.primID = RTC_INVALID_GEOMETRY_ID;
}

int main(const int argc, const char* argv[])
{
    try
    {
        // Dummy ISPC command
        int N = 16;
        float v[N];
        render(v, N);

        // Make an open box
        std::vector<Vec3f> vertexBuffer{{ 0.5f, -0.5f, -1.0f}, // bottom vertices
                                        { 0.5f, -0.5f, -2.0f},
                                        {-0.5f, -0.5f, -2.0f},
                                        {-0.5f, -0.5f, -1.0f},
                                        { 0.5f,  0.5f, -1.0f}, // top vertices
                                        { 0.5f,  0.5f, -2.0f},
                                        {-0.5f,  0.5f, -2.0f},
                                        {-0.5f,  0.5f, -1.0f}};
        std::vector<int> indexBuffer{0, 1, 2, 3,  // floor
                                     4, 7, 6, 5,  // ceiling
                                     0, 4, 5, 1,  // right wall
                                     1, 5, 6, 2,  // back wall
                                     3, 2, 6, 7}; // left wall

        // Init RTCDevice, RTCScene
        RTCDevice device = rtcNewDevice(0x0);
        RTCScene scene   = rtcNewScene(device);

        // Create quad mesh
        RTCGeometry geom = rtcNewGeometry(device,
                                          RTC_GEOMETRY_TYPE_QUAD);


        // Share geometry buffers
        rtcSetSharedGeometryBuffer(geom,
                                   RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
                                   vertexBuffer.data(), 0, sizeof(Vec3f), vertexBuffer.size());
        rtcSetSharedGeometryBuffer(geom,
                                   RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT4,
                                   indexBuffer.data(), 0, sizeof(Quad), indexBuffer.size());

        // Commit RTCGeometry
        rtcCommitGeometry(geom);

        // Attach RTCGeometry to RTCScene
        rtcAttachGeometryByID(scene, geom, 0);

        // Commit RTCScene
        rtcCommitScene(scene);


        // Render something!
        // Redirect output to file
        std::streambuf* coutbuf = std::cout.rdbuf();
        std::ofstream img("Normals.ppm");
        std::cout.rdbuf(img.rdbuf());


        // Make a camera, assume 45 degrees fov when generating cam rays
        Vec3f camorg{0.0f, 0.0f, 1.2f};
        Vec3f camdir{0.0f, 0.0f, -1.0f};
        Vec3f camright = camdir.cross(Vec3f(0.0f, 1.0f, 0.0f)).normalize();
        Vec3f camup = camright.cross(camdir);

        // Trace rays and compute colors
        const float aspectRatio = (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH;
        Vec3f bottomleft = camorg + 1.0f*camdir
                                  - 0.5f*aspectRatio*camup
                                  - 0.5f*camright;
        RTCIntersectContext rctx;
        rtcInitIntersectContext(&rctx);
        RTCRayHit query;


        std::cout << "P3\n" << SCREEN_WIDTH << " " << SCREEN_HEIGHT << "\n255\n";
        for (int y = SCREEN_HEIGHT-1; y >= 0; --y)
        {
            for (int x = 0; x < SCREEN_WIDTH; ++x)
            {
                float sx = (float)x/(float)SCREEN_WIDTH;
                float sy = (float)y/(float)SCREEN_HEIGHT;

                // Trace a ray!
                Vec3f camraydir = (bottomleft + Vec3f(sx, aspectRatio*sy, 0.0f) - camorg).normalize();
                initRTCRayHit(query, camorg, camraydir);

                rtcIntersect1(scene,
                              &rctx,
                              &query);
                Vec3f col;
                if (query.hit.geomID == RTC_INVALID_GEOMETRY_ID)
                {
                    col = Vec3f(0.0f);
                }
                else
                {
                    col = 0.5f*Vec3f(query.hit.Ng_x,
                                     query.hit.Ng_y,
                                     query.hit.Ng_z) +
                          Vec3f(0.5f);
                }

                int ir = int(255.9*col.x);
                int ig = int(255.9*col.y);
                int ib = int(255.9*col.z);

                std::cout << ir << " " << ig << " " << ib << "\n";
            }
        }

        // Redirect output back to where it was
        std::cout.rdbuf(coutbuf);

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
