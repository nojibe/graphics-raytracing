#include "scene.h"
#include "debugging.h"

// Include all of glm here.
#include "glm/glm.hpp"

using namespace glm;

namespace graphics101 {

void Scene::render(Image& into_image) {
    // Your code goes here.

    // Iterate over the pixels of the image. For each pixel:
    // 1. Use camera->getPixelUV() and camera->getRay() to create a ray3.
    // 2. Call rayColor() to get a vec3 color for that ray.
    // 3. Use into_image.pixel() to set the pixel color.
    //    Don't forget that ColorRGBA8 values range from [0,255],
    //    whereas rayColor() returns a vec3 with values ranging from [0,1].
    
    // Define min and max values for clamping to range [0,255]
    vec3 vMin = vec3(0, 0, 0);
    vec3 vMax = vec3(255, 255, 255);
    // Pixel by pixel
    for(int w = 0; w < into_image.width(); w++){
        for(int h = 0; h < into_image.height(); h++){
            // Get 2D UV coordinates to create a 3D ray
            vec2 uv = camera->getPixelUV(vec2(w, h), into_image.width(), into_image.height());
            // Get color for the 3D ray arg1:ray arg2:max_recursion
            // Increase max_recursion in order to add to reflection number.
            vec3 color = rayColor(camera->getRay(uv), 1);
            // Clamp the color values
            color = clamp(round(color * vMax), vMin, vMax);
            into_image.pixel(w, h) = ColorRGBA8(color.r, color.g, color.b);
        }
    }

}

Intersection Scene::closestIntersection(const ray3& ray) const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.
    
    // Iterate over all the shapes, calling rayIntersect() on each.
    // In C++, a nice iterator for loop can be written as:
    // for(const ShapePtr& shape: shapes) { ... shape->rayIntersect(...) ... }
    
        // Iterate through all shapes
        for (const auto& shape : shapes){
        // Get intersection info from the current ray with all shapes
            Intersection intersection = shape->rayIntersect(ray);
        if ((intersection.t < result.t) && intersection.valid){
            result = intersection;
        }
    }
    return result;
}
vec3 Scene::rayColor(const ray3& ray, int max_recursion) const {
    // Your code goes here.

    // In this ray casting assignment, you will simply call closestIntersection(),
    // and, if there is one, return the .material.color_diffuse;
    // Otherwise, return black (0,0,0).

    assert(max_recursion >= 0);

    const real eps = 1e-7;

    vec3 color(0,0,0);
    
    const Intersection intersection = closestIntersection(ray);
    // Return black if no intersection found
    if (!intersection.valid) return color;
    // Recursions based on max_recursion handling
    // Reflections:
   if ( intersection.material.reflective && max_recursion > 0 ) {
       const vec3 reflect_direction = glm::reflect( ray.d, glm::normalize( intersection.normal ) );
       const ray3 reflect_ray = ray3( intersection.position + eps * reflect_direction, reflect_direction );
       color += intersection.material.color_reflect * rayColor( reflect_ray, max_recursion - 1 );
   }

/* Modifications for RayTracing */

    /*
    See the expression in assignment Readme.
    In this expression, KA, KD, KS, n, KR, and KT refer to 
    
    =Material Objects
    KA  .color_ambient, 
    KD  .color_diffuse, 
    KS  .color_specular, 
    n   .shininess, 
    KR  .color_reflect,
    KT  .color_refract. 
    
    =Light Objects=
    IAL .color_ambient,
    IL  .color
    IR  .position

    =Vectors=



    The summation is over all the Light structures in the Scene; 
    IAL and IL refer to Light.color_ambient and .color. 
    You can multiply colors stored as vec3's, and it will do the 
    right thing (multiply each channel or wavelength of light). 
    The other terms will be defined below.

    1) Ambient Lightning
    2) Specular Lightning
    3) Shadows
    4) Reflections
    5) Refraction

    K_R I_R + K_T I_T + \sum_L \big( K_A I_{AL} + \big[ K_D I_L ( N \cdot L ) + K_S I_L ( V \cdot R )^n  \big] S_L \big)

    in other words

    reflect * IR + retract * IT + sum{ambient * IAL + (diffuse*IL[dotNL] + specular * IL[dotVR]^n) * SL}
    */


    // Recursively getting through every light
    

    /*
        Summary of Light Model
        reflection + refraction + SUM(ambient + (diffuse + POW(specular, shine)) * shadow)


    */


    // KA IAL
    vec3 lambient;
    // KS IL ( V · R )n. V
    vec3 lspecular;
    // KD IL ( N · L ). N 
    vec3 ldiffuse;
    
    for (const Light& l : lights) {

        // Add ambient lighting to the color at intersection
        // ambient lighting = KA * IAL
        lambient = intersection.material.color_ambient * l.color_ambient;
        color = color + lambient;

        const vec3 L = normalize(l.position - intersection.position);
        const vec3 N = normalize(intersection.normal);
        const vec3 V = normalize(ray.p - intersection.position);
        const vec3 R = normalize(reflect((-1.) * L, N));
        const real n = intersection.material.shininess;
        const real dotNL = dot(N, L);
        // Shadow handling
        real shadow = 1.;
        // epsilon offset
        const Intersection& block = closestIntersection(ray3(intersection.position + eps * L, L));
        // if there is an object between surface position and the light
        if (block.valid && (block.t < glm::distance(intersection.position, l.position))) {
            shadow = 0.;
        }
        // else{
        //     shadow = 1.;
        // }
        // Light diffusion handling
        if (dotNL >= 0 && shadow != 0) {
            ldiffuse = l.color * dotNL;
            color = color + intersection.material.color_diffuse * ldiffuse;
            // Note that if the dot product is negative, then the light is reflected 
            // away from the viewer and you should not add any specular lighting.
            
            if(dotNL != 0){
                lspecular = l.color * pow(max(0., dot(V, R)), n);
                color = color + intersection.material.color_specular * lspecular;
            }
            
            // if(dotNL > 0){
            //     lspecular = l.color * pow(dot(V, R), n);
            //     color = color + intersection.material.color_specular * lspecular;
            // }
            // else{
                // reflect.
            // }
        }
    }

    return color;
}

}
