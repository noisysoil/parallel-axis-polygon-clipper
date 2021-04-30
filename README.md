# Parallel axis polygon clipper
Efficient polygon clipping with parallel axis boundaries.
A paper about the algorithm is presented as: ['Polygon clipping with parallel axis boundaries.pdf'](https://github.com/noisysoil/parallel-axis-polygon-clipper/blob/main/Polygon%20clipping%20with%20parallel%20axis%20boundaries.pdf)
## Description

ANSI C/99 code implementing an efficient polygon clipper that culls any n-sided convex polygon (arranged in either clockwise or anticlockwise order) to a rectangular window.
It uses the properties of parallel axis boundaries to afford these advantages:
* Fast visible / invisible / clip rejection tests.
* No duplicate vertices or degenerate edges.
* No round-off or disjoint errors when connecting polygons share the same vertices in any orientation, beneficial when using integer math.
* The algorithm can be extended to multiple clip axis beyond two dimensions.

### Code

*Structures*
```C
typedef struct
{
    int16_t x;
    int16_t y;
} vertex;

typedef struct
{
    int16_t clipLeft;
    int16_t clipRight;
    int16_t clipTop;
    int16_t clipBottom;
} clipRect;
```
*Parameters*
* const vertex *sourcePolygon
    * Pointer to vertices of the polygon to be clipped.
    * Vertices are arranged as 'struct vertex'.
* int16_t nVertices
    * Number of vertices in the source polygon.
* const clipRect *clipRectangle
    * Pointer to the clipping rectangle.
    * The rectangle is arranged as 'struct clipRect'.
* vertex *clipBuffer
    * Pointer to the vertex buffer.
    * Vertices are arranged as 'struct vertex'.
    * Size of buffer should be number of vertices in source polygon * 2.
* vertex *outputPolygon
    * Pointer to the output polygon.
    * Vertices are arranged as 'struct vertex'.
    * Size of buffer should be number of vertices in source polygon * 2.

*Returns*
* int16_t
    * Number of vertices in output polygon.
    * If this value is < 3 then polygon is not visible.

To clip a polygon to a rectangular window, the sample C code `TwoAxisPolygonClip.c` can be called as follows:

```C
numVertices = TwoAxisPolyClip(&thePolygon[0], numVertices, &theClipRect, &vertexBuffer[0], &outputPolygon[0]);
```
