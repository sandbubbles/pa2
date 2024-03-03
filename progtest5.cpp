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
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
#endif /* __PROGTEST */
using namespace std;

#ifndef __PROGTEST__
class CTimeStamp
{
  public:
                             CTimeStamp                    ( int               year,
                                                             int               month,
                                                             int               day,
                                                             int               hour,
                                                             int               minute,
                                                             int               sec );
    int                      Compare                       ( const CTimeStamp & x ) const;
    friend ostream &         operator <<                   ( ostream          & os,
                                                             const CTimeStamp & x );
  private:
    int                      m_Year;
    int                      m_Month;
    int                      m_Day;
    int                      m_Hour;
    int                      m_Min;
    int                      m_Sec;
};
//=================================================================================================

                             CTimeStamp::CTimeStamp        ( int               year,
                                                             int               month,
                                                             int               day,
                                                             int               hour,
                                                             int               minute,
                                                             int               sec )
:m_Year  ( year ),
 m_Month ( month ),
 m_Day   ( day ),
 m_Hour  ( hour ),
 m_Min   ( minute ),
 m_Sec   ( sec )
{}
//-------------------------------------------------------------------------------------------------
int                          CTimeStamp::Compare           ( const CTimeStamp & x ) const
{
  if ( m_Year  !=  x . m_Year )
    return m_Year  - x . m_Year;
  if ( m_Month !=  x . m_Month )
    return m_Month - x . m_Month;
  if ( m_Day   !=  x . m_Day )
    return m_Day   - x . m_Day;
  if ( m_Hour  !=  x . m_Hour )
    return m_Hour  - x . m_Hour;
  if ( m_Min   !=  x . m_Min )
    return m_Min   - x . m_Min;
  if ( m_Sec   !=  x . m_Sec )
    return m_Sec   - x . m_Sec;
  return 0;
}
//-------------------------------------------------------------------------------------------------
ostream &                    operator <<                   ( ostream          & os,
                                                             const CTimeStamp & x )
{
  //YYYY-MM-DD HH24:MI:SS
  os << setfill('0') << setw(4) 
     << x . m_Year << "-"
     << setw(2)
     << x . m_Month << "-" << x . m_Day << " "
     << x . m_Hour << ":" << x . m_Min << ":" << x . m_Sec;
  return os;
}
//=================================================================================================
class CMailBody
{
  public:
  // copy cons/op=/destructor is correctly implemented in the testing environment
                             CMailBody                     ( int               size,
                                                             const char      * data );
                             CMailBody                     ( const CMailBody & x );
                            ~CMailBody                     ( void );
    CMailBody              & operator =                    ( const CMailBody & x );
    friend ostream         & operator <<                   ( ostream         & os,
                                                             const CMailBody & x );
  private:
    int                      m_Size;
    char                   * m_Data;
};
//=================================================================================================
                             CMailBody::CMailBody          ( int               size,
                                                             const char      * data )
:m_Size ( size ),
 m_Data ( new char[ m_Size + 1 ] )
{
  memcpy ( m_Data, data, m_Size + 1 );
}
//-------------------------------------------------------------------------------------------------
                             CMailBody::CMailBody          ( const CMailBody & x )
:m_Size ( x . m_Size ),
 m_Data ( new char[ m_Size + 1 ] )
{
  memcpy ( m_Data, x . m_Data, m_Size + 1 );
}
//-------------------------------------------------------------------------------------------------
                             CMailBody::~CMailBody         ( void )
{
  delete [] m_Data;
}
//-------------------------------------------------------------------------------------------------
CMailBody                  & CMailBody::operator =         ( const CMailBody & x )
{
  if ( this == &x )
    return *this;
  m_Size = x . m_Size ;
  delete [] m_Data;
  m_Data = new char[ m_Size + 1 ];
  memcpy ( m_Data, x . m_Data, m_Size + 1 );
  return *this; 
}
//-------------------------------------------------------------------------------------------------
ostream                    & operator <<                   ( ostream         & os,
                                                             const CMailBody & x )
{
  return os << "mail body: " << x . m_Size << " B";
}
//=================================================================================================
class CAttach
{
  public:
                             CAttach                       ( int               x )
      : m_X      ( x ),
        m_RefCnt ( 1 )
    {
    }
    void                     AddRef                        ( void ) const 
    { 
      m_RefCnt ++; 
    }
    void                     Release                       ( void ) const 
    { 
      if ( !--m_RefCnt ) 
        delete this; 
    }
  private:
    int                      m_X;
    mutable int              m_RefCnt;
                             CAttach                       ( const CAttach   & x );
    CAttach                & operator =                    ( const CAttach   & x );
                            ~CAttach                       ( void ) = default;
    friend ostream &         operator <<                   ( ostream         & os,
                                                             const CAttach   & x )
    {
      return os << "attachment: " << x . m_X << " B";
    }
};
//=================================================================================================
#endif /* __PROGTEST__, DO NOT remove */

class CMail
{
  public:
                             CMail                         ( const CTimeStamp & timeStamp,
                                                             const string    & from,
                                                             const CMailBody & body,
                                                             const CAttach   * attach );
                            ~CMail                         ( void );
                             CMail                         ( const CMail     & x );
    CMail &                  operator =                    ( const CMail     & x );                         
    const string           & From                          ( void ) const;
    const CMailBody        & Body                          ( void ) const;
    const CTimeStamp       & TimeStamp                     ( void ) const;
    const CAttach          * Attachment                    ( void ) const;
    
    friend bool              operator <                    ( const CMail     & lho, 
                                                             const CMail     & rho );
    friend ostream         & operator <<                   ( ostream         & os,
                                                             const CMail     & x );
  private:
    CTimeStamp               m_TimeStamp;
    string                   m_From;
    CMailBody                m_Body;
    const CAttach          * m_Attach;
};
//=================================================================================================
                             CMail::CMail                  ( const CTimeStamp & timeStamp,
                                                             const string    & from,
                                                             const CMailBody & body,
                                                             const CAttach   * attach )
:m_TimeStamp ( timeStamp ),
 m_From      ( from ),
 m_Body      ( body ),
 m_Attach    ( attach )
{
  if ( m_Attach != nullptr )
    m_Attach -> AddRef ();
}
//-------------------------------------------------------------------------------------------------
                             CMail::~CMail                  ( void )
{
  if ( m_Attach != nullptr )
    m_Attach -> Release ();
}
//-------------------------------------------------------------------------------------------------
                             CMail::CMail                  ( const CMail     & x )
:m_TimeStamp ( x . m_TimeStamp ),
 m_From      ( x . m_From ),
 m_Body      ( x . m_Body ),
 m_Attach    ( x . m_Attach )
{
  if ( m_Attach != nullptr )
    m_Attach -> AddRef ();
}
//-------------------------------------------------------------------------------------------------
CMail &                      CMail::operator =             ( const CMail     & x )
{
  if ( this == &x )
    return *this;

  m_TimeStamp = x . m_TimeStamp;
  m_From = x . m_From;
  m_Body = x . m_Body;

  if ( m_Attach != nullptr )
    m_Attach -> Release ();
  m_Attach = x . m_Attach;
  if ( m_Attach != nullptr )
    m_Attach -> AddRef ();
  return *this;
}  
//-------------------------------------------------------------------------------------------------
const string               & CMail::From                   ( void ) const
{
  return m_From;
}
//-------------------------------------------------------------------------------------------------
const CMailBody            & CMail::Body                   ( void ) const
{
  return m_Body;
}
//-------------------------------------------------------------------------------------------------
const CTimeStamp           & CMail::TimeStamp              ( void ) const
{
  return m_TimeStamp;
}
//-------------------------------------------------------------------------------------------------
const CAttach              * CMail::Attachment             ( void ) const
{
  return m_Attach;
}
//-------------------------------------------------------------------------------------------------
bool                         operator <                    ( const CMail     & lho, 
                                                             const CMail     & rho )
{
  return lho . m_TimeStamp . Compare ( rho . m_TimeStamp ) < 0;
}

//-------------------------------------------------------------------------------------------------
ostream                    & operator <<                   ( ostream         & os,
                                                             const CMail     & x )
{
  os << x . m_TimeStamp << " " 
     << x . m_From << " "
     << x . m_Body;
  
  if ( x . m_Attach != nullptr )
    os << " + " << * ( x . m_Attach ) ;

  return os;
}
//=================================================================================================
class CMailBox
{
  public:
                             CMailBox                      ( void );
    bool                     Delivery                      ( const CMail     & mail );
    bool                     NewFolder                     ( const string    & folderName );
    bool                     MoveMail                      ( const string    & fromFolder,
                                                             const string    & toFolder );
    list<CMail>              ListMail                      ( const string    & folderName,
                                                             const CTimeStamp & from,
                                                             const CTimeStamp & to ) const;
    set<string>              ListAddr                      ( const CTimeStamp & from,
                                                             const CTimeStamp & to ) const;
  private:
    map<string, set<CMail>>  m_Folders;
};
//=================================================================================================
                             CMailBox::CMailBox            ( void )
{
  NewFolder ( "inbox" );
}
//-------------------------------------------------------------------------------------------------
bool                         CMailBox::Delivery            ( const CMail     & mail )
{
  return m_Folders . at ("inbox") . insert ( mail ) . second;
}
//-------------------------------------------------------------------------------------------------
bool                         CMailBox::NewFolder           ( const string    & folderName )
{
  if ( m_Folders . count ( folderName ) == 1 )
    return false;
  m_Folders[folderName];
  return true;
}
//-------------------------------------------------------------------------------------------------
bool                         CMailBox::MoveMail            ( const string    & fromFolder,
                                                             const string    & toFolder )
{
  if ( m_Folders . count ( fromFolder ) != 1 ||  m_Folders . count ( toFolder ) != 1   )
    return false;
  //m_Folders[toFolder] . merge ( move ( m_Folders[fromFolder] ) );
  m_Folders[toFolder] . merge (  m_Folders[fromFolder] );
  return true;
}
//-------------------------------------------------------------------------------------------------
list<CMail>                  CMailBox::ListMail            ( const string    & folderName,
                                                             const CTimeStamp & from,
                                                             const CTimeStamp & to ) const
{
  list<CMail> tmp;

  if ( m_Folders . count ( folderName ) != 1 )
    return tmp;

  auto i     = m_Folders . at (folderName) . lower_bound ( CMail (from, "", CMailBody ( 0, "" ), nullptr ) );
  auto upper = m_Folders . at (folderName) . upper_bound ( CMail (to  , "", CMailBody ( 0, "" ), nullptr ) );
 
  for ( ; i != upper ; i++ )
    tmp . push_back ( *i );

  return tmp;
}
//-------------------------------------------------------------------------------------------------

set<string>                  CMailBox::ListAddr            ( const CTimeStamp & from,
                                                             const CTimeStamp & to ) const
{
  set<string> tmp;
  for ( const auto & seto : m_Folders ) 
  {
    auto i     = seto . second . lower_bound ( CMail (from, "", CMailBody ( 0, "" ), nullptr ) );
    auto upper = seto . second . upper_bound ( CMail (to  , "", CMailBody ( 0, "" ), nullptr ) );
    for ( ; i != upper ; i++ )
    tmp . insert ( i-> From () ); 
  }
  return tmp;
}
//=================================================================================================
#ifndef __PROGTEST__
#if 1
static string showMail ( const list<CMail> & l )
{
  ostringstream oss;
  for ( const auto & x : l )
    oss << x << endl;
  return oss . str ();
}
static string showUsers ( const set<string> & s )
{
  ostringstream oss;
  for ( const auto & x : s )
    oss << x << endl;
  return oss . str ();
}
#endif
int main ( void )
{
  list<CMail> mailList;
  set<string> users;
  CAttach   * att;

  CMailBox m0;

  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 15, 24, 13 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 1" ), nullptr ) ) );
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 15, 26, 23 ), "user2@fit.cvut.cz", CMailBody ( 22, "some different content" ), nullptr ) ) );
  att = new CAttach ( 200 );
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 11, 23, 43 ), "boss1@fit.cvut.cz", CMailBody ( 14, "urgent message" ), att ) ) );
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 18, 52, 27 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 2" ), att ) ) );
  att -> Release ();
  att = new CAttach ( 97 );
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 16, 12, 48 ), "boss1@fit.cvut.cz", CMailBody ( 24, "even more urgent message" ), att ) ) );
  att -> Release ();
#if 1
  assert ( showMail ( m0 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B + attachment: 97 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n" );
  assert ( showMail ( m0 . ListMail ( "inbox",
                      CTimeStamp ( 2014, 3, 31, 15, 26, 23 ),
                      CTimeStamp ( 2014, 3, 31, 16, 12, 48 ) ) ) == "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B + attachment: 97 B\n" );
  assert ( showUsers ( m0 . ListAddr ( CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                       CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "boss1@fit.cvut.cz\n"
                        "user1@fit.cvut.cz\n"
                        "user2@fit.cvut.cz\n" );
  assert ( showUsers ( m0 . ListAddr ( CTimeStamp ( 2014, 3, 31, 15, 26, 23 ),
                       CTimeStamp ( 2014, 3, 31, 16, 12, 48 ) ) ) == "boss1@fit.cvut.cz\n"
                        "user2@fit.cvut.cz\n" );

  CMailBox m1;
  assert ( m1 . NewFolder ( "work" ) );
  assert ( m1 . NewFolder ( "spam" ) );
  assert ( !m1 . NewFolder ( "spam" ) );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 15, 24, 13 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 1" ), nullptr ) ) );
  att = new CAttach ( 500 );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 15, 26, 23 ), "user2@fit.cvut.cz", CMailBody ( 22, "some different content" ), att ) ) );
  att -> Release ();
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 11, 23, 43 ), "boss1@fit.cvut.cz", CMailBody ( 14, "urgent message" ), nullptr ) ) );
  att = new CAttach ( 468 );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 18, 52, 27 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 2" ), att ) ) );
  att -> Release ();
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 16, 12, 48 ), "boss1@fit.cvut.cz", CMailBody ( 24, "even more urgent message" ), nullptr ) ) );
  assert ( showMail ( m1 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n" );
  assert ( showMail ( m1 . ListMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "" );
  assert ( m1 . MoveMail ( "inbox", "work" ) );
  assert ( showMail ( m1 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "" );
  assert ( showMail ( m1 . ListMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n" );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 19, 24, 13 ), "user2@fit.cvut.cz", CMailBody ( 14, "mail content 4" ), nullptr ) ) );
  att = new CAttach ( 234 );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 13, 26, 23 ), "user3@fit.cvut.cz", CMailBody ( 9, "complains" ), att ) ) );
  att -> Release ();
  assert ( showMail ( m1 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B\n"
                        "2014-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B\n" );
  assert ( showMail ( m1 . ListMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n" );
  assert ( m1 . MoveMail ( "inbox", "work" ) );
  assert ( showMail ( m1 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "" );
  assert ( showMail ( m1 . ListMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n"
                        "2014-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B\n" );
#endif
  return 0;
}
#endif /* __PROGTEST__ */
