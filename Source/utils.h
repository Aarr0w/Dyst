/*
  ==============================================================================

    utils.h
    Created: 22 May 2023 1:37:59pm
    Author:  Silver

  ==============================================================================
*/

#pragma once

float Lerp(float start, float end, float t)
{
    return (1 - t)*start + (t * end);
}

float beziLerp(float start, float end, float cntrl, float t)
{
    auto L0 = (1 - t) * start + (cntrl * t);
    auto L1 = (1 - t) * cntrl + (end * t);
    auto Q = (1 - t) * L0 + (L1 * t);
    return Q;
};

float exp_interpolation(float start, float end, float t)
{
    return pow(start*(end / start) ,  (t));
}

float delta_m(float a, float b)
{
    return abs(abs(a) - abs(b));
}

float sign(float value) {
    if (value < 0) {
        return -1.f;
    }
    else if (value > 0) {
        return 1.f;
    }
    else {
        return 0.f;
    }
}