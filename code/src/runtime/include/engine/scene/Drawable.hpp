#pragma once
#include <engine/scene/Entity.hpp>
#include <ACGL/Math/Math.hh>
#include <engine/graphics/Geometry.hpp>
#include <engine/graphics/Material.hpp>

struct Drawable : Component<Drawable> {
  explicit Drawable(Geometry geom, Material mat, int tf = 0, uint32_t renderPassIndex = 0) : geometry(geom), material(mat), recursionDepth(tf), renderPassIndex(renderPassIndex){}
  Geometry geometry;
  Material material;
  bool visible = true;
	int recursionDepth = false;
  uint32_t renderPassIndex;
};

