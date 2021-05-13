#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.h"
#include "perlin.h"
#include "rtw_stb_image.h"

#include <iostream>

class texture{
public:
    virtual color value(double u, double v, const point3& p)const=0;
};

class solid_color: public texture{
public:
    solid_color() {}
    solid_color(color c): color_value(c){}
    
    solid_color(double red, double green, double blue)
    :solid_color(color(red, green, blue)){}
    
    virtual color value(double u, double v, const vec3& p)const override{
        return color_value;
    }
private:
    color color_value;
};

class checker_texture : public texture{
public:
    checker_texture() {}
    checker_texture(shared_ptr<texture> _odd, shared_ptr<texture> _even)
    : odd(_odd), even(_even){}
    checker_texture(color c1, color c2)
    : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}
    
    virtual color value(double u, double v, const point3& p)const override{
        auto sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
        if(sines<0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }
public:
    shared_ptr<texture> odd;
    shared_ptr<texture> even;
};

class checker_texture_plane : public texture{
public:
    checker_texture_plane() {}
    checker_texture_plane(shared_ptr<texture> _odd, shared_ptr<texture> _even, const double numx, const double numy, const int axis, const double x, const double y)
    : odd(_odd), even(_even), numx(numx), numy(numy), axis(axis), x(x), y(y){}
    checker_texture_plane(color c1, color c2, const double numx, const double numy, const int axis, const double x, const double y)
    : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)), numx(numx), numy(numy), axis(axis), x(x), y(y){}
    
    virtual color value(double u, double v, const point3& p)const override{
        switch(axis){
            case 0:{
                if((static_cast<int>((p.y()-x)/numx) + static_cast<int>((p.z()-y)/numy))%2 == 0)
                    return odd->value(u, v, p);
                else
                    return even->value(u, v, p);
                break;}
            case 1:{
                if((static_cast<int>((p.x()-x)/numx) + static_cast<int>((p.z()-y)/numy))%2 == 0)
                    return odd->value(u, v, p);
                else
                    return even->value(u, v, p);
                break;}
            case 2:{
                if((static_cast<int>((p.x()-x)/numx) +static_cast<int>((p.y()-y)/numy))%2 == 0)
                    return odd->value(u, v, p);
                else
                    return even->value(u, v, p);
                break;}
            default:
                return color(0, 1, 1);
        }
    }
public:
    shared_ptr<texture> odd;
    shared_ptr<texture> even;
    double numx, numy;
    int axis;//0:x,1:y,2:z
    double x, y;
};

class noise_texture: public texture{
public:
    noise_texture() {}
    noise_texture(double sc) :scale(sc){}
    virtual color value(double u, double v, const point3& p) const override{
        return color(1,1,1)*0.5*(1 + sin(scale*p.z() + 10*noise.turb(p)));
    }
    
public:
    perlin noise;
    double scale;
};

class image_texture: public texture{
public:
    const static int bytes_per_pixel = 3;
    image_texture()
        :data(nullptr), width(0), height(0), bytes_per_scanline(0){}
    image_texture(const char* filename){
        auto componets_per_pixel = bytes_per_pixel;
        data = stbi_load(filename, &width, &height, &componets_per_pixel, componets_per_pixel);
        
        if(!data){
            std::cerr << "ERROR: Could not load texture image file"<<filename <<"\n";
            width=height=0;
        }
        bytes_per_scanline = bytes_per_pixel*width;
    }
    ~image_texture(){
        delete data;
    }
    virtual color value(double u, double v, const vec3&p)const override{
        if(data == nullptr){
            return color(0,1,1);
        }
        
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);
        
        auto i = static_cast<int>(u*width);
        auto j = static_cast<int>(v*height);
        
        if(i>=width) i = width-1;
        if(j>=height) j = height-1;
        
        const auto color_scale = 1.0/255.0;
        auto pixel = data + j*bytes_per_scanline + i*bytes_per_pixel;
        return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }
    
private:
    unsigned char *data;
    int width, height;
    int bytes_per_scanline;
};

#endif
