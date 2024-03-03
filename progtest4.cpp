#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;
#endif /* __PROGTEST__ */
#define DEBUG_LEVEL 0
#if DEBUG_LEVEL >= 1
    #define DEBUG_PRINT(...) printf (__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
#endif

class CString
{
  public:
                             CString                       ( void );
                             CString                       ( const char *      data);
                             CString                       ( const CString &   x ); 
                            ~CString                       ( void );
    CString                & operator=                     ( CString           rho );
    friend ostream         & operator <<                   ( ostream &         os,
                                                             const CString &   x ); 
    bool                     operator ==                   ( const CString &   x ) const;
    bool                     operator ==                   ( const char *      x ) const;

    struct TRaw
    {
        size_t               m_Length;
        char               * m_Text;
        uint32_t             m_Refs;
                             TRaw                          ( const char *      data );
                            ~TRaw                          ( void );
    };

    TRaw                   * m_Info;
};

//=================================================================================================
                             CString::TRaw::TRaw           ( const char *      data )
: m_Length ( strlen ( data ) ),
  m_Text ( new char [m_Length + 1] ),
  m_Refs ( 1 )
{
  strncpy ( m_Text, data, m_Length + 1 ); 
}

//-------------------------------------------------------------------------------------------------
                             CString::TRaw::~TRaw          ( void )
{
  DEBUG_PRINT ("%s\n",__PRETTY_FUNCTION__);
  delete [] m_Text;
}

//=================================================================================================

                             CString::CString              ( void )
: m_Info ( nullptr )
{
  DEBUG_PRINT ("%s\n",__PRETTY_FUNCTION__);
}
//-------------------------------------------------------------------------------------------------
                             CString::CString              ( const char *      data )
: m_Info ( new TRaw ( data ) )
{
  DEBUG_PRINT ("%s\n",__PRETTY_FUNCTION__);
}

//-------------------------------------------------------------------------------------------------
                             CString::CString              ( const CString &   x )   
:m_Info ( x . m_Info )                       
{
  DEBUG_PRINT ("%s\n",__PRETTY_FUNCTION__);
  m_Info -> m_Refs ++;
}

//-------------------------------------------------------------------------------------------------
                             CString::~CString             ( void )
{
  DEBUG_PRINT ("%s\n",__PRETTY_FUNCTION__);

  if ( m_Info != nullptr && --( m_Info -> m_Refs ) == 0)
    delete m_Info;
}

//-------------------------------------------------------------------------------------------------
CString &                    CString::operator=            ( CString           rho )
{
  
  DEBUG_PRINT ("%s\n",__FUNCTION__);
  swap ( m_Info, rho . m_Info );                  
  return *this;

}

//-------------------------------------------------------------------------------------------------
ostream &                    operator <<                   ( ostream &         os,
                                                             const CString &   x )
{
  return os << x . m_Info -> m_Text;
} 

//-------------------------------------------------------------------------------------------------
bool                         CString::operator ==          ( const CString &   x ) const
{
  if ( x . m_Info -> m_Length != m_Info -> m_Length )
    return false;
  return 0 == strncmp ( m_Info -> m_Text, x . m_Info -> m_Text , m_Info -> m_Length );
} 

//-------------------------------------------------------------------------------------------------
bool                         CString::operator ==          ( const char *      x ) const
{
  if ( strlen ( x ) != m_Info -> m_Length )
    return false;
  return 0 == strncmp ( m_Info -> m_Text, x ,  m_Info -> m_Length );
} 

//=================================================================================================
template <typename T>
class CVekturek
{
  public:
                             CVekturek                     ( void );
                             CVekturek                     ( const CVekturek<T> & x );
                            ~CVekturek                     ( void );
    CVekturek<T>           & operator =                    ( const CVekturek<T> & rho );
    void                     Push                          ( const T &         x );
    void                     Clear                         ( void );
    size_t                   Size                          ( void ) const;
    const T                & operator []                   ( int idx ) const;
    T                      & operator []                   ( int idx );

  private:
    T                      * m_Data;
    size_t                   m_Size;
    size_t                   m_Max;
    static const size_t      INITSIZE = 10 ;
    void                     Enlarge                       ( void );
};

//=================================================================================================
template <typename T>
                             CVekturek<T>::CVekturek       ( void )
:m_Data ( nullptr ),
 m_Size ( 0 ),
 m_Max ( 0 )
{
}

//-------------------------------------------------------------------------------------------------
template <typename T>
                             CVekturek<T>::CVekturek        ( const CVekturek<T> & x )
:m_Data ( new T [x . m_Max] ),
 m_Size ( x . m_Size ),
 m_Max ( x . m_Max )
{
  for ( size_t i = 0; i < m_Size; i ++ )
    m_Data[i] = x . m_Data[i];
}

//-------------------------------------------------------------------------------------------------
template <typename T>
                             CVekturek<T>::~CVekturek       ( void )
{
  delete [] m_Data;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
CVekturek<T> &               CVekturek<T>::operator =       ( const CVekturek<T> & rho )
{
  if ( this == &rho )
    return *this;

  Clear ();
  m_Data = new T [rho . m_Max];
  m_Size = rho . m_Size;
  m_Max  = rho . m_Max;

  for ( size_t i = 0; i < m_Size; i ++ )
    m_Data[i] = rho . m_Data[i];
  
  return *this;
}
//-------------------------------------------------------------------------------------------------
template <typename T>
void                         CVekturek<T>::Clear            ( void )
{
  delete [] m_Data;
  m_Data = nullptr;
  m_Size = 0;
  m_Max = 0;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void                         CVekturek<T>::Push            ( const T &         x )
{
  if ( m_Size == m_Max )
    Enlarge ();
  m_Data[m_Size++] = x ;  
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void                         CVekturek<T>::Enlarge         ( void )
{ 
    m_Max = ( m_Max == 0 ) ? INITSIZE : m_Max * 1.1;
    T * tmp = new T [m_Max];

    for ( size_t i = 0; i < m_Size; i ++ )
      tmp[i] = m_Data[i];
   
    delete [] m_Data;
    m_Data = tmp;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
size_t                       CVekturek<T>::Size            ( void ) const
{
  return m_Size;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
const T &                    CVekturek<T>::operator []     ( int idx ) const
{
  return m_Data[idx];
}

//-------------------------------------------------------------------------------------------------
template <typename T>
T &                          CVekturek<T>::operator []     ( int idx ) 
{
  return m_Data[idx];
}
//=================================================================================================
class TTransaction
{
  public:
  struct TRaw
  {
    bool                     m_IsFrom;
    CString                  m_OAcc;                     
    uint32_t                 m_Amount;
    CString                  m_Sign;
    uint32_t                 m_Refs;                      
                             TRaw                          ( void );
                             TRaw                          ( bool              from,
                                                             const CString &   acc,
                                                             uint32_t          amount,
                                                             const CString &   sign );   
  };
    TRaw                  * m_Info;
                            TTransaction                   ( void );
                            TTransaction                   ( bool              from,
                                                             const CString &   acc,
                                                             uint32_t          amount,
                                                             const CString &   sign );
                            TTransaction                   ( const TTransaction & x );
                           ~TTransaction                   ( void );
    TTransaction &          operator=                      ( TTransaction     rho );
};

//=================================================================================================
                             TTransaction::TRaw::TRaw      ( void )
:m_IsFrom ( false ),
 m_Amount ( 0 ),
 m_Refs ( 0 )
{
}

//-------------------------------------------------------------------------------------------------
                             TTransaction::TRaw::TRaw      ( bool              from,
                                                             const CString &   acc,
                                                             uint32_t          amount,
                                                             const CString &   sign )
:m_IsFrom ( from ),
 m_OAcc ( acc ),
 m_Amount ( amount ),
 m_Sign ( sign ),
 m_Refs ( 1 )
{
}
//=================================================================================================
                             TTransaction::TTransaction    ( void )
:m_Info ( nullptr )
{
}

//-------------------------------------------------------------------------------------------------
                             TTransaction::TTransaction    ( bool              from,
                                                             const CString &   acc,
                                                             uint32_t          amount,
                                                             const CString &   sign )
:m_Info ( new TRaw ( from, acc, amount, sign ) )
{
}
//-------------------------------------------------------------------------------------------------
                             TTransaction::TTransaction    ( const TTransaction & x )
:m_Info ( x . m_Info )                       
{
  m_Info -> m_Refs ++;
}
//------------------------------------------------------------------------------------------------- 
                             TTransaction::~TTransaction    ( void )                      
{
  if ( m_Info != nullptr && --( m_Info -> m_Refs ) == 0)
    delete m_Info;
}

TTransaction &               TTransaction::operator=        ( TTransaction     rho )
{
  
  DEBUG_PRINT ("%s\n",__FUNCTION__);
  swap ( m_Info, rho . m_Info );                  
  return *this;

}


//=================================================================================================
class CAccount
{
  public:
                             CAccount                      ( void );
                             CAccount                      ( const CString &   acc,
                                                             int64_t           balance );
    int32_t                  Balance                       ( void ) const;
    friend ostream         & operator <<                   ( ostream &         os,
                                                             const CAccount &  x );
    CString                  m_AccID;    
    int32_t                  m_InitBal;                
    int32_t                  m_Balance;
    CVekturek <TTransaction> m_Trans;
};
//=================================================================================================
                             CAccount::CAccount            ( void )
:m_InitBal ( 0 ),
 m_Balance ( 0 )
{
}
//-------------------------------------------------------------------------------------------------

                             CAccount::CAccount            ( const CString &   acc,
                                                             int64_t           balance )
:m_AccID ( acc ),
 m_InitBal ( balance ),
 m_Balance ( balance )                                                  
{
}
//-------------------------------------------------------------------------------------------------

int32_t                      CAccount::Balance             ( void ) const
{
  return m_Balance;
}

//-------------------------------------------------------------------------------------------------
ostream &                    operator <<                   ( ostream &         os,
                                                             const CAccount &  x )
{
  os << x . m_AccID << ":" << "\n";
  os << "   " << x . m_InitBal << "\n";
  for ( size_t i = 0; i < x . m_Trans . Size (); i++ )
    {
      bool isFrom = x . m_Trans[i] . m_Info -> m_IsFrom;
      os << ( isFrom ? " - "  :  " + "    ) << x . m_Trans[i] . m_Info -> m_Amount << ", ";
      os << ( isFrom ? "to: " :  "from: " ) << x . m_Trans[i] . m_Info -> m_OAcc << ", ";
      os << "sign: "<< x . m_Trans[i] . m_Info -> m_Sign << "\n";
    }
  os << " = " << x . m_Balance << endl;
  return os;
} 

//=================================================================================================
class CBank
{
  public:
    bool                     NewAccount                    ( const char *      accID,
                                                             int               initialBalance );
    bool                     Transaction                   ( const char *      debAccID,
                                                             const char *      credAccID,
                                                             unsigned int      amount,
                                                             const char *      signature );
    bool                     TrimAccount                   ( const char *      accID );
    const CAccount &         Account                       ( const char *      accID ) const;

  private:
    bool                     IsPresent                     ( const char *      accID, 
                                                             size_t &          indx ) const;
    CVekturek <CAccount>     m_Accs;
};

//------------------------------------------------------------------------------------------------- 
bool                         CBank::NewAccount             ( const char *      accID,
                                                             int               initialBalance )
{
  size_t indx;
  if ( IsPresent ( accID , indx ) )
    return false;

  m_Accs . Push ( { accID , initialBalance } );
  return true;
}

//-------------------------------------------------------------------------------------------------
bool                         CBank::Transaction            ( const char *      debAccID,
                                                             const char *      credAccID,
                                                             unsigned int      amount,
                                                             const char *      signature )
{
  size_t indxD, indxC;

  if ( ! IsPresent ( debAccID , indxD ) )
    return false;
  if ( ! IsPresent ( credAccID , indxC ) )
    return false;
  if ( indxD == indxC )
    return false;
    
  CString sign ( signature );

  m_Accs[indxD] . m_Balance -= amount;
  m_Accs[indxC] . m_Balance += amount;

  m_Accs[indxD] . m_Trans . Push ( {true, m_Accs[indxC] . m_AccID, amount, sign} );
  m_Accs[indxC] . m_Trans . Push ( {false, m_Accs[indxD] . m_AccID, amount, sign } );

  return true;
}

//-------------------------------------------------------------------------------------------------
bool                         CBank::TrimAccount            ( const char *      accID )
{
  size_t indx;
  if ( ! IsPresent ( accID , indx ) )
    return false;

  m_Accs[indx] . m_Trans . Clear ();
  m_Accs[indx] . m_InitBal = m_Accs[indx] . m_Balance;
  return true;
}

//-------------------------------------------------------------------------------------------------
const CAccount &             CBank::Account                ( const char *      accID ) const
{
  size_t indx;
  if ( ! IsPresent ( accID , indx ) )
    throw invalid_argument ( "Nonexisting account" );
  return m_Accs[indx];
}

//-------------------------------------------------------------------------------------------------
bool                         CBank::IsPresent              ( const char *      accID, 
                                                             size_t &          indx ) const
{
  for ( size_t i = 0; i < m_Accs . Size (); i++ )
    if ( m_Accs[i] . m_AccID == accID )
    {
      indx = i;
      return true;
    }
  return false;
}

//=================================================================================================
/*
void * operator new ( size_t size )
{
  cout << "Allocating " << size << " bytes." << endl;
  return malloc (size);
}
*/

#ifndef __PROGTEST__
int main ( void )
{
  return 0;
}
#endif /* __PROGTEST__ */
