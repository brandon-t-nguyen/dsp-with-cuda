#ifndef __DSP__SIGNAL_H__
#define __DSP__SIGNAL_H__

#include <vector>
#include <complex>

namespace dsp
{
    class Signal
    {
    public:
        // Copy constructor
        Signal( );
        Signal( const Signal & other );

        // Empty signal constructor
        Signal( int length );

        // Real constructors
        Signal( int length, const float samples[] );
        Signal( const std::vector<float> & samples );

        // Complex constructors
        Signal( int length, const std::complex<float> samples[] );
        Signal( const std::vector< std::complex<float> > & samples );
        ~Signal();

        /**
         * Extends this signal to desired length
         * @param[in]   length  The desired length
         * @return      Another an extended signal
         */
        Signal extend( int length );

        /**
         * Returns the position of the 0th element
         * relative to the global origin
         * @return  postion of the start of the signal
         */
        int relative_position(void) const;

        /**
         * Shifts the position of the signal relative
         * to the global origin.
         *
         * @param[in]   shift   How much to shift
         * @return      The new position
         */
        int shift_position(int shift);

        /**
         * Returns the length of this signal
         * @return  length of signal
         */
        int length(void) const;

        /**
         * Accesses the signal by its sample index
         * @param[in]   index   Local sample index
         */
        std::complex<float> & operator[](int index);
        const std::complex<float> & operator[](int index) const;


        static Signal convolve( const Signal & x, const Signal & h );
        static Signal convolve_cpu( const Signal & x, const Signal & h );
        static Signal convolve_gpu( const Signal & x, const Signal & h );

        static Signal fft( const Signal & x );
        static Signal fft_cpu( const Signal & x );
        static Signal fft_gpu( const Signal & x );

        static Signal ifft( const Signal & x );
        static Signal ifft_cpu( const Signal & x );
        static Signal ifft_gpu( const Signal & x );

    private:
        // I'm going to roll with single precision
        // Apparently there's more hoops to jump through
        // for using doubles
        std::complex<float> * m_samples;
        int     m_length;
        int     m_position;

        void destroy();
    };
    /**
     * Perform a convolution on the signal
     */
    static Signal operator*( const Signal & lhs, const Signal & rhs );
}

#endif//__DSP__SIGNAL_H__
