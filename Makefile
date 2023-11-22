mysay: mysay.c 
	@echo ""
	gcc -arch arm64e -x objective-c -framework Foundation -framework AudioToolbox -shared mysay.c -o mysay.dylib
	@echo ""
	gcc -x objective-c -arch arm64e -framework Foundation -framework AVFoundation authorize_terminal.c -o authorize_terminal
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
