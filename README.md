# SavePersonalVoiceAudio

Allows recording of Personal Voices to a file using builtin say command in Mac OS Sonoma

Since Apple released the Personal Voice feature on Mac OS, you cannot use it in command line using the
"say -v MyPersonalVoice" command, 
because terminal, and therefore "say" command does not have authorization for personal voices use.
Even if you enable terminal, say command does not allow saving personal voice speech to file using the -o option,
and the logged error is "Cannot use AVSpeechSynthesizerBufferCallback with Personal Voices, defaulting to output channel."


This project creates an executable that asks for authorization from terminal, so that terminal gets authorized in Settings - Accessibility - Personal Voices.

It also creates a library that you can then insert into say process using DYLD_INSERT_LIBRARIES that enables it to save personal voice speech to audio files, something that is not possible with Mac OS Sonoma at the time.

## Requirements: 
Latest Xcode Command Line Tools and a terminal

## Installation
Download the code, cd to the extracted folder in Terminal and run "make".

## Usage:

### ./authorize_terminal
to request and get authorization for Personal Voices in Terminal. You only need to do this once.

### DYLD_INSERT_LIBRARIES=./mysay.dylib say -v MyPersonalVoiceName "Text to speak" -e outputfilename.caf
to run say with the ability to save an audio file of your speech to "outputfilename.caf" file

Note: I have noticed that PersonalVoice randonly stops working system-wide. A solution to this is to open Settings - Accessibility - Live Speech, enable Live Speech, set your personal voice as the speaking voice, and have it speak a word.

## Warning
PersonalVoice is private, you should avoid sharing your recordings with others as it can be used to create a replica of your own voice.
