uint wang_hash(uint seed) {
  seed = (seed ^ 61) ^ (seed >> 16);
  seed *= 9;
  seed = seed ^ (seed >> 4);
  seed *= 0x27d4eb2d;
  seed = seed ^ (seed >> 15);
  return seed;
}

float modI(float a,float b) {
    float m=a-floor((a+0.5)/b)*b;
    return floor(m+0.5);
}

float wang_float(uint hash) {
  return hash / float(0x7FFFFFFF) / 2.0;
}

uint uniformUInt(uint min, uint max, inout uint random) {
  random = wang_hash(random);
  return (random % (max-min)) + min;
}

float uniformFloat(float min, float max, inout uint random) {
  random = wang_hash(random);
  return (max - min) * wang_float(random) + min;
}

vec2 uniformVec2(vec2 min, vec2 max, inout uint random) {
  float x = uniformFloat(min.x, max.x, random);
  float y = uniformFloat(min.y, max.y, random);
  return vec2(x, y);
}

vec3 uniformVec3(vec3 min, vec3 max, inout uint random) {
  float x = uniformFloat(min.x, max.x, random);
  float y = uniformFloat(min.y, max.y, random);
  float z = uniformFloat(min.z, max.z, random);
  return vec3(x, y, z);
}

vec3 directionUniformSphere(inout uint random) {
  float u1 = uniformFloat(0, 1, random);
  float phi = uniformFloat(0, 2 * PI, random);
  float f = sqrt(1 - u1 * u1);

  float x = f * cos(phi);
  float y = f * sin(phi);
  float z = u1;

  vec3 dir = vec3(x, y, z);

  return dir;
}

vec3 directionCosTheta(vec3 normal, inout uint random) {
  float u1 = uniformFloat(0, 1, random);
  float phi = uniformFloat(0, 2 * PI, random);

  float r = sqrt(u1);

  float x = r * cos(phi);
  float y = r * sin(phi);
  float z = sqrt(1.0 - u1);

  vec3 xDir = abs(normal.x) < abs(normal.y) ? vec3(1, 0, 0) : vec3(0, 1, 0);
  vec3 yDir = normalize(cross(normal, xDir));
  xDir = cross(yDir, normal);
  return xDir * x + yDir * y + z * normal;
}

vec3 directionUniform(vec3 normal, inout uint random) {
  float u1 = uniformFloat(0, 1, random);
  float phi = uniformFloat(0, 2 * PI, random);
  float f = sqrt(1 - u1 * u1);

  float x = f * cos(phi);
  float y = f * sin(phi);
  float z = u1;

  vec3 dir = vec3(x, y, z);
  dir *= sign(dot(dir, normal));

  return dir;
}

vec3 sampleSphereSolidAngle(vec3 x, vec3 sPos, float sRad, out float p, inout uint random) {
      vec3 sw = sPos - x;
      vec3 su = normalize(cross(abs(sw.x) > .1 ? vec3(0, 1, 0) :  vec3(1, 0, 0), sw));
      vec3 sv = cross(sw, su);

      float cos_a_max = sqrt(1.0 - sRad * sRad / dot(sw,sw));
      float eps1 = uniformFloat(0, 1, random);
      float eps2 = uniformFloat(0, 1, random);
      
      float cos_a = 1 - eps1 + eps1 * cos_a_max;
      float sin_a = sqrt(1 - cos_a * cos_a);
      float phi = 2 * PI * eps2;
      vec3 l = su * cos(phi) * sin_a + sv * sin(phi) * sin_a + sw * cos_a;
      l = normalize(l);
      
      p = 2 * (1 - cos_a_max);
      
      return l;
}

vec2 concentricSampleDisk(inout uint random) {
  float r, theta;
  float sx = uniformFloat(-1, 1, random);
  float sy = uniformFloat(-1, 1, random);

  if (sx == 0.0 && sy == 0.0) {
    return vec2(0);
  }

  if (sx >= -sy) {
    if (sx > sy) {
      r = sx;
      if (sy > 0.0f)
        theta = sy / r;
      else
        theta = 8.0 + sy / r;
    } else {
      r = sy;
      theta = 2.0 - sx / r;
    }
  } else {
    if (sx <= sy) {
      r = -sx;
      theta = 4.0 - sy / r;
    } else {
      r = -sy;
      theta = 6.0 + sx / r;
    }
  }

  theta *= PI / 4.0;
  return vec2(cos(theta), sin(theta)) * r;
}

vec3 samplePrimitive(Primitive p, inout uint random) {
  float a1 = uniformFloat(0, 1, random);
  float a2 = uniformFloat(0, 1, random);

  if(a1 + a2 > 1) {
    a1 = 1.0 - a1;
    a2 = 1.0 - a2;
  }

  return p.a.pos + (p.b.pos - p.a.pos) * a1 + (p.c.pos - p.a.pos) * a2;
}
