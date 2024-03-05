#pragma once

#include "PluginProcessor.h"

struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
                                        {

                                        }
};

//==============================================================================
class SimpleEQAudioProcessorEditor final : public juce::AudioProcessorEditor,
juce::AudioProcessorParameter::Listener,
juce::Timer
{
public:
    explicit SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor&);
    ~SimpleEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void parameterValueChanged (int parameterIndex, float newValue) override;

    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override;

    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleEQAudioProcessor& processorRef;

    // Because Listeners MUST be very fast and avoid blocking, we define an atomic flag for signalling
    juce::Atomic<bool> parametersChanged { false };

    CustomRotarySlider peakFreqSlider,
                        peakGainSlider,
                        peakQualitySlider,
                        loCutFreqSlider,
                        hiCutFreqSlider,
                        loCutSlopeSlider,
                        hiCutSlopeSlider;

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment  peakFreqSliderAttachment,
                peakGainSliderAttachment,
                peakQualitySliderAttachment,
                loCutFreqSliderAttachment,
                hiCutFreqSliderAttachment,
                loCutSlopeSliderAttachment,
                hiCutSlopeSliderAttachment;

    std::vector<juce::Component*> getComps();

    MonoChain monoChain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessorEditor)
};
