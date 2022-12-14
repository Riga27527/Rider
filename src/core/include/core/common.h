#pragma once

#include <iostream>
#include <chrono>
#include <cmath>
#include <tbb/tick_count.h>
#include <numeric>
#include <vector>

#define RIGA_NAMESPACE_BEGIN namespace riga {
#define RIGA_NAMESPACE_END }

#define TICK(x) auto bench_##x = std::chrono::steady_clock::now();
#define TOCK(x) std::cout << #x ": " << \
std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - bench_##x).count() << "s" << std::endl;

#define TICK_TBB(x) auto bench_##x = tbb::tick_count::now();
#define TOCK_TBB(x) std::cout << #x ": " << (tbb::tick_count::now() - bench_##x).seconds() << "s" << std::endl;

#define MachineEpsilon (std::numeric_limits<float>::epsilon() * 0.5)
#define Infinity std::numeric_limits<float>::infinity()
#define RayEpsilon 1e-4
// #define Epsilon  std::numeric_limits<float>::epsilon()

RIGA_NAMESPACE_BEGIN

static constexpr float PI         	= 3.14159265358979323846f;
static constexpr float INV_PI       = 0.31830988618379067154f;
static constexpr float INV_TWOPI    = 0.15915494309189533577f;
static constexpr float INV_FOURPI   = 0.07957747154594766788f;
static constexpr float SQRT_TWO     = 1.41421356237309504880f;
static constexpr float INV_SQRT_TWO = 0.70710678118654752440f;

static const float OneMinusEpsilon = 0.99999994;

template <class T>
class Vec2;
template <class T>
class Vec3;
template <class T>
class Point2;
template <class T>
class Point3;
template <class T>
class Normal3;
template <class T>
class Bounds2;
template <class T>
class Bounds3;

template<int nSamples>
class CoefficientSpectrum;
class RGBSpectrum;
typedef RGBSpectrum Spectrum;

class Transform;
class Shape;
class Primitive;
class GeometricPrimitive;
struct Interaction;
class SurfaceInteraction;
class Camera;
struct CameraSample;
class ProjectiveCamera;
class Scene;

class BSDF;
class BxDF;

class Material;
template<typename T>
class Texture;

class Light;
class AreaLight;
class VisibilityTester;

template <class T>
bool isNaN(const T x){
	return std::isnan(x);
}	

template <class T>
bool isInf(const T x){
	return std::isinf(x);
}

inline float Radians(float deg){
	return (PI / 180.f) * deg;
}

inline float Degrees(float rad){
	return (180.f / PI) * rad;
}

inline float Clamp(float x, float small, float large){
	return std::min(std::max(x, small), large);
}

template <typename T>
inline T Mod(T a, T b) {
    T result = a - (a / b) * b;
    return (T)((result < 0) ? result + b : result);
}

// need delete--------------------------------------****************************------------------
inline float random_float(){
	return rand() / (RAND_MAX + 1.0f);
}

std::vector<std::string> tokenize(const std::string &string, const std::string &delim, bool includeEmpty);

// Convert a string into an unsigned integer value
unsigned int toUInt(const std::string &str);

inline int CountTrailingZeros(uint32_t v) {
#if defined(MSVC)
    unsigned long index;
    if (_BitScanForward(&index, v))
        return index;
    else
        return 32;
#else
    return __builtin_ctz(v);
#endif
}

inline float Gamma(int n){
	return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

RIGA_NAMESPACE_END