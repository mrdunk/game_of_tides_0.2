#ifndef __GOT_BOATS_H_
#define __GOT_BOATS_H_


struct HullSection{
    float                                   position;
    float                                   deckHeightFore;
    float                                   deckHeightAft;
    std::vector<std::pair<float,float> >    widthHeight;
};

struct Mast{
    float                                   position;
    float                                   height;
};

struct Vessel{
    std::string                         description;

    std::vector<struct HullSection>     sections;
    std::vector<struct Mast>            masts;
};





#endif // __GOT_BOATS_H_
