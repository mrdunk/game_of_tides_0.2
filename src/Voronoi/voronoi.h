#ifndef __GOT_VORONOI_H_
#define __GOT_VORONOI_H_

#include <unordered_map>
#include <inttypes.h>

#include "MapManagerLibrary/voronoi/VoronoiDiagramGenerator.h"
#include "../data/data.h"


class MyVoronoi : public VoronoiDiagramGenerator {
    public:
        void SetRecursion(int _recursion){
            recursion = _recursion;
        }
        void RegisterContainer(std::unordered_map<int64_t, struct MapSite>* _Container);
        static std::unordered_map<int64_t, struct MapSite>* Container;
        int recursion;
        bool VerifyGraph(int recursion);
    protected:
        void MyPushNode(struct Edge *e);
        void clip_line(struct Edge *e);
};



#endif  // __GOT_VORONOI_H_
