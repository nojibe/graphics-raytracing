#include "shape.h"

#include "debugging.h"

#include <vector>
#include <cmath> // std::sqrt()

using namespace glm;

namespace graphics101 {

Intersection Plane::rayIntersect(const ray3& ray)const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.

    // Remember that this intersection occurs in object-space.
    // You convert the world-space ray to object-space by multiplying `transformInverse()` times `ray.p` and `ray.d`.
    // When filling out `result`, don't forget to return the resulting position and normal in world-space.
    // For the normal, that means multiplying `transpose(transformInverse())` times the object-space normal.
    
    // F(x,y,z) = z
    // Get p from the ray
    vec3 p = transformInverse()* vec4(ray.p, 1);
    // Get d from the ray
    vec3 d = transformInverse()* vec4(ray.d, 0);
    vec3 n = vec3(0, 0, 1);
    real t = - p[2] / (1.0 * d[2]);
    if (t >= 0){
        result.valid = true;
        result.t = t;
        // Convert object world-space ray to object-space
        result.normal = transpose(transformInverse())* vec4(n , 0);
        result.material = material();
        result.position = ray.p + t * ray.d;
    }
    return result;
}

Intersection Sphere::rayIntersect(const ray3& ray)const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.

    // Remember that this intersection occurs in object-space.
    // You convert the world-space ray to object-space by multiplying `transformInverse()` times `ray.p` and `ray.d`.
    // When filling out `result`, don't forget to return the resulting position and normal in world-space.
    // For the normal, that means multiplying `transpose(transformInverse())` times the object-space normal.
    // Get p from the ray
    vec3 p = transformInverse()* vec4(ray.p, 1);
    // Get d from the ray
    vec3 d = transformInverse()* vec4(ray.d, 0);

    // Formulas for sphere
    // F(x,y,z) = x² + y² + z² - 1
    real a = pow(d[0], 2)+ pow(d[1], 2)+ pow(d[2], 2);
    real b = 2.0 * (p[0]*d[0] + p[1]*d[1] + p[2]*d[2]);
    real c = pow(p[0], 2)+ pow(p[1], 2)+ pow(p[2], 2)- 1.0;
    // b^2 - 4*a*c
    real radical = b * b - 4. * a * c;
    std::vector<Intersection> ts; real t;
    vec3 q, n;
    if (radical >= 0){
        Intersection intersection;
        t = (- b + sqrt(radical))/(2. * a);
        if (t >= 0){
            q = p + t * d;
            n = vec3(2*q[0], 2*q[1], 2*q[2]);

            intersection.valid = true;
            intersection.t = t;
            intersection.position = ray.p + t * ray.d;
            intersection.normal = transpose(transformInverse())* vec4(n, 0);
            intersection.material = material();
            ts.push_back(intersection);
        }

        t = (- b - sqrt(radical))/(2. * a);
        if (t >= 0){
            q = p + t * d;
            n = vec3(2*q[0], 2*q[1], 2*q[2]);

            intersection.valid = true;
            intersection.t = t;
            intersection.position = ray.p + t * ray.d;
            intersection.normal = transpose(transformInverse())* vec4(n, 0);
            intersection.material = material();
            ts.push_back(intersection);
        }
    }

    if (!ts.empty()){
        for (const Intersection& itm : ts){
            if (itm.t < result.t){
                result = itm;
            }
        }
    }
    return result;
}

Intersection Cylinder::rayIntersect(const ray3& ray)const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.

    // Remember that this intersection occurs in object-space.
    // You convert the world-space ray to object-space by multiplying `transformInverse()` times `ray.p` and `ray.d`.
    // When filling out `result`, don't forget to return the resulting position and normal in world-space.
    // For the normal, that means multiplying `transpose(transformInverse())` times the object-space normal.

    /*
    if 0 < z < 1: F(x,y,z) = x² + y² - 1
    if x² + y² < 1: F(x,y,z) = -z
    if x² + y² < 1: F(x,y,z) = z-1
    */
    vec3 p = transformInverse()* vec4(ray.p, 1);
    vec3 d = transformInverse()* vec4(ray.d, 0);

    std::vector< Intersection > ts;
    real t;
    vec3 q, n;
    Intersection intersection;

    // Top & Bottom
    t = -p[2]/(1.0 * d[2]);
    if (t >= 0){
        q = p + t * d;
        if ((pow(q[0], 2)+ pow(q[1], 2))< 1){
            intersection.valid = true;
            intersection.t = t;
            intersection.position = ray.p + t*ray.d;
            intersection.normal = transpose(transformInverse())* vec4(0, 0, -1, 0);
            intersection.material = material();
            if (pow(q[0], 2)+ pow(q[1], 2)< 1){
                ts.push_back(intersection);
            }
        }
    }

    // Top Face
    t = (1. - p[2])/(1. * d[2]);
    if (t >= 0){
        q = p + t * d;
        if ((pow(q[0], 2)+ pow(q[1], 2))< 1){
            intersection.valid = true;
            intersection.t = t;
            intersection.position = ray.p + t*ray.d;
            intersection.normal = transpose(transformInverse())* vec4(0, 0, 1, 0);
            intersection.material = material();
            if (pow(q[0], 2)+ pow(q[1], 2)< 1){
                ts.push_back(intersection);
            }
        }
    }

    // Side
    real a = pow(d[0], 2)+ pow(d[1], 2);
    real b = 2.0 * (p[0]*d[0] + p[1]*d[1]);
    real c = pow(p[0], 2)+ pow(p[1], 2)- 1.0;


    real radical = b * b - 4. * a * c;

    if (radical >= 0){
        t = (- b + sqrt(radical))/(2. * a);
        if (t >= 0){
            q = p + t * d;
            n = vec3(2*q[0], 2*q[1], 0);

            intersection.valid = true;
            intersection.t = t;
            intersection.position = ray.p + t * ray.d;
            intersection.normal = transpose(transformInverse())* vec4(n, 0);
            intersection.material = material();
            if (q[2] > 0 && q[2] < 1){
                ts.push_back(intersection);
            }
        }

        t = (- b - sqrt(radical))/(2. * a);
        if (t >= 0){
            q = p + t * d;
            n = vec3(2.*q[0], 2.*q[1], 0);

            intersection.valid = true;
            intersection.t = t;
            intersection.position = ray.p + t * ray.d;
            intersection.normal = transpose(transformInverse())* vec4(n, 0);
            intersection.material = material();
            if (q[2] > 0 && q[2] < 1){
                ts.push_back(intersection);
            }
        }
    }

    if (!ts.empty()){
        for (const Intersection& itm : ts){
            if (itm.t < result.t){
                result = itm;
            }
        }
    }
    return result;
}

Intersection Cone::rayIntersect(const ray3& ray)const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.

    // Remember that this intersection occurs in object-space.
    // You convert the world-space ray to object-space by multiplying `transformInverse()` times `ray.p` and `ray.d`.
    // When filling out `result`, don't forget to return the resulting position and normal in world-space.
    // For the normal, that means multiplying `transpose(transformInverse())` times the object-space normal.

    /*
    if 0 < z ≤ 1: F(x,y,z) = x² + y² - (1 - z)²
    if x² + y² < 1: F(x,y,z) = -z
    */

    vec3 p = transformInverse()* vec4(ray.p, 1);
    vec3 d = transformInverse()* vec4(ray.d, 0);

    std::vector< Intersection > ts;
    real t;
    vec3 q, n;
    Intersection intersection;

    // Bottom face
    t = -p[2]/(1. * d[2]);
    if (t >= 0){
        q = p + t * d;
        if ((pow(q[0], 2)+ pow(q[1], 2))< 1){
            intersection.valid = true;
            intersection.t = t;
            intersection.position = ray.p + t*ray.d;
            intersection.normal = transpose(transformInverse())* vec4(0, 0, -1, 0);
            intersection.material = material();
            if (pow(q[0], 2)+ pow(q[1], 2)< 1){
                ts.push_back(intersection);
            }
        }
    }
    real a = pow(d[0], 2)+ pow(d[1], 2)- pow(d[2], 2);
    real b = 2.0 * (p[0]*d[0] + p[1]*d[1] + (1 - p[2])*d[2]);
    real c = pow(p[0], 2)+ pow(p[1], 2)- pow((1.0 - p[2]), 2);

    real radical = b * b - 4. * a * c;

    if (radical >= 0.){
        t = (- b + sqrt(radical))/(2. * a);
        if (t >= 0){
            q = p + t * d;
            n = vec3(2*q[0], 2*q[1], 2*(1 - q[2]));

            intersection.valid = true;
            intersection.t = t;
            intersection.position = ray.p + t * ray.d;
            intersection.normal = transpose(transformInverse())* vec4(n, 0);
            intersection.material = material();
            if (q[2] > 0 && q[2] <= 1){
                ts.push_back(intersection);
            }
        }

        t = (- b - sqrt(radical))/(2. * a);
        if (t >= 0){
            q = p + t * d;
            n = vec3(2*q[0], 2*q[1], 2*(1 - q[2]));

            intersection.valid = true;
            intersection.t = t;
            intersection.position = ray.p + t * ray.d;
            intersection.normal = transpose(transformInverse())* vec4(n, 0);
            intersection.material = material();
            if (q[2] > 0 && q[2] <= 1){
                ts.push_back(intersection);
            }
        }
    }
    if (!ts.empty()){
        for (const Intersection& itm : ts){
            if (itm.t < result.t){
                result = itm;
            }
        }
    }
    return result;
}

Intersection Cube::rayIntersect(const ray3& ray)const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.

    // Remember that this intersection occurs in object-space.
    // You convert the world-space ray to object-space by multiplying `transformInverse()` times `ray.p` and `ray.d`.
    // When filling out `result`, don't forget to return the resulting position and normal in world-space.
    // For the normal, that means multiplying `transposetransformInverse())` times the object-space normal.

    vec3 p = transformInverse()* vec4(ray.p, 1.);
    vec3 d = transformInverse()* vec4(ray.d, 0.);

    std::vector< Intersection > ts;
    real t;
    vec3 q, n;
    Intersection intersection;

    /*    
    if -1 ≤ y,z ≤ 1: F(x,y,z) = x-1
    if -1 ≤ y,z ≤ 1: F(x,y,z) = -(x+1)
    if -1 ≤ x,z ≤ 1: F(x,y,z) = y-1
    if -1 ≤ x,z ≤ 1: F(x,y,z) = -(y+1)
    if -1 ≤ x,y ≤ 1: F(x,y,z) = z-1
    if -1 ≤ x,y ≤ 1: F(x,y,z) = -(z+1)
    */

    // F1
    t = (1. - p[0])/ (1. * d[0]);
    if (t >= 0){
        q = p + t * d;
        n = vec3(1, 0, 0);

        intersection.valid = true;
        intersection.t = t;
        intersection.position = ray.p + t * ray.d;
        intersection.normal = transpose(transformInverse())* vec4(n, 0);
        intersection.material = material();
        if (q[1] >= -1 &&  q[2]>= -1 && q[1] <= 1 && q[2] <= 1){
            ts.push_back(intersection);
        }
    }


    // F2
    t = - (1. + p[0])/ (1. * d[0]);
    if (t >= 0){
        q = p + t * d;
        n = vec3(-1, 0, 0);

        intersection.valid = true;
        intersection.t = t;
        intersection.position = ray.p + t * ray.d;
        intersection.normal = transpose(transformInverse())* vec4(n, 0);
        intersection.material = material();
        if (q[1] >= -1 && q[2] >= -1 && q[1] <= 1 && q[2] <= 1){
            ts.push_back(intersection);
        }
    }

    // F3
    t = (1 - p[1])/ (1. * d[1]);
    if (t >= 0){
        q = p + t * d;
        n = vec3(0, 1, 0);

        intersection.valid = true;
        intersection.t = t;
        intersection.position = ray.p + t * ray.d;
        intersection.normal = transpose(transformInverse())* vec4(n, 0);
        intersection.material = material();
        if (q[0] >= -1 && q[2] >= -1 && q[0] <= 1 && q[2] <= 1){
            ts.push_back(intersection);
        }
    }

    // F4
    t = - (1 + p[1])/ (1. * d[1]);
    if (t >= 0.){
        q = p + t * d;
        n = vec3(0, -1, 0);

        intersection.valid = true;
        intersection.t = t;
        intersection.position = ray.p + t * ray.d;
        intersection.normal = transpose(transformInverse())* vec4(n, 0);
        intersection.material = material();
        if (q[0] >= -1 && q[2] >= -1 && q[0] <= 1 && q[2] <= 1){
            ts.push_back(intersection);
        }
    }

    // F5
    t = (1 - p[2])/ (1. * d[2]);
    if (t >= 0){
        q = p + t * d;
        n = vec3(0, 0, 1);

        intersection.valid = true;
        intersection.t = t;
        intersection.position = ray.p + t * ray.d;
        intersection.normal = transpose(transformInverse())* vec4(n, 0);
        intersection.material = material();
        if (q[0] >= -1 && q[1] >= -1 && q[0] <= 1 && q[1] <= 1){
            ts.push_back(intersection);
        }
    }

    // F6
    t = - (1 + p[2])/ (1. * d[2]);
    if (t >= 0){
        q = p + t * d;
        n = vec3(0, 0, -1);

        intersection.valid = true;
        intersection.t = t;
        intersection.position = ray.p + t * ray.d;
        intersection.normal = transpose(transformInverse())* vec4(n, 0);
        intersection.material = material();
        if (q[0] >= -1 && q[1] >= -1 && q[0] <= 1 && q[1] <= 1){
            ts.push_back(intersection);
        }
    }

    if (!ts.empty()){
        for (const Intersection& itm : ts){
            if (itm.t < result.t){
                result = itm;
            }
        }
    }
    return result;
}

Intersection Mesh::rayIntersect(const ray3& ray)const {
    // The Intersection constructor sets `.valid = false`
    Intersection result;
    
    // Your code goes here.

    // Remember that this intersection occurs in object-space.
    // You convert the world-space ray to object-space by multiplying `transformInverse()` times `ray.p` and `ray.d`.
    // When filling out `result`, don't forget to return the resulting position and normal in world-space.
    // For the normal, that means multiplying `transpose(transformInverse())` times the object-space normal.
    
    // Normal should be perpendicular to 

    // normal = cross(triangleAB, triangleBC)
    // // if normal perpendicular 
    // // Get p from the ray
    // vec3 p = transformInverse()* vec4(ray.p, 1);
    // // Get d from the ray
    // vec3 d = transformInverse()* vec4(ray.d, 0);
    // vec3 n = vec3(0, 0, 1);
    // real t = - p[2] / (1.0 * d[2]);
    // if (t >= 0){
    //     result.valid = true;
    //     result.t = t;
    //     // Convert object world-space ray to object-space
    //     result.normal = transpose(transformInverse())* vec4(n , 0);
    //     result.material = material();
    //     result.position = ray.p + t * ray.d;
    // }
    // return result;

    return result;
}

}
