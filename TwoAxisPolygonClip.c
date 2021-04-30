/*
Copyright (C) 2003 Jose Commins <axora@axora.net>.
        Revised April 2021.

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "TwoAxisPolygonClip.h"



int16_t TwoAxisPolygonClip(const vertex* sourcePolygon, int16_t nVertices, const clipRect* clipRectangle, vertex *clipBuffer, vertex *clippedPolygon)
{

    /* A general two-pass, two-axis polygon clipping routine.
    By Jose Commins <axora@axora.net>


  Description:
          An efficient two-pass, two-axis polygon clipper that culls to a rectangular region any
        convex polygon arranged in either clockwise or anticlockwise order.  It uses the properties
        of axis boundaries to afford these advantages:
           - Fast visibility/clip tests.
           - No roundoff or disjoint errors when connecting polygons share the same vertices in any
             orientation, which is beneficial when using integer math.
           - No duplicate vertices or degenerate edges.
           - Can be easily extended beyond two dimensions.

   PARAMETERS:
        const vertex *sourcePolygon       : Pointer to vertices of the polygon to be clipped.
                Vertices are arranged as 'struct vertex'.
        int16_t nVertices                 : Number of vertices in the source polygon.
        const clipRect *clipRectangle     : Pointer to the clipping rectangle.
                The rectangle is arranged as 'struct clipRect'.
        vertex *clipBuffer                : Pointer to the vertex buffer.
                Vertices are arranged as 'struct vertex'.
                Size of buffer should be number of vertices in source polygon * 2.
        vertex *outputPolygon             : Pointer to the output polygon.
                Vertices are arranged as 'struct vertex'.
                Size of buffer should be number of vertices in source polygon * 2.
    RETURNS:
        int16_t                           : Number of vertices in output polygon.
                If this value is < 3 then polygon is not visible.


An example on how to call the clipper:
    numVertices = TwoAxisPolyClip(&thePolygon[0], numVertices, &theClipRect, &vertexBuffer[0], &outputPolygon[0]);

*/

    int16_t pIndex;
    int16_t ox1, oy1, clip_left, clip_right, clip_bottom, clip_top;
    int16_t rx1, ry1, rx2, ry2;

    // Initialise a few vars - this is mostly done for register optimisations.
    clip_left = clipRectangle->clipLeft;
    clip_right = clipRectangle->clipRight;
    clip_bottom = clipRectangle->clipBottom;
    clip_top = clipRectangle->clipTop;

    // Initialise the destination output polygon index to zero - at the end of the pass this holds how many vertices the pass has produced.
    pIndex = 0;
    // Start by first and last points of polygon.
    rx1 = sourcePolygon[0].x;
    ry1 = sourcePolygon[0].y;

    // Clip the polygon to the X axis.
    while (nVertices--) {
        rx2 = sourcePolygon[nVertices].x;
        ry2 = sourcePolygon[nVertices].y;
        ox1 = rx2;
        oy1 = ry2;

        // Check for line orientation and deal with the lines accordingly.
        if (rx1 > rx2) {
            // We are in right-to-left orientation - perform a quick bounds check for lines completely outside the clip boundary.
            if (rx1 < clip_left || rx2 > clip_right)
                goto lr_boundOut;

            // Test if the line's rightmost coordinate crosses the right-side clip boundary; if so, clip.
            if (rx1 > clip_right) {
                ry1 = ry1 + ((ry2 - ry1) * (clip_right - rx1)) / (rx2 - rx1);
                rx1 = clip_right;
            }

            // Store the line's rightmost coordinate (clipped or unclipped).
            clipBuffer[pIndex].x = rx1;
            clipBuffer[pIndex++].y = ry1;

            // Test if the line's leftmost coordinate crosses the left-side clip boundary; if so, clip and add the new polygon edge.
            if (rx2 < clip_left) {
                clipBuffer[pIndex].y = ry2 + ((ry1 - ry2) * (clip_left - rx2)) / (rx1 - rx2);
                clipBuffer[pIndex++].x = clip_left;
            }
        } else {
            // Same as above, but now handling left-to-right orientation.
            if (rx2 < clip_left || rx1 > clip_right)
                goto lr_boundOut;

            if (rx1 < clip_left) {
                ry1 = ry1 + ((ry2 - ry1) * (clip_left - rx1)) / (rx2 - rx1);
                rx1 = clip_left;
            }

            clipBuffer[pIndex].x = rx1;
            clipBuffer[pIndex++].y = ry1;

            if (rx2 > clip_right) {
                clipBuffer[pIndex].y = ry2 + ((ry1 - ry2) * (clip_right - rx2)) / (rx1 - rx2);
                clipBuffer[pIndex++].x = clip_right;
            }
        }

    lr_boundOut:
        rx1 = ox1;
        ry1 = oy1;
    }

    // Now clip to another axis - same as above, here using the Y axis to clip to.

    // Exit if the left-right axis clipped out the polygon entirely.
    if (pIndex < 3)
        return (pIndex);

    // Previous clip may return a different number of vertices; use this amount for the next clip pass.
    nVertices = pIndex;

    rx1 = clipBuffer[0].x;
    ry1 = clipBuffer[0].y;
    pIndex = 0;

    while (nVertices--) {
        rx2 = clipBuffer[nVertices].x;
        ry2 = clipBuffer[nVertices].y;
        ox1 = rx2;
        oy1 = ry2;

        if (ry1 > ry2) {
            if (ry1 < clip_top || ry2 > clip_bottom)
                goto tb_boundOut;

            if (ry1 > clip_bottom) {
                rx1 = rx1 + ((rx2 - rx1) * (clip_bottom - ry1)) / (ry2 - ry1);
                ry1 = clip_bottom;
            }

            clippedPolygon[pIndex].x = rx1;
            clippedPolygon[pIndex++].y = ry1;

            if (ry2 < clip_top) {
                clippedPolygon[pIndex].x = rx2 + ((rx1 - rx2) * (clip_top - ry2)) / (ry1 - ry2);
                clippedPolygon[pIndex++].y = clip_top;
            }
        } else {
            if (ry2 < clip_top || ry1 > clip_bottom)
                goto tb_boundOut;

            if (ry1 < clip_top) {
                rx1 = rx1 + ((rx2 - rx1) * (clip_top - ry1)) / (ry2 - ry1);
                ry1 = clip_top;
            }

            clippedPolygon[pIndex].x = rx1;
            clippedPolygon[pIndex++].y = ry1;

            if (ry2 > clip_bottom) {
                clippedPolygon[pIndex].x = rx2 + ((rx1 - rx2) * (clip_bottom - ry2)) / (ry1 - ry2);
                clippedPolygon[pIndex++].y = clip_bottom;
            }
        }

    tb_boundOut:
        rx1 = ox1;
        ry1 = oy1;
    }

    return (pIndex);
}
