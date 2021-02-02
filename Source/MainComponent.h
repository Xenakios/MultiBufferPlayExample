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
    bool loadFileToSlot(int index,juce::File f);
    juce::File browseForAudioFile();
private:
    std::vector<juce::AudioBuffer<float>> playBuffers;
    std::vector<int> playPositions;
    std::vector<bool> playing; // technically not completely correct, these bools should be atomic, but good enough for an example...
    std::vector<float> playVolumes; // again, really should use atomics here...
    std::vector<std::unique_ptr<juce::ToggleButton>> playButtons;
    std::vector<std::unique_ptr<juce::TextButton>> loadButtons;
    std::vector<std::unique_ptr<juce::Slider>> volumeSliders;
    // using a lock, even a spinlock, isn't generally advisable in audio code but avoiding using this would just make the example unnecessarily complicated
    juce::SpinLock playLock;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
