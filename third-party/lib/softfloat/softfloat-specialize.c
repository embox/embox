/*============================================================================

This C source fragment is part of the SoftFloat IEC/IEEE Floating-point
Arithmetic Package, Release 2b.

Written by John R. Hauser.  This work was made possible in part by the
International Computer Science Institute, located at Suite 600, 1947 Center
Street, Berkeley, California 94704.  Funding was partially provided by the
National Science Foundation under grant MIP-9311980.  The original version
of this code was written as part of a project to build a fixed-point vector
processor in collaboration with the University of California at Berkeley,
overseen by Profs. Nelson Morgan and John Wawrzynek.  More information
is available through the Web page `http://www.cs.berkeley.edu/~jhauser/
arithmetic/SoftFloat.html'.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort has
been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT TIMES
RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO PERSONS
AND ORGANIZATIONS WHO CAN AND WILL TAKE FULL RESPONSIBILITY FOR ALL LOSSES,
COSTS, OR OTHER PROBLEMS THEY INCUR DUE TO THE SOFTWARE, AND WHO FURTHERMORE
EFFECTIVELY INDEMNIFY JOHN HAUSER AND THE INTERNATIONAL COMPUTER SCIENCE
INSTITUTE (possibly via similar legal warning) AGAINST ALL LOSSES, COSTS, OR
OTHER PROBLEMS INCURRED BY THEIR CUSTOMERS AND CLIENTS DUE TO THE SOFTWARE.

Derivative works are acceptable, even for commercial purposes, so long as
(1) the source code for the derivative work includes prominent notice that
the work is derivative, and (2) the source code includes prominent notice with
these four paragraphs for those parts of this code that are retained.

=============================================================================*/

#include <lib/softfloat.h>

/*----------------------------------------------------------------------------
| Underflow tininess-detection mode, statically initialized to default value.
| (The declaration in `softfloat.h' must match the `int8' type here.)
*----------------------------------------------------------------------------*/
int8 float_detect_tininess = float_tininess_after_rounding;

/*----------------------------------------------------------------------------
| Raises the exceptions specified by `flags'.  Floating-point traps can be
| defined here if desired.  It is currently not possible for such a trap to
| substitute a result value.  If traps are not implemented, this routine
| should be simply `float_exception_flags |= flags;'.
*----------------------------------------------------------------------------*/

void float_raise( int8 flags )
{

    float_exception_flags |= flags;

}

/*----------------------------------------------------------------------------
| Returns 1 if the single-precision floating-point value `a' is a NaN;
| otherwise returns 0.
*----------------------------------------------------------------------------*/

flag float32_is_nan( float32 a )
{

    return ( 0xFF000000 < (bits32) ( a<<1 ) );

}

/*----------------------------------------------------------------------------
| Returns 1 if the single-precision floating-point value `a' is a signaling
| NaN; otherwise returns 0.
*----------------------------------------------------------------------------*/

flag float32_is_signaling_nan( float32 a )
{

    return ( ( ( a>>22 ) & 0x1FF ) == 0x1FE ) && ( a & 0x003FFFFF );

}

/*----------------------------------------------------------------------------
| Returns the result of converting the single-precision floating-point NaN
| `a' to the canonical NaN format.  If `a' is a signaling NaN, the invalid
| exception is raised.
*----------------------------------------------------------------------------*/

commonNaNT float32ToCommonNaN( float32 a )
{
    commonNaNT z;

    if ( float32_is_signaling_nan( a ) ) float_raise( float_flag_invalid );
    z.sign = a>>31;
    z.low = 0;
    z.high = ( (bits64) a )<<41;
    return z;

}

/*----------------------------------------------------------------------------
| Returns the result of converting the canonical NaN `a' to the single-
| precision floating-point format.
*----------------------------------------------------------------------------*/

float32 commonNaNToFloat32( commonNaNT a )
{

    return ( ( (bits32) a.sign )<<31 ) | 0x7FC00000 | ( a.high>>41 );

}

/*----------------------------------------------------------------------------
| Takes two single-precision floating-point values `a' and `b', one of which
| is a NaN, and returns the appropriate NaN result.  If either `a' or `b' is a
| signaling NaN, the invalid exception is raised.
*----------------------------------------------------------------------------*/

float32 propagateFloat32NaN( float32 a, float32 b )
{
    flag aIsNaN, aIsSignalingNaN, bIsNaN, bIsSignalingNaN;

    aIsNaN = float32_is_nan( a );
    aIsSignalingNaN = float32_is_signaling_nan( a );
    bIsNaN = float32_is_nan( b );
    bIsSignalingNaN = float32_is_signaling_nan( b );
    a |= 0x00400000;
    b |= 0x00400000;
    if ( aIsSignalingNaN | bIsSignalingNaN ) float_raise( float_flag_invalid );
    if ( aIsNaN ) {
        return ( aIsSignalingNaN & bIsNaN ) ? b : a;
    }
    else {
        return b;
    }

}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is a NaN;
| otherwise returns 0.
*----------------------------------------------------------------------------*/

flag float64_is_nan( float64 a )
{

    return ( LIT64( 0xFFE0000000000000 ) < (bits64) ( a<<1 ) );

}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is a signaling
| NaN; otherwise returns 0.
*----------------------------------------------------------------------------*/

flag float64_is_signaling_nan( float64 a )
{

    return
           ( ( ( a>>51 ) & 0xFFF ) == 0xFFE )
        && ( a & LIT64( 0x0007FFFFFFFFFFFF ) );

}

/*----------------------------------------------------------------------------
| Returns the result of converting the double-precision floating-point NaN
| `a' to the canonical NaN format.  If `a' is a signaling NaN, the invalid
| exception is raised.
*----------------------------------------------------------------------------*/

commonNaNT float64ToCommonNaN( float64 a )
{
    commonNaNT z;

    if ( float64_is_signaling_nan( a ) ) float_raise( float_flag_invalid );
    z.sign = a>>63;
    z.low = 0;
    z.high = a<<12;
    return z;

}

/*----------------------------------------------------------------------------
| Returns the result of converting the canonical NaN `a' to the double-
| precision floating-point format.
*----------------------------------------------------------------------------*/

float64 commonNaNToFloat64( commonNaNT a )
{

    return
          ( ( (bits64) a.sign )<<63 )
        | LIT64( 0x7FF8000000000000 )
        | ( a.high>>12 );

}

/*----------------------------------------------------------------------------
| Takes two double-precision floating-point values `a' and `b', one of which
| is a NaN, and returns the appropriate NaN result.  If either `a' or `b' is a
| signaling NaN, the invalid exception is raised.
*----------------------------------------------------------------------------*/

float64 propagateFloat64NaN( float64 a, float64 b )
{
    flag aIsNaN, aIsSignalingNaN, bIsNaN, bIsSignalingNaN;

    aIsNaN = float64_is_nan( a );
    aIsSignalingNaN = float64_is_signaling_nan( a );
    bIsNaN = float64_is_nan( b );
    bIsSignalingNaN = float64_is_signaling_nan( b );
    a |= LIT64( 0x0008000000000000 );
    b |= LIT64( 0x0008000000000000 );
    if ( aIsSignalingNaN | bIsSignalingNaN ) float_raise( float_flag_invalid );
    if ( aIsNaN ) {
        return ( aIsSignalingNaN & bIsNaN ) ? b : a;
    }
    else {
        return b;
    }

}

/*----------------------------------------------------------------------------
| Returns 1 if the extended double-precision floating-point value `a' is a
| NaN; otherwise returns 0.
*----------------------------------------------------------------------------*/

flag floatx80_is_nan( floatx80 a )
{

    return ( ( a.high & 0x7FFF ) == 0x7FFF ) && (bits64) ( a.low<<1 );

}

/*----------------------------------------------------------------------------
| Returns 1 if the extended double-precision floating-point value `a' is a
| signaling NaN; otherwise returns 0.
*----------------------------------------------------------------------------*/

flag floatx80_is_signaling_nan( floatx80 a )
{
    bits64 aLow;

    aLow = a.low & ~ LIT64( 0x4000000000000000 );
    return
           ( ( a.high & 0x7FFF ) == 0x7FFF )
        && (bits64) ( aLow<<1 )
        && ( a.low == aLow );

}

/*----------------------------------------------------------------------------
| Returns the result of converting the extended double-precision floating-
| point NaN `a' to the canonical NaN format.  If `a' is a signaling NaN, the
| invalid exception is raised.
*----------------------------------------------------------------------------*/

commonNaNT floatx80ToCommonNaN( floatx80 a )
{
    commonNaNT z;

    if ( floatx80_is_signaling_nan( a ) ) float_raise( float_flag_invalid );
    z.sign = a.high>>15;
    z.low = 0;
    z.high = a.low<<1;
    return z;

}

/*----------------------------------------------------------------------------
| Returns the result of converting the canonical NaN `a' to the extended
| double-precision floating-point format.
*----------------------------------------------------------------------------*/

floatx80 commonNaNToFloatx80( commonNaNT a )
{
    floatx80 z;

    z.low = LIT64( 0xC000000000000000 ) | ( a.high>>1 );
    z.high = ( ( (bits16) a.sign )<<15 ) | 0x7FFF;
    return z;

}

/*----------------------------------------------------------------------------
| Takes two extended double-precision floating-point values `a' and `b', one
| of which is a NaN, and returns the appropriate NaN result.  If either `a' or
| `b' is a signaling NaN, the invalid exception is raised.
*----------------------------------------------------------------------------*/

floatx80 propagateFloatx80NaN( floatx80 a, floatx80 b )
{
    flag aIsNaN, aIsSignalingNaN, bIsNaN, bIsSignalingNaN;

    aIsNaN = floatx80_is_nan( a );
    aIsSignalingNaN = floatx80_is_signaling_nan( a );
    bIsNaN = floatx80_is_nan( b );
    bIsSignalingNaN = floatx80_is_signaling_nan( b );
    a.low |= LIT64( 0xC000000000000000 );
    b.low |= LIT64( 0xC000000000000000 );
    if ( aIsSignalingNaN | bIsSignalingNaN ) float_raise( float_flag_invalid );
    if ( aIsNaN ) {
        return ( aIsSignalingNaN & bIsNaN ) ? b : a;
    }
    else {
        return b;
    }

}
