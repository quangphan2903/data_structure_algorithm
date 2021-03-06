// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <algorithm>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <vector>
#include <iostream>

// Types for IDs
using StopID = long int;
using RegionID = std::string;
using RouteID = std::string;
using Name = std::string;

// Return values for cases where required thing was not found
RouteID const NO_ROUTE = "!!NO_ROUTE!!";
StopID const NO_STOP = -1;
RegionID const NO_REGION = "!!NO_REGION!!";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
Name const NO_NAME = "!!NO_NAME!!";

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};


// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};
Coord const ORIGIN = {0, 0};

// Type for time of day in minutes from midnight (i.e., 60*hours + minutes)
using Time = int;

// Return value for cases where color was not found
Time const NO_TIME = std::numeric_limits<Time>::min();

// Type for a duration of time (in minutes)
using Duration = int;

// Return value for cases where Duration is unknown
Duration const NO_DURATION = NO_VALUE;

// Type for a distance (in metres)
using Distance = int;

// Return value for cases where Duration is unknown
Distance const NO_DISTANCE = NO_VALUE;

// Developer defined constant
int const MAX_VALUE = std::numeric_limits<int>::max();
int const NO_CONNECTION = -1;

struct Stop
{
    Name name;
    Coord coord;
    RegionID parent;
    std::unordered_map<RouteID, std::pair<StopID, StopID>> routes;
    std::unordered_map<RouteID, std::set<Time>> time_tables;
};

struct Region
{
    RegionID parent;
    Name name;
    std::vector<RegionID> subregions;
    std::vector<StopID> stops;
};

struct Route
{
    std::vector<StopID> stops;
};

struct Connection {
    int id;
    RouteID routeid;
    StopID fromstop;
    StopID tostop;
    Time departure;
    Time arrrival;
};

// Compare two connections.
inline bool operator==(const Connection& lhs, const Connection& rhs)
{
    return lhs.id == rhs.id &&
           lhs.routeid == rhs.routeid &&
           lhs.fromstop == rhs.fromstop &&
           lhs.tostop == rhs.tostop &&
           lhs.departure == rhs.departure &&
           lhs.arrrival == rhs.arrrival;
}

// Short name for types
using stops_vec    = std::vector<StopID>;
using regions_vec  = std::vector<RegionID>;
using routes_vec   = std::vector<RouteID>;
using name_pair    = std::pair<StopID, Name>;
using coord_pair   = std::pair<StopID, Coord>;
using stop_pair    = std::pair<StopID, Stop>;
using queue_list   = std::list<StopID>;
using parent_map   = std::unordered_map<StopID, std::pair<RouteID, StopID>>;
using path_vec     = std::vector<std::pair<RouteID, StopID>>;
using return_tuple = std::vector<std::tuple<StopID, RouteID, Distance>>;



// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: O(1).
    // Short rationale for estimate: const time for size() method.
    int stop_count();

    // Estimate of performance: O(n).
    // Short rationale for estimate: linear in the size of map for clear() method.
    void clear_all();

    // Estimate of performance: O(n)
    // Short rationale for estimate: traverse through n elements of a map.
    std::vector<StopID> all_stops();

    // Estimate of performance: O(log n)
    // Short rationale for estimate: map [] operator and multimap::insert() method.
    bool add_stop(StopID id, Name const& name, Coord xy);

    // Estimate of performance: O(log n) ~ θ(1)
    // Short rationale for estimate: map [] operator.
    Name get_stop_name(StopID id);

    // Estimate of performance: O(log n) ~ θ(1)
    // Short rationale for estimate: map [] operator.
    Coord get_stop_coord(StopID id);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(n)
    // Short rationale for estimate: traverse through n elements of a multimap.
    std::vector<StopID> stops_alphabetically();

    // Estimate of performance: O(n log n)
    // Short rationale for estimate: std::sort complexity.
    std::vector<StopID> stops_coord_order();

    // Estimate of performance: O(n) ~ θ(1)
    // Short rationale for estimate: Worst case happens RARELY when all stops
    // have the same coordinates.
    StopID min_coord();

    // Estimate of performance: O(n) ~ θ(1)
    // Short rationale for estimate: Worst case happens RARELY when all stops
    // have the same coordinates.
    StopID max_coord();

    // Estimate of performance: O(n) ~ θ(log n)
    // Short rationale for estimate: Worst case happens RARELY when all stops
    // have the same name. On average, the complexity of equal_range is O(log n).
    std::vector<StopID> find_stops(Name const& name);

    // Estimate of performance: O(n) ~ θ(1)
    // Short rationale for estimate: Worst case is n when all stops have same
    // name but on average it is constant time.
    bool change_stop_name(StopID id, Name const& newname);

    // Estimate of performance: O(n) ~ θ(1)
    // Short rationale for estimate: Worst case is n when all stops have same
    // coord but on average, it is constant time.
    bool change_stop_coord(StopID id, Coord newcoord);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(log n)
    // Short rationale for estimate: map [] operator.
    bool add_region(RegionID id, Name const& name);

    // Estimate of performance: O(log n)
    // Short rationale for estimate: map [] operator.
    Name get_region_name(RegionID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: traverse through n elements of a map.
    std::vector<RegionID> all_regions();

    // Estimate of performance: O(n)
    // Short rationale for estimate: std::find complexity is at most n.
    bool add_stop_to_region(StopID id, RegionID parentid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: std::find complexity is at most n.
    bool add_subregion_to_region(RegionID id, RegionID parentid);

    // Estimate of performance: O(n) ~ θ(1)
    // Short rationale for estimate: Worst case happen RARELY when we are going
    // from the leaf to the root and there are n regions in total.
    std::vector<RegionID> stop_regions(StopID id);

    // Non-compulsory operations

    // Estimate of performance: Θ(1)
    // Short rationale for estimate: left empty.
    void creation_finished();

    // Estimate of performance: O(n)
    // Short rationale for estimate: get_stops_fromRegion results in n,
    // same for loops and std::min(max)_element.
    std::pair<Coord, Coord> region_bounding_box(RegionID id);

    // Estimate of performance: O(n log n)
    // Short rationale for estimate: std::sort complexity.
    std::vector<StopID> stops_closest_to(StopID id);

    // Estimate of performance: O(n) ~ Θ(log n)
    // Short rationale for estimate: Worst case is n when going from leaf to root,
    // but on average it is log n for equal_range().
    bool remove_stop(StopID id);

    // Estimate of performance: O(n) ~ Θ(log n)
    // Short rationale for estimate: at most n for std::find_first_of and two while loops.
    RegionID stops_common_region(StopID id1, StopID id2);


    // Phase 2 operations

    // Estimate of performance: O(log n)
    // Short rationale for estimate: map [] operator.
    std::vector<RouteID> all_routes();

    // Estimate of performance: O(log n)
    // Short rationale for estimate: map [] operator.
    bool add_route(RouteID id, std::vector<StopID> stops);

    // Estimate of performance: O(log n)
    // Short rationale for estimate: map [] operator.
    std::vector<std::pair<RouteID, StopID>> routes_from(StopID stopid);

    // Estimate of performance: O(log n)
    // Short rationale for estimate: map [] operator.
    std::vector<StopID> route_stops(RouteID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: linear in the size of map for clear() method.
    void clear_routes();

    // Estimate of performance: O(n^2) ~ Θ(n log n)
    // Short rationale for estimate: RARELY at most n^2 for std::find_first_of, on average n log n.
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_any(StopID fromstop, StopID tostop);

//    // Non-compulsory operations

    // Estimate of performance: O(n^2) ~ Θ(n log n)
    // Short rationale for estimate: RARELY at most n^2 for std::find_first_of, on average n log n.
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_least_stops(StopID fromstop, StopID tostop);

    // Estimate of performance: O(n)
    // Short rationale for estimate: DFS through all stops.
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_with_cycle(StopID fromstop);

    // Estimate of performance: O(n log n)
    // Short rationale for estimate: loop through all routes through cur_stop (n) and push (log n).
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_shortest_distance(StopID fromstop, StopID tostop);

    // Estimate of performance: O(m log n) ~ Θ(log n)
    // Short rationale for estimate: map [] operator in m times (size of stop_times).
    bool add_trip(RouteID routeid, const std::vector<Time> &stop_times);

    // Estimate of performance: O(n)
    // Short rationale for estimate: n trips in the timetable.
    std::vector<std::pair<Time, Duration> > route_times_from(RouteID routeid, StopID stopid);

    // Estimate of performance: O(n log n)
    // Short rationale for estimate: n elements, each has map [] operator (log n).
    std::vector<std::tuple<StopID, RouteID, Time>> journey_earliest_arrival(StopID fromstop, StopID tostop, Time starttime);

    // Estimate of performance: O(1)
    // Short rationale for estimate: left empty.
    void add_walking_connections(); // Note! This method is completely optional, and not part of any testing

private:
    // Add stuff needed for your class implementation here
    int counter_ = 0;
    stops_vec all_stops_ = {};
    std::unordered_map<StopID, Stop> stops_map_ = {};
    std::multimap<Name, StopID> names_map_ = {};
    std::multimap<int, StopID> distance_map_ = {};
    std::unordered_map<RegionID, Region> regions_map_ = {};
    std::unordered_map<RouteID, Route> routes_map_ = {};
    std::multimap<Time, Connection> connections_dep_ = {};
    std::unordered_map<int, Connection> connections_id_ = {};
    bool existStop(StopID id);
    bool existRegion(RegionID id);
    bool existRoute(RouteID id);
    void get_stops_fromRegion(Region &cur_region, stops_vec &stops);
    bool compCoord(Coord c1, Coord c2, Coord root);
    Distance getDistance(StopID fromstop, StopID tostop);
    StopID isIntersecting(stops_vec *s_visited, stops_vec *t_visited);
    void BFS(queue_list *queue, stops_vec *visited, parent_map *parent, bool flow);
    bool DFS(stops_vec *visited, parent_map *parent, StopID cur_stop,
             std::tuple<RouteID, StopID, StopID> *return_pair);
    path_vec bidirPath(parent_map *s_parent, parent_map *t_parent,
                       StopID fromstop, StopID tostop, StopID intersectNode);
    return_tuple getTuple(path_vec *path);
};

#endif // DATASTRUCTURES_HH
