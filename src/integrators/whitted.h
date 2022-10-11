#pragma once

#include "common.h"
#include "integrator.h"

RIGA_NAMESPACE_BEGIN

class WhittedIntegrator : public SamplerIntegrator{
public:
	WhittedIntegrator(int maxDepth, std::shared_ptr<const Camera> cam, std::shared_ptr<Sampler> samp)
		: SamplerIntegrator(cam, samp), maxDepth(maxDepth){}
	Spectrum Li(const Ray& ray, const Scene& scene, Sampler& sampler, int depth = 0) const;
private:
	const int maxDepth;
};

RIGA_NAMESPACE_END