#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* #define abs(a) ((a) > 0 ? (a) : -(a)) */
/* #define min(a,b) ((a > b) ? (b) : (a)) */

#define SIZEX 128
#define SIZEY 128
#define SIZEZ 128

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
 * only computed once at the beginning of the function.
 */
void stencil3d_inv_loop_onediv(SCALAR *restrict a, const SCALAR *restrict b);

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
  else {
    fprintf(stderr, "Error : %s : no corresponding kernel.\n", kernel);
    exit(EXIT_FAILURE);
  }

  a = (SCALAR *)malloc(sizeof(SCALAR) * SIZEX * SIZEY * SIZEZ);
  b = (SCALAR *)malloc(sizeof(SCALAR) * SIZEX * SIZEY * SIZEZ);
  s = 0;

  /* Initialization */
  for (i = 0 ; i < SIZEX ; i++)    
    for (j = 0 ; j < SIZEY ; j++)    
      for (k = 0 ; k < SIZEZ ; k++)    
	a[k * SIZEY * SIZEX + i * SIZEY + j] = b[k * SIZEY * SIZEX + i * SIZEY + j] = (j + 1.) / ((k + 1) * (i + 1));

  for (j = SIZEY / 4 ; j < SIZEY / 2 ; j++) 
    for (i = SIZEX / 4 ; i < SIZEX / 2 ; i++) 
      b[i * SIZEY + j] = a[i * SIZEY + j] = 1;

  /* Computation loop */
  for(h = 0 ; h < 100 ; h++) {
    stencil3d(a, b);
    for (i = 0 ; i < SIZEX ; i++)    
      for (j = 0 ; j < SIZEY ; j++)    
        for (k = 0 ; k < SIZEZ ; k++)    
          s += a[k * SIZEY * SIZEX + i * SIZEY + j];
    stencil3d(b, a);
    fprintf(stderr,".");
  }

  fprintf(stderr,"%f\n",s);

  free(a);
  free(b);

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
  SCALAR inv_thirteen = 1.0 / 13;

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
                            cell(b, k - 1, i - 1, j - 1)) * inv_thirteen;
}
