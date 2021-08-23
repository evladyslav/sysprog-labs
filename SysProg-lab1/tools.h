#pragma once
#include <sstream>
std::string strtr(int i) {
	std::string s;
	std::stringstream out;
	out << i;
	s = out.str();
	return s;
}