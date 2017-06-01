#include <dsp/signal.h>

using namespace dsp;

Signal::Signal( int length, const float samples[] )
{
    std::complex<float> * sample_buffer = new std::complex<float>[length];
    for(int i = length-1; i >=0; --i)
    {
        sample_buffer[i] = std::complex<float>(samples[i], 0.0f);
    }
    m_samples = sample_buffer;
    m_length  = length;
}

Signal::Signal( const std::vector<float> & samples )
{
    std::complex<float> * sample_buffer = new std::complex<float>[samples.size()];
    for(int i = samples.size()-1; i >=0; --i)
    {
        sample_buffer[i] = std::complex<float>(samples[i], 0.0f);
    }
    m_samples = sample_buffer;
    m_length  = samples.size();
}

Signal::~Signal()
{
    delete[] m_samples;
}

std::complex<float> & Signal::operator[](int index)
{
    return m_samples[index];
}

