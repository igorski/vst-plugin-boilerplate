/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2022 Igor Zinken - https://www.igorski.nl
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
#include "../global.h"
#include "controller.h"
#include "uimessagecontroller.h"
#include "../paramids.h"

#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/vst/ivstmidicontrollers.h"

#include "base/source/fstring.h"
#include "base/source/fstreamer.h"

#include "vstgui/uidescription/delegationcontroller.h"

#include <stdio.h>
#include <math.h>

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
// PluginController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API PluginController::initialize( FUnknown* context )
{
    tresult result = EditControllerEx1::initialize( context );

    if ( result != kResultOk )
        return result;

    //--- Create Units-------------
    UnitInfo unitInfo;
    Unit* unit;

    // create root only if you want to use the programListId
    /*	unitInfo.id = kRootUnitId;	// always for Root Unit
    unitInfo.parentUnitId = kNoParentUnitId;	// always for Root Unit
    Steinberg::UString (unitInfo.name, USTRINGSIZE (unitInfo.name)).assign (USTRING ("Root"));
    unitInfo.programListId = kNoProgramListId;

    unit = new Unit (unitInfo);
    addUnitInfo (unit);*/

    // create a unit1
    unitInfo.id = 1;
    unitInfo.parentUnitId = kRootUnitId;    // attached to the root unit

    Steinberg::UString( unitInfo.name, USTRINGSIZE( unitInfo.name )).assign( USTRING( "__PLUGIN_NAME__" ));

    unitInfo.programListId = kNoProgramListId;

    unit = new Unit( unitInfo );
    addUnit( unit );
    int32 unitId = unitInfo.id;

    // plugin controls

    parameters.addParameter(
        STR16( "Bypass" ), nullptr, 1, 0, ParameterInfo::kCanAutomate | ParameterInfo::kIsBypass, kBypassId
    );

// --- AUTO-GENERATED START

    RangeParameter* bitDepthParam = new RangeParameter(
        USTRING( "Resolution" ), kBitDepthId, USTRING( "%" ),
        0.f, 1.f, 1.f,
        0, ParameterInfo::kCanAutomate, unitId
    );
    parameters.addParameter( bitDepthParam );

    RangeParameter* bitCrushLfoParam = new RangeParameter(
        USTRING( "Bit crush LFO" ), kBitCrushLfoId, USTRING( "Hz" ),
        0.f, 10.f, 0.f,
        0, ParameterInfo::kCanAutomate, unitId
    );
    parameters.addParameter( bitCrushLfoParam );

    RangeParameter* bitCrushLfoDepthParam = new RangeParameter(
        USTRING( "Bit crush LFO depth" ), kBitCrushLfoDepthId, USTRING( "%" ),
        0.f, 1.f, 0.f,
        0, ParameterInfo::kCanAutomate, unitId
    );
    parameters.addParameter( bitCrushLfoDepthParam );

    RangeParameter* wetMixParam = new RangeParameter(
        USTRING( "Wet mix" ), kWetMixId, USTRING( "%" ),
        0.f, 1.f, 1.f,
        0, ParameterInfo::kCanAutomate, unitId
    );
    parameters.addParameter( wetMixParam );

    RangeParameter* dryMixParam = new RangeParameter(
        USTRING( "Dry mix" ), kDryMixId, USTRING( "%" ),
        0.f, 1.f, 0.f,
        0, ParameterInfo::kCanAutomate, unitId
    );
    parameters.addParameter( dryMixParam );


// --- AUTO-GENERATED END

    // initialization

    String str( "__PLUGIN_NAME__" );
    str.copyTo16( defaultMessageText, 0, 127 );

    return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PluginController::terminate()
{
    return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API PluginController::setComponentState( IBStream* state )
{
    // we receive the current state of the component (processor part)
    if ( !state )
        return kResultFalse;

    IBStreamer streamer( state, kLittleEndian );

    int32 savedBypass = 0;
    if ( streamer.readInt32( savedBypass ) == false )
        return kResultFalse;
    setParamNormalized( kBypassId, savedBypass ? 1 : 0 );

// --- AUTO-GENERATED SETCOMPONENTSTATE START

    float savedBitDepth = 1.f;
    if ( streamer.readFloat( savedBitDepth ) == false )
        return kResultFalse;
    setParamNormalized( kBitDepthId, savedBitDepth );

    float savedBitCrushLfo = 0.f;
    if ( streamer.readFloat( savedBitCrushLfo ) == false )
        return kResultFalse;
    setParamNormalized( kBitCrushLfoId, savedBitCrushLfo );

    float savedBitCrushLfoDepth = 0.f;
    if ( streamer.readFloat( savedBitCrushLfoDepth ) == false )
        return kResultFalse;
    setParamNormalized( kBitCrushLfoDepthId, savedBitCrushLfoDepth );

    float savedWetMix = 1.f;
    if ( streamer.readFloat( savedWetMix ) == false )
        return kResultFalse;
    setParamNormalized( kWetMixId, savedWetMix );

    float savedDryMix = 0.f;
    if ( streamer.readFloat( savedDryMix ) == false )
        return kResultFalse;
    setParamNormalized( kDryMixId, savedDryMix );


// --- AUTO-GENERATED SETCOMPONENTSTATE END

    return kResultOk;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API PluginController::createView( const char* name )
{
    // create the visual editor
    if ( name && strcmp( name, "editor" ) == 0 )
    {
        VST3Editor* view = new VST3Editor( this, "view", "plugin.uidesc" );
        return view;
    }
    return 0;
}

//------------------------------------------------------------------------
IController* PluginController::createSubController( UTF8StringPtr name,
                                                    const IUIDescription* /*description*/,
                                                    VST3Editor* /*editor*/ )
{
    if ( UTF8StringView( name ) == "MessageController" )
    {
        UIMessageController* controller = new UIMessageController( this );
        addUIMessageController( controller );
        return controller;
    }
    return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PluginController::setState( IBStream* state )
{
	IBStreamer streamer( state, kLittleEndian );

	int8 byteOrder;
	if ( streamer.readInt8( byteOrder ) == false )
		return kResultFalse;
	if ( streamer.readRaw( defaultMessageText, 128 * sizeof( TChar )) == false )
		return kResultFalse;

	// if the byteorder doesn't match, byte swap the text array ...
	if ( byteOrder != BYTEORDER ) {
		for ( int32 i = 0; i < 128; i++ ) {
			SWAP_16( defaultMessageText[ i ]);
		}
	}

	// update our editors
	for ( auto & uiMessageController : uiMessageControllers )
		uiMessageController->setMessageText( defaultMessageText );

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PluginController::getState( IBStream* state )
{
	// here we can save UI settings for example
	// as we save a Unicode string, we must know the byteorder when setState is called

	IBStreamer streamer( state, kLittleEndian );

	int8 byteOrder = BYTEORDER;
	if ( streamer.writeInt8( byteOrder ) == false )
		return kResultFalse;

	if ( streamer.writeRaw( defaultMessageText, 128 * sizeof( TChar )) == false )
		return kResultFalse;

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PluginController::receiveText( const char* text )
{
    // received from Component
    if ( text )
    {
        fprintf( stderr, "[PluginController] received: " );
        fprintf( stderr, "%s", text );
        fprintf( stderr, "\n" );
    }
    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PluginController::setParamNormalized( ParamID tag, ParamValue value )
{
    // called from host to update our parameters state
    tresult result = EditControllerEx1::setParamNormalized( tag, value );
    return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PluginController::getParamStringByValue( ParamID tag, ParamValue valueNormalized, String128 string )
{
    char text[32];
    // these controls are floating point values in 0 - 1 range, we can
    // simply read the normalized value which is in the same range
    switch ( tag )
    {
// --- AUTO-GENERATED GETPARAM START

        case kBitDepthId:
            sprintf( text, "%.d Bits", ( int ) ( 15 * valueNormalized ) + 1 );
            Steinberg::UString( string, 128 ).fromAscii( text );
            return kResultTrue;

        case kBitCrushLfoId:
            sprintf( text, "%.2f Hz", normalizedParamToPlain( tag, valueNormalized ));
            Steinberg::UString( string, 128 ).fromAscii( text );
            return kResultTrue;

        case kBitCrushLfoDepthId:
            sprintf( text, "%.2d %%", ( int ) ( valueNormalized * 100.f ));
            Steinberg::UString( string, 128 ).fromAscii( text );
            return kResultTrue;

        case kWetMixId:
            sprintf( text, "%.2d %%", ( int ) ( valueNormalized * 100.f ));
            Steinberg::UString( string, 128 ).fromAscii( text );
            return kResultTrue;

        case kDryMixId:
            sprintf( text, "%.2d %%", ( int ) ( valueNormalized * 100.f ));
            Steinberg::UString( string, 128 ).fromAscii( text );
            return kResultTrue;


// --- AUTO-GENERATED GETPARAM END

        // everything else
        default:
            return EditControllerEx1::getParamStringByValue( tag, valueNormalized, string );
    }
}

//------------------------------------------------------------------------
tresult PLUGIN_API PluginController::getParamValueByString( ParamID tag, TChar* string, ParamValue& valueNormalized )
{
    /* example, but better to use a custom Parameter as seen in RangeParameter
    switch (tag)
    {
        case kAttackId:
        {
            Steinberg::UString wrapper ((TChar*)string, -1); // don't know buffer size here!
            double tmp = 0.0;
            if (wrapper.scanFloat (tmp))
            {
                valueNormalized = expf (logf (10.f) * (float)tmp / 20.f);
                return kResultTrue;
            }
            return kResultFalse;
        }
    }*/
    return EditControllerEx1::getParamValueByString( tag, string, valueNormalized );
}

//------------------------------------------------------------------------
void PluginController::addUIMessageController( UIMessageController* controller )
{
    uiMessageControllers.push_back( controller );
}

//------------------------------------------------------------------------
void PluginController::removeUIMessageController( UIMessageController* controller )
{
    UIMessageControllerList::const_iterator it = std::find(
        uiMessageControllers.begin(), uiMessageControllers.end (), controller
    );
    if ( it != uiMessageControllers.end())
        uiMessageControllers.erase( it );
}

//------------------------------------------------------------------------
void PluginController::setDefaultMessageText( String128 text )
{
    String tmp( text );
    tmp.copyTo16( defaultMessageText, 0, 127 );
}

//------------------------------------------------------------------------
TChar* PluginController::getDefaultMessageText()
{
    return defaultMessageText;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PluginController::queryInterface( const char* iid, void** obj )
{
    QUERY_INTERFACE( iid, obj, IMidiMapping::iid, IMidiMapping );
    return EditControllerEx1::queryInterface( iid, obj );
}

//------------------------------------------------------------------------
tresult PLUGIN_API PluginController::getMidiControllerAssignment( int32 busIndex, int16 /*midiChannel*/,
    CtrlNumber midiControllerNumber, ParamID& tag )
{
    // we support for the Gain parameter all MIDI Channel but only first bus (there is only one!)
/*
    if ( busIndex == 0 && midiControllerNumber == kCtrlVolume )
    {
        tag = kDelayTimeId;
        return kResultTrue;
    }
*/
    return kResultFalse;
}

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
