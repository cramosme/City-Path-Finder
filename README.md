# City Path Finder

```city_map.cpp``` implements Dijkstra's Algorithm to find the fastest route through a city. It was designed to work alongside ```city_map_base.cpp```. The program initializes the city by reading and storing intersections, then constructs an adjacency list to represent
the road network. The program calculates the shortest path from the first intersection to the last while considering road directions and traffic light delays.

#### Usage:
```bash
make clean; make
./bin/city_map none|best|worst|avg time|print|jgraph - map on standard input
```
#### Example:
```bash
./bin/city_map best time < maps/city-1.txt
```
