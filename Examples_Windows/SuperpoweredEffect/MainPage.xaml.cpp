// Typical UWP stuff.
#include "pch.h"
#include "MainPage.xaml.h"
using namespace SuperpoweredEffect;

#include <helpers.h>
#include <Superpowered.h>
#include <SuperpoweredWindowsAudioIO.h>
#include <SuperpoweredReverb.h>

static SuperpoweredWindowsAudioIO *audioIO = NULL;
static SuperpoweredReverb *reverb = NULL;
static int lastSamplerate = 44100;

MainPage::MainPage() {
	InitializeComponent();
    SuperpoweredInitialize(
                           "ExampleLicenseKey-WillExpire-OnNextUpdate",
                           false, // enableAudioAnalysis (using SuperpoweredAnalyzer, SuperpoweredLiveAnalyzer, SuperpoweredWaveform or SuperpoweredBandpassFilterbank)
                           false, // enableFFTAndFrequencyDomain (using SuperpoweredFrequencyDomain, SuperpoweredFFTComplex, SuperpoweredFFTReal or SuperpoweredPolarFFT)
                           false, // enableAudioTimeStretching (using SuperpoweredTimeStretching)
                           true, // enableAudioEffects (using any SuperpoweredFX class)
                           false, // enableAudioPlayerAndDecoder (using SuperpoweredAdvancedAudioPlayer or SuperpoweredDecoder)
                           false, // enableCryptographics (using Superpowered::RSAPublicKey, Superpowered::RSAPrivateKey, Superpowered::hasher or Superpowered::AES)
                           false  // enableNetworking (using Superpowered::httpRequest)
                           );
}

// Audio input/output.
static bool audioProcessing(void *clientdata, float *audio, int numberOfSamples, int samplerate) {
	if (!audio) {
		if (samplerate == 0) {
			Log("Audio I/O stopped.\n");
			delete reverb;
		}
		else Log("Audio I/O error %i.\n", samplerate);
		return false;
	}

	if (lastSamplerate != samplerate) {
		lastSamplerate = samplerate;
		reverb->setSamplerate(samplerate);
	}

	reverb->process(audio, audio, numberOfSamples);
	return true;
}

// The button handler.
void MainPage::Toggle(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	Windows::UI::Xaml::Controls::Button^ button = safe_cast<Windows::UI::Xaml::Controls::Button^>(sender);

	if (!audioIO) {
		reverb = new SuperpoweredReverb(lastSamplerate);
		reverb->enable(true);

		audioIO = new SuperpoweredWindowsAudioIO(audioProcessing, NULL, true, true);
		audioIO->start();

		button->Content = "Stop";
	}
	else {
		audioIO->stop();
		delete audioIO;
		audioIO = NULL;
		button->Content = "Start";
	}
}
