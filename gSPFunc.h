
#ifndef __GSPFUNC_H__
#define __GSPFUNC_H__

#include "OpenGL.h"
#include "DepthBuffer.h"

#define gSPFlushTriangles() \
    if ((RSP.nextCmd != G_TRI1) && \
        (RSP.nextCmd != G_TRI2) && \
        (RSP.nextCmd != G_TRI4) && \
        (RSP.nextCmd != G_QUAD))\
        OGL_DrawTriangles()

inline static void gSPTriangle(const s32 v0, const s32 v1, const s32 v2)
{
    if ((v0 < 80) && (v1 < 80) && (v2 < 80))
    {
        // Don't bother with triangles completely outside clipping frustrum
        if ((OGL.enableClipping && (((gSP.vertices[v0].xClip != 0) &&
            (gSP.vertices[v0].xClip == gSP.vertices[v1].xClip) &&
            (gSP.vertices[v1].xClip == gSP.vertices[v2].xClip)) ||
           ((gSP.vertices[v0].yClip != 0) &&
            (gSP.vertices[v0].yClip == gSP.vertices[v1].yClip) &&
            (gSP.vertices[v1].yClip == gSP.vertices[v2].yClip)) ||
           ((gSP.vertices[v0].zClip != 0) &&
            (gSP.vertices[v0].zClip == gSP.vertices[v1].zClip) &&
            (gSP.vertices[v1].zClip == gSP.vertices[v2].zClip)))))
        {
            return;
        }
        OGL_AddTriangle(v0, v1, v2);
    }

#ifdef CHECK_DEPTH
    if (depthBuffer.current) depthBuffer.current->cleared = FALSE;
    gDP.colorImage.changed = TRUE;
    gDP.colorImage.height = (unsigned int)(max( gDP.colorImage.height, gDP.scissor.lry ));
#endif
}

inline void gSP1Triangle( const s32 v0, const s32 v1, const s32 v2)
{
    gSPTriangle( v0, v1, v2);
    gSPFlushTriangles();
}

inline void gSP2Triangles(const s32 v00, const s32 v01, const s32 v02, const s32 flag0,
                    const s32 v10, const s32 v11, const s32 v12, const s32 flag1 )
{
    gSPTriangle( v00, v01, v02);
    gSPTriangle( v10, v11, v12);
    gSPFlushTriangles();
}

inline void gSP4Triangles(const s32 v00, const s32 v01, const s32 v02,
                    const s32 v10, const s32 v11, const s32 v12,
                    const s32 v20, const s32 v21, const s32 v22,
                    const s32 v30, const s32 v31, const s32 v32 )
{
    gSPTriangle( v00, v01, v02);
    gSPTriangle( v10, v11, v12);
    gSPTriangle( v20, v21, v22);
    gSPTriangle( v30, v31, v32);
    gSPFlushTriangles();
}

#endif
