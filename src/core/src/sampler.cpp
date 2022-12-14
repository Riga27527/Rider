#include <core/sampler.h>
#include <core/camera.h>

RIGA_NAMESPACE_BEGIN

Sampler::~Sampler() {}

Sampler::Sampler(int64_t samplesPerPixel) : samplesPerPixel(samplesPerPixel) {}

CameraSample Sampler::getCameraSample(const Point2i &pRaster) {
    CameraSample cs;
    cs.samplePoints = (Point2f)pRaster + get2D();
    return cs;
}

void Sampler::startPixel(const Point2i &p) {
    currentPixel = p;
    currentPixelSampleIndex = 0;
    // Reset array offsets for next pixel sample
    array1DOffset = array2DOffset = 0;
}

bool Sampler::startNextSample() {
    // Reset array offsets for next pixel sample
    array1DOffset = array2DOffset = 0;
    return ++currentPixelSampleIndex < samplesPerPixel;
}

bool Sampler::setSampleNumber(int64_t sampleNum) {
    // Reset array offsets for next pixel sample
    array1DOffset = array2DOffset = 0;
    currentPixelSampleIndex = sampleNum;
    return currentPixelSampleIndex < samplesPerPixel;
}

void Sampler::request1DArray(int n) {
    samples1DArraySizes.push_back(n);
    sampleArray1D.push_back(std::vector<float>(n * samplesPerPixel));
}

void Sampler::request2DArray(int n) {
    samples2DArraySizes.push_back(n);
    sampleArray2D.push_back(std::vector<Point2f>(n * samplesPerPixel));
}

const float *Sampler::get1DArray(int n) {
    if (array1DOffset == sampleArray1D.size()) return nullptr;
    
    if((samples1DArraySizes[array1DOffset] != n) || (currentPixelSampleIndex > samplesPerPixel)) 
    	throw("Sampler::Get1DArray get an error! ");

    return &sampleArray1D[array1DOffset++][currentPixelSampleIndex * n];
}

const Point2f *Sampler::get2DArray(int n) {
    if (array2DOffset == sampleArray2D.size()) return nullptr;

    if((samples2DArraySizes[array2DOffset] != n) || (currentPixelSampleIndex > samplesPerPixel)) 
    	throw("Sampler::Get2DArray get an error! ");
    
    return &sampleArray2D[array2DOffset++][currentPixelSampleIndex * n];
}

void GlobalSampler::startPixel(const Point2i &p) {
    Sampler::startPixel(p);
    dimension = 0;
    intervalSampleIndex = getIndexForSample(0);
    // Compute _arrayEndDim_ for dimensions used for array samples
    arrayEndDim =
        arrayStartDim + sampleArray1D.size() + 2 * sampleArray2D.size();

    // Compute 1D array samples for _GlobalSampler_
    for (size_t i = 0; i < samples1DArraySizes.size(); ++i) {
        int nSamples = samples1DArraySizes[i] * samplesPerPixel;
        for (int j = 0; j < nSamples; ++j) {
            int64_t index = getIndexForSample(j);
            sampleArray1D[i][j] = sampleDimension(index, arrayStartDim + i);
        }
    }

    // Compute 2D array samples for _GlobalSampler_
    int dim = arrayStartDim + samples1DArraySizes.size();
    for (size_t i = 0; i < samples2DArraySizes.size(); ++i) {
        int nSamples = samples2DArraySizes[i] * samplesPerPixel;
        for (int j = 0; j < nSamples; ++j) {
            int64_t idx = getIndexForSample(j);
            sampleArray2D[i][j].x = sampleDimension(idx, dim);
            sampleArray2D[i][j].y = sampleDimension(idx, dim + 1);
        }
        dim += 2;
    }
    if(arrayEndDim != dim)
    	throw("GlobalSampler::StartPixel get an error!");
}

bool GlobalSampler::startNextSample() {
    dimension = 0;
    intervalSampleIndex = getIndexForSample(currentPixelSampleIndex + 1);
    return Sampler::startNextSample();
}

bool GlobalSampler::setSampleNumber(int64_t sampleNum) {
    dimension = 0;
    intervalSampleIndex = getIndexForSample(sampleNum);
    return Sampler::setSampleNumber(sampleNum);
}

float GlobalSampler::get1D() {
    if (dimension >= arrayStartDim && dimension < arrayEndDim)
        dimension = arrayEndDim;
    return sampleDimension(intervalSampleIndex, dimension++);
}

Point2f GlobalSampler::get2D() {
    if (dimension + 1 >= arrayStartDim && dimension < arrayEndDim)
        dimension = arrayEndDim;
    Point2f p(sampleDimension(intervalSampleIndex, dimension),
              sampleDimension(intervalSampleIndex, dimension + 1));
    dimension += 2;
    return p;
}

RIGA_NAMESPACE_END