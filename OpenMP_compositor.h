#ifndef ALPHA_COMPOSITION_OPENMP_COMPOSITOR_H
#define ALPHA_COMPOSITION_OPENMP_COMPOSITOR_H
#include "image.h"

bool OpenMP_compose(const Image &foreground, Image &background);

#endif //ALPHA_COMPOSITION_OPENMP_COMPOSITOR_H
