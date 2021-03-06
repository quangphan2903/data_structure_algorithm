// Datastructures.cc

#include "datastructures.hh"

#include <random>
#include <cmath>
#include <stdexcept>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}


// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
    counter_         = 0;
    all_stops_       = {};
    stops_map_       = {};
    names_map_       = {};
    distance_map_    = {};
    regions_map_     = {};
    routes_map_      = {};
    connections_dep_ = {};
    connections_id_  = {};
}

Datastructures::~Datastructures()
{

}

int Datastructures::stop_count()
{
    if (all_stops_.empty()) {
        return 0;
    } else {
        return int(all_stops_.size());
    }
}

void Datastructures::clear_all()
{
    counter_ = 0;
    all_stops_.clear();    
    stops_map_.clear();
    names_map_.clear();
    distance_map_.clear();
    regions_map_.clear();
    routes_map_.clear();
    connections_dep_.clear();
    connections_id_.clear();
}

std::vector<StopID> Datastructures::all_stops()
{
    if (all_stops_.empty()) {
        return {};
    }
    return all_stops_;
}

bool Datastructures::add_stop(StopID id, const Name& name, Coord xy)
{
    if (existStop(id)) {
        return false;
    } else {
        // Initially stop does not belong to any region.
        Stop new_stop  = {name, xy, "", {}, {}};
        int distance   = xy.x*xy.x + xy.y*xy.y;
        all_stops_.push_back(id);
        stops_map_[id] = new_stop;
        names_map_.insert({name, id});
        distance_map_.insert({distance, id});
        return true;
    }
}

Name Datastructures::get_stop_name(StopID id)
{
    if (!existStop(id)) {
        return NO_NAME;
    } else {
        return stops_map_[id].name;
    }
}

Coord Datastructures::get_stop_coord(StopID id)
{
    if (!existStop(id)) {
        return NO_COORD;
    } else {
        return stops_map_[id].coord;
    }
}

std::vector<StopID> Datastructures::stops_alphabetically()
{
    stops_vec alp_stops = {};
    for (auto &pair : names_map_) {
        alp_stops.push_back(pair.second); // Get StopIDs from multimap.
    }
    return alp_stops;
}

std::vector<StopID> Datastructures::stops_coord_order()
{
    if (stops_map_.empty()) {
        return {};
    }

    std::vector<coord_pair> id_coord_vec = {};
    // Copy map to vector as map can not be sorted.
    for (auto &pair : stops_map_) {
        id_coord_vec.push_back({pair.first, get_stop_coord(pair.first)});
    }

    // Sort the coordinates based on function compCoord wrt ORIGIN.
    std::sort(id_coord_vec.begin(), id_coord_vec.end(), [this](coord_pair &p1, coord_pair &p2)
    { return compCoord(p1.second, p2.second, ORIGIN); });

    stops_vec coord_stops = {};
    for (auto &pair : id_coord_vec) {
        coord_stops.push_back(pair.first);
    }
    return coord_stops;
}

StopID Datastructures::min_coord()
{
    if (stops_map_.empty()) {
        return NO_STOP;
    }

    // Take the range of elements which have the same minimum distance,
    // then find the minimum y-coord among them. Initially, min_y is
    // assigned with upper limit of integer assuming that no point
    // reaches the limit.
    int min_y = std::numeric_limits<int>::max();
    StopID return_id = NO_STOP;
    auto iters = distance_map_.equal_range(distance_map_.begin()->first);
    for (auto it = iters.first; it != iters.second; it++) {
        if (get_stop_coord(it->second).y < min_y) {
            min_y = get_stop_coord(it->second).y;
            return_id = it->second;
        }
    }
    return return_id;
}

StopID Datastructures::max_coord()
{
    if (stops_map_.empty()) {
        return NO_STOP;
    }

    // Take the range of elements which have the same maximum distance,
    // then find the maximum y-coord among them. Initially, max_y is
    // assigned with -1 as no coord is negative.
    int max_y = -1;
    StopID return_id = NO_STOP;
    auto iters = distance_map_.equal_range(distance_map_.rbegin()->first);
    for (auto it = iters.first; it != iters.second; it++) {
        if (get_stop_coord(it->second).y >= max_y) {
            max_y = get_stop_coord(it->second).y;
            return_id = it->second;
        }
    }
    return return_id;
}

std::vector<StopID> Datastructures::find_stops(Name const& name)
{
    stops_vec stops = {};
    // Return a range of elements which have the same key "name" using equal_range.
    auto iters = names_map_.equal_range(name);
    for (auto it = iters.first; it != iters.second; it++) {
        stops.push_back(it->second);
    }

    if (stops.empty()) {
        return {NO_STOP};
    } else {
        return stops;
    }
}

bool Datastructures::change_stop_name(StopID id, const Name& newname)
{
    if (!existStop(id)) {
        return false;
    }

    Name oldname = get_stop_name(id);
    // Look for the stop with given id in names_map_ using equal_range.
    // Erase the element found and insert new element with "newname".
    auto iters = names_map_.equal_range(oldname);
    for (auto it = iters.first; it != iters.second; it++) {
        if (it->second == id) {
            names_map_.erase(it);
            break;
        }
    }
    names_map_.insert({newname, id});

    stops_map_[id].name = newname; // Change in main container.
    return true;
}

bool Datastructures::change_stop_coord(StopID id, Coord newcoord)
{
    if (!existStop(id)) {
        return false;
    }

    // Look for the stop with given id in distance_map_ using equal_range.
    // Erase the element found and insert new element with "newcoord".
    int new_distance = newcoord.x*newcoord.x + newcoord.y*newcoord.y;
    Coord old_coord = get_stop_coord(id);
    int old_distance = old_coord.x*old_coord.x + old_coord.y*old_coord.y;
    auto iters = distance_map_.equal_range(old_distance);
    for (auto it = iters.first; it != iters.second; it++) {
        if (it->second == id) {
            distance_map_.erase(it);
            break;
        }
    }
    distance_map_.insert({new_distance, id});

    stops_map_[id].coord = newcoord; // Change in main container.
    return true;
}

bool Datastructures::add_region(RegionID id, const Name &name)
{
    if (existRegion(id)) {
        return false;
    } else {
        // Initially a region is not a subregion of another.
//        regions_map_[id].parent = "";
//        regions_map_[id].name = name;
//        regions_map_[id].subregions = {};
//        regions_map_[id].stops = {};
        regions_map_[id] = {"", name , {}, {}};
        return true;
    }
}

Name Datastructures::get_region_name(RegionID id)
{
    if (!existRegion(id)) {
        return NO_NAME;
    } else {
        return regions_map_[id].name;
    }
}

std::vector<RegionID> Datastructures::all_regions()
{
    if (regions_map_.empty()) {
        return {};
    }
    regions_vec regions = {};
    for (auto &pair : regions_map_) {
        regions.push_back(pair.first); // Get RegionIDs.
    }
    return regions;
}

bool Datastructures::add_stop_to_region(StopID id, RegionID parentid)
{
    if (!existRegion(parentid)) {
        return false;
    }
    stops_vec cur_stops = regions_map_[parentid].stops;
    // Check if given stop already belongs to given region besides existness.
    if ((std::find(cur_stops.begin(), cur_stops.end(), id)
         != cur_stops.end()) || !existStop(id)) {
        return false;
    } else {
        // Connections are made in both stop and region 's properties.
        stops_map_[id].parent = parentid;
        regions_map_[parentid].stops.push_back(id);
        return true;
    }
}

bool Datastructures::add_subregion_to_region(RegionID id, RegionID parentid)
{
    if (!existRegion(parentid)) {
        return false;
    }
    std::vector<RegionID> cur_subregions = regions_map_[parentid].subregions;
    // Check if given region is already a subregion of given parent
    // region besides existness.
    if ((std::find(cur_subregions.begin(), cur_subregions.end(), id)
         != cur_subregions.end()) || !existRegion(id)) {
        return false;
    } else {
        // Make connections in both regions' properties.
        regions_map_[id].parent = parentid;
        regions_map_[parentid].subregions.push_back(id);
        return true;
    }
}

std::vector<RegionID> Datastructures::stop_regions(StopID id)
{
    if (!existStop(id)) {
        return {NO_REGION};
    }

    regions_vec regions = {};
    RegionID cur_region_id = stops_map_[id].parent;
    // If given stop does not belong to any region.
    if (cur_region_id == "") {
        return {NO_REGION};
    }
    regions.push_back(cur_region_id); // Direct region.

    // Check for parents of direct region and add if exist.
    while (regions_map_[cur_region_id].parent != "") {
        regions.push_back(regions_map_[cur_region_id].parent);
        cur_region_id = regions_map_[cur_region_id].parent;
    }
    return regions;
}

void Datastructures::creation_finished()
{
    // Replace this comment with your implementation
    // You don't have to use this method for anything, if you don't need it
}

std::pair<Coord,Coord> Datastructures::region_bounding_box(RegionID id)
{
    if (!existRegion(id)) {
        return {NO_COORD, NO_COORD};
    }

    stops_vec stops = {};
    get_stops_fromRegion(regions_map_[id], stops);

    // No stops in given region.
    if (stops.empty()) {
        return {NO_COORD, NO_COORD};
    } else {
        std::vector<int> x_coords = {}, y_coords = {};
        for (auto &stop_id : stops) {
            x_coords.push_back(stops_map_[stop_id].coord.x);
            y_coords.push_back(stops_map_[stop_id].coord.y);
        }
        // Get max and min element in x,y vectors using STL algorithm.
        Coord bottom_left = {*std::min_element(x_coords.begin(), x_coords.end()),
                             *std::min_element(y_coords.begin(), y_coords.end())};
        Coord top_right   = {*std::max_element(x_coords.begin(), x_coords.end()),
                             *std::max_element(y_coords.begin(), y_coords.end())};

        return {bottom_left, top_right};
    }
}

std::vector<StopID> Datastructures::stops_closest_to(StopID id)
{
    if (!existStop(id)) {
        return {NO_STOP};
    }

    Stop cur_stop = stops_map_[id];
    // Save the stops to a vector, as map can not be sorted.
    std::vector<coord_pair> stops = {};
    for (auto &pair : stops_map_) {
        stops.push_back({pair.first, get_stop_coord(pair.first)});
    }
    // Sort the resulting vector using function compCoord wrt current stop coords.
    std::sort(stops.begin(), stops.end(), [cur_stop, this](coord_pair stop1, coord_pair stop2)
    { return compCoord(get_stop_coord(stop1.first), get_stop_coord(stop2.first), cur_stop.coord); });

    // Get StopIDs from resulting sorted vector.
    stops_vec closest_stops = {};
    for (auto it = stops.begin(); it!= stops.end(); it++) {
        closest_stops.push_back(it->first);
    }

    stops_vec sub_vec = {};
    if (closest_stops.size() < 6) {
        // Output everything if there are less than 6 stops.
        sub_vec = stops_vec(closest_stops.begin() + 1, closest_stops.end());
    } else {
        // Output first five closest stops.
        sub_vec = stops_vec(closest_stops.begin() + 1, closest_stops.begin() + 6);
    }
    return sub_vec;
}

bool Datastructures::remove_stop(StopID id)
{
    if (!existStop(id)) {
        return false;
    }

    auto stop_it = stops_map_.find(id);
    // If given stop belongs to a region, then delete the connection there.
    if (stop_it->second.parent != "") {
        auto region_it = regions_map_.find(stop_it->second.parent);
        if (region_it != regions_map_.end()) {
            auto pos = std::find(region_it->second.stops.begin(),
                                 region_it->second.stops.end(), id);
            region_it->second.stops.erase(pos);
        }
    }
    // Delete the stop in the containers. Find element in multimap using
    // StopID as search key because of its uniqueness.
    Name oldname = get_stop_name(id);
    auto name_iters = names_map_.equal_range(oldname);
    for (auto it = name_iters.first; it != name_iters.second; it++) {
        if (it->second == id) {
            names_map_.erase(it);
            break;
        }
    }
    Coord coord = get_stop_coord(id);
    int distance = coord.x*coord.x + coord.y*coord.y;
    auto distance_iters = distance_map_.equal_range(distance);
    for (auto it = distance_iters.first; it != distance_iters.second; it++) {
        if (it->second == id) {
            distance_map_.erase(it);
            break;
        }
    }
    std::vector<Connection> connections_to_del = {};

    stops_map_.erase(stop_it);
    return true;
}

RegionID Datastructures::stops_common_region(StopID id1, StopID id2)
{
    if (!existStop(id1) || !existStop(id2)) {
        return NO_REGION;
    }

    RegionID region_id1 = stops_map_[id1].parent;
    RegionID region_id2 = stops_map_[id2].parent;
    // If any of the given stop has no direct region, then no need to check.
    if (region_id1 == "" || region_id2 == "") {
        return NO_REGION;
    }

    std::vector<RegionID> parent1 = {region_id1}, parent2 = {region_id2};
    // Get all parents of the two given stops.
    while (regions_map_[region_id1].parent != "") {
        parent1.push_back(regions_map_[region_id1].parent);
        region_id1 = regions_map_[region_id1].parent;
    }
    while (regions_map_[region_id2].parent != "") {
        parent2.push_back(regions_map_[region_id2].parent);
        region_id2 = regions_map_[region_id2].parent;
    }

    // Find first common region between two stops.
    auto common_dir = std::find_first_of(parent1.begin(), parent1.end(),
                                         parent2.begin(), parent2.end());
    if (common_dir == parent1.end()) {
        return NO_REGION;
    } else {
        return *common_dir;
    }
}

std::vector<RouteID> Datastructures::all_routes()
{
    if (routes_map_.empty()) {
        return {};
    }
    routes_vec routes = {};
    for (auto &pair : routes_map_) {
        routes.push_back(pair.first); // Get RouteIDs.
    }
    return routes;
}

bool Datastructures::add_route(RouteID id, std::vector<StopID> stops)
{   
    if (existRoute(id) || stops.size() <= 1) {
        return false;
    }

    for (auto it = stops.begin(); it != stops.end(); it++) {
        if (!existStop(*it)) {
            return false; // Stop does not exist.
        }
        if (it == stops.begin()) {
            stops_map_[*it].routes[id] = {NO_STOP, *(it+1)}; // Starting stop.
        } else if (it == stops.end() - 1) {
            stops_map_[*it].routes[id] = {*(it-1), NO_STOP}; // End stop.
        } else {
            stops_map_[*it].routes[id] = {*(it-1), *(it+1)}; // Connecting stop.
        }
    }
    routes_map_[id] = {stops};
    return true;
}

path_vec Datastructures::routes_from(StopID stopid)
{
    if (!existStop(stopid)) {
        return {{NO_ROUTE, NO_STOP}};
    }

    auto routes = stops_map_[stopid].routes;
    if (routes.empty()) {
        return {}; // No route passing through this stop.
    }
    path_vec return_vec = {};
    for (auto pair : routes) {
        if (pair.second.second == NO_STOP) {
            continue;
        }
        return_vec.push_back({pair.first, pair.second.second});
    }
    return return_vec;
}

stops_vec Datastructures::route_stops(RouteID id)
{
    if (!existRoute(id)) {
        return {NO_STOP};
    }
    stops_vec return_vec = routes_map_[id].stops;
    return return_vec;
}

void Datastructures::clear_routes()
{
    for (auto& pair : stops_map_) {
        pair.second.routes.clear();
        pair.second.time_tables.clear();
    }
    routes_map_.clear();
    connections_dep_.clear();
    connections_id_.clear();
}

return_tuple Datastructures::journey_any(StopID fromstop, StopID tostop)
{
    return_tuple return_vec = journey_least_stops(fromstop, tostop);
    return return_vec;
}

return_tuple Datastructures::journey_least_stops(StopID fromstop, StopID tostop)
{
        if (!existStop(fromstop) || !existStop(tostop) || fromstop == tostop) {
        return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
    }    
    // In this function, I use bidirectional BFS search for fromstop and tostop.

    // Visited stops of forward and backward searches
    stops_vec fw_visited = {}, bw_visited = {};

    // Save previous StopID and routeID from that stop
    parent_map fw_parent = {};
    parent_map bw_parent = {};

    // queue for front and backward search
    std::list<StopID> fw_queue, bw_queue;

    StopID intersectNode = NO_STOP;

    fw_queue.push_back(fromstop);
    fw_visited.push_back(fromstop);

    bw_queue.push_back(tostop);
    bw_visited.push_back(tostop);

    while (!fw_queue.empty() && !bw_queue.empty())
    {
        // Do BFS from source and target vertices.
        // True indicates forward search and false shows backward search.
        BFS(&fw_queue, &fw_visited, &fw_parent, true);
        BFS(&bw_queue, &bw_visited, &bw_parent, false);

        // check for intersecting vertex
        intersectNode = isIntersecting(&fw_visited, &bw_visited);

        // If intersecting vertex is found that means there exist a path
        if(intersectNode != NO_STOP) {
            // Get the path and return
            auto path = bidirPath(&fw_parent, &bw_parent, fromstop, tostop, intersectNode);
            return getTuple(&path);
        }
    }
    return {{NO_STOP, NO_ROUTE, NO_DISTANCE}}; // No route has been found.
}

return_tuple Datastructures::journey_with_cycle(StopID fromstop)
{
    if (!existStop(fromstop)) {
        return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
    } 

    // In this function, I use recursive DFS.

    stops_vec visited = {}; // Vector storing visited stops.
    parent_map parent = {}; // Parent of each visited stops.
    // This variable saves the way from the second last stop to the repeated
    // stop in the path. For example, the cycle is 1 -> 2 -> 3 -> 4 -> 5 -> 2,
    // then cycle = {route from 5 to 2, 5, 2}.
    std::tuple<RouteID, StopID, StopID> cycle_point = {NO_ROUTE, NO_STOP, NO_STOP};
    bool found = DFS(&visited, &parent, fromstop, &cycle_point); // Recursive DFS.

    if (!found) {
        return {}; // Can not find a cycle.
    }
    StopID cur_stop = std::get<1>(cycle_point);
    path_vec path = {};
    // Now we get the path from the starting stop to the second last stop
    // by tracing back the parent of each stop.
    path.push_back({std::get<0>(cycle_point), cur_stop}); // Second last stop
    while (cur_stop != fromstop) {
        // Backward traversal
        path.push_back({parent[cur_stop].first, parent[cur_stop].second});
        cur_stop = parent[cur_stop].second;
    }
    // As we go backward, we have to reverse the path found.
    std::reverse(path.begin(), path.end());
    path.push_back({NO_ROUTE, std::get<2>(cycle_point)}); // Repeating stop, end point.
    return getTuple(&path);
}

return_tuple Datastructures::journey_shortest_distance(StopID fromstop, StopID tostop)
{
    if (!existStop(fromstop) || !existStop(tostop) || fromstop == tostop) {
        return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
    }

    // In this function, I use A* algorithm, in which distances so far are g
    // scores and the Euclidian distance between the current stop and end
    // stop is h score. f score for each stop: f = g + h which is put into
    // the queue together with the stopid.

    parent_map parent = {};
    std::unordered_map<StopID, Distance> distances = {}; // g score for each stop.
    // A priority queue to save StopID based on its f score.
    typedef std::pair<Distance, StopID> queue_pair;
    std::priority_queue<queue_pair, std::vector<queue_pair>,
                        std::greater<queue_pair>> frontier;

    for (auto stopid : all_stops_) {
        distances[stopid] = MAX_VALUE; // Initiate g score with infinity.
    }
    // Statistics for starting point.
    distances[fromstop] = 0;
    frontier.push({getDistance(fromstop, tostop), fromstop});

    while (!frontier.empty()) {
        // Get top element from the queue
        StopID cur_stop = frontier.top().second;
        frontier.pop();

        if (cur_stop == tostop) {
            break; // Reach the end point.
        }

        for (auto pair : stops_map_[cur_stop].routes) {
            StopID next_stop = pair.second.second;
            if (next_stop == NO_STOP) {
                continue; // Terminus stop has no adjacent
            }
            Distance new_dist = distances[cur_stop] + getDistance(cur_stop, next_stop);
            if (new_dist < distances[next_stop]) {
                distances[next_stop] = new_dist; // New h score for next_stop.
                // New f score for next stop, divided h by 100 to get a less
                // significant h score compared to g score.
                Distance priority = new_dist + getDistance(next_stop, tostop)/100;
                frontier.push({priority, next_stop});
                parent[next_stop] = {pair.first, cur_stop};
            }
        }
    }
    if (parent.find(tostop) == parent.end()) {
        return {}; // No journey to the end point.
    }
    path_vec path = {};
    // Trace back from end point using parent connection. Reverse the path
    // when we encounter the starting point.
    StopID cur_stop = tostop;
    while (cur_stop != fromstop) {
        path.push_back({parent[cur_stop].first, parent[cur_stop].second});
        cur_stop = parent[cur_stop].second;
    }
    std::reverse(path.begin(), path.end());
    path.push_back({NO_ROUTE, tostop});
    return getTuple(&path);
}

bool Datastructures::add_trip(RouteID routeid, std::vector<Time> const& stop_times)
{
    if (!existRoute(routeid) || stop_times.empty()) {
        return false;
    }

    Route& route = routes_map_[routeid];
    std::vector<StopID> stops = route.stops;
    if (stop_times.size() != stops.size()) {
        return false; // Departure time must be given to all stops in route.
    }

    // Add departure time to each stop property.
    auto stop_it = stops.begin();
    auto time_it = stop_times.begin();
    while (true) {
        stops_map_[*stop_it].time_tables[routeid].insert(*time_it);
        if (stop_it != stops.end() - 1) {
            // Add new connections for all stops except terminus stop.
            Connection new_connection = {counter_, routeid, (*stop_it),
                                         (*(stop_it+1)), (*time_it), (*(time_it+1))};
            connections_dep_.insert({*time_it,new_connection});
            connections_id_[counter_] = new_connection;
            counter_++;
        }
        stop_it++;
        time_it++;
        if (stop_it == stops.end() || time_it == stop_times.end()) {
            return true;
        }
    }
}

std::vector<std::pair<Time, Duration>> Datastructures::route_times_from(RouteID routeid, StopID stopid)
{
    if (!existRoute(routeid) || !existStop(stopid)) {
        return {{NO_TIME, NO_DURATION}};
    }

    Stop cur_stop = stops_map_[stopid];
    if (cur_stop.time_tables.find(routeid) == cur_stop.time_tables.end()) {
        return {{NO_TIME, NO_DURATION}}; // Route does not go through the stop.
    }
    StopID next_stop = cur_stop.routes[routeid].second;
    if (next_stop == NO_STOP) { // Terminus stop
        return {};
    }

    std::vector<std::pair<Time, Duration>> return_vec = {};
    std::set<Time> cur_timetable = cur_stop.time_tables[routeid];
    std::set<Time> next_timetable = stops_map_[next_stop].time_tables[routeid];
    // Return departure time and duration to next stop for each departure time
    // of this stop in the route.
    auto cur_it = cur_timetable.begin();
    auto next_it = next_timetable.begin();
    while (true) {
        return_vec.push_back({*cur_it, *next_it - *cur_it});
        cur_it++;
        next_it++;
        if (cur_it == cur_timetable.end() || next_it == next_timetable.end()) {
            return return_vec;
        }
    }
}

return_tuple Datastructures::journey_earliest_arrival(StopID fromstop, StopID tostop, Time starttime)
{
    if (!existStop(fromstop) || !existStop(tostop) || fromstop == tostop) {
        return {{NO_STOP, NO_ROUTE, NO_TIME}};
    }

    // In this function, I use Connection Scan Algorithm.

    std::unordered_map<StopID, Time> earliest_arrival = {};
    std::unordered_map<StopID, int> in_connection = {};
    // Initialization
    for (auto& stopid : all_stops_) {
        earliest_arrival[stopid] = MAX_VALUE;
        in_connection[stopid] = NO_CONNECTION;
    }
    // Statistics for starting stop.
    earliest_arrival[fromstop] = starttime;
    Time earliest = MAX_VALUE;

    for (auto& pair : connections_dep_) {
        Connection connection = connections_id_[pair.second.id];
        // If this trip departs not earlier and arrive earlier than any
        // trip from fromstop, then we mark this trip for it.
        if (connection.departure >= earliest_arrival[(connection.fromstop)]
            && connection.arrrival < earliest_arrival[connection.tostop]) {            
            earliest_arrival[connection.tostop] = connection.arrrival;
            // Get this connection id.
            in_connection[connection.tostop] = pair.second.id;

            // If we manage to arrive, get our earliest time.
            // This is a criterion for exiting the loop.
            if (connection.tostop == tostop) {
                earliest = std::min(earliest, connection.arrrival);
            }
        } else if (connection.arrrival  > earliest) {
            break;
        }
    }

    // Reconstruct the path
    if (in_connection[tostop] == NO_CONNECTION) {
        return {}; // Can not reach end point.
    } else {
        // Traverse backward through connection ids marked.
        std::vector<Connection> route = {};
        int last_connection_pos = in_connection[tostop];
        while (last_connection_pos != NO_CONNECTION) {
            Connection cur_connection = connections_id_[last_connection_pos];
            route.push_back(cur_connection);
            last_connection_pos = in_connection[cur_connection.fromstop];
        }
        std::reverse(route.begin(), route.end());

        // Get return vector from the path.
        return_tuple return_vec = {};
        for (auto it = route.begin(); it != route.end(); it++) {
            return_vec.push_back({it->fromstop, it->routeid, it->departure});
        }
        auto last_stop = *route.rbegin();
        return_vec.push_back({last_stop.tostop, NO_ROUTE, last_stop.arrrival});
        return return_vec;
    }
}

void Datastructures::add_walking_connections()
{
    // Replace this comment and the line below with your implementation
}

bool Datastructures::existStop(StopID id) {
    // Check if given stop id is added to main stop container.
    if (stops_map_.find(id) != stops_map_.end()) {
        return true;
    } else {
        return false;
    }
}

bool Datastructures::existRegion(RegionID id) {
    // Check if given region id is added to main region container.
    if (regions_map_.find(id) != regions_map_.end()) {
        return true;
    } else {
        return false;
    }
}

bool Datastructures::existRoute(RouteID id) {
    // Check if given route id is added to main route container.
    if (routes_map_.find(id) != routes_map_.end()) {
        return true;
    } else {
        return false;
    }
}

void Datastructures::get_stops_fromRegion(Region &cur_region, std::vector<StopID> &stops) {
    // A recursive function to get direct stops of current region, and then
    // apply for its subregions.
    for (auto &stop_id : cur_region.stops) {
        stops.push_back(stop_id); // Get direct stops.
    }
    // Trivial case when current regions has no subregions.
    if (cur_region.subregions.empty()) {
        return;
    }
    // Recursive part.
    for (auto &subregion_id : cur_region.subregions) {
        get_stops_fromRegion(regions_map_[subregion_id] , stops);
    }
}

bool Datastructures::compCoord(Coord c1, Coord c2, Coord root) {
    // A function to check the distances between two coordinates and the root.
    int d_c1 = (c1.x - root.x)*(c1.x - root.x) + (c1.y - root.y)*(c1.y - root.y);
    int d_c2 = (c2.x - root.x)*(c2.x - root.x) + (c2.y - root.y)*(c2.y - root.y);

    // Coordinate with smaller distance or similar distance but smaller
    // relative y-coordinate comes first.
    if ((d_c1 < d_c2) || ((d_c1 == d_c2) && (c1.y - root.y < c2.y - root.y))) { return true; }
    else { return false; }
}

Distance Datastructures::getDistance(StopID fromstop, StopID tostop) {
    Coord from = stops_map_[fromstop].coord;
    Coord to = stops_map_[tostop].coord;
    int d_2 = (from.x - to.x)*(from.x - to.x) + (from.y - to.y)*(from.y - to.y);
    return int(sqrt(d_2));
}

StopID Datastructures::isIntersecting(stops_vec *fw_visited, stops_vec *bw_visited) {
    auto common_stop = std::find_first_of(fw_visited->begin(), fw_visited->end(),
                                          bw_visited->begin(), bw_visited->end());
    if (*common_stop != *fw_visited->end()) {
        return *common_stop;
    } else {
        return NO_STOP;
    }
};

void Datastructures::BFS(std::list<StopID> *queue, stops_vec *visited, parent_map *parent, bool flow) {
    // flow -> forward search, not flow -> backward search.
    // Retrieve first item from queue.
    StopID current = queue->front();
    queue->pop_front();
    auto adj_map = stops_map_[current].routes;
    for (auto pair : adj_map) {
        // If adjacent vertex is not visited earlier
        // mark it visited by assigning true value
        StopID adj_id;
        if (flow) {
            adj_id = pair.second.second;
        } else {
            adj_id = pair.second.first;
        }
        if (adj_id == NO_STOP) {
            continue; // Terminus stop.
        }
        // Not visited
        if (std::find(visited->begin(), visited->end(), adj_id) == visited->end()) {
            // set current as parent of this vertex
            (*parent)[adj_id] = {pair.first, current};

            // Mark this vertex visited
            (*visited).push_back(adj_id);

            // Push to the end of queue
            queue->push_back(adj_id);
        }
    }
};

bool Datastructures::DFS(stops_vec *visited, parent_map *parent, StopID cur_stop, std::tuple<RouteID, StopID, StopID> *return_pair) {
    bool found = false;
    visited->push_back(cur_stop);
    auto adj_map = stops_map_[cur_stop].routes;
    for (auto pair : adj_map) {
        StopID adj_id = pair.second.second;
        if (adj_id == NO_STOP) {
            continue; // Terminus stop.
        }

        // Found a repeated stop as it has two parents.
        if (parent->find(adj_id) != parent->end() && (*parent)[adj_id].second != cur_stop) {
            *return_pair = {pair.first, cur_stop, adj_id};
            return true;
        }

        // Not visited.
        if (std::find(visited->begin(), visited->end(), adj_id) == visited->end()) {
            // set current as parent of this vertex
            (*parent)[adj_id] = {pair.first, cur_stop};

            // DFS recursion for all unvisited stops
            found = DFS(visited, parent, adj_id, return_pair);
            if (found) {
                return true;
            }
        }
    }
    return false;
}

path_vec Datastructures::bidirPath(parent_map *fw_parent, parent_map *bw_parent, StopID fromstop, StopID tostop, StopID intersectNode)
{
    path_vec path;
    StopID id = intersectNode;
    // Now we get the path from forward search.
    while (id != fromstop) {
        path.push_back({(*fw_parent)[id].first, (*fw_parent)[id].second});
        id = (*fw_parent)[id].second;
    }
    reverse(path.begin(), path.end()); // Reverse as we go backward.
    id = intersectNode;
    // Add intersectNode manually.
    path.push_back({(*bw_parent)[intersectNode].first, intersectNode});
    // Retrieve path from backward search.
    // This time we are in the right track.
    while(id != tostop) {
        path.push_back({(*bw_parent)[(*bw_parent)[id].second].first, (*bw_parent)[id].second});
        id = (*bw_parent)[id].second;
    }
    // Last element needs to be modified to match the format in the description.
    path.pop_back();
    path.push_back({NO_ROUTE, tostop});
    return path;
}


return_tuple Datastructures::getTuple(path_vec *path) {
    // Get output format from given path.
    Distance distance = 0;
    return_tuple return_vec = {};
    auto it = path->begin();
    while(true) {
        return_vec.push_back({it->second, it->first, distance});
        it++;
        if (it == path->end()) {
            break;
        }
        distance += getDistance(it->second, (it-1)->second);
    }
    return return_vec;
};
