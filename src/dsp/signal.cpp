#include <dsp/signal.h>

using namespace dsp;


Signal::Signal( )
{
    m_samples   = nullptr;
    m_length    = 0;
    m_position  = 0;
}

Signal::Signal( const Signal & other )
{
    std::complex<float> * sample_buffer = new std::complex<float>[m_length];
    const std::complex<float> * other_buffer = other.m_samples;
    m_length    = other.m_length;
    m_position  = other.m_position;
    m_samples   = sample_buffer;
    for(int i = m_length-1; i >= 0; --i)
    {
        // use sample_buffer since it's immediate
        sample_buffer[i] = other_buffer[i];
    }
}

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

const std::complex<float> & Signal::operator[](int index) const
{
    return m_samples[index];
}

int Signal::length(void) const
{
    return m_length;
}
