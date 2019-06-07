EXE = emulator.exe

space_invaders:
	$(MAKE) -C src
	move src\$(EXE)
 