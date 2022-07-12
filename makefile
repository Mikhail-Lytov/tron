all:
	@echo "	Lytov Mikhail Nikolaevich\n	Game of TRON"
	@echo "	make com - for compilation tron.c\n	make clear\n	make allclear"
com:
	@gcc tron.c -lncurses -lm -o tron
	@clear
clear:
	@rm tron
allclear:
	@rm tron tron.с

