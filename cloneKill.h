// Copyright 2019 Andreas Ommundsen
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#ifndef ODDS_CLONEKILL_H
#define ODDS_CLONEKILL_H

#include "odd.h"

/**
 * @param original The ODD to be cloned with a deep copy.
 * @return The cloned ODD whose internal state is distinct in memory from the input.
 */
ODD *cloneODD(ODD *original);

/**
 * @param original The Layer to be cloned with a deep copy.
 * @return The cloned Layer whose internal state is distinct in memory from the input.
 */
Layer *cloneLayer(Layer *original);

/**
 * @param target The ODD whose internal state is to be freed.
 */
void killODD(ODD *target);

/**
 * @param target The Layer whose internal state is to be freed.
 */
void killLayer(Layer *target);

#endif //ODDS_CLONEKILL_H
