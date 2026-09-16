# Welcome to the PS3Sweeper+!
*This game is based of the [Minesweeper Plus](https://jorel-simpson.itch.io/minesweeper-plus) game made by Jorel Simpson. Please, play the original game to support the original author of the Minesweeper Plus.*\
Isn't it the most awesome minesweeper game to play on YOUR PS3? Well, I hope so! With multiple chapters, multiple routes, and much more upon awaits you in this simple homebrew, made by one person! Currently, this version only contains two chapters (third in works) and the main route among many, but it's at least better, than nothing, right?

Don't like the adventure? You can always count on free play! Customize your most finest game of minesweeper. It may be relaxing, it may be HARDCORE. Free choice, Free play!

Please be aware, that on RPCS3 or aPS3e, the game is not optimized well and may halt any second! On aPS3e it halts almost immediately. RPCS3 can handle the game, but freeze may occur (rarely). The game was optimized to work with actual PS3 consoles, and there is nothing I can do about it.

NOTE: The Source code should be available RIGHT AFTER 1 month after the INITIAL release has been made. Thanks for understanding.

# How to compile?

You need a machine running Linux. WSL is supported.

1. Get [PS3DEV](https://github.com/ps3dev/ps3dev) and it's dependencies.
2. Copy `/usr/local/ps3dev/portlibs/ppu/include/freetype2/freetype` folder to `/usr/local/ps3dev/portlibs/ppu/include` cuz idk why they don't fix it.
3. Compile the game using `make pkg` command.
4. Install PKG in your PS3 system.

# Additional Notes
* The game's code is partially AI-Assisted.
* The game is not finished yet.

# Special Tools

1. Board Converter!
	- This python script converts your `board.png` into `board.txt`, so you can import it in the game!
2. More tools soon!

<a href="https://github.com/AcidPS3-Project">
	<img src="https://img.shields.io/badge/Powered%20By-AcidPS3%20Project-blue?logo=playstation3" />
</a>