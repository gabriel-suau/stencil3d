#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* #define abs(a) ((a) > 0 ? (a) : -(a)) */
/* #define min(a,b) ((a > b) ? (b) : (a)) */

#define INV_THIRTEEN 0.07692307692307692307692

#define SIZEX 130
#define SIZEY 130
#define SIZEZ 130

#define TILE_SIZE 16

#define SCALAR float
/* #define SCALAR double */

#define cell(tab, k, i, j) (tab[(k) * SIZEX * SIZEY + (i) * SIZEX + (j)])


/**
 * \param[out] a array of values to be computed
 * \param[in] b array of values to compute a with
 *
 * Base implementation of the stencil3d kernel. Performances are
 * expected to be bad.
 */
void stencil3d_base(SCALAR *restrict a, const SCALAR *restrict b);

/**
 * \param[out] a array of values to be computed
 * \param[in] b array of values to compute a with
 *
 * Implementation of the stencil3d kernel with loop reordering, in order to
 * optimize data locality and reduce cache misses.
 */
void stencil3d_inv_loop(SCALAR *restrict a, const SCALAR *restrict b);

/**
 * \param[out] a array of values to be computed
 * \param[in] b array of values to compute a with
 *
 * Same implementation as \a stencil3d_inv_loop, but the division by 13 is
 * replace by the multiplication of 1/13 (which value is known at compile time).
 */
void stencil3d_inv_loop_onediv(SCALAR *restrict a, const SCALAR *restrict b);

/**
 * \param[out] a array of values to be computed
 * \param[in] b array of values to compute a with
 *
 * Tiled implementation of \a stencil3d_inv_loop_onediv. The tile size is
 * chosen so that the whole tile can fit in the L2-cache
 */
void stencil3d_inv_loop_onediv_tiled(SCALAR *restrict a, const SCALAR *restrict b);

/**
 * Function pointer to handle the user's kernel choice at runtime.
 */
static void (*stencil3d)(SCALAR *restrict, const SCALAR *restrict) = NULL;

int main(int argc, char **argv) {
  int i, j, k, h;
  SCALAR *a, *b, s;
  char *kernel;

  /* Read command-line arguments */
  if (argc != 2) {
    fprintf(stdout, "Usage : %s kernel\n", argv[0]);
    fprintf(stdout, "Available kernels : base inv_loop inv_loop_onediv\n");
    exit(EXIT_SUCCESS);
  }

  kernel = (char *) calloc(strlen(argv[1]) + 1, sizeof(char));
  if (kernel == NULL) {
    fprintf(stderr, "Memory problem : calloc\n");
    exit(EXIT_FAILURE);
  }
  strcpy(kernel, argv[1]);

  /* Set the kernel pointer */
  if (!strcmp(kernel, "base")) {
    stencil3d = stencil3d_base;
  }
  else if (!strcmp(kernel, "inv_loop")) {
    stencil3d = stencil3d_inv_loop;
  }
  else if (!strcmp(kernel, "inv_loop_onediv")) {
    stencil3d = stencil3d_inv_loop_onediv;
  }
  else if (!strcmp(kernel, "inv_loop_onediv_tiled")) {
    stencil3d = stencil3d_inv_loop_onediv_tiled;
  }
  else {
    fprintf(stderr, "Error : %s : no corresponding kernel.\n", kernel);
    exit(EXIT_FAILURE);
  }

  a = (SCALAR *)malloc(sizeof(SCALAR) * SIZEX * SIZEY * SIZEZ);
  b = (SCALAR *)malloc(sizeof(SCALAR) * SIZEX * SIZEY * SIZEZ);
  s = 0;

  /* Initialization */
  for (k = 0 ; k < SIZEZ ; k++)
    for (i = 0 ; i < SIZEY ; i++)
      for (j = 0 ; j < SIZEX ; j++)
        cell(a, k, i, j) = cell(b, k, i, j) = (j + 1.) / ((k + 1) * (i + 1));

  for (i = SIZEY / 4 ; i < SIZEY / 2 ; i++)
    for (j = SIZEX / 4 ; j < SIZEX / 2 ; j++)
      cell(b, 0, i, j) = cell(a, 0, i, j) = 1;

  /* Computation loop */
  for (h = 0 ; h < 100 ; h++) {
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
  free(kernel);

  return 0;
}


void stencil3d_base(SCALAR *restrict a, const SCALAR *restrict b) {
  int i, j, k;

  for (j = 1 ; j < SIZEY - 1 ; j++)
    for (i = 1 ; i < SIZEX - 1 ; i++)
      for (k = 1 ; k < SIZEZ - 1 ; k++)
	a[k * SIZEY * SIZEX + i * SIZEY + j] = (12 * b[k * SIZEY * SIZEX + i * SIZEY + j] +
                                                b[k * SIZEY * SIZEX + i * SIZEY + j + 1] +
                                                b[k * SIZEY * SIZEX + i * SIZEY + j - 1] +
                                                b[k * SIZEY * SIZEX + (i + 1) * SIZEY + j + 1] +
                                                b[k * SIZEY * SIZEX + (i - 1) * SIZEY + j - 1] +
                                                b[(k + 1) * SIZEY * SIZEX + i * SIZEY + j + 1] +
                                                b[(k + 1) * SIZEY * SIZEX + i * SIZEY + j - 1] +
                                                b[(k + 1) * SIZEY * SIZEX + (i + 1) * SIZEY + j + 1] +
                                                b[(k + 1) * SIZEY * SIZEX + (i - 1) * SIZEY + j - 1] +
                                                b[(k - 1) * SIZEY * SIZEX + i * SIZEY + j + 1] +
                                                b[(k - 1) * SIZEY * SIZEX + i * SIZEY + j - 1] +
                                                b[(k - 1) * SIZEY * SIZEX + (i + 1) * SIZEY + j + 1] +
                                                b[(k - 1) * SIZEY * SIZEX + (i - 1) * SIZEY + j - 1]) / 13;
}


void stencil3d_inv_loop(SCALAR *restrict a, const SCALAR *restrict b) {
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


void stencil3d_inv_loop_onediv(SCALAR *restrict a, const SCALAR *restrict b) {
  int i, j, k;

  for (k = 1 ; k < SIZEZ - 1 ; k++)
    for (i = 1 ; i < SIZEY - 1 ; i++)
/* #pragma GCC unroll 4 */
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
                            cell(b, k - 1, i - 1, j - 1)) * INV_THIRTEEN;
}


static inline void stencil3d_do_tile(SCALAR *restrict a, const SCALAR *restrict b) {
  int i, j, k;

  for (k = 0 ; k < TILE_SIZE ; k++)
    for (i = 0 ; i < TILE_SIZE ; i++)
      for (j = 0 ; j < TILE_SIZE ; j++)
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
                            cell(b, k - 1, i - 1, j - 1)) * INV_THIRTEEN;        
}


void stencil3d_inv_loop_onediv_tiled(SCALAR *restrict a, const SCALAR *restrict b) {
  int i, j, k;

  for (k = 1 ; k < SIZEZ - 1 ; k += TILE_SIZE)
    for (i = 1 ; i < SIZEY - 1 ; i += TILE_SIZE)
      for (j = 1 ; j < SIZEX - 1 ; j += TILE_SIZE)
        stencil3d_do_tile(&cell(a, k, i, j), &cell(b, k, i, j));
}
