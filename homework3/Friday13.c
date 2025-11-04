#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
typedef struct TDate
{
  unsigned       m_Year;
  unsigned short m_Month;
  unsigned short m_Day;
} TDATE;
TDATE makeDate ( unsigned       y,
                 unsigned short m,
                 unsigned short d )
{
  TDATE res = { y, m, d };
  return res;
}
bool  equalDate ( TDATE a,
                  TDATE b )
{
  return a . m_Year == b . m_Year
         && a . m_Month == b . m_Month
         && a . m_Day == b . m_Day;
}
#endif /* __PROGTEST__ */

// -----------------------------------------------------------
// helpers
// -----------------------------------------------------------

static int isLeap ( unsigned y )
{
  if ( y % 4000 == 0 ) return 0;
  if ( y % 400 == 0 )  return 1;
  if ( y % 100 == 0 )  return 0;
  if ( y % 4 == 0 )    return 1;
  return 0;
}

static int daysInMonth ( unsigned y, unsigned short m )
{
  static const int dim[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
  if ( m == 2 && isLeap ( y ) )
    return 29;
  return dim[m-1];
}

static bool validDate ( TDATE d )
{
  if ( d.m_Year < 1900 ) return false;
  if ( d.m_Month < 1 || d.m_Month > 12 ) return false;
  if ( d.m_Day < 1 || d.m_Day > daysInMonth ( d.m_Year, d.m_Month ) ) return false;
  return true;
}

static int dayOfWeek ( TDATE d )
{
  // Zeller’s congruence (Monday=1 … Sunday=7)
  int y = d.m_Year;
  int m = d.m_Month;
  if ( m < 3 )
  {
    m += 12;
    y--;
  }
  int K = y % 100;
  int J = y / 100;
  int h = ( d.m_Day + (13*(m+1))/5 + K + K/4 + J/4 + 5*J ) % 7;
  // 0=Saturday, 1=Sunday, 2=Monday … so Friday=6
  int w = ((h + 5) % 7) + 1;
  return w;
}

static int cmpDate ( TDATE a, TDATE b )
{
  if ( a.m_Year != b.m_Year ) return ( a.m_Year < b.m_Year ) ? -1 : 1;
  if ( a.m_Month != b.m_Month ) return ( a.m_Month < b.m_Month ) ? -1 : 1;
  if ( a.m_Day != b.m_Day ) return ( a.m_Day < b.m_Day ) ? -1 : 1;
  return 0;
}

static void incMonth ( TDATE * d )
{
  d->m_Month++;
  if ( d->m_Month > 12 )
  {
    d->m_Month = 1;
    d->m_Year++;
  }
  if ( d->m_Day > daysInMonth ( d->m_Year, d->m_Month ) )
    d->m_Day = daysInMonth ( d->m_Year, d->m_Month );
}

static void decMonth ( TDATE * d )
{
  if ( d->m_Month == 1 )
  {
    d->m_Month = 12;
    d->m_Year--;
  }
  else
    d->m_Month--;
  if ( d->m_Day > daysInMonth ( d->m_Year, d->m_Month ) )
    d->m_Day = daysInMonth ( d->m_Year, d->m_Month );
}

// -----------------------------------------------------------
// main functions
// -----------------------------------------------------------

bool countFriday13 ( TDATE from, TDATE to, long long int * cnt )
{
  if ( ! cnt || ! validDate ( from ) || ! validDate ( to ) ) return false;
  if ( cmpDate ( from, to ) > 0 ) return false;

  long long int total = 0;
  TDATE cur = makeDate ( from.m_Year, from.m_Month, 13 );

  // move cur to the first 13th >= from
  if ( cmpDate ( cur, from ) < 0 )
    incMonth ( &cur );

  while ( cmpDate ( cur, to ) <= 0 )
  {
    if ( validDate ( cur ) && dayOfWeek ( cur ) == 5 )
      total++;
    incMonth ( &cur );
  }

  *cnt = total;
  return true;
}

bool prevFriday13 ( TDATE * date )
{
  if ( ! date || ! validDate ( *date ) ) return false;

  TDATE cur = *date;
  // move before current day if after or on 13th
  if ( cur.m_Day <= 13 ) decMonth ( &cur );
  cur.m_Day = 13;

  while ( cur.m_Year >= 1900 )
  {
    if ( validDate ( cur ) && dayOfWeek ( cur ) == 5 )
    {
      *date = cur;
      return true;
    }
    decMonth ( &cur );
  }
  return false;
}

bool nextFriday13 ( TDATE * date )
{
  if ( ! date || ! validDate ( *date ) ) return false;

  TDATE cur = *date;
  if ( cur.m_Day >= 13 ) incMonth ( &cur );
  cur.m_Day = 13;

  while ( 1 )
  {
    if ( validDate ( cur ) && dayOfWeek ( cur ) == 5 )
    {
      *date = cur;
      return true;
    }
    incMonth ( &cur );
  }
  // never reached
  return false;
}

#ifndef __PROGTEST__
int main ()
{
  long long int cnt;
  TDATE x;
  assert ( countFriday13 ( makeDate ( 1900, 1, 1 ), makeDate ( 2025, 5, 1 ), &cnt )
           && cnt == 215LL );
  assert ( countFriday13 ( makeDate ( 1900, 1, 1 ), makeDate ( 2025, 6, 1 ), &cnt )
           && cnt == 215LL );
  assert ( countFriday13 ( makeDate ( 1900, 1, 1 ), makeDate ( 2025, 5,13 ), &cnt )
           && cnt == 215LL );
  assert ( countFriday13 ( makeDate ( 1900, 1, 1 ), makeDate ( 2025, 6,13 ), &cnt )
           && cnt == 216LL );
  // ... (rest of provided asserts unchanged)
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
