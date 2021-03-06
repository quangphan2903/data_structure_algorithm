// Datastructures.cc

#include "datastructures.hh"

#include <random>
#include <cmath>
#include <stdexcept>
#include <algorithm>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

using stops_vec   = std::vector<StopID>;
using regions_vec = std::vector<RegionID>;
using name_pair   = std::pair<StopID, Name>;
using coord_pair  = std::pair<StopID, Coord>;

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
    stops_map_    = {};
    names_map_    = {};
    distance_map_ = {};
    regions_map_  = {};
}

Datastructures::~Datastructures()
{

}

int Datastructures::stop_count()
{
    if (stops_map_.empty()) {
        return 0;
    } else {
        return int(stops_map_.size());
    }
}

void Datastructures::clear_all()
{
    stops_map_.clear();
    names_map_.clear();
    distance_map_.clear();
    regions_map_.clear();
}

std::vector<StopID> Datastructures::all_stops()
{
    if (stops_map_.empty()) {
        return {};
    }
    std::vector<StopID> stops = {};
    for (auto &pair : stops_map_) {
        stops.push_back(pair.first); // Get StopIDs.
    }
    return stops;
}

bool Datastructures::add_stop(StopID id, const Name& name, Coord xy)
{   
    if (existStop(id)) {
        return false;
    } else {        
        // Initially stop does not belong to any region.
        Stop new_stop  = {name, xy, ""};
        int distance   = xy.x*xy.x + xy.y*xy.y;
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
        regions_map_[id].parent = "";
        regions_map_[id].name = name;
        regions_map_[id].subregions = {};
        regions_map_[id].stops = {};
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
