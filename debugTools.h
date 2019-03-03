// Copyright 2019 Andreas Ommundsen
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#ifndef ODDS_DEBUGTOOLS_H
#define ODDS_DEBUGTOOLS_H

/*
 * Prints the all the layers and their respective states of the parameter ODD to stdout.
 */
void showODD(ODD *odd);

/*
 * Prints any given layer and its respective state to stdout.
 */
void showLayer(Layer *layer);

#endif //ODDS_DEBUGTOOLS_H
