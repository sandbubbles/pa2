#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */


struct TCitizen
{
    string                   m_Name;
    string                   m_Address;
    string                   m_Account;
    int                      m_SumIncome;
    int                      m_SumExpense;

                             TCitizen                      ( const string    & name,
                                                             const string    & address, 
                                                             const string    & account )
  : m_Name ( name ),
    m_Address ( address ),
    m_Account ( account ),
    m_SumIncome ( 0 ),
    m_SumExpense ( 0 ) 
  {
  }
};

class CIterator
{
  public:
                             CIterator                      ( const vector < TCitizen * > & pointers )
  : m_Pointers ( pointers ), 
    m_Index ( 0 )
  {
  }

    bool                     AtEnd                         ( void ) const;
    void                     Next                          ( void );
    string                   Name                          ( void ) const;
    string                   Addr                          ( void ) const;
    string                   Account                       ( void ) const;
  private:
    const vector < TCitizen * >  & m_Pointers;
    size_t                   m_Index;
};

//---------------------------------------------------------------------------------------------------------------------
bool                      CIterator::AtEnd                 ( void ) const
{
  if ( m_Index < m_Pointers . size ( ) )
    return false;
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void                      CIterator::Next                  ( void )
{
  ++m_Index;
}

//---------------------------------------------------------------------------------------------------------------------
string                    CIterator::Name                  ( void ) const
{
  return m_Pointers[m_Index] -> m_Name;
}

//---------------------------------------------------------------------------------------------------------------------
string                    CIterator::Addr                  ( void ) const
{
  return m_Pointers[m_Index] -> m_Address;
}

//---------------------------------------------------------------------------------------------------------------------
string                    CIterator::Account               ( void ) const
{
  return m_Pointers[m_Index] -> m_Account;
}

class CTaxRegister
{
  public:
                             CTaxRegister                  ( void );
                            ~CTaxRegister                  ( void );

    bool                     Birth                         ( const string    & name,
                                                             const string    & addr,
                                                             const string    & account );

    bool                     Death                         ( const string    & name,
                                                             const string    & addr );
                                                        
    bool                     Income                        ( const string    & account,
                                                             int               amount );
    bool                     Income                        ( const string    & name,
                                                             const string    & addr,
                                                             int               amount );
                                                      
    bool                     Expense                       ( const string    & account,
                                                             int               amount );
    bool                     Expense                       ( const string    & name,
                                                             const string    & addr,
                                                             int               amount );
                                                         
    bool                     Audit                         ( const string    & name,
                                                             const string    & addr,
                                                             string          & account,
                                                             int             & sumIncome,
                                                             int             & sumExpense ) const;
                                                                                                               
    CIterator                ListByName                    ( void ) const;

    bool                     FindByName                    ( const string    & toFindName,
                                                             const string    & toFindAddress,
                                                             size_t          & index ) const;

    bool                     FindByAccount                 ( const string    & toFind,
                                                             size_t          & index ) const;                                                         

  private:
    vector < TCitizen * >    m_ByNnA;
    vector < TCitizen * >    m_ByAccount;
};

//---------------------------------------------------------------------------------------------------------------------
                         CTaxRegister::CTaxRegister                  ( void )                   
{
}
//---------------------------------------------------------------------------------------------------------------------                                                       
                         CTaxRegister::~CTaxRegister                 ( void )
{
  for ( size_t i = 0; i < m_ByAccount . size ( ); i++ )
    delete m_ByAccount[i];
   
}
//---------------------------------------------------------------------------------------------------------------------
bool                     CTaxRegister::Birth                         ( const string    & name,
                                                                       const string    & addr,
                                                                       const string    & account )
{
  size_t index1, index2;
  if ( FindByName ( name, addr, index1 ) || FindByAccount ( account, index2 ) )
    return false;
  
  TCitizen * baby = new TCitizen ( name, addr, account);
  m_ByNnA     . insert ( m_ByNnA     . begin ( ) + index1, baby );
  m_ByAccount . insert ( m_ByAccount . begin ( ) + index2, baby );

  return true;
}
//---------------------------------------------------------------------------------------------------------------------
bool                     CTaxRegister::Death                         ( const string    & name,
                                                                       const string    & addr )
{
  size_t index1, index2;
  if ( ! FindByName ( name, addr, index1 ) )
    return false;

  TCitizen * c = m_ByNnA[index1];
  FindByAccount ( c->m_Account, index2 );

  delete c; 
  m_ByNnA     . erase ( m_ByNnA     . begin ( ) + index1 );
  m_ByAccount . erase ( m_ByAccount . begin ( ) + index2 );
  return true;
}                                                            
//---------------------------------------------------------------------------------------------------------------------                                                       
bool                     CTaxRegister::Income                        ( const string    & account,
                                                                       int               amount )
{
  size_t index;
  if ( ! FindByAccount ( account, index ) )
    return false;
 
  m_ByAccount[index] -> m_SumIncome += amount;
  return true; 
}

//---------------------------------------------------------------------------------------------------------------------
bool                     CTaxRegister::Income                        ( const string    & name,
                                                                       const string    & addr,
                                                                       int               amount )
{
  size_t index;
  if ( ! FindByName ( name, addr, index ) )
    return false;
  
  m_ByNnA[index] -> m_SumIncome += amount;
  return true;
}                                                                      
//---------------------------------------------------------------------------------------------------------------------
bool                     CTaxRegister::Expense                       ( const string    & account,
                                                                       int               amount )
{
  size_t index;
  if ( ! FindByAccount ( account, index ) )
    return false;
  
  m_ByAccount[index] -> m_SumExpense += amount;
  return true; 
} 

//---------------------------------------------------------------------------------------------------------------------
bool                     CTaxRegister::Expense                       ( const string    & name,
                                                                       const string    & addr,
                                                                       int               amount )
{
  size_t index;
  if ( ! FindByName ( name, addr, index ) )
    return false;
  
  m_ByNnA[index] -> m_SumExpense += amount;
  return true;
}                                                                       
//---------------------------------------------------------------------------------------------------------------------                                                        
bool                     CTaxRegister::Audit                         ( const string    & name,
                                                                       const string    & addr,
                                                                       string          & account,
                                                                       int             & sumIncome,
                                                                       int             & sumExpense ) const
{
  size_t index;
  if ( ! FindByName ( name, addr, index ) )
    return false;
  
  account        = m_ByNnA[index] -> m_Account;
  sumIncome      = m_ByNnA[index] -> m_SumIncome;
  sumExpense     = m_ByNnA[index] -> m_SumExpense;
  return true;
}                                                                     
//---------------------------------------------------------------------------------------------------------------------                                                 
CIterator                CTaxRegister::ListByName                    ( void ) const
{
  return CIterator ( m_ByNnA );
}

//--------------------------------------------------------------------------------------------------------------------- 

bool                     CTaxRegister::FindByName                    ( const string    & toFindName,
                                                                       const string    & toFindAddress,
                                                                       size_t          & index ) const                                             
{
  size_t rb = m_ByNnA . size ( );
  size_t lb = 0;
  size_t mid;

  while ( rb > lb )
  {
    mid = ( rb + lb ) / 2;

    if ( m_ByNnA[mid]-> m_Name == toFindName
        && m_ByNnA[mid]-> m_Address == toFindAddress)
    {
      index = mid;
      return true;
    }
    if ( m_ByNnA[mid]-> m_Name > toFindName
        || ( m_ByNnA[mid]-> m_Name == toFindName && m_ByNnA[mid]-> m_Address > toFindAddress ) )
      rb = mid;
    else
      lb = mid + 1;
  }
  index = lb;
  return false;
}  

//---------------------------------------------------------------------------------------------------------------------
bool                     CTaxRegister::FindByAccount                 ( const string    & toFind,
                                                                       size_t          & index ) const 
{
  size_t rb = m_ByAccount . size ( );
  size_t lb = 0;
  size_t mid;

  while ( rb > lb )
  {
    mid = ( rb + lb ) / 2;

    if ( m_ByAccount[mid]-> m_Account == toFind )
    {
      index = mid;
      return true;
    }
    if ( m_ByAccount[mid]-> m_Account > toFind )
      rb = mid;
    else
      lb = mid + 1;
  }
  index = lb;
  return false;
}                                                                                                                                

#ifndef __PROGTEST__
int                      main                                        ( void )
{
  string acct = " ";
  int    sumIncome = 0, sumExpense = 9;
  CTaxRegister b1;
 
  assert ( b1 . Birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  assert ( b1 . Birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  assert ( b1 . Birth ( "Peter Hacker", "Main Street 17", "634oddT" ) );
  assert ( b1 . Birth ( "John Smith", "Main Street 17", "Z343rwZ" ) );


  //assert ( b1 . Death ( "John Smith", "Main Street 17" ) );
  assert ( b1 . Income ( "Xuj5#94", 1000 ) );
  assert ( b1 . Income ( "634oddT", 2000 ) );
  assert ( b1 . Income ( "123/456/789", 3000 ) );
  assert ( b1 . Income ( "634oddT", 4000 ) );
  assert ( b1 . Income ( "Peter Hacker", "Main Street 17", 2000 ) );
  assert ( b1 . Expense ( "Jane Hacker", "Main Street 17", 2000 ) );
  assert ( b1 . Expense ( "John Smith", "Main Street 17", 500 ) );
  assert ( b1 . Expense ( "Jane Hacker", "Main Street 17", 1000 ) );
  assert ( b1 . Expense ( "Xuj5#94", 1300 ) );
  assert ( b1 . Audit ( "John Smith", "Oak Road 23", acct, sumIncome, sumExpense ) );
  assert ( acct == "123/456/789" );
  assert ( sumIncome == 3000 );
  assert ( sumExpense == 0 );
  assert ( b1 . Audit ( "Jane Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "Xuj5#94" );
  assert ( sumIncome == 1000 );
  assert ( sumExpense == 4300 );
  assert ( b1 . Audit ( "Peter Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "634oddT" );
  assert ( sumIncome == 8000 );
  assert ( sumExpense == 0 );
  assert ( b1 . Audit ( "John Smith", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "Z343rwZ" );
  assert ( sumIncome == 0 );
  assert ( sumExpense == 500 );
  CIterator it = b1 . ListByName ();
  assert ( ! it . AtEnd ()
           && it . Name () == "Jane Hacker"
           && it . Addr () == "Main Street 17"
           && it . Account () == "Xuj5#94" );
  it . Next ();
  assert ( ! it . AtEnd ()
           && it . Name () == "John Smith"
           && it . Addr () == "Main Street 17"
           && it . Account () == "Z343rwZ" );
  it . Next ();
  assert ( ! it . AtEnd ()
           && it . Name () == "John Smith"
           && it . Addr () == "Oak Road 23"
           && it . Account () == "123/456/789" );
  it . Next ();
  assert ( ! it . AtEnd ()
           && it . Name () == "Peter Hacker"
           && it . Addr () == "Main Street 17"
           && it . Account () == "634oddT" );
  it . Next ();
  assert ( it . AtEnd () );

  assert ( b1 . Death ( "John Smith", "Main Street 17" ) );

  CTaxRegister b2;
  assert ( b2 . Birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  assert ( b2 . Birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  assert ( !b2 . Income ( "634oddT", 4000 ) );
  assert ( !b2 . Expense ( "John Smith", "Main Street 18", 500 ) );
  assert ( !b2 . Audit ( "John Nowak", "Second Street 23", acct, sumIncome, sumExpense ) );
  assert ( !b2 . Death ( "Peter Nowak", "5-th Avenue" ) );
  assert ( !b2 . Birth ( "Jane Hacker", "Main Street 17", "4et689A" ) );
  assert ( !b2 . Birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
  assert ( b2 . Death ( "Jane Hacker", "Main Street 17" ) );
  assert ( b2 . Birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
  assert ( b2 . Audit ( "Joe Hacker", "Elm Street 23", acct, sumIncome, sumExpense ) );
  assert ( acct == "Xuj5#94" );
  assert ( sumIncome == 0 );
  assert ( sumExpense == 0 );
  assert ( !b2 . Birth ( "Joe Hacker", "Elm Street 23", "AAj5#94" ) );

  return 0;
}
#endif /* __PROGTEST__ */
