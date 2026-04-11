#pragma once

#include <stdio.h>;
#include <raster.h>;
#include <fstream>;
#include <sstream>;
#include <string>;


object3d* parse_obj_file(std::string filename, object3d * o) {
    std::ifstream in(filename); // ifstream opens file in input mode only vs "ofstream" of "fstream"

    if (!in)
    {
        std::cerr << "Cannot open " << filename << std::endl;
        exit(1);

    }
    else {

        std::string line;
        std::vector<point3d> vectors;

        while (std::getline(in, line)) {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> items;

            while (std::getline(ss, segment, ' ')) {
                // Process each space-separated value
                //printf("%s ", segment.c_str());
                items.push_back(segment);
            }
            if (line[0] == 'v') {  // adding a vector/ point
                point3d p = { std::stof(items[1]), std::stof(items[2]), std::stof(items[3]) };
                vectors.push_back(p);
            }
            else if (line[0] == 'f') {  // adding polyps by vector index def
                poly3d p = { vectors[std::stoi(items[1]) -1], vectors[std::stoi(items[2]) -1], vectors[std::stoi(items[3]) -1] };
      
                o->polys3d.push_back(p);
            }
            
        }

        return o;
    }
    return nullptr;
}
