#include "voronoi.h"

std::unordered_map<int64_t, struct MapSite>* MyVoronoi::Container = 0;
//int MyVoronoi::recursion = 0;

void MyVoronoi::RegisterContainer(std::unordered_map<int64_t, struct MapSite>* _Container){
    Container = _Container;
}

void MyVoronoi::clip_line(struct Edge *e){
    VoronoiDiagramGenerator::clip_line(e);
    MyPushNode(e);
}

/* Build our own data structre from the primatives used in the Voronoi generator.
 * The bult in data structures provide a list of either Sites or edges but we 
 * need a data structure that also tells us about the relationship between the 2. */
void MyVoronoi::MyPushNode(struct Edge *e){
    MapSite node_reg_0 = {};
    MapSite node_reg_1 = {};
    MapSite node_ep_0 = {};
    MapSite node_ep_1 = {};

    // Fetch nodes from graph.
    if(e->reg[0]){
        node_reg_0.x = e->reg[0]->coord.x;
        node_reg_0.y = e->reg[0]->coord.y;
        node_reg_0.type = TYPE_SITE;
        if(Container->count(node_reg_0.coord())){
            // already exists so get it from the unordered_map.
            node_reg_0 = (*Container)[node_reg_0.coord()];
        } else {
            node_reg_0.recDepth = recursion;
        }
    }
    if(e->reg[1]){
        node_reg_1.x = e->reg[1]->coord.x;
        node_reg_1.y = e->reg[1]->coord.y;
        node_reg_1.type = TYPE_SITE;
        if(Container->count(node_reg_1.coord())){
            node_reg_1 = (*Container)[node_reg_1.coord()];
        } else {
            node_reg_1.recDepth = recursion;
        }
    }
    if(e->ep[0]){
        node_ep_0.x = e->ep[0]->coord.x;
        node_ep_0.y = e->ep[0]->coord.y;
        if(node_ep_0.x < 0.1) node_ep_0.x = 0.1;
        if(node_ep_0.y < 0.1) node_ep_0.y = 0.1;
        node_ep_0.type = TYPE_VPOINT;
        if(Container->count(node_ep_0.coord())){
            node_ep_0 = (*Container)[node_ep_0.coord()];
        } else {
            node_ep_0.recDepth = recursion;
        }
    }
    if(e->ep[1]){
        node_ep_1.x = e->ep[1]->coord.x;
        node_ep_1.y = e->ep[1]->coord.y;
         if(node_ep_1.x < 0.1) node_ep_1.x = 0.1;
         if(node_ep_1.y < 0.1) node_ep_1.y = 0.1;
        node_ep_1.type = TYPE_VPOINT;
        if(Container->count(node_ep_1.coord())){
            node_ep_1 = (*Container)[node_ep_1.coord()];
        } else {
            node_ep_1.recDepth = recursion;
        }
    }

    // Update nodes with other connected nodes.
    if(e->reg[0]){// and (node_reg_0.recDepth == recursion or node_reg_0.terrain == TERRAIN_SHORE)){
        if(e->reg[1]){
            node_reg_0.push_site(recursion, node_reg_1.coord());
        }
        if(e->ep[0]){
            node_reg_0.push_corner(recursion, node_ep_0.coord());
        }
        if(e->ep[1]){
            node_reg_0.push_corner(recursion, node_ep_1.coord());
        }
        // Save the node.
        (*Container)[node_reg_0.coord()] = node_reg_0;
    }

    if(e->reg[1]){// and (node_reg_1.recDepth == recursion or node_reg_1.terrain == TERRAIN_SHORE)){
        if(e->reg[0]){
            node_reg_1.push_site(recursion, node_reg_0.coord());
        }
        if(e->ep[0]){
            node_reg_1.push_corner(recursion, node_ep_0.coord());
        }
        if(e->ep[1]){
            node_reg_1.push_corner(recursion, node_ep_1.coord());
        }
        // Save the node.
        (*Container)[node_reg_1.coord()] = node_reg_1;
    }

    if(e->ep[0]){
        if(e->reg[0]){
            node_ep_0.push_site(recursion, node_reg_0.coord());
        }
        if(e->reg[1]){
            node_ep_0.push_site(recursion, node_reg_1.coord());
        }
        if(e->ep[1]){
            node_ep_0.push_corner(recursion, node_ep_1.coord());
        }
        (*Container)[node_ep_0.coord()] = node_ep_0;
    }
    if(e->ep[1]){
        if(e->reg[0]){
            node_ep_1.push_site(recursion, node_reg_0.coord());
        }
        if(e->reg[1]){
            node_ep_1.push_site(recursion, node_reg_1.coord());
        }
        if(e->ep[0]){
            node_ep_1.push_corner(recursion, node_ep_0.coord());
        }
        (*Container)[node_ep_1.coord()] = node_ep_1;
    }
}

