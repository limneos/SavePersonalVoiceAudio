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


#import <AVFoundation/AVFoundation.h>
#import <objc/runtime.h>
#import <mach-o/dyld.h>
#import <dlfcn.h>

  

int main(int argc, char** argv){
	
	
	[AVSpeechSynthesizer requestPersonalVoiceAuthorizationWithCompletionHandler:^(AVSpeechSynthesisPersonalVoiceAuthorizationStatus status){
		 
		if (status == AVSpeechSynthesisPersonalVoiceAuthorizationStatusAuthorized){
			printf("Authorized!\n");
			NSArray * voices = [AVSpeechSynthesisVoice speechVoices];
			AVSpeechSynthesisVoice *voice=NULL;
			for (AVSpeechSynthesisVoice* avoice in voices){
				if ([avoice voiceTraits] & AVSpeechSynthesisVoiceTraitIsPersonalVoice){
					voice=avoice;
					printf("Personal Voice Found: %s\n",[[voice name] UTF8String]);
				}
			}
			if (!voice){ 
				printf("No Personal Voices found, please add one in Sesttings - Accessibility - Personal Voice.\n");
				exit(1);
			}
			exit(0);
 
		}
		else if (status == AVSpeechSynthesisPersonalVoiceAuthorizationStatusDenied){
			printf("Authorization denied for Personal Voice.\n");
			exit(1);
		}
		else if (status == AVSpeechSynthesisPersonalVoiceAuthorizationStatusNotDetermined){
			printf("Authorization not determined for Personal Voice.\n");
			exit(2);
		}
		else if (status == AVSpeechSynthesisPersonalVoiceAuthorizationStatusUnsupported){
			printf("Personal Voice not supported.\n");
			exit(3);
		}
		else{
			printf("Unknown error.\n");
			exit(4);
		}
		
	}];
	
	[[NSRunLoop currentRunLoop] run];
}