//
//  ObjectLoader.hpp
//  RS
//
//  Created by Tom Cruise on 2017/3/21.
//  Copyright © 2017年 Tom Cruise. All rights reserved.
//

#ifndef ObjectLoader_hpp
#define ObjectLoader_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
class ObjLoader {
public:
    ObjLoader();
    bool LoadObjFromFileVertexAndNormal(const char*,std::vector<float>*,std::vector<float>*);
};
#endif /* ObjectLoader_hpp */
