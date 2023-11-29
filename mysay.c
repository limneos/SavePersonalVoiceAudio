/* Copyright 2023 Elias Limneos
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/


#import <mach-o/dyld.h>
#import <Foundation/Foundation.h>
#import <dlfcn.h>
#import <objc/runtime.h>
#import <AudioToolbox/AudioToolbox.h>
 
 

#define DYLD_INTERPOSE(_replacement,_replacee) \
   __attribute__((used)) static struct{ const void* replacement; const void* replacee; } _interpose_##_replacee \
            __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacement, (const void*)(unsigned long)&_replacee };

const char *outputPath = NULL;
static ExtAudioFileRef audioFile=NULL;
 
OSStatus replaced_AudioUnitRender(AudioUnit inUnit, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inOutputBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData){
	
	static int printed=0;
	if (!outputPath){
		 
		return AudioUnitRender(inUnit,ioActionFlags,inTimeStamp,inOutputBusNumber,inNumberFrames,ioData);
	}
 	if (!printed){
 		printf("Writing audio file at path: %s\n",outputPath);
 		printed=1;
 	}

	OSStatus status;

	AudioStreamBasicDescription desc;
	UInt32 size=sizeof(desc);
	AudioUnitGetProperty(inUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0 , &desc,&size);

	if (!audioFile){
	
		AudioStreamBasicDescription fileDescription={0};
		fileDescription.mSampleRate=48000;
		fileDescription.mFormatID='lpcm';
		fileDescription.mFormatFlags = 41; 
		fileDescription.mBytesPerPacket=4;
		fileDescription.mFramesPerPacket=1;
		fileDescription.mBytesPerFrame=4;
		fileDescription.mChannelsPerFrame=1;
		fileDescription.mBitsPerChannel=32;
		fileDescription.mReserved=0;
		 
		
		CFURLRef url = CFURLCreateWithFileSystemPath(NULL, (CFStringRef)[NSString stringWithUTF8String:outputPath], kCFURLPOSIXPathStyle, false);	
		status = ExtAudioFileCreateWithURL(url, kAudioFileCAFType, &fileDescription, NULL, kAudioFileFlags_EraseFile, &audioFile);
		CFRelease(url);
		
		if (status){
			printf("error creating file %s-> %u\n",outputPath,status);
		}
		
		status = ExtAudioFileSetProperty(audioFile, kExtAudioFileProperty_ClientDataFormat, sizeof(AudioStreamBasicDescription), &desc);
		
		if (status){
			printf("error setting client format to file %u\n",status);
		}
		
	}
	
	status=ExtAudioFileWrite(audioFile,inNumberFrames,ioData);

	if (status){
		printf ("error writing %u\n",status);
	}
	 
	return AudioUnitRender(inUnit,ioActionFlags,inTimeStamp,inOutputBusNumber,inNumberFrames,ioData);
}


DYLD_INTERPOSE(replaced_AudioUnitRender,AudioUnitRender);
 

__attribute__((constructor)) void constructor (int argc, char **argv){
	
	char *voice=NULL;
	
	for (int i=0; i<argc; i++){
		if (!strcmp(argv[i],"-v")){
			voice = strdup(argv[i+1]);
		}
	}
	
	if (!voice){
		printf("Please provide a custom voice when using this library\n");
		exit(1);
	}
	
	for (int i=1; i<argc; i++){

		if (!strcmp(argv[i],"-o") && i<argc-1){ // add -e option to extract audio file to given path
			
			outputPath=strdup(argv[i+1]);
			argc-=2;
			argv[i]="-v"; 		//replace "-o"  option with -v so that say allows personal voice to play instead of being written to file
			argv[i+1]=voice;	//replace output file path with actual given "-v voice" 
			break;
		}
	}
	
	if (!outputPath){
		printf("Please provide an output file name when using this library ( -o filename.caf ) \n");
		exit(1);
	}
	
	 
}
