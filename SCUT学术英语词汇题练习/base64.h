#pragma once
#include <iostream>
#include <stdint.h>
using namespace std;
namespace base64 {
	string encode(const string& str);
	string decode(const string& str);
}