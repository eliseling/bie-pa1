#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define CITY_NAME_MAX 100

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

typedef struct {
  char from[CITY_NAME_MAX + 1];
  char to[CITY_NAME_MAX + 1];
  int cost;
} Edge;

typedef struct {
  TTRIP * trip;
  int cost;
} TripResult;

typedef struct {
  TripResult * items;
  int count;
  int capacity;
} ResultList;

typedef struct {
  Edge * edges;
  int edgeCount;
  char startCity[CITY_NAME_MAX + 1];
  int costMax;
  char visitedCities[500][CITY_NAME_MAX + 1];
  int visitedCount;
  int currentCost;
  char path[50000];
  ResultList * results;
} DFSContext;

static int isCityVisited(DFSContext * ctx, const char * cityName)
{
  int i;
  for (i = 0; i < ctx->visitedCount; i++)
    if (!strcmp(ctx->visitedCities[i], cityName))
      return 1;
  return 0;
}

static int findEdge(Edge * edges, int edgeCount, const char * from, const char * to)
{
  int i;
  for (i = 0; i < edgeCount; i++)
    if (!strcmp(edges[i].from, from) && !strcmp(edges[i].to, to))
      return i;
  return -1;
}

static void dfs(DFSContext * ctx, const char * currentCity)
{
  int i;
  int retEdge;
  int newCost;
  char * desc;
  Edge * edges;
  int oldVisitedCount;
  char oldPath[50000];
  int oldCost;
  char * newPath;
  size_t newPathLen;
  
  edges = ctx->edges;
  
  /* Try to return to start city */
  retEdge = findEdge(edges, ctx->edgeCount, currentCity, ctx->startCity);
  if (retEdge >= 0 && ctx->visitedCount > 0)
  {
    newCost = ctx->currentCost + edges[retEdge].cost;
    if (newCost <= ctx->costMax)
    {
      size_t descLen = strlen(ctx->path) + strlen(ctx->startCity) + 10;
      desc = (char *)malloc(descLen);
      if (desc)
      {
        snprintf(desc, descLen, "%s -> %s", ctx->path, ctx->startCity);
        
        if (ctx->results->count >= ctx->results->capacity)
        {
          ctx->results->capacity *= 2;
          TripResult * newItems = (TripResult *)realloc(ctx->results->items, 
                                                         ctx->results->capacity * sizeof(TripResult));
          if (newItems)
            ctx->results->items = newItems;
        }
        
        if (ctx->results->count < ctx->results->capacity)
        {
          ctx->results->items[ctx->results->count].trip = makeTrip(desc, ctx->visitedCount + 1, newCost, NULL);
          ctx->results->items[ctx->results->count].cost = newCost;
          ctx->results->count++;
        }
      }
    }
  }
  
  /* Try extending the path to each unvisited city */
  for (i = 0; i < ctx->edgeCount; i++)
  {
    if (!strcmp(edges[i].from, currentCity) && 
        !strcmp(edges[i].to, ctx->startCity))
      continue; /* Skip direct return edge */
    
    if (!strcmp(edges[i].from, currentCity) && 
        !isCityVisited(ctx, edges[i].to))
    {
      newCost = ctx->currentCost + edges[i].cost;
      if (newCost <= ctx->costMax)
      {
        newPathLen = strlen(ctx->path) + strlen(edges[i].to) + 10;
        newPath = (char *)malloc(newPathLen);
        if (newPath)
        {
          oldVisitedCount = ctx->visitedCount;
          strcpy(oldPath, ctx->path);
          oldCost = ctx->currentCost;
          
          if (strlen(ctx->path) > 0)
            snprintf(newPath, newPathLen, "%s -> %s", ctx->path, edges[i].to);
          else
            snprintf(newPath, newPathLen, "%s", edges[i].to);
          
          strcpy(ctx->path, newPath);
          ctx->currentCost = newCost;
          strcpy(ctx->visitedCities[ctx->visitedCount], edges[i].to);
          ctx->visitedCount++;
          
          dfs(ctx, edges[i].to);
          
          strcpy(ctx->path, oldPath);
          ctx->currentCost = oldCost;
          ctx->visitedCount = oldVisitedCount;
          
          free(newPath);
        }
      }
    }
  }
}

TTRIP * findTrips ( const char data[], const char from[], int costMax )
{
  Edge * edges;
  int edgeCount = 0;
  int edgeCapacity = 100;
  char * dataCopy;
  char * line;
  char * saveptr;
  int cost;
  char fromCity[CITY_NAME_MAX + 1];
  char toCity[CITY_NAME_MAX + 1];
  int i, j;
  DFSContext ctx;
  ResultList results;
  TTRIP * head;
  
  if (!data || !from)
    return NULL;
  
  edges = (Edge *)malloc(edgeCapacity * sizeof(Edge));
  if (!edges)
    return NULL;
  
  dataCopy = (char *)malloc(strlen(data) + 1);
  if (!dataCopy)
  {
    free(edges);
    return NULL;
  }
  strcpy(dataCopy, data);
  
  /* Parse edges from input data */
  line = strtok_r(dataCopy, "\n", &saveptr);
  while (line)
  {
    while (*line && (*line == ' ' || *line == '\t'))
      line++;
    
    if (*line && *line != '\0')
    {
      if (sscanf(line, "%d: %100s -> %100s", &cost, fromCity, toCity) == 3)
      {
        if (edgeCount >= edgeCapacity)
        {
          edgeCapacity *= 2;
          Edge * newEdges = (Edge *)realloc(edges, edgeCapacity * sizeof(Edge));
          if (!newEdges)
          {
            free(dataCopy);
            free(edges);
            return NULL;
          }
          edges = newEdges;
        }
        edges[edgeCount].cost = cost;
        strncpy(edges[edgeCount].from, fromCity, CITY_NAME_MAX);
        edges[edgeCount].from[CITY_NAME_MAX] = '\0';
        strncpy(edges[edgeCount].to, toCity, CITY_NAME_MAX);
        edges[edgeCount].to[CITY_NAME_MAX] = '\0';
        edgeCount++;
      }
    }
    line = strtok_r(NULL, "\n", &saveptr);
  }
  free(dataCopy);
  
  if (edgeCount == 0)
  {
    free(edges);
    return NULL;
  }
  
  /* Initialize results list */
  results.capacity = 100;
  results.count = 0;
  results.items = (TripResult *)malloc(results.capacity * sizeof(TripResult));
  if (!results.items)
  {
    free(edges);
    return NULL;
  }
  
  /* Initialize DFS context */
  ctx.edges = edges;
  ctx.edgeCount = edgeCount;
  ctx.costMax = costMax;
  ctx.currentCost = 0;
  ctx.visitedCount = 0;
  ctx.results = &results;
  strncpy(ctx.startCity, from, CITY_NAME_MAX);
  ctx.startCity[CITY_NAME_MAX] = '\0';
  strncpy(ctx.path, from, CITY_NAME_MAX);
  ctx.path[CITY_NAME_MAX] = '\0';
  
  /* Run DFS to find all valid trips */
  dfs(&ctx, from);
  
  /* Sort results by cost using bubble sort */
  for (i = 0; i < results.count - 1; i++)
  {
    for (j = i + 1; j < results.count; j++)
    {
      if (results.items[j].cost < results.items[i].cost)
      {
        TripResult tmp = results.items[i];
        results.items[i] = results.items[j];
        results.items[j] = tmp;
      }
    }
  }
  
  /* Build linked list in sorted order */
  head = NULL;
  for (i = results.count - 1; i >= 0; i--)
  {
    results.items[i].trip->m_Next = head;
    head = results.items[i].trip;
  }
  
  free(results.items);
  free(edges);
  
  return head;
}

#ifndef __PROGTEST__
int main ()
{
  const char * data0 = 
"100: Prague -> London\n"
"80: Prague -> Paris\n"
"90: Paris -> London\n"
"75: London -> Madrid\n"
"95: Madrid -> Prague\n"
"1000: London -> Prague\n"
"50: Berlin -> Prague\n"
"80: Madrid -> Berlin\n"
"90: Rome -> Prague\n"
"100: Wien -> Rome\n"
"90: Prague -> Lisabon\n"
"80: Lisabon -> Dublin\n";
  const char * data1 = 
"100: Prague -> London\n"
"107: London -> Prague\n"
"80: Prague -> Paris\n"
"78: Paris -> Prague\n"
"38: Paris -> London\n"
"43: London -> Paris\n"
"69: Prague -> Wien\n"
"89: London -> Wien\n"
"73: Paris -> Wien\n"
"63: Wien -> Prague\n"
"82: Wien -> London\n"
"77: Wien -> Paris\n"
"163: Zagreb -> Tallinn\n"
"282: Tallinn -> Lisboa\n"
"377: Lisboa -> Zagreb\n";
  TTRIP * t;
  t = findTrips ( data0, "Prague", 300 );
  assert ( t );
  assert ( t -> m_Cost == 270 );
  assert ( t -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Desc, "Prague -> London -> Madrid -> Prague" ) );
  assert ( t -> m_Next == NULL );
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
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
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
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
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
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
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
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
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
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
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
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
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
  assert ( t -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
  freeTripList ( t );
  t = findTrips ( data1, "Lisboa", 1000 );
  assert ( t );
  assert ( t -> m_Cost == 822 );
  assert ( t -> m_Cities == 3 );
  assert ( ! strcmp ( t -> m_Desc, "Lisboa -> Zagreb -> Tallinn -> Lisboa" ) );
  assert ( t -> m_Next == NULL );
  freeTripList ( t );
  t = findTrips ( data1, "Oslo", 1000 );
  assert ( t == NULL );
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */