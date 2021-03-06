// Toy DBMS, v0.4
// George Chernishev (c) 2016-2017, chernishev<at>google mail
// A task framework for undergraduate students at Saint-Petersburg Academic University, DBMS development course
// More details regarding the course can be found here: www.math.spbu.ru/user/chernishev/
// CHANGELOG:
// 0.4: no chance for efficiency competition, so, this year I reoriented task towards ideas:
//      1) tried to remove C code, now we are using lots of std:: stuff
//      2) implemented support for multiple attributes in the DBMS
//      3) code clean-up and restructurization
// 0.3: added:
//      1) support for restricting physical join node size
//      2) support for deduplication node, LUniqueNode
//      3) print methods for Predicate and BaseTable
//      updated:
//      1) new format for data files: third line is the sort status now
//      2) added projection code
//      3) contract contains print methods for physical and logical nodes
// 0.2: first public release

#include <iostream>
#include <fstream>
#include <string>

#include <string.h>
#include <vector>
#include <sstream>
#include <tuple>

#include "pselectnode.h"

PSelectNode::PSelectNode(LAbstractNode* p, std::vector<Predicate> predicate): PGetNextNode(){
  this->table = ((LSelectNode*)p)->GetBaseTable();
  this->predicate = predicate;
  this->prototype = p;
  pos = 0;
  data.clear();
  Initialize();
}

PSelectNode::~PSelectNode(){
}

void PSelectNode::Initialize(){
  int val = 0;
  std::string line = "", word = "";
  std::ifstream f(table.relpath);

  if(f.is_open()){
    // skipping first 4 lines
    getline(f, line);
    getline(f, line);
    getline(f, line);
    getline(f, line);

    while(getline(f, line)){
    	LSelectNode * selnode =  dynamic_cast<LSelectNode *>(prototype);
    	std::vector<Value> tmp;
      std::istringstream iss(line, std::istringstream::in);
      bool skip_row = false;
      for (int i = 0; (iss >> word); i++) {
        // Yeah, no predicates :) -- Homework
      	Value h;
        if (prototype->fieldTypes[i] == VT_INT)
          h = Value(std::stoi(word));
        else
          h = Value(word);

        for (auto &pr : predicate) {
					if (pr (i, h) == EC_ERROR) {
							skip_row = true;
							break;
					}
				}

        tmp.push_back(h);

        if (skip_row)
        	break;
      }
      if (!skip_row)
      	data.push_back(tmp);
    }
    f.close();
  } else std::cout << "Unable to open file";
}

std::vector<std::vector<Value>> PSelectNode::GetNext(){
  return data;
}

void PSelectNode::Print(int indent){
  for (int i = 0; i < indent; i++){
    std::cout<<" ";
  }
  std::cout<<"SCAN "<<table.relpath<<" with predicates ";
  for (auto & pr : predicate) {
    std::cout<< "[" << pr << "] ";
  }
  std::cout << std::endl;
  if(left != NULL) left->Print(indent + 2);
  if(right != NULL) right->Print(indent + 2);
}

