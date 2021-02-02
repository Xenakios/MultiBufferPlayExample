#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    
    juce::AudioFormatManager aman;
    aman.registerBasicFormats();
    juce::StringArray testfiles =
    {
        "C:\\MusicAudio\\sourcesamples\\count.wav",
        "C:\\MusicAudio\\sourcesamples\\sheila.wav"
    };
    playBuffers.resize(testfiles.size());
    playPositions.resize(testfiles.size());
    playing.resize(testfiles.size(),false);
    for (size_t i = 0; i < testfiles.size(); ++i)
    {
        juce::File f(testfiles[i]);
        if (loadFileToSlot(i, f))
        {
            auto button = std::make_unique<juce::ToggleButton>();
            button->setButtonText("PLAY " + f.getFileName());
            addAndMakeVisible(button.get());
            button->setBounds(1, 1 + i * 25, 200, 24);
            button->onClick = [this, i]()
            {
                playing[i] = playButtons[i]->getToggleState();
            };
            playButtons.push_back(std::move(button));
            auto loadbutton = std::make_unique<juce::TextButton>();
            loadbutton->setButtonText("Load...");
            addAndMakeVisible(loadbutton.get());
            loadbutton->setBounds(205, 1 + i * 25, 100, 24);
            loadbutton->onClick = [this, i]()
            {
                auto f = browseForAudioFile();
                if (f != juce::File())
                {
                    if (loadFileToSlot(i, f))
                        playButtons[i]->setButtonText(f.getFileName());
                }
                    
            };
            loadButtons.push_back(std::move(loadbutton));
        }
    }
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    setSize(800, 600);
}

juce::File MainComponent::browseForAudioFile()
{
    juce::FileChooser myChooser("Please select audio file to load...",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*.wav");

    if (myChooser.browseForFileToOpen())
    {
        return myChooser.getResult();
    }
    return juce::File();
}

bool MainComponent::loadFileToSlot(int index, juce::File f)
{
    if (f.existsAsFile())
    {
        juce::AudioFormatManager aman;
        aman.registerBasicFormats();
        auto reader = std::unique_ptr<juce::AudioFormatReader>(aman.createReaderFor(f));
        if (reader)
        {
            juce::AudioBuffer<float> tempBuffer(1, reader->lengthInSamples);
            reader->read(&tempBuffer, 0, reader->lengthInSamples, 0, true, true);
            // dummy scope here so that the destructor of tempBuffer is run outside the lock...
            {
                juce::SpinLock::ScopedLockType locker(playLock);
                playPositions[index] = 0;
                std::swap(playBuffers[index], tempBuffer);
            }
            
            return true;
        }
    }
    return false;
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // reset play positions
    for (auto& e : playPositions)
        e = 0;
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    juce::SpinLock::ScopedLockType locker(playLock);
    bufferToFill.clearActiveBufferRegion();
    for (size_t i = 0; i < playBuffers.size(); ++i)
    {
        if (playing[i])
        {
            for (int j = 0; j < bufferToFill.numSamples; ++j)
            {
                float s = playBuffers[i].getSample(0, playPositions[i]);
                // copy mono source samples to both output channels
                for (int chan = 0; chan < 2; ++chan)
                {
                    bufferToFill.buffer->addSample(chan, bufferToFill.startSample + j, s);
                }
                ++playPositions[i];
                if (playPositions[i] >= playBuffers[i].getNumSamples())
                    playPositions[i] = 0;
            }
        }
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
