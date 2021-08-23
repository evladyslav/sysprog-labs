#pragma once
#include "resource.h"
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <vector>
#include <Windows.h>
#include <conio.h>
#include <fstream>
#include <thread>
#include <vector>

using namespace std;

string InttoStr(int i)
{
    std::string s;
    std::stringstream out;
    out << i;
    s = out.str();
    return s;
}

void WriteToFile(int index, string data)
{
    CString filename;
    filename.Format("..\\%d.txt", index);
    std::ofstream f;
    f.open(filename, std::ios::app);
    f << data.c_str() << std::endl;
    f.close();
    std::cout << "\nThread " + InttoStr(index) + " created file " << std::endl;
}

string CharToStr(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}