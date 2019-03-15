// Copyright 2019 Andreas Ommundsen
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#ifndef ODDS_CLONEKILL_H
#define ODDS_CLONEKILL_H

#include "odd.h"

/**
 * @param input The ODD to be cloned using deep copy.
 * @return The cloned ODD whose internal state is distinct in memory from the input.
 */
ODD *cloneODD(ODD *input);

/**
 * @param input The Layer to be cloned with a deep copy.
 * @return The cloned Layer whose internal state is distinct in memory from the input.
 */
Layer *cloneLayer(Layer *input);

#endif //ODDS_CLONEKILL_H
