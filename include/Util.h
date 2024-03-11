/*
 * Util.h
 *
 *  Created on: Apr 28, 2013
 *      Author: jinzhang
 */

#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <stdint.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <typeinfo>
#include <vector>

using namespace std;

uint32_t getFilelength(const char *file);
int readBlock(char *block, int length, FILE *infile);

extern map<int, char> intChar;
extern map<char, char> charChar;

extern map<string, char> tableAmino;

int InitialIntChar();

char getCharComp(char reada);
char getCharA(int reada);

int getPeptide(vector<char> &seq5p, vector<char> &seq, int start_pos, vector<char> &peptide, int &full, int &left);
