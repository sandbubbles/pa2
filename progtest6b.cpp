#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */


//=================================================================================================
/* Jelikoz mame nekolik typu bunek -> CEmpty, CImage, CText a CTable, ktere maji nektere promenne 
   stejne a jsou i metody, ktere sdili, hodi se udelat jim spolecneho rodice.
   Nicmene rodic CCell jako takovy se nezobrazuje v tabulce -> je to tedy abstraktni.
*/
class CCell
{
  public: 
/* Je tu jen konstruktor a neni tam zbytek metod do pravidla tri, protoze potrebuju aby se nad kazdym
   potomkem zavolal ten spravny destruktor, ale v CCell nam staci jen defaultni.
*/
    virtual                 ~CCell                         ( void ) = default;                     
    virtual size_t           Height                        ( void ) const;
    virtual size_t           Width                         ( void ) const;
     
    void                     SetHeight                     ( size_t            newsy );
    void                     SetWidth                      ( size_t            newsy );

/* Vypise vzdy jen jeden radek-> dostaneme na jakou vysku a sirku se mame zarovnat a jaky radek zrovna
   chceme vypsat.
*/    
    virtual ostream        & PrintLine                     ( ostream         & out,
                                                             size_t            hB,
                                                             size_t            wB,
                                                             size_t            me ) const = 0;
/* Vytvori novy cell s stejnym obsahem a vrati pointer na nej.
*/
    virtual CCell          * Clone                         ( void ) const = 0;  

    mutable size_t           m_Height = 0;
    mutable size_t           m_Width  = 0;
  
};
//================================================================================================= 
size_t                       CCell::Height                 ( void ) const
{
  return m_Height;
}

//-------------------------------------------------------------------------------------------------
size_t                       CCell::Width                  ( void ) const
{
  return m_Width;
}

//-------------------------------------------------------------------------------------------------
void                         CCell::SetHeight              ( size_t            newsy )
{
  m_Height = newsy;
}

//-------------------------------------------------------------------------------------------------
void                         CCell::SetWidth               ( size_t            newsy ) 
{
  m_Width = newsy;
} 

//=================================================================================================
class CText : public CCell
{
  public:
    enum ALIGNMENT 
    { 
      ALIGN_LEFT, ALIGN_RIGHT 
    };
                             CText                         ( const string    & text,
                                                             ALIGNMENT         align );
    CText                  & SetText                       ( const string    & text );
    virtual ostream        & PrintLine                     ( ostream         & out,
                                                             size_t            hB,
                                                             size_t            wB,
                                                             size_t            me ) const override;
    virtual CText          * Clone                         ( void ) const override;
    
  private:
    string                   m_Text;
    ALIGNMENT                m_Align;

/* Zjisti z stringu svuj nejdelsi radek, vysku a nastavi sirku, vysku bunky.
*/
    void                     ChurnIt                       ( void );
};
//=================================================================================================
                             CText::CText                  ( const string    & text,
                                                             ALIGNMENT         align )
: m_Text( text ),
  m_Align ( align )
{
  ChurnIt (); 
}

//-------------------------------------------------------------------------------------------------
CText                      & CText::SetText                ( const string    & text )
{
  m_Text = text;
  ChurnIt ();
  return *this; 
}

//-------------------------------------------------------------------------------------------------
ostream                    & CText::PrintLine              ( ostream         & out,
                                                             size_t            hB,
                                                             size_t            wB,
                                                             size_t            me ) const
{
  /* paklize vypisujeme radek prazdny (jen pro zarovnani), vypisi prazdny radek.
  */ 
  if ( me > m_Height )
    return out << setw ( wB ) << setfill ( ' ' ) << "";

  /* V tomto foru preskakam vsechny radky az do toho ktery chceme vypsat
  */
  size_t pos = 0 ;
  for ( size_t i = 1; i < me ; i++, pos++ )
    pos = m_Text . find ( '\n', pos ); 

  string toPrint = m_Text . substr ( pos , m_Text . find ( '\n', pos ) - pos  );

  if ( m_Align == ALIGN_LEFT )
    toPrint . append ( wB - toPrint . size (), ' ' );
  else 
    toPrint . insert ( 0, wB - toPrint . size (), ' ' );
  
  return out << toPrint;
}
//-------------------------------------------------------------------------------------------------
CText                      * CText::Clone                  ( void ) const
{
  CText * res = new CText ( m_Text, m_Align );
  res -> m_Height  = m_Height;
  res -> m_Width   = m_Width;
  return res;
}

//-------------------------------------------------------------------------------------------------
void                         CText::ChurnIt                       ( void )
{
  size_t index = 0;
  size_t tmplen = 0;
  size_t pos;
  size_t height = 0 , width = 0;
  while ( index < m_Text . size () )
  {
    ++height;
    pos = m_Text . find ( '\n', index );
    if ( pos == string::npos )
    {
      tmplen = m_Text . size () - index ;
      if ( tmplen > width )
        width = tmplen;
      break;
    }
    tmplen = pos - index;
    if ( tmplen > width )
      width = tmplen;
    index = pos + 1;
  }
  m_Width = width;
  m_Height = height;  
}

//=================================================================================================
class CEmpty : public CCell
{
    virtual ostream        & PrintLine                     ( ostream         & out,
                                                             size_t            hB,
                                                             size_t            wB,
                                                             size_t            me ) const override;
    virtual CEmpty         * Clone                         ( void ) const override;
};
//=================================================================================================
ostream                    & CEmpty::PrintLine             ( ostream         & out,
                                                             size_t            hB,
                                                             size_t            wB,
                                                             size_t            me ) const
{
  return out << setw ( wB ) << setfill ( ' ' ) << "";
}

//------------------------------------------------------------------------------------------------
CEmpty                     * CEmpty::Clone                 ( void ) const
{
  CEmpty * res = new CEmpty;
  res -> m_Height = m_Height;
  res -> m_Width  = m_Width;
  return res;
}

//=================================================================================================
class CImage : public CCell
{
  public:
    CImage                 & AddRow                        ( const string    & text );
    virtual ostream        & PrintLine                     ( ostream         & out,
                                                             size_t            hB,
                                                             size_t            wB,
                                                             size_t            me ) const override;
    virtual CImage         * Clone                         ( void ) const override;
    
  private:
    vector<string>           m_Image; 
};
//=================================================================================================
CImage                     & CImage::AddRow                ( const string    & text )
{
  if ( m_Image . empty () )
      m_Width = text . size ();
  m_Image . push_back ( text );
  ++m_Height;
  return *this;
}

//-------------------------------------------------------------------------------------------------
ostream                    & CImage::PrintLine             ( ostream         & out,
                                                             size_t            hB,
                                                             size_t            wB,
                                                             size_t            me ) const
{
  /* Vypocitame offsety pro zarovnani
  */
  size_t offL = ( wB - m_Width ) / 2;                                        //left
  size_t offR = wB - m_Width - offL;                                         //right
                                          
  size_t offU = ( hB - m_Height ) / 2;                                       //up
  size_t offB = hB - m_Height - offU;                                        //bellow
  
  if ( me <= offU || me > hB - offB )
    return out << setw ( wB ) << setfill ( ' ' ) << "";

  out << setw ( offL ) << setfill ( ' ' ) << "" ;
  out << m_Image[me - offU - 1];
  return out << setw ( offR ) << setfill ( ' ' ) <<"";
}

//-------------------------------------------------------------------------------------------------
CImage                     * CImage::Clone                 ( void ) const
{
  CImage * res = new CImage;
  res -> m_Image  = m_Image;
  res -> m_Height = m_Height;
  res -> m_Width  = m_Width;
  return res;
}

//=================================================================================================
class CTable : public CCell
{
  public:
                             CTable                        ( size_t            rows,
                                                             size_t            cols );
                             CTable                        ( const CTable    & x );
                             CTable                        ( CTable         && x );
                            ~CTable                        ( void );
    CCell                  & GetCell                       ( size_t            row,
                                                             size_t            col );
    bool                     SetCell                       ( size_t            row,
                                                             size_t            col,
                                                             const CCell     & newContent );
    CTable &                 operator =                    ( CTable            x );
    friend ostream         & operator <<                   ( ostream         & out,
                                                             const CTable    & x );  
    virtual ostream        & PrintLine                     ( ostream         & out,
                                                             size_t            hB,
                                                             size_t            wB,
                                                             size_t            me ) const override;
    virtual CTable         * Clone                         ( void ) const override;  
    virtual size_t           Height                        ( void ) const override;
    virtual size_t           Width                         ( void ) const override;
    
  private:
    size_t                   m_Rows;
    size_t                   m_Cols;
    vector<vector<CCell*>>   m_Cells;
   
/* Jsou mutable, protoze jen zjisti jake ma mit zarovnani, a zda se stala zmena od posledniho pocitani
*/
    mutable vector<size_t>   m_RMax;
    mutable vector<size_t>   m_CMax;
    mutable stringstream     m_Dump;
    mutable bool             m_ShouldDump = true;
/* Jen zjisti sve spravne rozmery sirky a vysky jednotlivych radek a sloupcich, nemeni obsah bunek
*/
    void                     SetSizes                      ( void ) const;
    void                     SetSize                       ( size_t            row,
                                                             size_t            col ) const;
    void                     SetRow                        ( size_t            row ) const;
    void                     SetCol                        ( size_t            col ) const;
};
//=================================================================================================
                             CTable::CTable                ( size_t            rows,
                                                             size_t            cols )
: m_Rows ( rows ),
  m_Cols ( cols )
{
  m_Cells . resize ( m_Rows, vector<CCell*> (m_Cols) );
  m_RMax . resize ( m_Rows, 0 );
  m_CMax . resize ( m_Cols, 0 );

  for ( size_t i = 0; i < m_Rows; i++ )
    for ( size_t j = 0; j < m_Cols; j++ )
    {
      CEmpty * celly = new CEmpty;
      m_Cells[i][j] = celly;
    }
}
//-------------------------------------------------------------------------------------------------
                             CTable::CTable                ( const CTable     & x )
: m_Rows ( x . m_Rows ),
  m_Cols ( x . m_Cols ),
  m_RMax ( x . m_RMax ),
  m_CMax ( x . m_CMax ),
  m_ShouldDump ( x . m_ShouldDump )
{
  m_Cells . resize ( m_Rows , vector<CCell *> ( m_Cols ) );

  for ( size_t i = 0; i < m_Rows; i++ )
    for ( size_t j = 0; j < m_Cols; j++ )
      m_Cells[i][j] = x . m_Cells[i][j]-> Clone();
}
//-------------------------------------------------------------------------------------------------
/* Mam rvalue, staci mi jen movenout vsechny data (vcetne vektoru pointeru na CCell)
*/
                             CTable::CTable                ( CTable         && x )
: m_Rows  ( move ( x . m_Rows ) ),
  m_Cols  ( move ( x . m_Cols ) ),
  m_Cells ( move ( x . m_Cells ) ),
  m_RMax  ( move ( x . m_RMax ) ),
  m_CMax  ( move ( x . m_CMax ) ),
  m_Dump  ( move ( x . m_Dump ) ),
  m_ShouldDump ( move (  x . m_ShouldDump ) )
{
}
//-------------------------------------------------------------------------------------------------
                             CTable::~CTable               ( void )
{
  for ( size_t i = 0; i < m_Rows; i++ )
   for ( size_t j = 0; j < m_Cols; j++ )
    delete m_Cells[i][j];
}
//-------------------------------------------------------------------------------------------------
CCell                      & CTable::GetCell               ( size_t            row,
                                                             size_t            col )
{
  if ( row >= m_Rows || col >= m_Cols )
    cout<< "co s tim"<< endl;
    //invalid argument vyjimka by to mohla vyresit

  return * ( m_Cells[row][col] );
}

//-------------------------------------------------------------------------------------------------
bool                         CTable::SetCell               ( size_t            row,
                                                             size_t            col,
                                                             const CCell     & newContent )
{
  if (row >= m_Rows || col >= m_Cols)
  {
   cout<< "naughty" << endl;
   return false;
  }
  
  CCell * tmp = newContent . Clone ();

  delete m_Cells[row][col]; 
  m_Cells[row][col] = tmp;


  return true;
}

//-------------------------------------------------------------------------------------------------
CTable &                     CTable::operator =            ( CTable            x )
{
 swap ( m_Rows, x . m_Rows );
 swap ( m_Cols, x . m_Cols );
 swap ( m_Cells, x . m_Cells );
 swap ( m_RMax, x . m_RMax );
 swap ( m_CMax, x . m_CMax );
 swap ( m_Dump, x . m_Dump );
 swap ( m_ShouldDump, x . m_ShouldDump );

 return *this;
}
//-------------------------------------------------------------------------------------------------

ostream                    & operator <<                   ( ostream         & out,
                                                             const CTable    & x )
{
  //vyprazydni whatever
  x . SetSizes ();

  /* Iteruji pres radky CTable, pres vysku toho radku, a pote pres slouce CTable. 
  */

  for ( size_t i = 0; i < x . m_Rows; i++ )
  { 
    out << "+";
    for ( size_t j = 0; j < x . m_Cols; j++ )
      out << setw ( x . m_CMax[j] ) << setfill ( '-' ) << "" << "+";
    out << "\n";
    for ( size_t me = 1; me <= x . m_RMax[i]; me++)
    {
      for ( size_t j = 0; j < x . m_Cols; j++ )
      {
        out << "|";
        x . m_Cells[i][j] -> PrintLine ( out, x . m_RMax[i], x . m_CMax[j], me  ) ;
      }
      out << "|" << endl; 
    }
  }
  out << "+";
  for ( size_t j = 0; j < x . m_Cols; j++ )
    out << setw ( x . m_CMax[j] ) << setfill ( '-' ) << "" << "+";
  return out << endl;
}
//-------------------------------------------------------------------------------------------------
void                         CTable::SetSizes               ( void ) const
{
  /* pres kazdy radek najit maximum a zapsat ho vsude. Pote pro vsechny sloupce;
  */
  for ( size_t i = 0; i < m_Rows; i++ )
    for ( size_t j = 0; j < m_Cols; j++ )
    {
       m_RMax[i] = 0;
       m_CMax[j] = 0; 
       SetSize (i, j);
    }
}
//-------------------------------------------------------------------------------------------------
void                         CTable::SetSize               ( size_t            row,
                                                             size_t            col ) const
{
  for ( size_t j = 0; j < m_Cols; j++ )
    m_RMax[row] = max ( m_RMax[row], m_Cells[row][j] -> Height () );
  for ( size_t i = 0; i < m_Rows; i++ )
    m_CMax[col] = max ( m_CMax[col], m_Cells[i][col] -> Width () );
}

//-------------------------------------------------------------------------------------------------
void                         CTable::SetRow                ( size_t            row ) const
{
  for ( size_t j = 0; j < m_Cols; j++ )
    m_RMax[row] = max ( m_RMax[row], m_Cells[row][j] -> Height () );
}

//-------------------------------------------------------------------------------------------------
void                         CTable::SetCol                ( size_t            col ) const
{
  for ( size_t i = 0; i < m_Rows; i++ )
    m_CMax[col] = max ( m_CMax[col], m_Cells[i][col] -> Width () );
}

//-------------------------------------------------------------------------------------------------
ostream                    & CTable::PrintLine             ( ostream         & out,
                                                             size_t            hB,
                                                             size_t            wB,
                                                             size_t            line ) const
{
  if ( line > m_Height )
  {
    m_ShouldDump = true;
    return out << setw ( wB ) << setfill ( ' ' ) << "";
  }
  
  if ( m_ShouldDump )
  {
    m_Dump . str ("");
    m_Dump . clear ();
    m_Dump << *this;
    m_ShouldDump = false;
  }
  if ( line == hB )
    m_ShouldDump = true;

  string toPrint;
  getline ( m_Dump, toPrint );

  return out << toPrint << setw ( wB - m_Width ) << setfill ( ' ' ) << "" ;
}

//------------------------------------------------------------------------------------------------
CTable                     * CTable::Clone                 ( void ) const
{
  CTable * tmp =  new CTable ( *this );
  return tmp;
}

//-------------------------------------------------------------------------------------------------
size_t                       CTable::Height                ( void ) const
{
  SetSizes();
  m_Height = m_Rows + 1;

  for ( size_t i = 0; i < m_Rows; i++ )
    m_Height += m_RMax[i];
  
  return m_Height;
}
//-------------------------------------------------------------------------------------------------
size_t                       CTable::Width                 ( void ) const
{
  SetSizes();
  m_Width = m_Cols + 1;

  for ( size_t j = 0; j < m_Cols; j++ )
    m_Width += m_CMax[j];
  
  return m_Width;
}
//-------------------------------------------------------------------------------------------------

#ifndef __PROGTEST__
int main ( void )
{
  ostringstream oss;
   CTable t0 ( 3, 2 );
   t0 . SetCell ( 0, 0, CText ( "Hello,\n"
        "Hello Kitty", CText::ALIGN_LEFT ) );
   t0 . SetCell ( 1, 0, CText ( "Lorem ipsum dolor sit amet", CText::ALIGN_LEFT ) );
   t0 . SetCell ( 2, 0, CText ( "Bye,\n"
        "Hello Kitty", CText::ALIGN_RIGHT ) );
   t0 . SetCell ( 1, 1, CImage ()
          . AddRow ( "###                   " )
          . AddRow ( "#  #                  " )
          . AddRow ( "#  # # ##   ###    ###" )
          . AddRow ( "###  ##    #   #  #  #" )
          . AddRow ( "#    #     #   #  #  #" )
          . AddRow ( "#    #     #   #  #  #" )
          . AddRow ( "#    #      ###    ###" )
          . AddRow ( "                     #" )
          . AddRow ( "                   ## " )
          . AddRow ( "                      " )
          . AddRow ( " #    ###   ###   #   " )
          . AddRow ( "###  #   # #     ###  " )
          . AddRow ( " #   #####  ###   #   " )
          . AddRow ( " #   #         #  #   " )
          . AddRow ( "  ##  ###   ###    ## " ) );
   t0 . SetCell ( 2, 1, CEmpty () );
   oss . str ("");
   oss . clear ();
   oss << t0;
   assert ( oss . str () ==
        "+--------------------------+----------------------+\n"
        "|Hello,                    |                      |\n"
        "|Hello Kitty               |                      |\n"
        "+--------------------------+----------------------+\n"
        "|Lorem ipsum dolor sit amet|###                   |\n"
        "|                          |#  #                  |\n"
        "|                          |#  # # ##   ###    ###|\n"
        "|                          |###  ##    #   #  #  #|\n"
        "|                          |#    #     #   #  #  #|\n"
        "|                          |#    #     #   #  #  #|\n"
        "|                          |#    #      ###    ###|\n"
        "|                          |                     #|\n"
        "|                          |                   ## |\n"
        "|                          |                      |\n"
        "|                          | #    ###   ###   #   |\n"
        "|                          |###  #   # #     ###  |\n"
        "|                          | #   #####  ###   #   |\n"
        "|                          | #   #         #  #   |\n"
        "|                          |  ##  ###   ###    ## |\n"
        "+--------------------------+----------------------+\n"
        "|                      Bye,|                      |\n"
        "|               Hello Kitty|                      |\n"
        "+--------------------------+----------------------+\n" );
   t0 . SetCell ( 0, 1, t0 . GetCell ( 1, 1 ) );
   t0 . SetCell ( 2, 1, CImage ()
          . AddRow ( "*****   *      *  *      ******* ******  *" )
          . AddRow ( "*    *  *      *  *      *            *  *" )
          . AddRow ( "*    *  *      *  *      *           *   *" )
          . AddRow ( "*    *  *      *  *      *****      *    *" )
          . AddRow ( "****    *      *  *      *         *     *" )
          . AddRow ( "*  *    *      *  *      *        *       " )
          . AddRow ( "*   *   *      *  *      *       *       *" )
          . AddRow ( "*    *    *****   ****** ******* ******  *" ) );
   dynamic_cast<CText &> ( t0 . GetCell ( 1, 0 ) ) . SetText ( "Lorem ipsum dolor sit amet,\n"
        "consectetur adipiscing\n"
        "elit. Curabitur scelerisque\n"
        "lorem vitae lectus cursus,\n"
        "vitae porta ante placerat. Class aptent taciti\n"
        "sociosqu ad litora\n"
        "torquent per\n"
        "conubia nostra,\n"
        "per inceptos himenaeos.\n"
        "\n"
        "Donec tincidunt augue\n"
        "sit amet metus\n"
        "pretium volutpat.\n"
        "Donec faucibus,\n"
        "ante sit amet\n"
        "luctus posuere,\n"
        "mauris tellus" );
   oss . str ("");
   oss . clear ();
   oss << t0;
   assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
   CTable t1 ( t0 );
   t1 . SetCell ( 1, 0, CEmpty () );
   t1 . SetCell ( 1, 1, CEmpty () );
   oss . str ("");
   oss . clear ();
   oss << t0;
   assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
   oss . str ("");
   oss . clear ();
   oss << t1;
   assert ( oss . str () ==
        "+-----------+------------------------------------------+\n"
        "|Hello,     |          ###                             |\n"
        "|Hello Kitty|          #  #                            |\n"
        "|           |          #  # # ##   ###    ###          |\n"
        "|           |          ###  ##    #   #  #  #          |\n"
        "|           |          #    #     #   #  #  #          |\n"
        "|           |          #    #     #   #  #  #          |\n"
        "|           |          #    #      ###    ###          |\n"
        "|           |                               #          |\n"
        "|           |                             ##           |\n"
        "|           |                                          |\n"
        "|           |           #    ###   ###   #             |\n"
        "|           |          ###  #   # #     ###            |\n"
        "|           |           #   #####  ###   #             |\n"
        "|           |           #   #         #  #             |\n"
        "|           |            ##  ###   ###    ##           |\n"
        "+-----------+------------------------------------------+\n"
        "+-----------+------------------------------------------+\n"
        "|       Bye,|*****   *      *  *      ******* ******  *|\n"
        "|Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|           |*    *  *      *  *      *           *   *|\n"
        "|           |*    *  *      *  *      *****      *    *|\n"
        "|           |****    *      *  *      *         *     *|\n"
        "|           |*  *    *      *  *      *        *       |\n"
        "|           |*   *   *      *  *      *       *       *|\n"
        "|           |*    *    *****   ****** ******* ******  *|\n"
        "+-----------+------------------------------------------+\n" );
   t1 = t0;
   t1 . SetCell ( 0, 0, CEmpty () );
   t1 . SetCell ( 1, 1, CImage ()
          . AddRow ( "  ********                    " )
          . AddRow ( " **********                   " )
          . AddRow ( "**        **                  " )
          . AddRow ( "**             **        **   " )
          . AddRow ( "**             **        **   " )
          . AddRow ( "***         ********  ********" )
          . AddRow ( "****        ********  ********" )
          . AddRow ( "****           **        **   " )
          . AddRow ( "****           **        **   " )
          . AddRow ( "****      **                  " )
          . AddRow ( " **********                   " )
          . AddRow ( "  ********                    " ) );
   oss . str ("");
   oss . clear ();
   oss << t0;
   assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
   oss . str ("");
   oss . clear ();
   oss << t1;
   assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                              |          ###                             |\n"
        "|                                              |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |                                          |\n"
        "|elit. Curabitur scelerisque                   |        ********                          |\n"
        "|lorem vitae lectus cursus,                    |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti|      **        **                        |\n"
        "|sociosqu ad litora                            |      **             **        **         |\n"
        "|torquent per                                  |      **             **        **         |\n"
        "|conubia nostra,                               |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                       |      ****        ********  ********      |\n"
        "|                                              |      ****           **        **         |\n"
        "|Donec tincidunt augue                         |      ****           **        **         |\n"
        "|sit amet metus                                |      ****      **                        |\n"
        "|pretium volutpat.                             |       **********                         |\n"
        "|Donec faucibus,                               |        ********                          |\n"
        "|ante sit amet                                 |                                          |\n"
        "|luctus posuere,                               |                                          |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
   CTable t2 ( 2, 2 );
   t2 . SetCell ( 0, 0, CText ( "OOP", CText::ALIGN_LEFT ) );
   t2 . SetCell ( 0, 1, CText ( "Encapsulation", CText::ALIGN_LEFT ) );
   t2 . SetCell ( 1, 0, CText ( "Polymorphism", CText::ALIGN_LEFT ) );
   t2 . SetCell ( 1, 1, CText ( "Inheritance", CText::ALIGN_LEFT ) );
   oss . str ("");
   oss . clear ();
   oss << t2;
   assert ( oss . str () ==
        "+------------+-------------+\n"
        "|OOP         |Encapsulation|\n"
        "+------------+-------------+\n"
        "|Polymorphism|Inheritance  |\n"
        "+------------+-------------+\n" );
   t1 . SetCell ( 0, 0, t2 );
   dynamic_cast<CText &> ( t2 . GetCell ( 0, 0 ) ) . SetText ( "Object Oriented Programming" );
   oss . str ("");
   oss . clear ();
   oss << t2;
   assert ( oss . str () ==
        "+---------------------------+-------------+\n"
        "|Object Oriented Programming|Encapsulation|\n"
        "+---------------------------+-------------+\n"
        "|Polymorphism               |Inheritance  |\n"
        "+---------------------------+-------------+\n" );
   oss . str ("");
   oss . clear ();
   oss << t1;
   //cout << t1;
   assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|+------------+-------------+                  |          ###                             |\n"
        "||OOP         |Encapsulation|                  |          #  #                            |\n"
        "|+------------+-------------+                  |          #  # # ##   ###    ###          |\n"
        "||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          |\n"
        "|+------------+-------------+                  |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |                                          |\n"
        "|elit. Curabitur scelerisque                   |        ********                          |\n"
        "|lorem vitae lectus cursus,                    |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti|      **        **                        |\n"
        "|sociosqu ad litora                            |      **             **        **         |\n"
        "|torquent per                                  |      **             **        **         |\n"
        "|conubia nostra,                               |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                       |      ****        ********  ********      |\n"
        "|                                              |      ****           **        **         |\n"
        "|Donec tincidunt augue                         |      ****           **        **         |\n"
        "|sit amet metus                                |      ****      **                        |\n"
        "|pretium volutpat.                             |       **********                         |\n"
        "|Donec faucibus,                               |        ********                          |\n"
        "|ante sit amet                                 |                                          |\n"
        "|luctus posuere,                               |                                          |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
   CTable t5= t1;
   dynamic_cast<CTable &>(t5. GetCell(0,0)).SetCell (0, 1, CImage());
   dynamic_cast<CImage &>(dynamic_cast<CTable &> (t5. GetCell(0,0)). GetCell(0, 1)). AddRow("#    #      ###    ###"). AddRow("#    #      ###    ###");
   //cout << t5;
   t1 . SetCell ( 0, 0, t1 );
   oss . str ("");
   oss . clear ();
   oss << t1;
   assert ( oss . str () ==
        "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|+----------------------------------------------+------------------------------------------+|                                          |\n"
        "||+------------+-------------+                  |          ###                             ||                                          |\n"
        "|||OOP         |Encapsulation|                  |          #  #                            ||                                          |\n"
        "||+------------+-------------+                  |          #  # # ##   ###    ###          ||                                          |\n"
        "|||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          ||                                          |\n"
        "||+------------+-------------+                  |          #    #     #   #  #  #          ||                                          |\n"
        "||                                              |          #    #     #   #  #  #          ||                                          |\n"
        "||                                              |          #    #      ###    ###          ||                                          |\n"
        "||                                              |                               #          ||                                          |\n"
        "||                                              |                             ##           ||                                          |\n"
        "||                                              |                                          ||                                          |\n"
        "||                                              |           #    ###   ###   #             ||                                          |\n"
        "||                                              |          ###  #   # #     ###            ||                                          |\n"
        "||                                              |           #   #####  ###   #             ||                                          |\n"
        "||                                              |           #   #         #  #             ||          ###                             |\n"
        "||                                              |            ##  ###   ###    ##           ||          #  #                            |\n"
        "|+----------------------------------------------+------------------------------------------+|          #  # # ##   ###    ###          |\n"
        "||Lorem ipsum dolor sit amet,                   |                                          ||          ###  ##    #   #  #  #          |\n"
        "||consectetur adipiscing                        |                                          ||          #    #     #   #  #  #          |\n"
        "||elit. Curabitur scelerisque                   |        ********                          ||          #    #     #   #  #  #          |\n"
        "||lorem vitae lectus cursus,                    |       **********                         ||          #    #      ###    ###          |\n"
        "||vitae porta ante placerat. Class aptent taciti|      **        **                        ||                               #          |\n"
        "||sociosqu ad litora                            |      **             **        **         ||                             ##           |\n"
        "||torquent per                                  |      **             **        **         ||                                          |\n"
        "||conubia nostra,                               |      ***         ********  ********      ||           #    ###   ###   #             |\n"
        "||per inceptos himenaeos.                       |      ****        ********  ********      ||          ###  #   # #     ###            |\n"
        "||                                              |      ****           **        **         ||           #   #####  ###   #             |\n"
        "||Donec tincidunt augue                         |      ****           **        **         ||           #   #         #  #             |\n"
        "||sit amet metus                                |      ****      **                        ||            ##  ###   ###    ##           |\n"
        "||pretium volutpat.                             |       **********                         ||                                          |\n"
        "||Donec faucibus,                               |        ********                          ||                                          |\n"
        "||ante sit amet                                 |                                          ||                                          |\n"
        "||luctus posuere,                               |                                          ||                                          |\n"
        "||mauris tellus                                 |                                          ||                                          |\n"
        "|+----------------------------------------------+------------------------------------------+|                                          |\n"
        "||                                          Bye,|*****   *      *  *      ******* ******  *||                                          |\n"
        "||                                   Hello Kitty|*    *  *      *  *      *            *  *||                                          |\n"
        "||                                              |*    *  *      *  *      *           *   *||                                          |\n"
        "||                                              |*    *  *      *  *      *****      *    *||                                          |\n"
        "||                                              |****    *      *  *      *         *     *||                                          |\n"
        "||                                              |*  *    *      *  *      *        *       ||                                          |\n"
        "||                                              |*   *   *      *  *      *       *       *||                                          |\n"
        "||                                              |*    *    *****   ****** ******* ******  *||                                          |\n"
        "|+----------------------------------------------+------------------------------------------+|                                          |\n"
        "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                                                                |                                          |\n"
        "|consectetur adipiscing                                                                     |                                          |\n"
        "|elit. Curabitur scelerisque                                                                |        ********                          |\n"
        "|lorem vitae lectus cursus,                                                                 |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti                                             |      **        **                        |\n"
        "|sociosqu ad litora                                                                         |      **             **        **         |\n"
        "|torquent per                                                                               |      **             **        **         |\n"
        "|conubia nostra,                                                                            |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                                                                    |      ****        ********  ********      |\n"
        "|                                                                                           |      ****           **        **         |\n"
        "|Donec tincidunt augue                                                                      |      ****           **        **         |\n"
        "|sit amet metus                                                                             |      ****      **                        |\n"
        "|pretium volutpat.                                                                          |       **********                         |\n"
        "|Donec faucibus,                                                                            |        ********                          |\n"
        "|ante sit amet                                                                              |                                          |\n"
        "|luctus posuere,                                                                            |                                          |\n"
        "|mauris tellus                                                                              |                                          |\n"
        "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|                                                                                       Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                                                                Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                                                                           |*    *  *      *  *      *           *   *|\n"
        "|                                                                                           |*    *  *      *  *      *****      *    *|\n"
        "|                                                                                           |****    *      *  *      *         *     *|\n"
        "|                                                                                           |*  *    *      *  *      *        *       |\n"
        "|                                                                                           |*   *   *      *  *      *       *       *|\n"
        "|                                                                                           |*    *    *****   ****** ******* ******  *|\n"
        "+-------------------------------------------------------------------------------------------+------------------------------------------+\n" );
   t1 . SetCell ( 0, 0, t1 );
   oss . str ("");
   oss . clear ();
   oss << t1;
   //cout << t1;
   assert ( oss . str () ==
        "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
        "||+----------------------------------------------+------------------------------------------+|                                          ||                                          |\n"
        "|||+------------+-------------+                  |          ###                             ||                                          ||                                          |\n"
        "||||OOP         |Encapsulation|                  |          #  #                            ||                                          ||                                          |\n"
        "|||+------------+-------------+                  |          #  # # ##   ###    ###          ||                                          ||                                          |\n"
        "||||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          ||                                          ||                                          |\n"
        "|||+------------+-------------+                  |          #    #     #   #  #  #          ||                                          ||                                          |\n"
        "|||                                              |          #    #     #   #  #  #          ||                                          ||                                          |\n"
        "|||                                              |          #    #      ###    ###          ||                                          ||                                          |\n"
        "|||                                              |                               #          ||                                          ||                                          |\n"
        "|||                                              |                             ##           ||                                          ||                                          |\n"
        "|||                                              |                                          ||                                          ||                                          |\n"
        "|||                                              |           #    ###   ###   #             ||                                          ||                                          |\n"
        "|||                                              |          ###  #   # #     ###            ||                                          ||                                          |\n"
        "|||                                              |           #   #####  ###   #             ||                                          ||                                          |\n"
        "|||                                              |           #   #         #  #             ||          ###                             ||                                          |\n"
        "|||                                              |            ##  ###   ###    ##           ||          #  #                            ||                                          |\n"
        "||+----------------------------------------------+------------------------------------------+|          #  # # ##   ###    ###          ||                                          |\n"
        "|||Lorem ipsum dolor sit amet,                   |                                          ||          ###  ##    #   #  #  #          ||                                          |\n"
        "|||consectetur adipiscing                        |                                          ||          #    #     #   #  #  #          ||                                          |\n"
        "|||elit. Curabitur scelerisque                   |        ********                          ||          #    #     #   #  #  #          ||                                          |\n"
        "|||lorem vitae lectus cursus,                    |       **********                         ||          #    #      ###    ###          ||                                          |\n"
        "|||vitae porta ante placerat. Class aptent taciti|      **        **                        ||                               #          ||                                          |\n"
        "|||sociosqu ad litora                            |      **             **        **         ||                             ##           ||                                          |\n"
        "|||torquent per                                  |      **             **        **         ||                                          ||                                          |\n"
        "|||conubia nostra,                               |      ***         ********  ********      ||           #    ###   ###   #             ||                                          |\n"
        "|||per inceptos himenaeos.                       |      ****        ********  ********      ||          ###  #   # #     ###            ||                                          |\n"
        "|||                                              |      ****           **        **         ||           #   #####  ###   #             ||                                          |\n"
        "|||Donec tincidunt augue                         |      ****           **        **         ||           #   #         #  #             ||                                          |\n"
        "|||sit amet metus                                |      ****      **                        ||            ##  ###   ###    ##           ||          ###                             |\n"
        "|||pretium volutpat.                             |       **********                         ||                                          ||          #  #                            |\n"
        "|||Donec faucibus,                               |        ********                          ||                                          ||          #  # # ##   ###    ###          |\n"
        "|||ante sit amet                                 |                                          ||                                          ||          ###  ##    #   #  #  #          |\n"
        "|||luctus posuere,                               |                                          ||                                          ||          #    #     #   #  #  #          |\n"
        "|||mauris tellus                                 |                                          ||                                          ||          #    #     #   #  #  #          |\n"
        "||+----------------------------------------------+------------------------------------------+|                                          ||          #    #      ###    ###          |\n"
        "|||                                          Bye,|*****   *      *  *      ******* ******  *||                                          ||                               #          |\n"
        "|||                                   Hello Kitty|*    *  *      *  *      *            *  *||                                          ||                             ##           |\n"
        "|||                                              |*    *  *      *  *      *           *   *||                                          ||                                          |\n"
        "|||                                              |*    *  *      *  *      *****      *    *||                                          ||           #    ###   ###   #             |\n"
        "|||                                              |****    *      *  *      *         *     *||                                          ||          ###  #   # #     ###            |\n"
        "|||                                              |*  *    *      *  *      *        *       ||                                          ||           #   #####  ###   #             |\n"
        "|||                                              |*   *   *      *  *      *       *       *||                                          ||           #   #         #  #             |\n"
        "|||                                              |*    *    *****   ****** ******* ******  *||                                          ||            ##  ###   ###    ##           |\n"
        "||+----------------------------------------------+------------------------------------------+|                                          ||                                          |\n"
        "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
        "||Lorem ipsum dolor sit amet,                                                                |                                          ||                                          |\n"
        "||consectetur adipiscing                                                                     |                                          ||                                          |\n"
        "||elit. Curabitur scelerisque                                                                |        ********                          ||                                          |\n"
        "||lorem vitae lectus cursus,                                                                 |       **********                         ||                                          |\n"
        "||vitae porta ante placerat. Class aptent taciti                                             |      **        **                        ||                                          |\n"
        "||sociosqu ad litora                                                                         |      **             **        **         ||                                          |\n"
        "||torquent per                                                                               |      **             **        **         ||                                          |\n"
        "||conubia nostra,                                                                            |      ***         ********  ********      ||                                          |\n"
        "||per inceptos himenaeos.                                                                    |      ****        ********  ********      ||                                          |\n"
        "||                                                                                           |      ****           **        **         ||                                          |\n"
        "||Donec tincidunt augue                                                                      |      ****           **        **         ||                                          |\n"
        "||sit amet metus                                                                             |      ****      **                        ||                                          |\n"
        "||pretium volutpat.                                                                          |       **********                         ||                                          |\n"
        "||Donec faucibus,                                                                            |        ********                          ||                                          |\n"
        "||ante sit amet                                                                              |                                          ||                                          |\n"
        "||luctus posuere,                                                                            |                                          ||                                          |\n"
        "||mauris tellus                                                                              |                                          ||                                          |\n"
        "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
        "||                                                                                       Bye,|*****   *      *  *      ******* ******  *||                                          |\n"
        "||                                                                                Hello Kitty|*    *  *      *  *      *            *  *||                                          |\n"
        "||                                                                                           |*    *  *      *  *      *           *   *||                                          |\n"
        "||                                                                                           |*    *  *      *  *      *****      *    *||                                          |\n"
        "||                                                                                           |****    *      *  *      *         *     *||                                          |\n"
        "||                                                                                           |*  *    *      *  *      *        *       ||                                          |\n"
        "||                                                                                           |*   *   *      *  *      *       *       *||                                          |\n"
        "||                                                                                           |*    *    *****   ****** ******* ******  *||                                          |\n"
        "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
        "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                                                                                                             |                                          |\n"
        "|consectetur adipiscing                                                                                                                  |                                          |\n"
        "|elit. Curabitur scelerisque                                                                                                             |        ********                          |\n"
        "|lorem vitae lectus cursus,                                                                                                              |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti                                                                                          |      **        **                        |\n"
        "|sociosqu ad litora                                                                                                                      |      **             **        **         |\n"
        "|torquent per                                                                                                                            |      **             **        **         |\n"
        "|conubia nostra,                                                                                                                         |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                                                                                                                 |      ****        ********  ********      |\n"
        "|                                                                                                                                        |      ****           **        **         |\n"
        "|Donec tincidunt augue                                                                                                                   |      ****           **        **         |\n"
        "|sit amet metus                                                                                                                          |      ****      **                        |\n"
        "|pretium volutpat.                                                                                                                       |       **********                         |\n"
        "|Donec faucibus,                                                                                                                         |        ********                          |\n"
        "|ante sit amet                                                                                                                           |                                          |\n"
        "|luctus posuere,                                                                                                                         |                                          |\n"
        "|mauris tellus                                                                                                                           |                                          |\n"
        "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|                                                                                                                                    Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                                                                                                             Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                                                                                                                        |*    *  *      *  *      *           *   *|\n"
        "|                                                                                                                                        |*    *  *      *  *      *****      *    *|\n"
        "|                                                                                                                                        |****    *      *  *      *         *     *|\n"
        "|                                                                                                                                        |*  *    *      *  *      *        *       |\n"
        "|                                                                                                                                        |*   *   *      *  *      *       *       *|\n"
        "|                                                                                                                                        |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n" );

  return 0;
}
#endif /* __PROGTEST__ */
