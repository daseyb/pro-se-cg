struct Vertex {
	vec3 pos;
	float u;
	vec3 norm;
	float v;
};

struct Primitive {
	Vertex a;
  Vertex b;
  Vertex c;
};


struct Ray {
  vec3 pos;
  vec3 dir;
};

struct Material {
  vec3 diffuseColor;
  vec3 specularColor;
  float roughness;
  float refractiveness;
  float eta;
};

struct HitInfo {
  vec3 pos;
  vec3 norm;
  float t;
  uint matId;
  Material material;
};

struct SphereLight {
  vec3 center;
  float radius;
  vec3 color;
};


bool intersectPrimitive(in Ray ray, in Primitive tri, out HitInfo hit) {
    const float INFINITY = 1e10;
    vec3 u, v, n; // triangle vectors
    vec3 w0, w;  // ray vectors
    float r, a, b; // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
    u = tri.b.pos - tri.a.pos;
    v = tri.c.pos - tri.a.pos;
    n = cross(u, v);

    w0 = ray.pos - tri.a.pos;
    a = -dot(n, w0);
    b = dot(n, ray.dir);
    if (abs(b) < 1e-5)
    {
        // ray is parallel to triangle plane, and thus can never intersect.
        return false;
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0)
        return false; // ray goes away from triangle.

    vec3 I = ray.pos + r * ray.dir;
    float uu, uv, vv, wu, wv, D;
    uu = dot(u, u);
    uv = dot(u, v);
    vv = dot(v, v);
    w = I - tri.a.pos;
    wu = dot(w, u);
    wv = dot(w, v);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)
        return false;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)
        return false;

    if(r <= 1e-5) return false;
    
    hit.norm = tri.a.norm * (1.0 - s - t) + tri.b.norm * s + tri.c.norm * t;
    hit.pos = ray.pos + r * ray.dir;
    hit.t = r;
    hit.matId = 0;
    
    return true;
}
