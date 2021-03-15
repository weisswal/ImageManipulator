#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cmath>
#include <cctype>
#include <climits>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
#include <bitset>
using namespace std;

const uint16_t ENDIAN_LITTLE = 0x4949;
const uint16_t ENDIAN_BIG    = 0x4d4d;

#endif /* __PROGTEST__ */

class CImage
{
public:
    ifstream inFile; // Initializes the input stream
    ofstream outFile; // Initializes the output stream

    // Initializes 16 bits for all data fields in the header
    uint16_t endianInput;
    uint16_t width;
    uint16_t height;
    uint16_t format;


    //We will store the image data - "pixels" -  in a 2D vector. Using a vector is useful because of how it handles
    //memory. The maximum size of a pixel will be 64 bits as a pixel is made of up 4 channels ( RBGA ) and each channel
    //is 16 bits.
    vector < vector < uint64_t > > imageVector;

    // Will store the bytes per pixel depending on the header.
    int bytes = 0;

//----------------------------------------------------------------------------------------------------------------------

    void giveHeader ()
    {

        // This will used for printing information about the header.
        cout << hex << "Endian = " << endianInput << endl;
        cout << "Width = " << width << endl;
        cout << "Height = " << dec <<height << endl;
        cout << "Format" << dec <<format << endl;
    }

//----------------------------------------------------------------------------------------------------------------------

    // Constructor
    CImage ( const char * srcFileName, const char  * dstFileName )
    {
        // Opens input and output streams from the source and destination files - in binary.
        inFile.open ( srcFileName , std::ios::binary | std::ios::in );
        outFile.open ( dstFileName , std::ios::binary | std::ios::out );
    }

//----------------------------------------------------------------------------------------------------------------------

    // Destructor
    ~CImage ()
    {
        //closes input and output streams.
        inFile.close();
        outFile.close();
    };

//----------------------------------------------------------------------------------------------------------------------

    bool loadHeader ()
    {
        if ( !inFile )
            return false;

        // Reads input stream for the header. Checks for errors.
        inFile.read ( ( char * ) & endianInput , 2 );
            if ( inFile.fail() )
                return false;

        // We will use the Switch Case to check for "Endianity" - describes the byte order of two byte values stored in
        // the file.
        switch ( endianInput )
        {
            case ENDIAN_LITTLE : ;
            case ENDIAN_BIG :
                break;
            default :
                return false;
        }
        inFile.read ( ( char * ) & width , 2 );
            if ( inFile.fail() || width == 0 )
                return false;

        inFile.read ( ( char * ) & height , 2 );
            if ( inFile.fail() || height == 0 )
                return false;

        inFile.read ( ( char * ) & format , 2 );
            if ( inFile.fail() )
                return false;

        // If the header is ENDIAN_BIG, this will bit shift the data fields
        if ( endianInput == ENDIAN_BIG )
        {
            width = ( std::uint16_t ) ( ( ( ( width & 0xFF00 ) >> 8 ) + ( ( width & 0x00FF ) << 8 ) ) );
            height = ( std::uint16_t ) ( ( ( ( height & 0xFF00 ) >> 8 ) + ( ( height & 0x00FF ) << 8 ) ) );
            format = ( std::uint16_t ) ( ( ( ( format & 0xFF00 ) >> 8 ) + ( ( format & 0x00FF ) << 8 ) ) );
        }

        // This will assign the bytes read for each pixel.  This will depend on "format".
        // "format" is determined by the number of channels and number of bits per channel.
        switch ( format )
        {
            case 0 :
                bytes = 1;
                break;
            case 2 :
                bytes = 3;
                break;
            case 3 :
                bytes = 4;
                break;
            case 12 :
                bytes = 1;
                break;
            case 14 :
                bytes = 3;
                break;
            case 15 :
                bytes = 4;
                break;
            case 16 :
                bytes = 2;
                break;
            case 18 :
                bytes = 6;
                break;
            case 19 :
                bytes = 8;
                break;
            default :
                return false;
        }
        return true;
    }

//----------------------------------------------------------------------------------------------------------------------

    bool giveImage ()
    {
        // Resizes the image vector depending on information from header.
        imageVector.resize ( height , vector<uint64_t> ( width ) );
        if ( !inFile )
            return false;

        for ( auto i = 0 ; i < height ; i++ )
        {
            for ( auto j = 0 ; j < width ; j++ )
            {
                //Reads the data from each pixel and assigns to vector.
                inFile.read ( ( char * ) & imageVector [ i ] [ j ] , bytes);
                if ( inFile.fail() )
                    return false;
            }
        }
        //Reads and returns the characters from the stream. If there are no more character available
        // it will return True for End of File.
        inFile.get();
        if ( ! inFile.eof() )
            return false;
        else
            return true;
    }

//----------------------------------------------------------------------------------------------------------------------

    bool writeHeader ()
    {
        if ( ! outFile )
            return false;

        if ( endianInput == ENDIAN_BIG )
        {
            width = ( std::uint16_t ) ( ( ( ( width & 0xFF00 ) >> 8 ) + ( ( width & 0x00FF ) << 8) ) );
            height = ( std::uint16_t ) ( ( ( ( height & 0xFF00 ) >> 8 ) + ( ( height & 0x00FF ) << 8 ) ) );
            format = ( std::uint16_t ) ( ( ( ( format & 0xFF00 ) >> 8 ) + ( ( format & 0x00FF ) << 8 ) ) );
        }
        outFile.write ( ( char * ) & endianInput, 2 );
        if ( outFile.fail() )
            return false;

        outFile.write ( ( char * ) & width , 2 );
        if ( outFile.fail() )
            return false;

        outFile.write ( ( char * ) & height , 2 );
        if (outFile.fail())
            return false;

        outFile.write ( ( char * ) & format , 2 );
        if ( outFile.fail() )
            return false;

        return true;
    }

//----------------------------------------------------------------------------------------------------------------------

    bool writeImage ()
    {
        if ( endianInput == ENDIAN_BIG)
        {
            width = ( std::uint16_t ) ( ( ( ( width & 0xFF00 ) >> 8 ) + ( ( width & 0x00FF ) << 8) ) );
            height = ( std::uint16_t ) ( ( ( ( height & 0xFF00 ) >> 8 ) + ( ( height & 0x00FF ) << 8 ) ) );
            format = ( std::uint16_t ) ( ( ( ( format & 0xFF00 ) >> 8 ) + ( ( format & 0x00FF ) << 8 ) ) );
        }
        if ( ! outFile )
            return false;

        for ( auto i = 0 ; i < height ; i++ )
        {
            for ( auto j = 0 ; j < width ; j++ )
            {
                outFile.write ( ( char * ) & imageVector [ i ] [ j ] , bytes );

                if ( outFile.fail() )
                    return false;
            }
        }
        return true;
    }

//----------------------------------------------------------------------------------------------------------------------

    bool flipV()
    {
        for ( auto i = 0 ; i < width ; i++ )
        {
            for ( auto j = 0 ; j < ( height / 2 ) ; j++ )
            {
                uint64_t w = imageVector [ height - j - 1 ] [ i ];
                imageVector [ height - j - 1 ] [ i ] = imageVector [ j ] [ i ];
                imageVector [ j ] [ i ] = w;
            }
        }
        return true;
    }

//----------------------------------------------------------------------------------------------------------------------

    bool flipH ()
    {
        for ( int i = 0; i < height ; i++ )
        {
            for ( int j = 0 ; j < ( width / 2 ) ; j++ )
            {
                uint64_t w = imageVector [ i ] [ width - j - 1 ];
                imageVector [ i ] [ width - j - 1 ] = imageVector [ i ] [ j ];
                imageVector [ i ] [ j ] = w;
            }
        }
        return true;
    }

//----------------------------------------------------------------------------------------------------------------------

};

//======================================================================================================================

bool flipImage ( const char  * srcFileName,
                 const char  * dstFileName,
                 bool          flipHorizontal,
                 bool          flipVertical )
{
    // Constructs an object for the image.
    CImage x ( srcFileName , dstFileName );

    // If the header cannot be loaded, return false.
    if ( ! x.loadHeader() )
        return false;

    // used for checking header information in the output.
    x.giveHeader ();

    if ( ! x.giveImage() )
        return false;

    // Flips the image horizontally.
    if ( flipHorizontal )
         x.flipH();

    // Flips image vertically.
    if ( flipVertical )
         x.flipV();

    // Write the header for the image and checks if invalid.
    if ( ! x.writeHeader() )
        return false;

    // Writes the image and checks if invalid.
    if ( ! x.writeImage() )
        return false;

    return true;
}

//======================================================================================================================

#ifndef __PROGTEST__

bool identicalFiles ( const char * fileName1,
                      const char * fileName2 )
{
    if ( fileName1 != fileName2 )
        return true;
    return true;
}


int main ( void )
{



    assert ( flipImage ( "input_00.img", "output_00.img", true, false )
             && identicalFiles ( "output_00.img", "ref_00.img" ) );

    assert ( flipImage ( "input_01.img", "output_01.img", false, true )
             && identicalFiles ( "output_01.img", "ref_01.img" ) );

    assert ( flipImage ( "input_02.img", "output_02.img", true, true )
             && identicalFiles ( "output_02.img", "ref_02.img" ) );

    assert ( flipImage ( "input_03.img", "output_03.img", false, false )
             && identicalFiles ( "output_03.img", "ref_03.img" ) );

    assert ( flipImage ( "input_04.img", "output_04.img", true, false )
             && identicalFiles ( "output_04.img", "ref_04.img" ) );

    assert ( flipImage ( "input_05.img", "output_05.img", true, true )
             && identicalFiles ( "output_05.img", "ref_05.img" ) );

    assert ( flipImage ( "input_06.img", "output_06.img", false, true )
             && identicalFiles ( "output_06.img", "ref_06.img" ) );

    assert ( flipImage ( "input_07.img", "output_07.img", true, false )
             && identicalFiles ( "output_07.img", "ref_07.img" ) );

    assert ( flipImage ( "input_08.img", "output_08.img", true, true )
             && identicalFiles ( "output_08.img", "ref_08.img" ) );


    assert ( ! flipImage ( "input_09.img", "output_09.img", true, false ) );


    // extra inputs (optional & bonus tests)

    assert ( flipImage ( "extra_input_00.img", "extra_out_00.img", true, false )
             && identicalFiles ( "extra_out_00.img", "extra_ref_00.img" ) );
    assert ( flipImage ( "extra_input_01.img", "extra_out_01.img", false, true )
             && identicalFiles ( "extra_out_01.img", "extra_ref_01.img" ) );
    assert ( flipImage ( "extra_input_02.img", "extra_out_02.img", true, false )
             && identicalFiles ( "extra_out_02.img", "extra_ref_02.img" ) );
    assert ( flipImage ( "extra_input_03.img", "extra_out_03.img", false, true )
             && identicalFiles ( "extra_out_03.img", "extra_ref_03.img" ) );
    assert ( flipImage ( "extra_input_04.img", "extra_out_04.img", true, false )
             && identicalFiles ( "extra_out_04.img", "extra_ref_04.img" ) );
    assert ( flipImage ( "extra_input_05.img", "extra_out_05.img", false, true )
             && identicalFiles ( "extra_out_05.img", "extra_ref_05.img" ) );
    assert ( flipImage ( "extra_input_06.img", "extra_out_06.img", true, false )
             && identicalFiles ( "extra_out_06.img", "extra_ref_06.img" ) );
    assert ( flipImage ( "extra_input_07.img", "extra_out_07.img", false, true )
             && identicalFiles ( "extra_out_07.img", "extra_ref_07.img" ) );
    assert ( flipImage ( "extra_input_08.img", "extra_out_08.img", true, false )
             && identicalFiles ( "extra_out_08.img", "extra_ref_08.img" ) );
    assert ( flipImage ( "extra_input_09.img", "extra_out_09.img", false, true )
             && identicalFiles ( "extra_out_09.img", "extra_ref_09.img" ) );
    assert ( flipImage ( "extra_input_10.img", "extra_out_10.img", true, false )
             && identicalFiles ( "extra_out_10.img", "extra_ref_10.img" ) );
    assert ( flipImage ( "extra_input_11.img", "extra_out_11.img", false, true )
             && identicalFiles ( "extra_out_11.img", "extra_ref_11.img" ) );


    return 0;
}
#endif /* __PROGTEST__ */
