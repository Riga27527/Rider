#pragma once
#include <core/common.h>
#include <core/sampling.h>
#include <core/geometry.h>

RIGA_NAMESPACE_BEGIN
static constexpr int PrimeTableSize = 1000;

// Low Discrepancy Declarations
float RadicalInverse(int baseIndex, uint64_t a);
std::vector<uint16_t> ComputeRadicalInversePermutations(pcg32 &rng);
extern const int Primes[PrimeTableSize];
float ScrambledRadicalInverse(int baseIndex, uint64_t a, const uint16_t *perm);
extern const int PrimeSums[PrimeTableSize];
extern uint32_t CMaxMinDist[17][32];

// Low Discrepancy Inline Functions
inline uint32_t ReverseBits32(uint32_t n) {
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    return n;
}

inline uint64_t ReverseBits64(uint64_t n) {
    uint64_t n0 = ReverseBits32((uint32_t)n);
    uint64_t n1 = ReverseBits32((uint32_t)(n >> 32));
    return (n0 << 32) | n1;
}

template <int base>
inline uint64_t InverseRadicalInverse(uint64_t inverse, int nDigits) {
    uint64_t index = 0;
    for (int i = 0; i < nDigits; ++i) {
        uint64_t digit = inverse % base;
        inverse /= base;
        index = index * base + digit;
    }
    return index;
}

inline uint32_t MultiplyGenerator(const uint32_t *C, uint32_t a) {
    uint32_t v = 0;
    for (int i = 0; a != 0; ++i, a >>= 1)
        if (a & 1) v ^= C[i];
    return v;
}

inline float SampleGeneratorMatrix(const uint32_t *C, uint32_t a,
                                   uint32_t scramble = 0) {
    return std::min((MultiplyGenerator(C, a) ^ scramble) * float(0x1p-32),
                    OneMinusEpsilon);
}

inline uint32_t GrayCode(uint32_t v) { return (v >> 1) ^ v; }

inline void GrayCodeSample(const uint32_t *C, uint32_t n, uint32_t scramble,
                           float *p) {
    uint32_t v = scramble;
    for (uint32_t i = 0; i < n; ++i) {
        p[i] = std::min(v * float(0x1p-32) /* 1/2^32 */,
                        OneMinusEpsilon);
        v ^= C[CountTrailingZeros(i + 1)];
    }
}

inline void GrayCodeSample(const uint32_t *C0, const uint32_t *C1, uint32_t n,
                           const Point2i &scramble, Point2f *p) {
    uint32_t v[2] = {(uint32_t)scramble.x, (uint32_t)scramble.y};
    for (uint32_t i = 0; i < n; ++i) {

        p[i].x = std::min(v[0] * float(0x1p-32), OneMinusEpsilon);
        p[i].y = std::min(v[1] * float(0x1p-32), OneMinusEpsilon);

        v[0] ^= C0[CountTrailingZeros(i + 1)];
        v[1] ^= C1[CountTrailingZeros(i + 1)];
    }
}

inline void VanDerCorput(int nSamplesPerPixelSample, int nPixelSamples,
                         float *samples, pcg32 &rng) {
    uint32_t scramble = rng.nextUInt();
    // Define _CVanDerCorput_ Generator Matrix
    const uint32_t CVanDerCorput[32] = {
        0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x8000000, 0x4000000,
        0x2000000,  0x1000000,  0x800000,   0x400000,   0x200000,  0x100000,
        0x80000,    0x40000,    0x20000,    0x10000,    0x8000,    0x4000,
        0x2000,     0x1000,     0x800,      0x400,      0x200,     0x100,
        0x80,       0x40,       0x20,       0x10,       0x8,       0x4,
        0x2,        0x1
    };
    int totalSamples = nSamplesPerPixelSample * nPixelSamples;
    GrayCodeSample(CVanDerCorput, totalSamples, scramble, samples);
    // Randomly shuffle 1D sample points
    for (int i = 0; i < nPixelSamples; ++i)
        Shuffle(samples + i * nSamplesPerPixelSample, nSamplesPerPixelSample, 1,
                rng);
    Shuffle(samples, nPixelSamples, nSamplesPerPixelSample, rng);
}


RIGA_NAMESPACE_END