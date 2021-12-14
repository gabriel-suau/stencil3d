#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define abs(a) ((a) > 0 ? (a) : -(a))
#define min(a,b) ((a > b) ? (b) : (a))
#define SIZEX 5
#define SIZEY 5
#define SIZEZ 5

#define cell(tab, k, i, j) (tab[(k) * SIZEX * SIZEY + (i) * SIZEX + (j)])

void stencil3d(float *restrict a, const float *restrict b) {
  int i, j, k;

  for (k = 1 ; k < SIZEZ - 1 ; k++)
    for (i = 1 ; i < SIZEY - 1 ; i++)
      for (j = 1 ; j < SIZEX - 1 ; j++)
	cell(a, k, i, j) = (12 * cell(b, k, i, j) +
                            cell(b, k, i, j + 1) +
                            cell(b, k, i, j - 1) +
                            cell(b, k, i + 1, j + 1) +
                            cell(b, k, i - 1, j - 1) +
                            cell(b, k + 1, i, j + 1) +
                            cell(b, k + 1, i, j - 1) +
                            cell(b, k + 1, i + 1, j + 1) +
                            cell(b, k + 1, i - 1, j - 1) +
                            cell(b, k - 1, i, j + 1) +
                            cell(b, k - 1, i, j - 1) +
                            cell(b, k - 1, i + 1, j + 1) +
                            cell(b, k - 1, i - 1, j - 1)) / 13;
}

int main() {
  int i, j, k, h;
  float *a, *b, s;

  a = (float *)malloc(sizeof(float) * SIZEX * SIZEY * SIZEZ);
  b = (float *)malloc(sizeof(float) * SIZEX * SIZEY * SIZEZ);
  s = 0;

  /* Initialization */
  for (k = 0 ; k < SIZEZ ; k++)
    for (i = 0 ; i < SIZEY ; i++)
      for (j = 0 ; j < SIZEX ; j++)
	cell(a, k, i, j) = cell(b, k, i, j) = (j + 1.) / ((k + 1) * (i + 1));

  for (i = SIZEY / 4 ; i < SIZEY / 2 ; i++) 
    for (j = SIZEX / 4 ; j < SIZEX / 2 ; j++) 
      cell(b, 0, i, j) = cell(a, 0, i, j) = 1;

  for(h = 0 ; h < 100 ; h++) {
    stencil3d(a, b);
    for (k = 0 ; k < SIZEZ ; k++)    
      for (i = 0 ; i < SIZEY ; i++)    
        for (j = 0 ; j < SIZEX ; j++)    
          s += cell(a, k, i, j);
    stencil3d(b, a);
    fprintf(stderr,".");
  }

  fprintf(stderr,"%f\n",s);

  free(a);
  free(b);

  return 0;
}
