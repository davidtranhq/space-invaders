#include "test.hpp"
#include <fstream>
#include <array>

namespace superzazu
{
	
uint8_t rd_byte(void *mem, const uint16_t addr)
{
	return static_cast<uint8_t *>(mem)[addr];
}

void wr_byte(void *mem, const uint16_t addr, const uint8_t val)
{
	static_cast<uint8_t *>(mem)[addr] = val;
}

namespace i8080
{
	
Test::Test()
	: cpu1_(), cpu2_ {nullptr}
{
	cpu2_ = new superzazu::i8080;
	superzazu::i8080_init(cpu2_);
	cpu2_->userdata = &memory_;
	cpu2_->read_byte = &superzazu::rd_byte;
	cpu2_->write_byte = &superzazu::wr_byte;
}

size_t Test::load_file(const std::string &path)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		std::vector<uint8_t> buf;
		in.seekg(0, std::ios::end);
		size_t len {in.tellg()};
		buf.resize(len);
		in.seekg(0, std::ios::beg);
		in.read(&buf[0], buf.size());
		in.close();
		
		return len;
	}
	else
		return 0;
}