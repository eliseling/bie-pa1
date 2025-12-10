#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

constexpr size_t CITY_NAME_MAX = 100;

typedef struct TTrip
{
  struct TTrip * m_Next;
  char         * m_Desc;
  int            m_Cities;
  int            m_Cost;
} TTRIP;

TTRIP * makeTrip ( char    desc[],
                   int     cities,
                   int     cost,
                   TTRIP * next )
{
  TTRIP * res = (TTRIP *) malloc ( sizeof ( *res ) );
  res -> m_Next = next;
  res -> m_Desc = desc;
  res -> m_Cities = cities;
  res -> m_Cost = cost;
  return res;
}

void freeTripList ( TTRIP * x )
{
  while ( x )
  {
    TTRIP * tmp = x -> m_Next;
    free ( x -> m_Desc );
    free ( x );
    x = tmp;
  }
}
#endif /* __PROGTEST__ */

TTRIP * findTrips ( const char data[], const char from[], int costMax )
{
  // todo
}

#ifndef __PROGTEST__
int main ()
{
  const char * data0 = R"(
100: Prague -> London
80: Prague -> Paris
90: Paris -> London
75: London -> Madrid
95: Madrid -> Prague
1000: London -> Prague
50: Berlin -> Prague
80: Madrid -> Berlin
90: Rome -> Prague
100: Wien -> Rome
90: Prague -> Lisabon
80: Lisabon -> Dublin
)";
  const char * data1 = R"(
100: Prague -> London
107: London -> Prague
80: Prague -> Paris
78: Paris -> Prague
38: Paris -> London
43: London -> Paris
69: Prague -> Wien
89: London -> Wien
73: Paris -> Wien
63: Wien -> Prague
82: Wien -> London
77: Wien -> Paris
163: Zagreb -> Tallinn
282: Tallinn -> Lisboa
377: Lisboa -> Zagreb
)";
  TTRIP * t;
  t = findTrips ( data0, "Prague", 300 );
  assert ( t );
  assert ( t -> m_Cost == 270 );
  assert ( t -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Desc, "Prague -> London -> Madrid -> Prague" ) );
  assert ( t -> m_Next == nullptr );
  freeTripList ( t );
  t = findTrips ( data0, "Prague", 700 );
  assert ( t );
  assert ( t -> m_Cost == 270 );
  assert ( t -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Desc, "Prague -> London -> Madrid -> Prague" ) );
  assert ( t -> m_Next );
  assert ( t -> m_Next -> m_Cost == 305 );
  assert ( t -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Desc, "Prague -> London -> Madrid -> Berlin -> Prague" ) );
  assert ( t -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Cost == 340 );
  assert ( t -> m_Next -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Desc, "Prague -> Paris -> London -> Madrid -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cost == 375 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cities == 5 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> Paris -> London -> Madrid -> Berlin -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next == nullptr );
  freeTripList ( t );
  t = findTrips ( data0, "Prague", 1100 );
  assert ( t );
  assert ( t -> m_Cost == 270 );
  assert ( t -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Desc, "Prague -> London -> Madrid -> Prague" ) );
  assert ( t -> m_Next );
  assert ( t -> m_Next -> m_Cost == 305 );
  assert ( t -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Desc, "Prague -> London -> Madrid -> Berlin -> Prague" ) );
  assert ( t -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Cost == 340 );
  assert ( t -> m_Next -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Desc, "Prague -> Paris -> London -> Madrid -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cost == 375 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cities == 5 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> Paris -> London -> Madrid -> Berlin -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 1100 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 2 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> London -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == nullptr );
  freeTripList ( t );
  t = findTrips ( data0, "Prague", 5000 );
  assert ( t );
  assert ( t -> m_Cost == 270 );
  assert ( t -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Desc, "Prague -> London -> Madrid -> Prague" ) );
  assert ( t -> m_Next );
  assert ( t -> m_Next -> m_Cost == 305 );
  assert ( t -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Desc, "Prague -> London -> Madrid -> Berlin -> Prague" ) );
  assert ( t -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Cost == 340 );
  assert ( t -> m_Next -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Desc, "Prague -> Paris -> London -> Madrid -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cost == 375 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cities == 5 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> Paris -> London -> Madrid -> Berlin -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 1100 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 2 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> London -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 1170 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> Paris -> London -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == nullptr );
  freeTripList ( t );
  t = findTrips ( data0, "London", 2000 );
  assert ( t );
  assert ( t -> m_Cost == 270 );
  assert ( t -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Desc, "London -> Madrid -> Prague -> London" ) );
  assert ( t -> m_Next );
  assert ( t -> m_Next -> m_Cost == 305 );
  assert ( t -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Desc, "London -> Madrid -> Berlin -> Prague -> London" ) );
  assert ( t -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Cost == 340 );
  assert ( t -> m_Next -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Desc, "London -> Madrid -> Prague -> Paris -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cost == 375 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cities == 5 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Madrid -> Berlin -> Prague -> Paris -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 1100 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 2 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Prague -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 1170 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Prague -> Paris -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == nullptr );
  freeTripList ( t );
  t = findTrips ( data1, "Prague", 270 );
  assert ( t );
  assert ( t -> m_Cost == 132 );
  assert ( t -> m_Cities == 2 );
  assert ( ! strcmp ( t -> m_Desc, "Prague -> Wien -> Prague" ) );
  assert ( t -> m_Next );
  assert ( t -> m_Next -> m_Cost == 158 );
  assert ( t -> m_Next -> m_Cities == 2 );
  assert ( ! strcmp ( t -> m_Next -> m_Desc, "Prague -> Paris -> Prague" ) );
  assert ( t -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Cost == 207 );
  assert ( t -> m_Next -> m_Next -> m_Cities == 2 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Desc, "Prague -> London -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cost == 216 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> Paris -> Wien -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 221 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> London -> Paris -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 224 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> Wien -> Paris -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 225 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> Paris -> London -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 252 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> London -> Wien -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 258 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> Wien -> London -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 270 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Prague -> Paris -> London -> Wien -> Prague" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == nullptr );
  freeTripList ( t );
  t = findTrips ( data1, "Wien", 270 );
  assert ( t );
  assert ( t -> m_Cost == 132 );
  assert ( t -> m_Cities == 2 );
  assert ( ! strcmp ( t -> m_Desc, "Wien -> Prague -> Wien" ) );
  assert ( t -> m_Next );
  assert ( t -> m_Next -> m_Cost == 150 );
  assert ( t -> m_Next -> m_Cities == 2 );
  assert ( ! strcmp ( t -> m_Next -> m_Desc, "Wien -> Paris -> Wien" ) );
  assert ( t -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Cost == 171 );
  assert ( t -> m_Next -> m_Next -> m_Cities == 2 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Desc, "Wien -> London -> Wien" ) );
  assert ( t -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cost == 198 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Desc, "Wien -> London -> Paris -> Wien" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 204 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Wien -> Paris -> London -> Wien" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 216 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Wien -> Prague -> Paris -> Wien" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 224 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Wien -> Paris -> Prague -> Wien" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 252 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Wien -> Prague -> London -> Wien" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 258 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Wien -> London -> Prague -> Wien" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 270 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "Wien -> Prague -> Paris -> London -> Wien" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == nullptr );
  freeTripList ( t );
  t = findTrips ( data1, "London", 400 );
  assert ( t );
  assert ( t -> m_Cost == 81 );
  assert ( t -> m_Cities == 2 );
  assert ( ! strcmp ( t -> m_Desc, "London -> Paris -> London" ) );
  assert ( t -> m_Next );
  assert ( t -> m_Next -> m_Cost == 171 );
  assert ( t -> m_Next -> m_Cities == 2 );
  assert ( ! strcmp ( t -> m_Next -> m_Desc, "London -> Wien -> London" ) );
  assert ( t -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Cost == 198 );
  assert ( t -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Desc, "London -> Paris -> Wien -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cost == 204 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Wien -> Paris -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 207 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 2 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Prague -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 221 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Paris -> Prague -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 225 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Prague -> Paris -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 252 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Wien -> Prague -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 258 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Prague -> Wien -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 270 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Wien -> Prague -> Paris -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 272 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Paris -> Prague -> Wien -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 279 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Paris -> Wien -> Prague -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 291 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Prague -> Wien -> Paris -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 342 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Prague -> Paris -> Wien -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cost == 344 );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Cities == 4 );
  assert ( ! strcmp ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Desc, "London -> Wien -> Paris -> Prague -> London" ) );
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == nullptr );
  freeTripList ( t );
  t = findTrips ( data1, "Lisboa", 1000 );
  assert ( t );
  assert ( t -> m_Cost == 822 );
  assert ( t -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Desc, "Lisboa -> Zagreb -> Tallinn -> Lisboa" ) );
  assert ( t -> m_Next == nullptr );
  freeTripList ( t );
  t = findTrips ( data1, "Oslo", 1000 );
  assert ( t == nullptr );
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
