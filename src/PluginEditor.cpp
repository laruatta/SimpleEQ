#include "PluginProcessor.h"
#include "PluginEditor.h"

ResponseCurveComponent::ResponseCurveComponent(SimpleEQAudioProcessor& p) : processorRef(p)
{
    const auto& params = processorRef.getParameters();

    for ( auto param : params )
    {
        param->addListener(this);
    }

    startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = processorRef.getParameters();
    for ( auto& param : params )
    {
        param->removeListener(this);
    }
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    juce::ignoreUnused(parameterIndex);
    juce::ignoreUnused(newValue);
    parametersChanged.set(true);
}

void ResponseCurveComponent::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
    juce::ignoreUnused(parameterIndex);
    juce::ignoreUnused(gestureIsStarting);
    // parametersChanged.set(true);
}

void ResponseCurveComponent::timerCallback()
{
    if( parametersChanged.compareAndSetBool(false, true))
    {
        // update Editor local monochain
        auto chainSettings = getChainSettings(processorRef.apvts);
        auto peakCoefficients = makePeakFilter(chainSettings, processorRef.getSampleRate());
        updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
        
        auto lowCutCoefficients = makeLoCutFilter(chainSettings, processorRef.getSampleRate());
        auto hiCutCoefficients  = makeHiCutFilter(chainSettings, processorRef.getSampleRate());

        updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);
        updateCutFilter(monoChain.get<ChainPositions::HiCut>(), hiCutCoefficients, chainSettings.highCutSlope);
        
        // signal a repaint
        repaint();
    }
}

void ResponseCurveComponent::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    auto responseArea = getLocalBounds();
    auto w = responseArea.getWidth();

    auto& lowcut = monoChain.get<ChainPositions::LowCut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto& hicut = monoChain.get<ChainPositions::HiCut>();

    auto sampleRate = processorRef.getSampleRate();

    std::vector<double> mags;

    mags.resize(w);

    for (int i = 0; i < w; i++)
    {
        double mag = 1.f;
        auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

        if(! monoChain.isBypassed<ChainPositions::Peak>() )
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if(! lowcut.isBypassed<0>() )
            mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if(! lowcut.isBypassed<1>() )
            mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if(! lowcut.isBypassed<2>() )
            mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if(! lowcut.isBypassed<3>() )
            mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if(! hicut.isBypassed<0>() )
            mag *= hicut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if(! hicut.isBypassed<1>() )
            mag *= hicut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if(! hicut.isBypassed<2>() )
            mag *= hicut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if(! hicut.isBypassed<3>() )
            mag *= hicut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        // Convert magnitude into decibels
        mags[i] = Decibels::gainToDecibels(mag);
    }
    
    Path responseCurve;

    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax](double input)
    {
        return jmap(input, -24.0, 24.0, outputMin, outputMax);
    };

    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));

    for (size_t i = 0; i < mags.size(); i++)
    {
        responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }

    g.setColour(Colours::orange);
    g.drawRoundedRectangle(responseArea.toFloat(),4.f, 1.f);

    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.f));
    

    
}

//==============================================================================
// Constructor of SimpleEQAudioProcessorEditor class;
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
    responseCurveComponent(processorRef),
    peakFreqSliderAttachment(processorRef.apvts, "Peak Freq", peakFreqSlider),
    peakGainSliderAttachment(processorRef.apvts, "Peak Gain", peakGainSlider),
    peakQualitySliderAttachment(processorRef.apvts, "Peak Quality", peakQualitySlider),
    loCutFreqSliderAttachment(processorRef.apvts, "LoCut Freq", loCutFreqSlider),
    hiCutFreqSliderAttachment(processorRef.apvts, "HiCut Freq", hiCutFreqSlider),
    loCutSlopeSliderAttachment(processorRef.apvts, "LoCut Slope", loCutSlopeSlider),
    hiCutSlopeSliderAttachment(processorRef.apvts, "HiCut Slope", hiCutSlopeSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    for( auto* comp : getComps( ))
    {
        addAndMakeVisible(comp);
    }
    
    setSize (600, 400);
}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{

}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SimpleEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

    responseCurveComponent.setBounds(responseArea);

    auto loCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto hiCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    loCutFreqSlider.setBounds(loCutArea.removeFromTop(loCutArea.getHeight() * 0.5));
    loCutSlopeSlider.setBounds(loCutArea);
    hiCutFreqSlider.setBounds(hiCutArea.removeFromTop(hiCutArea.getHeight() * 0.5));
    hiCutSlopeSlider.setBounds(hiCutArea);

    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    peakQualitySlider.setBounds(bounds);
}

std::vector<juce::Component*> SimpleEQAudioProcessorEditor::getComps()
{
    return 
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &loCutFreqSlider,
        &hiCutFreqSlider,
        &loCutSlopeSlider,
        &hiCutSlopeSlider,
        &responseCurveComponent
    };
}
