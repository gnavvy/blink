#ifndef CONSTANTS_H
#define CONSTANTS_H

const std::string kWindowTitle = "Capture - Face detection";

// Debugging
const bool kPlotVectorField = false;

// Size constants
const float kEyeTop      = .30;
const float kEyeSide     = .15;
const float kEyeHeight   = .20;
const float kEyeWidth    = .30;

const bool kUseCascade = false;

// Algorithm Parameters
const int kFastEyeWidth = 50;
const int kWeightBlurSize = 5;
const bool kEnableWeight = false;
const float kWeightDivisor = 150.0;
const double kGradientThreshold = 50.0;

// Postprocessing
const bool kEnablePostProcess = false;
const float kPostProcessThreshold = 0.97;

#endif // CONSTANTS_H
