/*
 *    Copyright (C) 2019
 *    Matthias P. Braendli (matthias.braendli@mpb.li)
 *
 *    Copyright (C) 2017
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 *    This file is based on SDR-J
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *
 *
 */

#pragma once

// see OFDMProcessor::processPRS() for more information about these methods
enum class FreqsyncMethod { GetMiddle = 0, CorrelatePRS = 1, PatternOfZeros = 2 };

enum class FFTPlacementMethod {
    /* Old method: places the FFT on the strongest peak, which must be at least
     * 3 times as high as the average.
     *
     * Issues: can lock on a peak that is not the earlisest peak (multipath)
     */
    StrongestPeak,

    /* Calculate peaks over bins of 25 samples, keep the 4 bins with the
     * highest peaks, take the index from the peak in the earliest bin, but not
     * any earlier than 500 samples.
     *
     * Issues: sometimes loses lock even in good receive conditions.
     */
    EarliestPeakWithBinning,

    /* Apply a windowing function that selects the peak correlation, then
     * place the FFT where the correlation goes above a threshold earliest.
     *
     * Issues: performance not yet assessed.
     */
    ThresholdBeforePeak,
};

// Default uses the old algorithm until the issues of the new one are solved.
constexpr auto DEFAULT_FFT_PLACEMENT = FFTPlacementMethod::ThresholdBeforePeak;

// Configuration for the backend
struct RadioReceiverOptions {
    // Select the algorithm used in the OFDMProcessor PRS sync logic
    // to place the FFT window for demodulation.
    //
    // Issues: initial lock can take longer than with original algorithm.
    FFTPlacementMethod fftPlacementMethod = DEFAULT_FFT_PLACEMENT;

    // Set to true to enable the TII decoder. Default is false because it is
    // consumes CPU resources.
    bool decodeTII = false;

    // Good receivers with accurate clocks do not need the coarse corrector.
    // Disabling it can accelerate lock.
    bool disableCoarseCorrector = false;

    // Which method to use for the freqsyncmethod used in the coarse corrector.
    // Has no effect when coarse corrector is disabled.
    FreqsyncMethod freqsyncMethod = FreqsyncMethod::PatternOfZeros;
};

