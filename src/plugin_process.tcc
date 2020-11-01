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
namespace Igorski
{
template <typename SampleType>
void PluginProcess::process( SampleType** inBuffer, SampleType** outBuffer, int numInChannels, int numOutChannels,
                             int bufferSize, uint32 sampleFramesSize ) {

    // input and output buffers can be float or double as defined
    // by the templates SampleType value. Internally we process
    // audio as floats

    SampleType inSample;
    int i, readIndex;

    bool mixDry = _dryMix != 0.f;

    SampleType dryMix = ( SampleType ) _dryMix;
    SampleType wetMix = ( SampleType ) _wetMix;

    prepareMixBuffers( inBuffer, numInChannels, bufferSize );

    for ( int32 c = 0; c < numInChannels; ++c )
    {
        SampleType* channelInBuffer  = inBuffer[ c ];
        SampleType* channelOutBuffer = outBuffer[ c ];
        float* channelPreMixBuffer   = _preMixBuffer->getBufferForChannel( c );
        float* channelPostMixBuffer  = _postMixBuffer->getBufferForChannel( c );

        // example processing: apply some bit crushing onto the premix buffer
        bitCrusher->process( channelPreMixBuffer, bufferSize );

        // POST MIX processing
        // apply the post mix effect processing

        // this is just cloning the pre mix buffer so we can hear output in this example
        for ( i = 0; i < bufferSize; ++i ) {
            channelPostMixBuffer[ i ] = ( float ) channelPreMixBuffer[ i ];
        }

        // mix the input and processed post mix buffers into the output buffer

        for ( i = 0; i < bufferSize; ++i ) {

            // before writing to the out buffer we take a snapshot of the current in sample
            // value as VST2 in Ableton Live supplies the same buffer for inBuffer and outBuffer!
            inSample = channelInBuffer[ i ];

            // wet mix (e.g. the effected signal)
            channelOutBuffer[ i ] = ( SampleType ) channelPostMixBuffer[ i ] * wetMix;

            // dry mix (e.g. mix in the input signal)
            if ( mixDry ) {
                channelOutBuffer[ i ] += ( inSample * dryMix );
            }
        }
    }

    // limit the output signal in case its gets hot
    //limiter->process<SampleType>( outBuffer, bufferSize, numOutChannels );
}

template <typename SampleType>
void PluginProcess::prepareMixBuffers( SampleType** inBuffer, int numInChannels, int bufferSize )
{
    // if the pre mix buffer wasn't created yet or the buffer size has changed
    // delete existing buffer and create new one to match properties

    if ( _preMixBuffer == nullptr || _preMixBuffer->bufferSize != bufferSize ) {
        delete _preMixBuffer;
        _preMixBuffer = new AudioBuffer( numInChannels, bufferSize );
    }

    // clone the in buffer contents
    // note the clone is always cast to float as it is
    // used for internal processing (see PluginProcess::process)

    for ( int c = 0; c < numInChannels; ++c ) {
        SampleType* inChannelBuffer = ( SampleType* ) inBuffer[ c ];
        float* outChannelBuffer     =  _preMixBuffer->getBufferForChannel( c );

        for ( int i = 0; i < bufferSize; ++i ) {
            outChannelBuffer[ i ] = ( float ) inChannelBuffer[ i ];
        }
    }

    // if the post mix buffer wasn't created yet or the buffer size has changed
    // delete existing buffer and create new one to match properties

    if ( _postMixBuffer == nullptr || _postMixBuffer->bufferSize != bufferSize ) {
        delete _postMixBuffer;
        _postMixBuffer = new AudioBuffer( numInChannels, bufferSize );
    }
}

}
