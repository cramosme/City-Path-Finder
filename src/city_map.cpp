/* city_map.cpp 
   Chris Ramos

   Sets up the constructor for a city, reading in all the intersections and setting them up in a list. Then sets up the adjacency
   list that will be used to calculate the fastest path from the first intersection to the last using Dijkstra's Algorithm.
*/


#include <vector>
#include <set>
#include <math.h>
#include "city_map.hpp"
using namespace std;

/* City Map constructor reads in a city map, it then sets the first and last intersection. It also sets up the adjacency list of the city. */
City_Map::City_Map(){

   /* Used to create adjacency list. Iterating through vector all.*/
   Road_Segment* rS;
   list<Intersection*>::iterator firstIt;
   list<Intersection*>::iterator secondIt;

   /* Used to create instances of Intersections and fill vector all */
   Intersection* intersection;
   int largestSt = 0, largestAve = 0;
   intersection = new Intersection();

   while( cin >> intersection->street >> intersection->avenue >> intersection->x >> intersection->y >> intersection->green[0] >> intersection->green[1] ){
      
      if( intersection->street == 0 && intersection->avenue == 0 ){
         first = intersection;
      }
      // /* Set last to intersetion [largestSt, largestAve]*/
      if( largestSt < intersection->street || largestAve < intersection->avenue ){
         largestSt = intersection->street;
         largestAve = intersection->avenue;
         last = intersection;
      }
      intersection->best_time = -1;
      intersection->backedge = NULL;

      all.push_back(intersection);
      intersection = new Intersection();
      
   }

   for( firstIt = all.begin(); firstIt != all.end(); firstIt++ ){
      
      for (secondIt = all.begin(); secondIt != all.end(); secondIt++) {

         double distance = sqrt( pow( ((*secondIt)->x - (*firstIt)->x) , 2 ) + pow( ((*secondIt)->y - (*firstIt)->y), 2 ) );

         /* Check to make sure they are on the same street */
         if( (*firstIt)->street == (*secondIt)->street ){

            /* Street is multiple of 5 - two-way ascending */
            
            if( ( (*firstIt)->street%5 == 0 ) && ( (*firstIt)->avenue == (*secondIt)->avenue+1 || (*firstIt)->avenue == (*secondIt)->avenue-1 ) ){

               rS = new Road_Segment;
               rS->type = STREET;
               rS->number = (*firstIt)->street;
               rS->distance = distance;
               rS->from = (*firstIt);
               rS->to = (*secondIt);
               (*firstIt)->adj.push_back(rS);

            }
            /* Street is even - one-way ascending order of avenue ( east to west )*/
            else if( ( (*firstIt)->street%2 == 0 ) && ( (*firstIt)->avenue == (*secondIt)->avenue-1 ) ){
               rS = new Road_Segment;
               rS->type = STREET;
               rS->number = (*firstIt)->street;
               rS->from = (*firstIt);
               rS->to = (*secondIt);
               rS->distance = distance;
               (*firstIt)->adj.push_back(rS);

            }
            /* Street is odd and not a multiple of 5 - one-way descending order of avenue ( west to east ) */
            else{
               if( ( (*firstIt)->street%2 != 0 ) && (*firstIt)->avenue == (*secondIt)->avenue+1 ){
                  rS = new Road_Segment;
                  rS->type = STREET;
                  rS->number = (*firstIt)->street;
                  rS->from = (*firstIt);
                  rS->to = (*secondIt);  
                  rS->distance = distance;
                  (*firstIt)->adj.push_back(rS);

               }
            }
         }

         /* Check if on the same avenue */
         
         if( (*firstIt)->avenue == (*secondIt)->avenue ){

            /* Highest avenue - two-way */
            if( ( (*firstIt)->avenue == largestAve || (*secondIt)->avenue%5 == 0 ) &&  ( (*firstIt)->street == (*secondIt)->street+1 || (*firstIt)->street == (*secondIt)->street-1 ) ){
               rS = new Road_Segment;
               rS->type = AVENUE;
               rS->number = (*firstIt)->street;
               rS->distance = distance;
               rS->from = (*firstIt);
               rS->to = (*secondIt);
               (*firstIt)->adj.push_back(rS);
               
            }
            /* Avenue is even - one-way ascending order of street ( south to north )*/
            else if( ( (*firstIt)->avenue%2 == 0 ) && ( (*firstIt)->street == (*secondIt)->street-1 ) ){
               rS = new Road_Segment;
               rS->type = AVENUE;
               rS->number = (*firstIt)->street;
               rS->distance = distance;
               rS->from = (*firstIt);
               rS->to = (*secondIt);
               (*firstIt)->adj.push_back(rS);

            }
            /* Avenue is odd and not a multiple of 5 - one-way descending order of street ( north to south )*/
            else{
               if( ( (*firstIt)->avenue%2 != 0 ) && (*firstIt)->street == (*secondIt)->street+1 ){

                  rS = new Road_Segment;
                  rS->type = AVENUE;
                  rS->number = (*firstIt)->street;
                  rS->from = (*firstIt);
                  rS->to = (*secondIt);
                  rS->distance = distance;
                  (*firstIt)->adj.push_back(rS);
 
               }
            }
         }
      }
   }
}

double City_Map::Dijkstra( int avg_best_worst ){

   double timeTaken = 0; //used to calculate best times
   set< Intersection* > visited; // make sure not to go down a path we have visited
   Intersection* curIntersection; // Used to copy data from multimap before erasing
   Intersection* toTracker;
   Road_Segment* rsPtr;
   list< Road_Segment* >::iterator rsIt;

   /* Inserting first into multimap with time of 0 since its the start. */
   first->best_time = 0;
   bfsq.insert( make_pair(0, first) );

   while( !bfsq.empty() ){

      /* Grab information before erasing from map */
      curIntersection = bfsq.begin()->second;
      bfsq.erase( bfsq.begin() );

      // Insert into visited so we don't revisit
      visited.insert( curIntersection );
      
      for( rsIt = curIntersection->adj.begin(); rsIt != curIntersection->adj.end(); rsIt++ ){

         toTracker = (*rsIt)->to;
         
         /* Haven't been visited */
         if( visited.find( toTracker ) == visited.end() ){

            timeTaken = curIntersection->best_time + ( (*rsIt)->distance / 30 * 3600 );

            /* Accumulate the avg time we wait for green light onto best time */
            if( avg_best_worst == 'A' ){
               timeTaken += pow( toTracker->green[(*rsIt)->type != AVENUE], 2 )/( 2 * ( toTracker->green[(*rsIt)->type != AVENUE] + toTracker->green[(*rsIt)->type != STREET] ) );
            }
            /* Add total we have to wait for a green */
            else if( avg_best_worst == 'W' ) timeTaken += toTracker->green[(*rsIt)->type != AVENUE];
            else if( avg_best_worst == 'B' ); //Do nothing in here

            /* if time hasn't been set or found a faster path , insert into map */
            if( toTracker->best_time == -1 || timeTaken < toTracker->best_time ){

               toTracker->best_time = timeTaken;
               toTracker->backedge = *rsIt;
               toTracker->bfsq_ptr = bfsq.insert( make_pair( timeTaken , toTracker ) );

            }
         }
      }
   }
   
   /* trace the backedges to find the path and insert at the front */
   for( rsPtr = last->backedge; rsPtr != NULL; rsPtr = rsPtr->from->backedge ){
      path.insert( path.begin(), rsPtr );
   }

   return last->best_time;
}