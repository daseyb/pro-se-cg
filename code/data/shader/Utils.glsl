
#define M_PI 3.1415926535897932384626433832795

vec3 unpackNormal(vec4 normalMotion) {
    vec2 ang = normalMotion.xy;
    vec2 scth = vec2(sin(ang.x * M_PI), cos(ang.x * M_PI));
    vec2 scphi = vec2(sqrt(1.0 - ang.y*ang.y), ang.y);
    return vec3(scth.y*scphi.x, scth.x*scphi.x, scphi.y);
}