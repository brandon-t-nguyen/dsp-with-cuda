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

Signal::Signal( int length )
{
    std::complex<float> * sample_buffer = new std::complex<float>[length];
    m_samples   = sample_buffer;
    m_length    = length;
    m_position  = 0;

    // zeroed signal
    for (int i = length-1; i >= 0; --i)
    {
        sample_buffer[i] = std::complex<float>(0.0f,0.0f);
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

Signal::Signal( int length, const std::complex<float> samples[] )
{
    std::complex<float> * sample_buffer = new std::complex<float>[length];
    for(int i = length-1; i >=0; --i)
    {
        sample_buffer[i] = samples[i];
    }
    m_samples = sample_buffer;
    m_length  = length;
}

Signal::Signal( const std::vector<std::complex<float>> & samples )
{
    std::complex<float> * sample_buffer = new std::complex<float>[samples.size()];
    for(int i = samples.size()-1; i >=0; --i)
    {
        sample_buffer[i] = samples[i];
    }
    m_samples = sample_buffer;
    m_length  = samples.size();
}

Signal::~Signal()
{
    destroy();
}

void Signal::destroy()
{
    if(m_samples != nullptr)
    {
        delete[] m_samples;
    }
    m_samples  = nullptr;
    m_length   = 0;
    m_position = 0;
}

std::complex<float> & Signal::operator[](int index)
{
    return m_samples[index];
}

const std::complex<float> & Signal::operator[](int index) const
{
    return m_samples[index];
}

Signal Signal::extend( int length )
{
    if (length <= m_length)
    {
        Signal copy = Signal(*this);
        return copy;
    }

    // copy original
    std::complex<float> * samples_buffer = new std::complex<float>[length];
    int i;
    for (i = 0; i < m_length; ++i)
    {
        samples_buffer[i] = m_samples[i];
    }

    // put zeros in
    for (; i < length; ++i)
    {
        samples_buffer[i] == std::complex<float>(0.0f, 0.0f);
    }

    Signal copy;
    copy.m_samples = samples_buffer;
    copy.m_length = length;
    copy.m_position = m_position;

    return copy;
}

int Signal::length(void) const
{
    return m_length;
}
