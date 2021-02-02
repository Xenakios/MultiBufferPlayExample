#pragma once

#include <JuceHeader.h>



class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    std::vector<juce::AudioBuffer<float>> playBuffers;
    std::vector<int> playPositions;
    std::vector<bool> playing; // technically not completely correct, these bools should be atomic, but good enough for an example...
    std::vector<std::unique_ptr<juce::ToggleButton>> playButtons;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
