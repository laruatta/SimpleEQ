#pragma once

#include "PluginProcessor.h"

struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
                                        {

                                        }
};

struct ResponseCurveComponent: juce::Component,
juce::AudioProcessorParameter::Listener,
juce::Timer
{
    ResponseCurveComponent(SimpleEQAudioProcessor&);
    ~ResponseCurveComponent();

    void parameterValueChanged (int parameterIndex, float newValue) override;

    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override;

    void timerCallback() override;

    void paint (juce::Graphics&) override;

    private:
        SimpleEQAudioProcessor& processorRef;

        // Because Listeners MUST be very fast and avoid blocking, we define an atomic flag for signalling
        juce::Atomic<bool> parametersChanged { false };
        MonoChain monoChain;
};

//==============================================================================
class SimpleEQAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor&);
    ~SimpleEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleEQAudioProcessor& processorRef;

    CustomRotarySlider peakFreqSlider,
                        peakGainSlider,
                        peakQualitySlider,
                        loCutFreqSlider,
                        hiCutFreqSlider,
                        loCutSlopeSlider,
                        hiCutSlopeSlider;

    ResponseCurveComponent responseCurveComponent;

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


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessorEditor)
};
