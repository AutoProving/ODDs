// Copyright 2019 Andreas Ommundsen
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#ifndef ODDS_DEBUGTOOLS_H
#define ODDS_DEBUGTOOLS_H

#include "odd.h"

/**
 * @param odd The input *odd, eg: &odd.
 * @Effect Prints the ODD info along with each layer's state to stdout.
 */
void showODD(ODD *odd);

/**
 * @param layer The input *layer, eg: &odd.layerSequence[i].
 * @Effect Prints the input layer's state to stdout.
 */
void showLayer(Layer *layer);

#endif //ODDS_DEBUGTOOLS_H
