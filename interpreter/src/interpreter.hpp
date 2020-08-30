#pragma once

#include <cstdint>

namespace chip8
{
	/* https://en.wikipedia.org/wiki/CHIP-8 */
	const int16_t MEMORY_SPACE = 4096;
	const int8_t MEMORY_REGISTERS = 16;
	const int8_t STACK_LEVELS = 16;

	const int8_t VIDEO_WIDTH = 64;
	const int8_t VIDEO_HEIGHT = 32;
	
	class interpreter
	{
	public:
		interpreter();
		~interpreter();

		void load_file(const char*);
		void exec_ins();
		
		uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT];
	
	private:
		/* Memory variables */
		uint8_t memory[MEMORY_SPACE]; // 1 byte * MEMORY_SPACE
		uint8_t registers[MEMORY_REGISTERS]; // 1 byte * MEMORY_REGISTERS
		uint16_t index; // 2 bytes
		uint16_t program_counter; // 2 bytes
		uint16_t instruction; // 2 bytes
		uint16_t stack[STACK_LEVELS]; // 2 bytes * STACK_LEVELS
		uint16_t stack_pointer; // 2 bytes

		/* Instruction identifier */
		void fnd_0000();
		void fnd_8000();

		/* Instruction functions */
		void ins_NOP();  // default nop
		void ins_00E0(); // clear screen
		void ins_00EE(); // return
		void ins_1NNN(); // jmp
		void ins_2NNN(); // call
		void ins_3XNN(); // if
		void ins_4XNN(); // if not
		void ins_5XY0(); // if
		void ins_6XNN(); // set
		void ins_7XNN(); // add
		void ins_8XY0(); // set
		void ins_8XY1(); // set
		void ins_8XY2(); // set
		void ins_8XY3(); // set
		void ins_8XY4(); // add
		void ins_8XY5(); // sub
		void ins_ANNN(); // mem
		void ins_BNNN(); // jmp
		void ins_CXNN(); // rand
		void ins_DXYN(); // draw

		/* Instruction function pointer tables */
		typedef void (interpreter::*function_table)();
		function_table table[0xF + 1];
		function_table table_0000[0xE + 1];
		function_table table_8000[0xE + 1];
	};
}
