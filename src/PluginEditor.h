#pragma once

#include "PluginProcessor.h"

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics&, 
                        int x, int y, int width, int height,
                        float sliderPosProportional, 
                        float rotaryStartAngle,
                        float rotaryEndAngle, 
                        juce::Slider&) override;
};

struct LabeledRotarySlider : juce::Slider
{
    // constructor
    LabeledRotarySlider(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) : 
    juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                 juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(&rap),
    suffix(unitSuffix)
    {
        setLookAndFeel(&laf);
    }

    // destructor
    ~LabeledRotarySlider()
    {
        setLookAndFeel(nullptr);
    }

    struct LabelPos
    {
        float pos;
        juce::String label;
    };

    juce::Array<LabelPos> labels;

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const{ return 14; }
    juce::String getDisplayString() const;

    private:
        LookAndFeel laf;
        juce::RangedAudioParameter* param;
        juce::String suffix;

};

struct ResponseCurveComponent: juce::Component,
juce::AudioProcessorParameter::Listener,
juce::Timer
{
    ResponseCurveComponent(SimpleEQAudioProcessor&);
    ~ResponseCurveComponent();

    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override { };
    void timerCallback() override;
    void paint (juce::Graphics&) override;

    void resized() override;

    private:
        SimpleEQAudioProcessor& processorRef;

        // Because Listeners MUST be very fast and avoid blocking, we define an atomic flag for signalling
        juce::Atomic<bool> parametersChanged { false };
        MonoChain monoChain;

        void updateChain();

        juce::Image background;

        juce::Rectangle<int> getRenderArea();
        juce::Rectangle<int> getAnalysisArea();
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

    LabeledRotarySlider peakFreqSlider,
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
