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
            //if(node_reg_0.type != TYPE_SITE){
            //    cout << "  ** baws 1 **\n";
            //}
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
            //if(node_reg_1.type != TYPE_SITE){
            //    cout << "  ** baws 2 **\n";
            //}
        } else {
            node_reg_1.recDepth = recursion;
        }
    }
    if(e->ep[0]){
        node_ep_0.x = round(e->ep[0]->coord.x);
        node_ep_0.y = round(e->ep[0]->coord.y);
        if(node_ep_0.x < 0) node_ep_0.x = 0;
        if(node_ep_0.y < 0) node_ep_0.y = 0;
        if(node_ep_0.x > MAP_SIZE) node_ep_0.x = MAP_SIZE;
        if(node_ep_0.y > MAP_SIZE) node_ep_0.y = MAP_SIZE;
        node_ep_0.type = TYPE_CORNER;
        if(Container->count(node_ep_0.coord())){
            node_ep_0 = (*Container)[node_ep_0.coord()];
            if(node_ep_0.type != TYPE_CORNER){
                cout << "  ** baws 3 **\n";
                e->ep[0] = 0;
            }
        } else {
            node_ep_0.recDepth = recursion;
        }
    }
    if(e->ep[1]){
        node_ep_1.x = round(e->ep[1]->coord.x);
        node_ep_1.y = round(e->ep[1]->coord.y);
         if(node_ep_1.x < 0) node_ep_1.x = 0;
         if(node_ep_1.y < 0) node_ep_1.y = 0;
         if(node_ep_1.x > MAP_SIZE) node_ep_1.x = MAP_SIZE;
         if(node_ep_1.y > MAP_SIZE) node_ep_1.y = MAP_SIZE;
        node_ep_1.type = TYPE_CORNER;
        if(Container->count(node_ep_1.coord())){
            node_ep_1 = (*Container)[node_ep_1.coord()];
            if(node_ep_1.type != TYPE_CORNER){
                cout << "  ** baws 4 **\n";
                e->ep[1] = 0;
            }
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

    if(node_reg_0.type != TYPE_SITE) std::cout << "voronoli baws. 1 " << node_reg_0.type << "\t" << node_reg_0.x << "\t" << node_reg_0.y << "\t" << node_reg_0.coord() << "\n";
    if(node_reg_1.type != TYPE_SITE) std::cout << "voronoli baws. 2 " << node_reg_1.type << "\t" << node_reg_1.x << "\t" << node_reg_1.y << "\t" << node_reg_1.coord() << "\n";
    if(node_ep_0.type != TYPE_CORNER) std::cout << "voronoli baws. 3 " << node_ep_0.type << "\t" << node_ep_0.x << "\t" << node_ep_0.y << "\t" << node_ep_0.coord() << "\n";
    if(node_ep_1.type != TYPE_CORNER) std::cout << "voronoli baws. 4 " << node_ep_1.type << "\t" << node_ep_1.x << "\t" << node_ep_1.y << "\t" << node_ep_1.coord() << "\n";
}

bool MyVoronoi::VerifyGraph(int recursion){
    cout << "\n--------------------------------------------\n";
    float x, y;
    bool fail;
    bool retval = true;
    int count = 0;

    for(auto it = Container->begin(); it != Container->end(); ++it){
        fail = false;

        if(it->first != it->second.coord()){
            cout << "baws\n";
            fail = true;
        }

        KeyToCoord(it->first, x, y);
        if(fabs(x - it->second.x) > 0 or fabs(y - it->second.y) > 0){
            cout << "baws\n";
            fail = true;
        }

        if(it->second.num_corners(recursion) >= 0){
            for(auto itCorner = it->second.corner_begin(recursion); itCorner != it->second.corner_end(recursion); ++itCorner){
                if((*Container)[*itCorner].type != TYPE_CORNER){
                    cout << "baws type for itCorner: " << (*Container)[*itCorner].type << "\n";
                    cout << "           " << *itCorner << "\n";
                    fail = true;
                }

                if(it->second.type == TYPE_CORNER){
                    if((*Container)[*itCorner].corner_count(recursion, it->first) == 0){
                        cout << "c" << (*Container)[*itCorner].corner_count(recursion, it->first) << " " << (*Container)[*itCorner].num_corners(recursion) << "\n";
                        //(*Container)[*itCorner].push_corner(recursion, it->first);
                        fail = true;
                    }
                } else if(it->second.type == TYPE_SITE){
                    if((*Container)[*itCorner].site_count(recursion, it->first) == 0){
                        cout << "s" << (*Container)[*itCorner].site_count(recursion, it->first) << " " << (*Container)[*itCorner].num_sites(recursion) << "\n";
                        fail = true;
                    }
                }
            }
        }

        if(it->second.num_sites(recursion) >= 0){
            for(auto itSite = it->second.site_begin(recursion); itSite != it->second.site_end(recursion); ++itSite){
                if((*Container)[*itSite].type != TYPE_SITE){
                    cout << "baws type for itSite:   " << (*Container)[*itSite].type << "\n";
                    cout << "                        " << *itSite << "\n";
                    fail = true;
                }

                if(it->second.type == TYPE_CORNER){
                    if((*Container)[*itSite].corner_count(recursion, it->first) == 0){
                        cout << "C" << (*Container)[*itSite].corner_count(recursion, it->first) << " " << (*Container)[*itSite].num_corners(recursion) << "\n";
                        //(*Container)[*itSite].push_corner(recursion, it->first);
                        fail = true;
                    }
                } else if(it->second.type == TYPE_SITE){
                    if((*Container)[*itSite].site_count(recursion, it->first) == 0){
                        cout << "S" << (*Container)[*itSite].site_count(recursion, it->first) << " " << (*Container)[*itSite].num_sites(recursion) << "\n";
                        fail = true;
                    }
                }
            }
        }
        if(!fail and ++count % 1000 == 0) cout << "." << flush;
        if(fail) retval = false;
    }
    cout << "\n--------------------------------------------\n";

    /*
    std::unordered_set<int64_t> open, closed;
    open.insert(Container->begin()->second.coord());
    while(open.size() > 0){
        int64_t working = *(open.begin());
        open.erase(working);
        closed.insert(working);
        for(auto itSite = (*Container)[working].site_begin(recursion); itSite != (*Container)[working].site_end(recursion); ++itSite){
            if(closed.count(*itSite) == 0){
                open.insert(*itSite);
            }
        }
        for(auto itCorner = (*Container)[working].corner_begin(recursion); itCorner != (*Container)[working].corner_end(recursion); ++itCorner){
            if(closed.count(*itCorner) == 0){
                open.insert(*itCorner);
            }
        }
    }
    cout << "closed:    " << closed.size() << "\n";
    cout << "Container: " << Container->size() << "\n";

    for(auto it = Container->begin(); it != Container->end(); ++it){
        if(it->second.recDepth == recursion and closed.count(it->first) == 0){
            cout << it->second.x << "\t" << it->second.y << "\n";
            Container->erase(it->first);
        }
    }

    cout << "\n--------------------------------------------\n";*/
    return retval;
}



