#pragma once

void math_triangulation_init();
int math_triangulation_triangulate(float const* verts, int count, int **indices);
