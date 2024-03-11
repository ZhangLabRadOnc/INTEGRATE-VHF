/*
 * main.cpp
 *
 *  Created on: Apr 26, 2013
 *      Author: jinzhang
 */

#include <cstring>
#include <ctime>
#include <iostream>
#include "version.h"

using namespace std;

#include "RunCode.h"

map<int, char> intChar;
map<char, char> charChar;
map<string, char> tableAmino;
locale loc2;
const collate<char> &coll2 = use_facet<collate<char>>(loc2);
int HASHSIZE = 10000000;

ostream &operator<<(ostream &out, const ALGraph<int, FusionEdge> &g);

int usage() {
    cout << "          Discover fusions by combining RNA-Seq and WGS data sets*" << endl;
    cout << endl;
    cout << "Usage:    Integrate <subcommand> [options] list of data sets" << endl;
    cout << endl;
    cout << "integrate subcommands include:" << endl;
    cout << endl;
    cout << "          fusion:   call fusions." << endl;
    cout << "          mkbwt:    build BWTs for reference genome. This has to be run one time before running subcommand fusion." << endl;
    cout << endl;
    cout << "*Note:    integrate can run with RNA only data sets." << endl;
    exit(0);
}

int main(int argc, const char *argv[]) {

    cout << PROJECT_NAME << " version " << VERSION_FULL << endl;

    RunCode runcode;

    if (argc < 2) {
        usage();
    }

    if (strcmp(argv[1], "mkbwt") == 0) {
        runcode.runBuildBWTs(argc, argv);
    } else if (strcmp(argv[1], "fusion") == 0) {
        runcode.runFindFusions(argc, argv);
    } else {
        usage();
    }

    return 0;
}
