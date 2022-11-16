/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Igor Zinken - https://www.igorski.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "plugin_process.h"
#include <math.h>
#include "calc.h"

namespace Igorski {

PluginProcess::PluginProcess( int amountOfChannels ) {
    _amountOfChannels = amountOfChannels;

    setDryMix( .5f );
    setWetMix( .5f );

    // create the child processors

    bitCrusher = new BitCrusher( 8, .5f, .5f );
    limiter    = new Limiter( 10.f, 500.f, .6f );

    // will be lazily created in the process function
    _preMixBuffer  = nullptr;
    _postMixBuffer = nullptr;
}

PluginProcess::~PluginProcess() {
    delete bitCrusher;
    delete limiter;
    delete _postMixBuffer;
    delete _preMixBuffer;
}

/* setters */

void PluginProcess::setDryMix( float value ) {
    _dryMix = value;
}

void PluginProcess::setWetMix( float value ) {
    _wetMix = value;
}

bool PluginProcess::setTempo( double tempo, int32 timeSigNumerator, int32 timeSigDenominator )
{
    if ( _tempo == tempo && _timeSigNumerator == timeSigNumerator && _timeSigDenominator == timeSigDenominator ) {
        return false; // no change
    }

    _timeSigNumerator   = timeSigNumerator;
    _timeSigDenominator = timeSigDenominator;
    _tempo              = tempo;

    _fullMeasureDuration = ( 60.f / _tempo ) * _timeSigDenominator; // seconds per measure
    _fullMeasureSamples  = Calc::secondsToBuffer( _fullMeasureDuration ); // samples per measure
    _beatSamples         = ceil( _fullMeasureSamples / _timeSigDenominator ); // samples per beat
    _halfMeasureSamples  = ceil( _fullMeasureSamples / 2 ); // samples per half measure
    _sixteenthSamples    = ceil( _fullMeasureSamples / 16 ); // samples per 16th note

    return true;
}

}
