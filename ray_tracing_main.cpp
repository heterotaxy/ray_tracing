#include "./src/rtweekend.h"

#include "./src/color.h"
#include "./src/hittable_list.h"
#include "./src/sphere.h"
#include "./src/camera.h"
#include "./src/material.h"
#include "./src/moving_sphere.h"
#include "./src/aarect.h"
#include "./src/box.h"
#include "./src/constant_medium.h"
#include "./src/bvh.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

color ray_color(const ray& r,const color& background , const hittable& world, int depth){
    hit_recode rec;
    if(depth <= 0)
        return color(0, 0, 0);
    
    if(!world.hit(r, 0.001, infinity, rec))
        return background;
    
    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    
    if(!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;
    
    return  emitted + attenuation * ray_color(scattered, background, world, depth-1);
}

int symbolstonumber(std::string symbol){
    if("sphe"==symbol){
        return 0;
    }else if("lambert"==symbol){
        return 1;
    }else if("mirr"==symbol){
        return 2;
    }else if("trpa"==symbol){
        return 3;
    }else if("tmap"==symbol){
        return 4;
    }else if("plig"==symbol){
        return 5;
    }else if("back"==symbol){
        return 6;
    }else if("elig"==symbol){
        return 7;
    }else if("eyep"==symbol){
        return 8;
    }else if("refp"==symbol){
        return 9;
    }else if("vang"==symbol){
        return 10;
    }else if("size"==symbol){
        return 11;
    }else if("rend"==symbol){
        return 12;
    }else if("quit"==symbol){
        return 13;
    }else if("check"==symbol){
        return 14;
    }else if("plane"==symbol){
        return 15;
    }else if("box"==symbol){
        return 16;
    }
    
    return -1;
}

//split string sentence by character
std::vector<std::string> StringSplit(const std::string &str, char sep){
    std::vector<std::string> split_str;
    std::stringstream ss(str);
    std::string buffer_split;
    while(std::getline(ss, buffer_split, sep)){
        split_str.push_back(buffer_split);
    }
    return split_str;
}

shared_ptr<material> make_material_surface(const int material_id, const std::vector<double> color_object){
    shared_ptr<material> surface;
    switch(material_id){
        case 0:
            surface = make_shared<lambertian>(color(color_object[0], color_object[1], color_object[2]));
            break;
        case 1:
        {
            surface = make_shared<metal>(color(color_object[0], color_object[1], color_object[2]), color_object[3]);
            break;}
        case 2:
        {
            surface = make_shared<dielectric>(color_object[0], color_object[1]);
            break;}
        case 3:
        {
            surface = make_shared<diffuse_light>(color(color_object[0], color_object[1], color_object[2]));
            break;}
        default:
            std::exit(1);
    }
    return surface;
}

shared_ptr<texture> make_checker_surface(const std::vector<double> color_object, const int object_id, const std::vector<double> object_param, const double ax){
    shared_ptr<texture> surface;
    switch(object_id){
        case 10:{//sphere
            surface = make_shared<checker_texture>(color(color_object[0], color_object[1], color_object[2]), color(color_object[3], color_object[4], color_object[5]));
        break;}
        case 11:{//plane
            const int axis = static_cast<int>(ax);
            surface = make_shared<checker_texture_plane>(
                                                   color(color_object[0], color_object[1], color_object[2]),
                                                   color(color_object[3], color_object[4], color_object[5]),
                                                   (object_param[1] - object_param[0])/color_object[6],//x
                                                   (object_param[3] - object_param[2])/color_object[6],//y
                                                   axis,
                                                   object_param[0],
                                                   object_param[2]
                                                   );
            break;}
    
    }
    return surface;
}
    
shared_ptr<hittable> makeobject(const std::vector<double> object_param, const std::string textureImage ,const int object_id, const int material_id){
    shared_ptr<hittable> object;
    switch(object_id){
        case 0:
        {
            object = make_shared<sphere>(point3(object_param[0], object_param[1], object_param[2]) ,object_param[3] , make_shared<lambertian>(make_shared<image_texture>(textureImage.c_str())));
            break;}
        case 2:
        {
            object = make_shared<box>(point3(object_param[0], object_param[1], object_param[2]) ,point3(object_param[3], object_param[4], object_param[5]) , make_shared<lambertian>(make_shared<image_texture>(textureImage.c_str())));
            break;}
        case 1:
        {
            switch(static_cast<int>(object_param[5])){
                case 0:
                {
                    object = make_shared<xy_rect>(object_param[0], object_param[1], object_param[2], object_param[3], object_param[4],
                                                  make_shared<lambertian>(make_shared<image_texture>(textureImage.c_str())));
                    break;}
                case 1:
                {
                    object = make_shared<yz_rect>(object_param[0], object_param[1], object_param[2], object_param[3], object_param[4],
                                                  make_shared<lambertian>(make_shared<image_texture>(textureImage.c_str())));
                    break;}
                case 2:
                {
                    object = make_shared<xz_rect>(object_param[0], object_param[1], object_param[2], object_param[3], object_param[4],
                                                  make_shared<lambertian>(make_shared<image_texture>(textureImage.c_str())));
                    break;}
                default:
                    exit(1);
            }
            break;}
    }
    
    return object;
}

shared_ptr<hittable> makeobject(const std::vector<double> object_param, const std::vector<double> color_object ,const int object_id, const int material_id){
    shared_ptr<hittable> object;
    
    switch(object_id){
        case 0://sphere
        {
            object = make_shared<sphere>(point3(object_param[0], object_param[1], object_param[2]) ,object_param[3] ,
                                         make_material_surface(material_id, color_object));
            break;}
        case 10://sphere
        {
            object = make_shared<sphere>(point3(object_param[0], object_param[1], object_param[2]) ,object_param[3] ,
                                         make_shared<lambertian>(make_checker_surface(color_object, object_id, object_param, -1)));
            break;}
        case 1://plane
        {
            switch(static_cast<int>(object_param[5])){
                case 0:
                {
                    object = make_shared<xy_rect>(object_param[0], object_param[1], object_param[2], object_param[3], object_param[4],
                                                  make_material_surface(material_id, color_object));
                    break;}
                case 1:
                {
                    object = make_shared<yz_rect>(object_param[0], object_param[1], object_param[2], object_param[3], object_param[4],
                                                  make_material_surface(material_id, color_object));
                    break;}
                case 2:
                {
                    object = make_shared<xz_rect>(object_param[0], object_param[1], object_param[2], object_param[3], object_param[4],
                                                  make_material_surface(material_id, color_object));
                    break;}
                default:
                    exit(1);
            }
            break;}
        case 11://plane
        {
            switch(static_cast<int>(object_param[5])){
                case 0:
                {
                    object = make_shared<xy_rect>(object_param[0], object_param[1], object_param[2], object_param[3], object_param[4],
                                                  make_shared<lambertian>(make_checker_surface(color_object, object_id, object_param, 2)));
                    break;}
                case 1:
                {
                    object = make_shared<yz_rect>(object_param[0], object_param[1], object_param[2], object_param[3], object_param[4],
                                                  make_shared<lambertian>(make_checker_surface(color_object, object_id, object_param, 0)));
                    break;}
                case 2:
                {
                    object = make_shared<xz_rect>(object_param[0], object_param[1], object_param[2], object_param[3], object_param[4],
                                                  make_shared<lambertian>(make_checker_surface(color_object, object_id, object_param, 1)));
                    break;}
                default:
                    exit(1);
            }
            break;}
        case 2://box
        {
            object = make_shared<box>(point3(object_param[0], object_param[1], object_param[2]) ,point3(object_param[3], object_param[4], object_param[5]) ,
                                      make_material_surface(material_id, color_object));
            break;}
        case 12://box
        {
            std::exit(1);
            break;}
        default:
        {
            std::cerr << "no exist object" << std::flush;
            std::exit(1);}
    }
    
    return object;
}

int  main(int argc, char *argv[]){
    if(argc < 2){
        std::cerr << "Please enter file" << std::flush;
        return 0;
    }
    //file open
    std::ifstream ifs(argv[1]);
    if(ifs.fail()){
        std::cerr << "Don't exist the file" << std::flush;
        return 0;
    }
    std::string wirtedinFile;
    
    //Image parameter
    auto aspect_ratio = 16.0/9.0;
    int image_width = 400;
    int samples_per_pixel = 1000;
    int max_depth = 50;

    //World parameter
    hittable_list world;
    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto hfov = 66.0;
    auto aperture = 0.0;
    color background = color(0.5, 0.5 ,0.5);
    
    lookfrom = point3(12, 2, 3);
    lookat = point3(0, 0, 0);
    
    //input world data
    bool objectInputmode = true;
    std::vector<double> objectparam;
    std::vector<double> colorparam;
    int objectID = -1;//0:spehre 2:box, 1:plane
    //materialID -> 0:lambertain 1:mirr 2:tranparent 3:tmap 4:light
    
    while(getline(ifs, wirtedinFile)){
        std::vector<std::string> line = StringSplit(wirtedinFile, ' ');
        
        for(int i=1; i<line.size() && objectInputmode; i++){
            objectparam.push_back(std::stod(line[i]));
        }
        for(int i=1; i<line.size() && (!objectInputmode && "tmap" != line[0]); i++){
            colorparam.push_back(std::stod(line[i]));
        }
        int judgeObject = symbolstonumber(line[0]);
        
        switch(judgeObject){
            case 0://sphere
            {
                objectID = 0;
                objectInputmode = !objectInputmode;
                break;}
            case 15://plane
            {
                objectID = 1;
                objectInputmode = !objectInputmode;
                break;}
            case 16://box
            {
                objectID = 2;
                objectInputmode = !objectInputmode;
                break;}
            case 1://lambert
            {
                auto object = makeobject(objectparam, colorparam, objectID, 0);
                world.add(object);
                objectID = -1;
                objectInputmode = !objectInputmode;
                objectparam.clear();
                colorparam.clear();
                break;
            }
            case 2://mirr
            {
                auto object = makeobject(objectparam, colorparam, objectID, 1);
                world.add(object);
                objectID = -1;
                objectInputmode = !objectInputmode;
                objectparam.clear();
                colorparam.clear();
                break;}
            case 3://transparent
            {
                auto object = makeobject(objectparam, colorparam, objectID, 2);
                world.add(object);
                objectID = -1;
                objectInputmode = !objectInputmode;
                objectparam.clear();
                colorparam.clear();
                break;}
            case 4://texture map
            {
                auto object = makeobject(objectparam, line[1], objectID, 1);
                world.add(object);
                objectID = -1;
                objectInputmode = !objectInputmode;
                objectparam.clear();
                colorparam.clear();
                break;}
            case 14://checker
            {
                auto object = makeobject(objectparam, colorparam, objectID+10, 0);
                world.add(object);
                objectID = -1;
                objectInputmode = !objectInputmode;
                objectparam.clear();
                colorparam.clear();
                break;}
            
            case 5://light
            {
                auto object = makeobject(objectparam, colorparam, objectID, 3);
                world.add(object);
                objectID = -1;
                objectInputmode = !objectInputmode;
                objectparam.clear();
                colorparam.clear();
                break;}
            case 6://back
            {
                background = color(objectparam[0], objectparam[1], objectparam[2]);
                objectparam.clear();
                colorparam.clear();
                break;}
            case 7://elig
            {
                //i do not define
                break;}
            case 8://viewpoint
            {
                lookfrom = point3(objectparam[0], objectparam[1], objectparam[2]);
                objectparam.clear();
                colorparam.clear();
                break;}
            case 9://view reference point
            {
                lookat = point3(objectparam[0], objectparam[1], objectparam[2]);
                objectparam.clear();
                colorparam.clear();
                break;}
            case 10://vang
            {
                hfov = objectparam[0];
                vfov = objectparam[1];
                aspect_ratio = tan(degrees_to_radians(hfov)/2.0)/tan(degrees_to_radians(vfov)/2.0);
                objectparam.clear();
                colorparam.clear();
                break;}
            case 11://size
            {
                image_width = objectparam[0];
                objectparam.clear();
                colorparam.clear();
                break;}
            case 12://rend
            {
                
                break;}
            case 13://quit
            {
                
                break;}
            default:
            {
                std::cerr << "no exit object" << std::flush;
                std::cerr << line[0] << std::flush;
                std::exit(1);
                break;}
        }
    }
    //Camera
    vec3 vup(0, 1, 0);
    auto dist_to_focus = (lookfrom - lookat).length();
    int image_height = static_cast<int>(image_width/aspect_ratio);
    
    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    //Render
    std::cout << "P3\n" << image_width << ' ' <<image_height << "\n255\n";

    for(int j=image_height-1; j>=0; --j){
        std::cerr << "\r Scanlnes remaining: " << j << ' ' << std::flush;
        for(int i=0; i<image_width; ++i){
            color pixel_color(0, 0, 0);
            for(int s=0; s<samples_per_pixel; ++s){
                auto u = (i + random_double())/(image_width - 1);
                auto v = (j + random_double())/(image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    std::cerr << "\nDone.\n";
}

