mysay: mysay.c 
	@echo ""
#	gcc -arch arm64e -x objective-c -framework Foundation -framework AudioToolbox -shared mysay.c -o mysay.dylib
	gcc -arch arm64e -x objective-c -framework Foundation -framework AudioToolbox -shared mysay.c -o mysay-arm64e.dylib
	gcc -arch x86_64 -x objective-c -framework Foundation -framework AudioToolbox -shared mysay.c -o mysay-x86_64.dylib
	lipo -create mysay-arm64e.dylib mysay-x86_64.dylib -o mysay.dylib
	rm -f mysay-arm64e.dylib
	rm -f mysay-x86_64.dylib
	@echo ""
#	gcc -x objective-c -arch arm64e -framework Foundation -framework AVFoundation authorize_terminal.c -o authorize_terminal
	gcc -x objective-c -arch arm64e -framework Foundation -framework AVFoundation authorize_terminal.c -o authorize_terminal_arm64e
	gcc -x objective-c -arch x86_64 -framework Foundation -framework AVFoundation authorize_terminal.c -o authorize_terminal_x86_64
	lipo -create authorize_terminal_arm64e authorize_terminal_x86_64 -o authorize_terminal
	rm -f authorize_terminal_arm64e
	rm -f authorize_terminal_x86_64
	@echo ""
	@echo "\x1b[32mAll Done!"
	@echo ""
	@echo "Usage:"
	@echo ""
	@echo "First authorize terminal to use Personal voices by running ./authorize_terminal"
	@echo ""
	@echo "Then, use the following command to use say with personal voice and write output to file"
	@echo ""
	@echo "DYLD_INSERT_LIBRARIES=./mysay.dylib say -v MyPersonalVoiceName \"text to speak\" -e <path to file>.caf\x1b[m"
clean:
	rm -f mysay.dylib 
	rm -f authorize_terminal
