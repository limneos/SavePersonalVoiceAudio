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
			printf("error creating file %u\n",status);
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


void func(const struct mach_header *mh, intptr_t slide){
	 
	static int hasHooked=0;
	if (outputPath && !hasHooked && dlsym(RTLD_DEFAULT,"AudioUnitRender")){
		hasHooked=1;
		DYLD_INTERPOSE(replaced_AudioUnitRender,AudioUnitRender);
	}
	
}	

__attribute__((constructor)) void constructor (int argc, char **argv){

	 
	
	for (int i=1; i<argc; i++){

		if (!strcmp(argv[i],"-e") && i<argc-1){ // add -e option to extract audio file to given path
			
			outputPath=strdup(argv[i+1]);
			argc-=2;
			argv[i]=""; 	//remove extra option "-e" so that say doesn't complain about its command line syntax
			argv[i+1]=""; 	//remove extra option "filepath" so that say doesn't complain about its command line syntax
			printf("Writing audio file at path: %s\n",outputPath);
			 
			break;
		}
	}
	
	_dyld_register_func_for_add_image(func);
	
	 
}
