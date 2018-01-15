#include <iostream>
#include <fstream>

#include <map>

using namespace std;
#include "binary.tab.hh"
extern unsigned int total;
extern Node* root;
extern FILE * yyin;
void yy::parser::error(std::string const&err)
{
  //std::cout << "It's one of the bad ones... " << err << std::endl;
	std::cout << err << std::endl;
}

map<size_t, BBlock*> GetBlocks(BBlock* start)
{
	map<size_t, BBlock *> done, todo;
	todo[start->_id] = start;
	while(todo.size()>0)
	{
		// Pop an arbitrary element from todo set
		auto first = todo.begin();
		BBlock *next = first->second;
		todo.erase(next->_id);

		
		// next->write(file);
		done[next->_id] = next;
		if(next->_trueExit && done.find(next->_trueExit->_id)==done.end())
			todo[next->_trueExit->_id]= next->_trueExit;
		if(next->_falseExit && done.find(next->_falseExit->_id)==done.end())
			todo[next->_falseExit->_id]=next->_falseExit;
		if(next->_function && done.find(next->_function->_id)==done.end())
		{
			todo[next->_function->_id]=next->_function;
		}
	}
	return done;
}
void dumpCFG(BBlock *start)
{
 // ofstream file;
 // file.open("comp.cc",ios::out | ios::ate | ios::trunc);
 // file << "#include<iostream>" << endl;
 // file << "using namespace std;" << endl;
 // file << "int main()\n{ \n";

//  set<string> vars;
 // start->getVars(vars);
 // file << "long _lol_var_";
 // for(auto& v: vars)
 //   file << ", " << v;
 // file << ";" << endl;

  	map<size_t, BBlock *> done = GetBlocks(start);
  	if(done[0]->_trueExit)
  	{
		for(auto& b : done)
			b.second->Dump();
	}
	else
	{
		done.erase(0);
		for(auto& b : done)
			b.second->Dump();
		start->Dump();
	}
 // file << "cout << x << endl;\nreturn 0;\n}"<< endl;
 // file.close();
}



string GenTree(BBlock* start)
{
	stringstream out;
	out << "digraph g {" << endl;
	out << "graph [" << endl;
	out << "rankdir = \"LR\"" << endl;
	out << "];" << endl;
	out << "node [" << endl;
	out << "fontsize = \"16\"" << endl;
	out << "shape = \"ellipse\"" << endl;
	out << "];" << endl;
	out << "edge [" << endl;
	out << "];" << endl;

	const map<size_t, BBlock *>& done = GetBlocks(start);

	for(auto b : done)
		out << b.second->GenTree();

	out << "}";
	return out.str();
}

string GenAsm(BBlock* start)
{
	stringstream out;
	stringstream allocs;

	map<size_t, BBlock *> done = GetBlocks(start);
	allocs << "\t\t" << ".data" << endl;
	set<string> vars;
	for(auto b : done)
		allocs << b.second->GetVars(vars);

	out << ".section	.rodata" << endl;
	out << ".text" << endl;
	if(done[0]->_trueExit)
  	{
		out << ".globl main" << endl;
  		out << "main: " << endl;

		for(auto& b : done)
			b.second->GenAsm(out, allocs, vars);
	}
	else
	{
		done.erase(0);
		for(auto& b : done)
			b.second->GenAsm(out, allocs, vars);

		out << ".globl main" << endl;
		out << "main: " << endl;

		start->GenAsm(out, allocs, vars);
	}
	allocs << endl;
	return allocs.str() + out.str();
}

int main(int argc, char **argv)
{
	if(argc > 1)
	{
		yyin = nullptr;
		cout << "Args > 1 Name: " << argv[1] << endl;
		yyin = fopen(argv[1], "r");
		if(!yyin)
		{
			cout << "Failed to open file." << endl;
			return -1;
		}

		cout << "Compiling..." << endl << endl;
		yy::parser parser;


		if(!parser.parse())  
		{

			cout << "Compiled!!" << endl << endl;

			BBlock *start = new BBlock(0);
  			BBlock *current = start;

  			Function* prints = new Print();
  			prints->ConvertStatement(&current);
  			Function* printn = new PrintN();
  			printn->ConvertStatement(&current);
  			Function* read = new Read();
  			read->ConvertStatement(&current);
  			delete read;
  			delete prints;
  			delete printn;

			root->Convert(&current);

			dumpCFG(start);
			
			std::string out = GenTree(start);
			ofstream f;
			f.open("file.dot");
			f << out;
			f.close();

			system("dot -Tpdf file.dot -otree.pdf");
			
			
			out = GenAsm(start);

			f.open("target.s");
			f << out;
			f.close();


			cout << "Show the ThreeAd code Tree?(y/n): ";
			char in;
			cin >> in;
			if(in == 'y')
			{
				system("evince tree.pdf");

			}
			delete start;

			cout << endl << endl << endl << "###OBS! This compiler outputs a .s assembly file not .cc###" << endl << endl << endl;

		}
	}

	delete root;
	return 0;
}