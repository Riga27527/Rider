#pragma once

#include <core/common.h>
#include <core/geometry.h>
#include <core/shape.h>
#include <core/material.h>
#include <vector>

RIGA_NAMESPACE_BEGIN

class Primitive{
public:
	virtual ~Primitive(){}
	virtual Bounds3f worldBound() const = 0;
	virtual bool intersect(const Ray& ray, SurfaceInteraction *) const = 0;
	virtual bool intersectP(const Ray& ray) const = 0;
	virtual void computeScatteringFunctions(
		SurfaceInteraction* isect, TransportMode mode) const = 0;
	virtual	AreaLight* getAreaLight() const = 0;
	virtual Material* getMaterial() const = 0;
};


class GeometricPrimitive : public Primitive{
public:
	GeometricPrimitive(
		const std::shared_ptr<Shape>& shape, const std::shared_ptr<Material>& mat, const std::shared_ptr<AreaLight>& light = nullptr) 
		: shape(shape), material(mat), areaLight(light){};
	Bounds3f worldBound() const;
	bool intersect(const Ray& ray, SurfaceInteraction* isect) const;
	bool intersectP(const Ray& ray) const;
	AreaLight* getAreaLight() const;
	Material* getMaterial() const;
	void computeScatteringFunctions(
		SurfaceInteraction* isect, TransportMode mode) const;

private:
	std::shared_ptr<Shape> shape;
	std::shared_ptr<Material> material;
	std::shared_ptr<AreaLight> areaLight;
};

class Aggregate{
public:
	Aggregate(std::vector<std::shared_ptr<Primitive>> p) : primitives(std::move(p)){}
	virtual ~Aggregate(){}
	virtual Bounds3f worldBound() const = 0;
	virtual bool intersect(const Ray& ray, SurfaceInteraction *) const = 0;
	virtual bool intersectP(const Ray& ray) const = 0;

protected:
	std::vector<std::shared_ptr<Primitive>> primitives;
};

RIGA_NAMESPACE_END