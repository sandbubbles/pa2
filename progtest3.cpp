#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

class CBigInt
{
  public:
//-------------------------------------------------------------------------------------------------
                             CBigInt                       ( void )
    : m_Sign ( false ),
      m_Number ("0")
    {}

//-------------------------------------------------------------------------------------------------
                             CBigInt                       ( int32_t           number )
    : m_Sign ( number < 0 ),
      m_Number ( to_string ( abs ( ( int64_t ) number ) ) )
    {
        reverse ( m_Number . begin ( ), m_Number . end ( ) );
    }

//-------------------------------------------------------------------------------------------------
                             CBigInt                       ( const char *      number )
    : m_Sign ( false ),
      m_Number ( "" )
    {
        size_t index = 0;
        if ( ( ! isdigit ( number[index] ) && number[index] != '-'  && number[index] != '+') )
            throw invalid_argument ( "Invalid string argument" );

        if ( number[index] == '-' )
        {
            m_Sign = true;
            ++index;
        }

        if ( number[index] == '+' )
            ++index;

        size_t sizeN = strlen ( number );
        for ( ; index < sizeN; index ++ )
        {
            if ( ! isdigit ( number[index] ) )
                throw invalid_argument ( "Invalid string argument" );
            m_Number += number[index] ;
        }
        if ( m_Number . size ( ) == 0 )
            throw invalid_argument ( "Invalid string argument" );
        
        reverse ( m_Number . begin ( ), m_Number . end ( ) );
        absolutelyDecimateTrailingZeros();   
    }

//-------------------------------------------------------------------------------------------------  
    friend CBigInt           operator +                    ( const CBigInt   & lho, 
                                                             const CBigInt   & rho )
    {
        CBigInt tmp = lho;
        tmp += rho;
        return tmp;
    }

//-------------------------------------------------------------------------------------------------
    friend CBigInt           operator *                    ( const CBigInt   & lho, 
                                                             const CBigInt   & rho )
    {
        CBigInt tmp = lho;
        tmp *= rho;
        return tmp;
    }

//-------------------------------------------------------------------------------------------------
    CBigInt &                operator +=                   ( const CBigInt   & rho )
    {
       if (  m_Sign == rho . m_Sign )
       {
           m_Number = add ( m_Number, rho . m_Number  );
           absolutelyDecimateTrailingZeros ( );
           return *this;
       }
       
       int8_t meBig = cmp ( m_Number, rho . m_Number );

       if ( meBig == 1 )
           m_Number = sub ( m_Number, rho . m_Number );   
       else if ( meBig == -1)
       {
           m_Number = sub ( rho . m_Number, m_Number );         
           m_Sign ^= 1;
       }
       else
       {
           m_Number = "0";
           m_Sign = false;
       }
       
       absolutelyDecimateTrailingZeros();
       return *this;
    }

//-------------------------------------------------------------------------------------------------
    CBigInt &                operator *=                   ( const CBigInt   & rho )
    {
        m_Sign =  m_Sign != rho . m_Sign;
        m_Number = mul ( m_Number, rho . m_Number );

        absolutelyDecimateTrailingZeros();
        return *this;
    }

//-------------------------------------------------------------------------------------------------
    friend bool              operator <                    ( const CBigInt   & lho, 
                                                             const CBigInt   & rho )
    {
        return -1 == cmpTo ( lho, rho );

    }

//-------------------------------------------------------------------------------------------------
    friend bool              operator <=                   ( const CBigInt   & lho, 
                                                             const CBigInt   & rho )
    {
        return 0 >= cmpTo ( lho, rho );
    }

//-------------------------------------------------------------------------------------------------
    friend bool              operator >                    ( const CBigInt   & lho, 
                                                             const CBigInt   & rho )
    {
        return 1 == cmpTo ( lho, rho );
    }

//-------------------------------------------------------------------------------------------------
    friend bool              operator >=                   ( const CBigInt   & lho, 
                                                             const CBigInt   & rho )
    {
        return 0 <= cmpTo ( lho, rho );
    }

//-------------------------------------------------------------------------------------------------
    friend bool              operator ==                   ( const CBigInt   & lho, 
                                                             const CBigInt   & rho )
    {
        return 0 == cmpTo ( lho, rho );
    }

//-------------------------------------------------------------------------------------------------
    friend bool              operator !=                   ( const CBigInt   & lho, 
                                                             const CBigInt   & rho )
    {
        return 0 != cmpTo ( lho, rho );
    }

//-------------------------------------------------------------------------------------------------
    friend ostream &         operator <<                   ( ostream         & sout, 
                                                             const CBigInt   & self )
    {
        string tmp;
        tmp . reserve ( self . m_Number . size ( ) + 1 );

        if ( self . m_Number == "0")
        {
            sout << "0";
            return sout;
        }

        tmp += self . m_Sign ? "-" : "" ;
        for ( ssize_t index = self . m_Number . size ( ) - 1; index >= 0; index-- )
            tmp+= self . m_Number[index];
        sout << tmp;
        return sout;
    }    

//-------------------------------------------------------------------------------------------------
    friend istream &         operator >>                   ( istream         & sin, 
                                                             CBigInt         & self )
    {
        char c;
        bool sign = false;
        string tmp;

        sin >> c;
        if ( sin . eof ( ) )
            return sin;

        if ( ! isdigit ( c )  && c != '-' && c != '+' )
        {
            sin . setstate ( ios_base::failbit );
            return sin;
        }

        if ( c == '-' )
            sign = true;
        else if ( isdigit ( c ) )
            tmp +=  c ;

        while ( sin . get ( c ) )
        { 
            if ( ! isdigit ( c ) )
                break;
            tmp +=  c ;
        }

        if ( tmp == "" )
        {
            sin . setstate ( ios_base::failbit );
            return sin;
        }

        reverse ( tmp . begin ( ), tmp . end ( ) );
        self . m_Number = tmp;
        self . m_Sign =  sign;
        self . absolutelyDecimateTrailingZeros ( );
        sin . clear ();
        return sin;  
    }  

  private:
    bool                     m_Sign;
    string                   m_Number;

//-------------------------------------------------------------------------------------------------
    void                     absolutelyDecimateTrailingZeros ( void )
    {
        size_t index = m_Number . size ( ) - 1;
        while ( index > 0 )
        {
            if ( m_Number [index] != '0' )
                break;
            --index;
        }
        m_Number . erase ( ++index );
    }

//-------------------------------------------------------------------------------------------------
    static string            add                           ( const string    & left, 
                                                             const string    & right)
    {
        string result;
        int32_t  sum = 0, write = 0, carry = 0;
        size_t index = 0;

        const string  & longer  = ( left . size ( ) > right . size ( ) ) ? left : right ;
        const string  & shorter = ( left . size ( ) > right . size ( ) ) ? right : left ;

        size_t sizeL = longer . size ( ), sizeS = shorter . size ( );

        while ( index < sizeL  )
        {
            sum = longer[index] - '0' + carry;
            if ( index < sizeS )
                sum += shorter[index] - '0' ;

            write = sum % 10;
            carry = sum / 10;

            result +=  write + '0';
            index++;
        }
        if (carry == 1)
            result += "1";
        return result;      
    }
//-------------------------------------------------------------------------------------------------
    static string            sub                           ( const string    & larger, 
                                                             const string    & smaller)
    {
        string result;
        int32_t  sum = 0, write = 0, carry = 0;
        size_t index = 0;

        while ( index < larger . size ( )  )
        {
            sum = larger[index] - '0' - carry; 
            if ( index < smaller . size ( )  )
                sum -= smaller[index] - '0';

            if ( sum < 0 )
                carry = 1;

            write = ( sum + 10 ) % 10;
            result += write + '0';
            index++;
        }
        return result;
    }

//-------------------------------------------------------------------------------------------------
    static string            mul                           ( const string    & left, 
                                                             const string    & right)
    {
       string result = "0";
       int32_t  sum = 0, write = 0, carry = 0;

       for ( size_t index = 0; index < left . size ( ); index++ )
       {
           string tmp (index, '0');
           for ( size_t pos = 0; pos < right . size ( ); pos++ )
           {
               sum = ( left[index] - '0' ) * ( right[pos] - '0' ) + carry;
               write = sum % 10;
               tmp += write + '0';
               carry = sum / 10;
           }

           if ( carry != 0 )
           {
               tmp += carry + '0';
               carry = 0;
           }

           result = add ( result, tmp );
       }
       return result;

    }

//-------------------------------------------------------------------------------------------------
    static int8_t            cmp                           ( const string    & left, 
                                                             const string    & right)
    {
        if ( left . size () > right . size () )
            return 1;
        if ( left . size () < right . size () )
            return -1;

        for ( ssize_t index = left . size () - 1; index >= 0 ; index--  )
            if ( left[index] != right[index] )
                return ( left[index] > right[index] ) - ( left[index] < right[index] );
      
        return 0;
        
    }
//-------------------------------------------------------------------------------------------------

    static int8_t            cmpTo                         ( const CBigInt    & left, 
                                                             const CBigInt    & right)
    {
        if ( left . m_Number == "0" && right . m_Number == "0" )
            return 0;

        if ( left . m_Sign != right . m_Sign )
            return ( right . m_Sign > left . m_Sign ) - ( right . m_Sign < left . m_Sign );

        if ( left . m_Sign == false )
            return cmp ( left . m_Number, right . m_Number );
        
        return -1 * cmp ( left . m_Number, right . m_Number );
    }
  };



#ifndef __PROGTEST__
static bool equal ( const CBigInt & x, const char * val )
{
  ostringstream oss;
  oss << x;
  if ( oss . str () != val )
  {
    cout << "Reference : "<< val << " Moje : "<<  oss . str () << endl; 
    return false;
  }

  return true;
}
int main ( void )
{
  CBigInt a, b, c;

  istringstream is;
    
  a = 10;
  assert ( equal ( a, "10" ) );
  a = INT_MIN;
  assert ( equal ( a, "-2147483648" ) );
  a = INT_MAX;
  assert ( equal ( a, "2147483647" ) );
  b = 12;
  assert ( equal ( b, "12" ) );
  b = a;
  assert ( equal ( b, "2147483647" ) );
  b = 13;
  assert ( equal ( b, "13" ) );
  assert ( equal ( a, "2147483647" ) );
  a = 4;
  assert ( equal ( a, "4" ) );
  a += b;
  assert ( equal ( a, "17" ) );
  assert ( equal ( b, "13" ) );
  a = -6;
  b = -128;
  b += a;
  assert ( equal ( a, "-6" ) );
  assert ( equal ( b, "-134" ) );

  a = "0";
  b = 128;
  b += a;
  assert ( equal ( a, "0" ) );
  assert ( equal ( b, "128" ) );

  a = "108";
  b = -9;
  a += b;
  assert ( equal ( a, "99" ) );
  assert ( equal ( b, "-9" ) );


  a = INT_MIN;
  b = -34;
  b += a;
  assert ( equal ( a, "-2147483648" ));
  assert ( equal ( b, "-2147483682" ) );


  a = 6;
  b = -128;
  b += a;
  assert ( equal ( a, "6" ) );
  assert ( equal ( b, "-122" ) );

  
  a = "-6";
  b = 128;
  b += a;
  assert ( equal ( a, "-6" ) );
  assert ( equal ( b, "122" ) );

  a = INT_MIN;
  b = 34;
  b += a;
  assert ( equal ( a, "-2147483648" ) );
  assert ( equal ( b, "-2147483614" ) );


  a = INT_MIN;
  b = 34;
  c = a + b;
  assert ( equal ( a, "-2147483648" ) );
  assert ( equal ( b, "34" ) );
  assert ( equal ( c, "-2147483614" ) );

  a = INT_MIN;
  b = "34";
  c = a + b;
  assert ( equal ( a, "-2147483648" ) );
  assert ( equal ( b, "34" ) );
  assert ( equal ( c, "-2147483614" ) );

  a = INT_MAX;
  b = INT_MAX;
  c = a + b;
  assert ( equal ( a, "2147483647" ) );
  assert ( equal ( b, "2147483647" ) );
  assert ( equal ( c, "4294967294" ) );

  a = 462341030;
  b = 645;
  c = a + b;
  assert ( equal ( a, "462341030" ) );
  assert ( equal ( b, "645" ) );
  assert ( equal ( c, "462341675" ) );
  
  a = 10;
  a += 20;
  assert ( equal ( a, "30" ) );

  a = "-34";
  assert ( equal ( a, "-34" ) );

  a = "49";
  assert ( equal ( a, "49" ) );

  a = "+34";
  assert ( equal ( a, "34" ) );

  a = 10;
  a += 20;
  assert ( equal ( a, "30" ) );
  a *= 5;
  assert ( equal ( a, "150" ) );
  b = a + 3;
  assert ( equal ( b, "153" ) );
  b = a * 7;
  assert ( equal ( b, "1050" ) );
  assert ( equal ( a, "150" ) );

  a = 10;
  a += -20;
  assert ( equal ( a, "-10" ) );
  a *= 5;
  assert ( equal ( a, "-50" ) );
  b = a + 73;
  assert ( equal ( b, "23" ) );
  b = a * -7;
  assert ( equal ( b, "350" ) );
  assert ( equal ( a, "-50" ) );

  a = "12345678901234567890";
  a += "-99999999999999999999";
  assert ( equal ( a, "-87654321098765432109" ) );
  a *= "54321987654321987654";
  assert ( equal ( a, "-4761556948575111126880627366067073182286" ) );
  a *= 0;
  assert ( equal ( a, "0" ) );
  a = 10;
  b = a + "400";
  assert ( equal ( b, "410" ) );
  b = a * "15";
  assert ( equal ( b, "150" ) );
  assert ( equal ( a, "10" ) );

  b = a * "-1";
  assert ( equal ( b, "-10" ) );
  assert ( equal ( a, "10" ) );
  
  b = a * "0";
  assert ( equal ( b, "0" ) );
  assert ( equal ( a, "10" ) );

  is . clear ();
  is . str ( " 1234" );
  assert ( is >> b );
  assert ( equal ( b, "1234" ) );
  is . clear ();
  is . str ( " 12 34" );
  assert ( is >> b );
  assert ( equal ( b, "12" ) );
  is . clear ();
  is . str ( "999z" );
  assert ( is >> b );
  assert ( equal ( b, "999" ) );
  is . clear ();
  is . str ( "abcd" );
  assert ( ! ( is >> b ) );
  is . clear ();
  is . str ( "- 758" );
  assert ( ! ( is >> b ) );
  a = 42;
  try
  {
    a = "-xyz";
    assert ( "missing an exception" == NULL );
  }
  catch ( const invalid_argument & e )
  {
    assert ( equal ( a, "42" ) );
  }

  a = "73786976294838206464";
  assert ( a < "1361129467683753853853498429727072845824" );
  assert ( a > "737869762948382064" );
  assert ( a > "73786976294838206462" );
  assert ( a > "-73786976294838206464" );
  assert ( ! (a > "73786976294838206464") );

  
  assert ( a <= "1361129467683753853853498429727072845824" );
  assert ( ! ( a > "1361129467683753853853498429727072845824" ) );
  assert ( ! ( a >= "1361129467683753853853498429727072845824" ) );
  assert ( ! ( a == "1361129467683753853853498429727072845824" ) );
  assert ( a != "1361129467683753853853498429727072845824" );
  assert ( ! ( a < "73786976294838206464" ) );
  assert ( a <= "73786976294838206464" );
  assert ( ! ( a > "73786976294838206464" ) );
  assert ( a >= "73786976294838206464" );
  assert ( a == "73786976294838206464" );
  assert ( ! ( a != "73786976294838206464" ) );
  assert ( a < "73786976294838206465" );
  assert ( a <= "73786976294838206465" );
  assert ( ! ( a > "73786976294838206465" ) );
  assert ( ! ( a >= "73786976294838206465" ) );
  assert ( ! ( a == "73786976294838206465" ) );
  assert ( a != "73786976294838206465" );
  a = "2147483648";
  assert ( ! ( a < -2147483648 ) );
  assert ( ! ( a <= -2147483648 ) );
  assert ( a > -2147483648 );
  assert ( a >= -2147483648 );
  assert ( ! ( a == -2147483648 ) );
  assert ( a != -2147483648 );

  return 0;
}
#endif /* __PROGTEST__ */
