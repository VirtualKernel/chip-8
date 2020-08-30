#include "interpreter.hpp"
#include "video.hpp"

int main()
{
	video graphics("chip8", chip8::VIDEO_WIDTH * 20, chip8::VIDEO_HEIGHT * 20, chip8::VIDEO_WIDTH, chip8::VIDEO_HEIGHT);

	chip8::interpreter interpreter;
	
	//interpreter.load_file("roms/BC_test.ch8");
	interpreter.load_file("roms/maze.rom");

	int videoPitch = sizeof(interpreter.video[0]) * chip8::VIDEO_WIDTH;
	
	bool stop = false;

	while (!stop)
	{
		interpreter.exec_ins();
		graphics.update(interpreter.video, videoPitch);
	}
}

