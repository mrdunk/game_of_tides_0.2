

#include "boats.h"



Vessel testBoat(void){
    Vessel returnBoat;
    returnBoat.description = "Test Boat";

    HullSection s1, s2, s3, s4, s5, s6, s7, s8;

    s1.position = 0;
    s1.widthHeight.push_back(make_pair(1.0, 0));
    s1.widthHeight.push_back(make_pair(0.4, 0));
    s1.widthHeight.push_back(make_pair(0.0, 0));
    s1.widthHeight.push_back(make_pair(-0.3, 0));
    s1.widthHeight.push_back(make_pair(-0.4, 0));

    s2.position = 1.5;
    s2.widthHeight.push_back(make_pair(1.0, 0.8));
    s2.widthHeight.push_back(make_pair(0.0, 1.1));
    s2.widthHeight.push_back(make_pair(-0.8, 0.8));
    s2.widthHeight.push_back(make_pair(-1.3, 0.1));
    s2.widthHeight.push_back(make_pair(-1.6, 0.0));

    s3.position = 3.5;
    s3.widthHeight.push_back(make_pair(1.0, 1.2));
    s3.widthHeight.push_back(make_pair(0.0, 1.6));
    s3.widthHeight.push_back(make_pair(-1.2, 1.1));
    s3.widthHeight.push_back(make_pair(-1.7, 0.1));
    s3.widthHeight.push_back(make_pair(-2.0, 0.0));

    s4.position = 3.5;
    s4.widthHeight.push_back(make_pair(0.5, 1.4));
    s4.widthHeight.push_back(make_pair(0.0, 1.6));
    s4.widthHeight.push_back(make_pair(-1.2, 1.1));
    s4.widthHeight.push_back(make_pair(-1.7, 0.1));
    s4.widthHeight.push_back(make_pair(-2.0, 0.0));

    s5.position = 7.5;
    s5.widthHeight.push_back(make_pair(0.5, 1.4));
    s5.widthHeight.push_back(make_pair(0.0, 1.6));
    s5.widthHeight.push_back(make_pair(-1.2, 1.1));
    s5.widthHeight.push_back(make_pair(-1.7, 0.1));
    s5.widthHeight.push_back(make_pair(-2.0, 0.0));

    s6.position = 7.5;
    s6.widthHeight.push_back(make_pair(1.0, 1.2));
    s6.widthHeight.push_back(make_pair(0.0, 1.6));
    s6.widthHeight.push_back(make_pair(-1.2, 1.1));
    s6.widthHeight.push_back(make_pair(-1.7, 0.1));
    s6.widthHeight.push_back(make_pair(-2.0, 0.0));

    s7.position = 9;
    s7.widthHeight.push_back(make_pair(1.0, 1.0));
    s7.widthHeight.push_back(make_pair(0.0, 1.3));
    s7.widthHeight.push_back(make_pair(-1.0, 0.8));
    s7.widthHeight.push_back(make_pair(-1.6, 0.1));
    s7.widthHeight.push_back(make_pair(-2.0, 0.0));

    s8.position = 10;
    s8.widthHeight.push_back(make_pair(1.0, 0.7));
    s8.widthHeight.push_back(make_pair(0.0, 0.9));
    s8.widthHeight.push_back(make_pair(0.0, 0.6));
    s8.widthHeight.push_back(make_pair(0.0, 0.1));
    s8.widthHeight.push_back(make_pair(0.0, 0.0));

    returnBoat.sections.push_back(s1);
    returnBoat.sections.push_back(s2);
    returnBoat.sections.push_back(s3);
    returnBoat.sections.push_back(s4);
    returnBoat.sections.push_back(s5);
    returnBoat.sections.push_back(s6);
    returnBoat.sections.push_back(s7);
    returnBoat.sections.push_back(s8);

    Mast m1, m2;

    m1.position = 2;
    m1.height = 10;

    m2.position = 7.5;
    m2.height = 8;

    Sail sail1, sail2;

    sail1.type = SAIL_MAIN;
    sail1.footHeight = 1.2;
    sail1.headHeight = 9;
    sail1.footLength = 6;
    sail1.headLength = 6;
    sail1.footPosition = 2;
    sail1.headPosition = 2;
    sail1.footAngle = 0;
    sail1.headAngle = 40;

    m1.sails.push_back(sail1);


    sail2.type = SAIL_MAIN;
    sail2.footHeight = 1.2;
    sail2.headHeight = 7;
    sail2.footLength = 4;
    sail2.headLength = 4;
    sail2.footPosition = 0;
    sail2.headPosition = 1;
    sail2.footAngle = 0;
    sail2.headAngle = 40;

    m2.sails.push_back(sail2);


    returnBoat.masts.push_back(m1);
    returnBoat.masts.push_back(m2);



    return returnBoat;
}
