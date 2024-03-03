#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;

#endif /* __PROGTEST__ */

#define DEBUG_LEVEL 0
#if DEBUG_LEVEL >= 1
    #define DEBUG_PRINT(...) printf (__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
#endif

#define INVALID_VALUE 255

class CIO {
    public:
        static bool getBit (bool & bit);
        static bool getByte (uint8_t & byte);
        static bool getUTF8 (uint32_t & byte, int & size);
        static bool init (const char * inFileName);
        static bool close (); 
        
    private:
        static uint8_t m_Byte;
        static int  m_Position;
        static ifstream m_Fin;
};

uint8_t CIO::m_Byte;
int CIO::m_Position;
ifstream CIO::m_Fin;

class CNode {
  public:
    CNode * m_Right;
    CNode * m_Left;
    uint32_t m_Value;
    int m_Size;

   CNode ()
     : m_Right (nullptr),
       m_Left (nullptr), 
       m_Value (INVALID_VALUE)
    {}

   ~CNode (){
        delete m_Right;
        delete m_Left;
    }
};


bool CIO::init (const char * inFileName)
{
    m_Fin.open (inFileName,  ios::binary | ios::in);
    if (!(m_Fin.good () && m_Fin.is_open ())){
      return false;
    }
    m_Position = 0;
    DEBUG_PRINT ("all is good\n");
    return true;
}

bool CIO::close ()
{
    m_Fin.get ();
    m_Fin.close ();
    return m_Fin.eof ();
}

bool CIO::getBit (bool & bit)
{
    if (m_Position == 0){
        m_Byte = m_Fin.get ();
        if (m_Fin.eof ()){
            return false;
        }
        m_Position = 8;
    }
    bit = (m_Byte & (1 << --m_Position ));
    return true;
}

bool CIO::getByte (uint8_t & byte)
{   
    bool bit;
    uint8_t tmp = 0;
    for (int j = 7; j >= 0 ; j--){
        if (getBit (bit)){
            tmp |= (bit << j);
        }
        else{
            return false;
        }
    }
    DEBUG_PRINT ("BYTE\n");
    byte = tmp;
    return true;
}

bool CIO::getUTF8 (uint32_t & symbol, int & size)
{
    uint8_t byte;
    uint32_t tmp_symbol;
    int tmp_size;
    if(! CIO::getByte (byte)){
        return false;
    }

    int counter = 0;
    for( int i = 7; i >= 3; i-- ){
        bool bit = byte & (1<<i);

        if(!bit){
            break;
        }
        ++counter;
    }

    if( counter == 5 || counter == 1){
        return false;
    }

    tmp_symbol = ((uint32_t) byte);
    tmp_size = counter == 0 ? 1: counter;

    for(int bleb = 0; bleb < (counter-1); bleb++){
       uint8_t finger;

       if(! CIO::getByte (finger) ){
           DEBUG_PRINT("FAILED CHECK finger ===> %02x \n", finger);
           return false;
       }
       if(((0b11000000 & finger) ^ 0b10000000)){
           return false;
       }
        tmp_symbol |= finger << (8 + bleb * 8);
    }
    
    DEBUG_PRINT("TMP SYMBOL ===> %02x \n", tmp_symbol);
    symbol = tmp_symbol;
    size = tmp_size;  
    return true;
}

bool getSymbol (CNode * root, uint32_t & symbol, int & size)
{
    bool bit;
    if (!root){
        return false;
    }
    DEBUG_PRINT ("> ROOT VALUE = %02x\n", root->m_Value);
    if (root->m_Value != INVALID_VALUE){
        symbol = root->m_Value;
        size = root->m_Size;
        DEBUG_PRINT (" => %c\n", symbol);
        return true;
    }
    if (!(CIO::getBit (bit))){
        return false;
    }
    DEBUG_PRINT (" + %d\n", (int)bit);
    if (bit == 0){
        return getSymbol (root->m_Left, symbol, size);
    }
    else{
        return getSymbol (root->m_Right, symbol, size);
    }
}

bool parseTree (CNode ** n)
{
    bool bit;

    if (!(CIO::getBit (bit))){
        return false;
    }
    DEBUG_PRINT("BIT\n");

    * n = new CNode ();
    if (bit == 0){
        return (parseTree (& ( * n )->m_Left  ) && parseTree (& ( * n )->m_Right ));
    }
    return CIO::getUTF8 ( ( * n )->m_Value, ( * n )->m_Size);
}

bool decompressFile (const char * inFileName, const char * outFileName)
{
    ofstream fout (outFileName, ios::binary);
    
    if (!(fout.is_open ())){
        DEBUG_PRINT ("SAAAD COULDNT OPEN %s\n", outFileName);
        return false;
    }

    CIO::init (inFileName);

    CNode * root = nullptr; 
    if (!(parseTree ( & root )) ){
        DEBUG_PRINT ("FUK TREE\n");
        delete root;
        CIO::close ();
        return false;
    }
    DEBUG_PRINT ("PODTIM\n");

    
    bool flag, bit;
    uint32_t symbol;
    int size;
    int remaining;
    int times;
    while (CIO::getBit (flag)){
        remaining = 0;
        if (flag == 0 ){
            for (int j = 11; j >= 0 ; j--){
                if (!(CIO::getBit (bit))){
                    DEBUG_PRINT ("DID NOT READ 12\n");
                    delete root;
                    fout.close();
                    CIO::close ();
                    return false; 
                }
                remaining |= (bit << j);
            } 
        }
            
        times = flag * 4096 + (!flag) * remaining;
        DEBUG_PRINT ("times %d \n", times);

        for (int i = 0; i < times ; i++){
            DEBUG_PRINT ( "===========\n" );
            if (!(getSymbol (root, symbol, size ))){
                DEBUG_PRINT ("WHERE IS CHAR\n");
                delete root;
                CIO::close ();
                fout.close();
                return false;
            }
         DEBUG_PRINT ("COUT WRITE __________\n");
         DEBUG_PRINT ("symbol %02x\n", symbol);
         DEBUG_PRINT ("size %02x\n", size);
         DEBUG_PRINT ("Somehitng %02x\n",(char *) & symbol);
         fout.write ((char *) & symbol, size);

         if (fout.bad() && !fout.eof()){
              delete root;
              fout.close();
              CIO::close ();
              return false;
            }
            
        }
        if (flag==0){
            break;
        }
    }
    if( flag != 0){
      delete root;
      fout.close();
      CIO::close ();
      return false;  
    }
    delete root;
    fout.close();
    return CIO::close ();
}

bool compressFile (const char * inFileName, const char * outFileName)
{
    // keep this dummy implementation (no bonus) or implement the compression (bonus)
    return false;
}


#ifndef __PROGTEST__
bool identicalFiles (const char * fileName1, const char * fileName2)
{
    ifstream fin1 (fileName1, ios::binary );
    ifstream fin2 (fileName2, ios::binary );
    if(!(fin1.is_open() && fin2.is_open())){
        return false;
    }
    while (fin1.get() == fin2.get()){
       if (fin1.eof() || fin1.eof()){
            break; 
       }
    }
    fin1.close();
    fin2.close();
    return fin2.eof() && fin1.eof();
    
}

int main ( void )
{
  assert ( identicalFiles ( "tests/test0.orig", "tests/test0.orig" ) );
  assert ( decompressFile ( "tests/test0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test4.orig", "tempfile" ) );

  assert ( ! decompressFile ( "tests/test5.huf", "tempfile" ) );


  assert ( decompressFile ( "tests/extra0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra4.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra5.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra5.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra6.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra6.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra7.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra7.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra8.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra8.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra9.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra9.orig", "tempfile" ) );

  return 0;
}
#endif /* __PROGTEST__ */
