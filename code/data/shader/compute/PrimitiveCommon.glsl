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
