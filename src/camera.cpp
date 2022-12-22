#include "camera.h"

#include "debugging.h"

#include <cmath> // fabs()
#include <algorithm> // std::max()

using namespace glm;

namespace graphics101 {

ray3 CameraPerspective::getRay( const vec2& uv ) const {
    
    // Your code goes here.

    // Create a ray3( ray origin point, ray direction ).
    
    // The Camera has `vec3` instance variables e,u,v,w, so you
    // can write an expression that looks very similar to the one
    // in the lectures.
    // CameraPerspective has an instance variable `focal_length`,
    // which is `d` in the lectures.
    
    /* eye() := e | look() := -w | up() := v | right(0) := u */
    vec3 d_perspective = (uv[0] * right()) + (uv[1] * up()) + (d * look());

    return ray3( eye(), d_perspective );
}

ray3 CameraOrthographic::getRay( const vec2& uv ) const {

    // Your code goes here.

    // Create a ray3( ray origin point, ray direction ).
    
    // The Camera has `vec3` instance variables e,u,v,w, so you 
    // can write an expression that looks very similar to the one
    // in the lectures.

    return ray3( e + (uv.x * u) + (uv.y * v), look() );
}

void Camera::imageDimensionsWithLongEdge( int long_edge_pixels, int& width_out, int& height_out ) const {
    assert( long_edge_pixels > 0 );
    
    const real w = fabs( film_right - film_left );
    const real h = fabs( film_bottom - film_top );
    
    if( w > h ) {
        width_out = long_edge_pixels;
        height_out = std::max( 1L, lround( long_edge_pixels * h / w ) );
    } else {
        height_out = long_edge_pixels;
        width_out = std::max( 1L, lround( long_edge_pixels * w / h ) );
    }
}

}
