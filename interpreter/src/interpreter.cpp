#include "interpreter.hpp"

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <stdlib.h>

#define log(string) std::cout << "[interpreter][" << __FUNCTION__ << "] " << string << std::endl;

#define merge(b1, b2) (b1 << 8 | b2)
#define extract(b1,msk) (b1 & msk)

namespace chip8
{
	interpreter::interpreter()
	{
		log("constructing interpreter");
		
		this->program_counter = 0x200;

		std::fill_n(this->table, 0xF + 1, &interpreter::ins_NOP);
		std::fill_n(this->table_0000, 0xE + 1, &interpreter::ins_NOP);
		std::fill_n(this->table_8000, 0xE + 1, &interpreter::ins_NOP);

		/* Find */
		this->table[0x0] = &interpreter::fnd_0000;
		this->table[0x8] = &interpreter::fnd_8000;
		
		/* Immediate */
		this->table[0x1] = &interpreter::ins_1NNN;
		this->table[0x2] = &interpreter::ins_2NNN;
		this->table[0x3] = &interpreter::ins_3XNN;
		this->table[0x4] = &interpreter::ins_4XNN;
		this->table[0x5] = &interpreter::ins_5XY0;
		this->table[0x6] = &interpreter::ins_6XNN;
		this->table[0x7] = &interpreter::ins_7XNN;
		this->table[0x9] = &interpreter::ins_9XY0;
		this->table[0xA] = &interpreter::ins_ANNN;
		this->table[0xB] = &interpreter::ins_BNNN;
		this->table[0xC] = &interpreter::ins_CXNN;
		this->table[0xD] = &interpreter::ins_DXYN;

		/* 0000 */
		this->table_0000[0x0] = &interpreter::ins_00E0;
		this->table_0000[0xE] = &interpreter::ins_00EE;

		/* 8000 */
		this->table_8000[0x0] = &interpreter::ins_8XY0;
		this->table_8000[0x1] = &interpreter::ins_8XY1;
		this->table_8000[0x2] = &interpreter::ins_8XY2;
		this->table_8000[0x3] = &interpreter::ins_8XY3;
		this->table_8000[0x4] = &interpreter::ins_8XY4;
		this->table_8000[0x5] = &interpreter::ins_8XY5;

		this->ins_00E0(); // Clean video data before we start executing.
	}

	interpreter::~interpreter() { log("deconstructing interpreter") }

	void interpreter::load_file(const char* file_name)
	{
		log("loading file");

		std::ifstream file(file_name, std::ios::binary | std::ios::ate);

		if (file.is_open())
		{
			std::streampos file_size = file.tellg();
			char* data = new char[file_size];
			file.seekg(0, std::ios::beg);
			file.read(data, file_size);
			file.close();

			for (long i = 0; i < file_size; ++i)
			{
				this->memory[0x200 + i] = data[i];
			}

			delete[] data;
		}
	}

	void interpreter::exec_ins()
	{
		this->instruction = merge(memory[program_counter], memory[program_counter + 1]);
		this->program_counter += 2;
		/* TODO
			Make the function table look nice.
		*/
		((*this).*this->table[extract(this->instruction, 0xF000) >> 12])();
	}

	void interpreter::fnd_0000()
	{
		log("0x00E? called, looking up last digit.");
		((*this).*(this->table_0000[extract(this->instruction, 0x000F)]))();
	}

	void interpreter::fnd_8000()
	{
		log("0x8XY? called, looking up last digit.");
		((*this).*(this->table_8000[extract(this->instruction, 0x000F)]))();	
	}

	void interpreter::ins_NOP()
	{	
		log("nop");
		std::cout << std::hex << this->instruction << std::endl;
	}

	void interpreter::ins_00E0()
	{	
		log("clear 00E0");
		/* disp_clear() */
		memset(this->video, 0, sizeof(this->video));
	}

	void interpreter::ins_00EE()
	{
		log("ret 00EE");
		/* return; */
		--this->stack_pointer;
		this->program_counter = this->stack[this->stack_pointer];
	}

	void interpreter::ins_1NNN()
	{
		log("jmp 1NNN");
		/* goto NNN; */
		uint16_t NNN = extract(this->instruction, 0x0FFF);
		this->program_counter = NNN;
	}

	void interpreter::ins_2NNN()
	{
		log("call 2NNN");
		/* *(0xNNN)() */
		uint16_t NNN = extract(this->instruction, 0x0FFF);
		this->stack[this->stack_pointer] = this->program_counter;
		++this->stack_pointer;
		this->program_counter = NNN;
	}

	void interpreter::ins_3XNN()
	{
		log("if 3XNN");
		/* if(Vx==NN) */
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t NN = extract(this->instruction, 0x00FF);
		if (this->registers[Vx] == NN)
			this->program_counter += 2;
	}
	
	void interpreter::ins_4XNN()
	{
		log("if not 4XNN");
		/* if(Vx!=NN) */	
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t NN = extract(this->instruction, 0x00FF);
		if (this->registers[Vx] != NN)
			this->program_counter += 2;
	}
	
	void interpreter::ins_5XY0()
	{
		log("if 5XY0");
		/* if(Vx==Vy) */
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t Vy = extract(this->instruction, 0x00F0) >> 4;
		if (this->registers[Vx] == this->registers[Vy])
			this->program_counter += 2;
	}

	void interpreter::ins_6XNN()
	{
		log("set 6XNN");
		/* Vx = NN */
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t NN = extract(this->instruction, 0x00FF);
		this->registers[Vx] = NN;
	}

	void interpreter::ins_7XNN()
	{
		log("add 7XNN");
		/* Vx += NN */
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t NN = extract(this->instruction, 0x00FF);
		this->registers[Vx] += NN;
	}

	void interpreter::ins_8XY0()
	{
		log("set 8XY0");
		/* Vx=Vy */
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t Vy = extract(this->instruction, 0x00F0) >> 4;
		this->registers[Vx] = this->registers[Vy];
	}

	void interpreter::ins_8XY1()
	{
		log("set 8XY1");
		/* Vx=Vx|Vy */
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t Vy = extract(this->instruction, 0x00F0) >> 4;
		this->registers[Vx] |= this->registers[Vy];
	}

	void interpreter::ins_8XY2()
	{
		log("set 8XY2");
		/* Vx=Vx&Vy */
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t Vy = extract(this->instruction, 0x00F0) >> 4;
		this->registers[Vx] &= this->registers[Vy];
	}

	void interpreter::ins_8XY3()
	{
		log("set 8XY3");
		/* Vx=Vx^Vy */
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t Vy = extract(this->instruction, 0x00F0) >> 4;
		this->registers[Vx] ^= this->registers[Vy];
	}

	void interpreter::ins_8XY4()
	{
		log("add 8XY4");
		/* Vx += Vy */
		/* Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't. */
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t Vy = extract(this->instruction, 0x00F0) >> 4;
		uint16_t V = (this->registers[Vx] + this->registers[Vy]);
		
		if (V > 255)
			this->registers[0xF] = 1;
		else
			this->registers[0xF] = 0;

		this->registers[Vx] = extract(V, 0x00FF);
	}

	void interpreter::ins_8XY5()
	{
		log("sub 8XY5");
		/* Vx -= Vy */
		/* VY is subtracted from VX. VF is set to 0 when there's a borrow */
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t Vy = extract(this->instruction, 0x00F0) >> 4;
		
		if (this->registers[Vx] > this->registers[Vy])
			this->registers[0xF] = 1;
		else
			this->registers[0xF] = 0;

		this->registers[Vx] -= this->registers[Vy];
	}

	void interpreter::ins_9XY0()
	{
		log("if 9XY0");
		/* if(Vx!=Vy) */
		/* Skips the next instruction if VX doesn't equal VY. */
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t Vy = extract(this->instruction, 0x00F0) >> 4;

		if (this->registers[Vx] != this->registers[Vy])
			this->program_counter += 2;

	}

	void interpreter::ins_ANNN()
	{
		log("mem ANNN");
		/* I = NNN */
		uint16_t NNN = extract(this->instruction, 0x0FFF);
		this->index = NNN;
	}

	void interpreter::ins_BNNN()
	{
		log("jmp BNNN");
		/* PC=V0+NNN */
		uint16_t NNN = extract(this->instruction, 0x0FFF);
		this->program_counter = this->registers[0] + NNN;
	}

	void interpreter::ins_CXNN()
	{
		log("rand CXNN");
		/* Vx=rand()&NN */
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t NN = extract(this->instruction, 0x00FF);
		this->registers[Vx] = rand() & NN; // TODO use a random seed.
	}
	
	void interpreter::ins_DXYN()
	{
		/* implementation stolen from some documentation, ty */
		
		log("draw DXYN");
		uint8_t Vx = extract(this->instruction, 0x0F00) >> 8;
		uint8_t Vy = extract(this->instruction, 0x00F0) >> 4;
		uint8_t height = extract(this->instruction, 0x000F);

		// Wrap if going beyond screen boundaries
		uint8_t xPos = this->registers[Vx] % VIDEO_WIDTH;
		uint8_t yPos = this->registers[Vy] % VIDEO_HEIGHT;

		this->registers[0xF] = 0;

		for (unsigned int row = 0; row < height; ++row)
		{
			uint8_t spriteByte = this->memory[index + row];

			for (unsigned int col = 0; col < 8; ++col)
			{
				uint8_t spritePixel = spriteByte & (0x80 >> col);
				uint32_t* screenPixel = &this->video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

				// Sprite pixel is on
				if (spritePixel)
				{
					// Screen pixel also on - collision
					if (*screenPixel == 0xFFFFFFFF)
					{
						this->registers[0xF] = 1;
					}

					// Effectively XOR with the sprite pixel
					*screenPixel ^= 0xFFFFFFFF;
				}
			}
		}
	}
}
