#include "convert.hh"

std::vector<string> strings;
Type::Type()
{
	type = Types::Nil;
}

Type::Type(const Type& other)
{
	this->haxxor = other.haxxor;
	this->haxxor2 = other.haxxor2;
	this->type = other.type;
	memcpy(&this->data, &other.data, sizeof(other.data));
}

Type& Type::operator=(const Type& other)
{
	this->haxxor = other.haxxor;
	this->haxxor2 = other.haxxor2;
	this->type = other.type;
	memcpy(&this->data, &other.data, sizeof(other.data));
}
Type& Type::operator=(const long& other)
{
	type = Types::Integer;
	data.Integer = other;
}
Type& Type::operator=(const float& other)
{
	type = Types::Float;
	data.Float = other;
}
Type& Type::operator=(const bool& other)
{
	type = Types::Boolean;
	data.Boolean = other;
}
Type& Type::operator=(const string& other)
{
	type = Types::String;
	strings.push_back(other);
	data.String = strings.size() - 1;
}

Type::Type(const string& s, bool var)
{
	if(var)
		type = Types::Variable;
	else
		type = Types::String;
	strings.push_back(s);
	data.String = strings.size() - 1;
}
Type::Type(long i)
{
	type = Types::Integer;
	data.Integer = i;
}
Type::Type(float f)
{
	type = Types::Float;
	data.Float = f;
}
Type::Type(bool b)
{
	type = Types::Boolean;
	data.Boolean = b;
}

Type::~Type()
{

}
string Type::Print()
{
	string out = "";
	if(type & Types::Variable)
	{
		out =  strings[data.String];
	}
	else if(type & Types::Table)
	{
		out = "#"+ strings[data.String];
	}
	else if(type & Types::Integer)
	{
		out =  to_string(data.Integer);
	}
	else if(type & Types::Float)
	{
		out =  to_string(data.Float);
	}
	else if(type & Types::FieldSize)
	{
		out = to_string(data.Integer);
	}
	else if(type & Types::Boolean)
	{
		out =  data.Boolean? "true" : "false";
	}
	else if(type & Types::String)
	{
		out =  strings[data.String];
	}
	else if(type & Types::FunctionCall)
	{
		out = strings[data.String];
	}
	else if(type & Types::Function)
	{
		out = "Func"+to_string(data.Integer);
	}
	else if(type & Types::FunctionPointer)
	{
		out = strings[data.String];
	}
	else if(type & Types::Asm)
	{
		out = strings[data.String];
	}

    
    return out;
}
string Type::Asm()
{
	stringstream out;

	if(type & Types::Asm)
	{
		out << strings[data.String];
	}
	else if(type & Types::Pointer)
	{
		out << strings[data.String] << "(%rip)";

	}
	else if(type & Types::Variable)
	{
		if(type & Types::String)
			out << "$";
		
		out << strings[data.String];

	}
	else if(type & Types::Table)
	{
		out << "$" << strings[data.String];
	}
	else if(type & Types::Integer)
	{
		out << "$" << data.Integer;
	}
	else if(type & Types::Function)
	{
		out << "$Func" << data.Integer;
	}
	else if(type & Types::Boolean)
	{
		out << "$" << (data.Boolean? 1 : 0);
	}
	else if(type & Types::FunctionCall)
	{
		out << strings[data.String];
	}
	
	else if(type & Types::String)
	{
		out << "\"" << strings[data.String] << "\"";
	}

	return out.str();
}
string Type::Move(const string& reg)
{
	stringstream out;
	out << "\t\t";
	if(type & Types::Pointer)
	{
		out << "movq " << Asm() << ", " << reg;
	}
	else
	{
		out << "movq " << Asm() << ", " << reg;
	}

	return out.str();
}

string Type::MoveTo(const string& reg)
{
	stringstream out;
	out << "\t\t";
	if(type & Types::Pointer)
	{
		out << "movq " << reg << ", " << Asm();
	}
	else
	{
		out << "movq " << reg << ", " << Asm();
	}

	return out.str();
}

string Type::AsmI()const
{
	return "";
}
void Type::SetAsPointer()
{
	type |= Types::Pointer;
}
std::ostream& operator<<(std::ostream& out, Type& s)
{
  return out << s.Print();
}

std::ostream& operator<<(std::ostream& out, const Operators& s)
{
	switch(s)
	{
		case Operators::Add:
		return out << "+";
		case Operators::Minus:
		return out << "-";
		case Operators::Mult:
		return out << "*";
		case Operators::Divide:
		return out << "/";
		case Operators::Equal:
		return out << "==";
		case Operators::Mod:
		return out << "%";
		case Operators::Less:
		return out << "\\<";
		case Operators::LessEqual:
		return out << "\\<=";
		case Operators::Greater:
		return out << "\\>";
		case Operators::GreaterEqual:
		return out << "\\>=";
		case Operators::NotEqual:
		return out << "!=";
		case Operators::And:
		return out << "&";
		case Operators::Or:
		return out << "|";
		case Operators::DBDot:
		return out << "..";
		case Operators::Pow:
		return out << "^";
		case Operators::Hash:
		return out << "#";
		case Operators::Not:
		return out << "!";
		case Operators::Negative:
		return out << "neg";
		case Operators::Copy:
		return out << "c";
		case Operators::FunctionCall:
		return out << "call";
		case Operators::Push:
		return out << "push";
		case Operators::Pop:
		return out << "pop";
		case Operators::Resolve:
		return out << "r";
		case Operators::Alloc:
		return out << "new";	
		case Operators::Return:
		return out << "ret";	
		case Operators::AllocStr:
		return out << "ret";
		case Operators::SaveVars:
		return out << "Backup";	
		case Operators::RestoreVars:
		return out << "Restore";		
	}
}
ThreeAd::ThreeAd(const Type& out, Operators o, const Type& l, const Type& r) : _result(out), _op(o), _lhs(l), _rhs(r)
{
}
string ThreeAd::GetFormat()
{
	 stringstream out;
	 out << _result << " := " << _lhs << " " << _op << " " << _rhs;
	 return out.str();
}
void ThreeAd::Dump()
{
	 cout << _result << " := " << _lhs << " " << _op << " " << _rhs << endl;
}

string ThreeAd::GetVars(set<string>& vars)
{
	if(_result.type & (Types::String | Types::Asm))
		return "";
	if(_result.type & Types::Table && !(_result.type & Types::Variable))
		return "";




	if(_result.type & (Types::Table | Types::Pointer))
	{
		string p = _result.Print();
		if(vars.find(p) == vars.end() && vars.find("#"+p) == vars.end())
		{
			vars.insert("#" + p);
			stringstream out;
			out << "\t\t" << ".globl " << p << endl;
			out << "\t\t" << ".align 8" << endl;
			out << "\t\t" << ".type " << p << ", @object" << endl;
			out << "\t\t" << ".size " << p << ", 8" << endl;

			out << p << ": " << endl << "\t\t"  << ".zero 8";
			return out.str();
		}
		else
			return "";
	}

	if(_result.type & (Types::Variable | Types::Function))
	{
		string p = _result.Print();
		if(vars.find(p) == vars.end() && vars.find("#"+p) == vars.end())
		{
			if(_result.type & Types::Function)
				vars.insert("#"+p);
			else
				vars.insert(p);
			stringstream out;
			out << "\t\t" << ".globl " << p << endl;
			out << "\t\t" << ".align 8" << endl;
			out << "\t\t" << ".type " << p << ", @object" << endl;
			out << "\t\t" << ".size " << p << ", 8" << endl;

			out << p << ": " << endl << "\t\t"  << ".quad 0";
			return out.str();
		}
		else
			return "";
	}

	return "";
}
void ThreeAd::GenAsm(BBlock* parent, stringstream& out, stringstream& allocs, set<string>& vars)
{
	switch(_op)
	{
		case Operators::Add:
		{
			out << _lhs.Move("%rax") << endl;
			out << _rhs.Move("%rbx") << endl;
			out << "\t\t" << "addq %rbx, %rax" << endl;
			out << _result.MoveTo("%rax") << endl;
			break;
		}
		case Operators::Minus:
		{
			out << _lhs.Move("%rax") << endl;
			out << _rhs.Move("%rbx") << endl;
			out << "\t\t" << "subq %rbx, %rax" << endl;
			out << _result.MoveTo("%rax") << endl;
			break;
		}
		case Operators::Mult:
		{
			out << _lhs.Move("%rax") << endl;
			out << _rhs.Move("%rbx") << endl;
			out << "\t\t" << "mulq %rbx" << endl;
			out << _result.MoveTo("%rax") << endl;
			break;
		}
		case Operators::Divide:
		{
			out << _lhs.Move("%rax") << endl;
			out << _rhs.Move("%rbx") << endl;
			out << "\t\t" << "movq $0, %rdx" << endl;
			out << "\t\t" << "divq %rbx" << endl;
			out << _result.MoveTo("%rax") << endl;
			break;
		}
		case Operators::Mod:
		{
			out << _lhs.Move("%rax") << endl;
			out << _rhs.Move("%rbx") << endl;
			out << "\t\t" << "movq $0, %rdx" << endl;
			out << "\t\t" << "divq %rbx" << endl;
			out << _result.MoveTo("%rdx") << endl;
			break;
		}
		case Operators::Copy:
		{
			if(_result.type & Types::Asm)
			{
				out << _lhs.Move(_result.Asm()) << endl;
			}
			else if(_lhs.type & Types::PrefixVariable)
			{
				if(_result.type & Types::PrefixVariable)
				{
					out << _lhs.Move("%rax") << endl;
					out << "\t\t" << "movq (%rax), %rax" << endl;
					out << _result.Move("%rbx") << endl;
					out << "\t\t" << "movq %rax, (%rbx)" << endl;
				}
				else
				{
					out << _lhs.Move("%rax") << endl;
					out << "\t\t" << "movq (%rax), %rax" << endl;
					out << _result.MoveTo("%rax") << endl;
				}
			}
			else if (_result.type & Types::PrefixVariable)
			{
					out << _lhs.Move("%rax") << endl;
					out << _result.Move("%rbx") << endl;
					out << "\t\t" << "movq %rax, (%rbx)" << endl;
			}
			else if(_result.type & Types::Pointer)
			{
				out << _result.Move("%rax") << endl;
				out << _lhs.Move("%rbx") << endl;
				out << "\t\t" << "movq %rbx, (%rax)" << endl;
			}
			else if(_result.type & Types::Table)
			{
				out << _result.MoveTo(_lhs.Asm()) << "(%rip)" << endl;
			}
			else if(_result.type & Types::String)
			{
				out << _lhs.Move("%rax") << endl;
				out << "\t\t" << "movq %rax, " << _result.Print() << endl;
			}
			else
			{
				out << _lhs.Move("%rax") << endl;

				out << _result.MoveTo("%rax") << endl;
			}
			break;
		}
		case Operators::FunctionCall:		
		{

			if(_lhs.type & Types::Asm)
			{
				out << "\t\t" << "call " << _lhs.Asm() << endl;
			}
			else
			{
				out << _lhs.Move("%rax") << endl;
				if(_lhs.type & Types::PrefixVariable)
				{
					out << "\t\t" << "movq (%rax), %rax" << endl;
				}
				out << "\t\t" << "call " << "*%rax" << endl;
				out << _result.MoveTo("%rax") << endl;
			}
			break;
		}
		case Operators::Pop:		
		{
			out << "\t\tpopq %r10" << endl;
			if(_result.type & Types::PrefixVariable)
			{
				out << _result.Move("%r11") << endl;
				out << "\t\t" << "movq %r10, (%r11)" << endl;
			}
			else
				out << _result.MoveTo("%r10") << endl;
			break;
		}
		case Operators::Push:		
		{
			out << _lhs.Move("%r10") << endl;
			if(_lhs.type & Types::PrefixVariable)
			{
				out << "\t\t" << "movq (%r10), %r10" << endl;
			}
				
				out << "\t\t" << "pushq %r10" << endl;
			
			break;
		}
		case Operators::Return:		
		{
			out << "\t\tret" << endl;
			break;
		}
		case Operators::AllocStr:		
		{
			allocs << _result.Print() << ": .string " << _lhs.Asm() << endl;
			break;
		}
		case Operators::Equal:		
		{
			out << _lhs.Move("%rax") << endl;
			out << _rhs.Move("%rbx") << endl;
			if(_lhs.type & Types::PrefixVariable)
				out << "\t\t" << "movq (%rax), %rax" << endl;
			if(_rhs.type & Types::PrefixVariable)
				out << "\t\t" << "movq (%rbx), %rbx" << endl;
			out << "\t\t" << "cmpq %rbx, %rax" << endl;
			if(parent->_falseExit)
			{
				out << "\t\t" << "je Block" << parent->_trueExit->_id << endl;
				out << "\t\t" << "jmp Block" << parent->_falseExit->_id << endl;
			
			}

			break;
		}
		case Operators::Less:		
		{
			out << _lhs.Move("%rax") << endl;
			out << _rhs.Move("%rbx") << endl;
			if(_lhs.type & Types::PrefixVariable)
				out << "\t\t" << "movq (%rax), %rax" << endl;
			if(_rhs.type & Types::PrefixVariable)
				out << "\t\t" << "movq (%rbx), %rbx" << endl;
			out << "\t\t" << "cmpq %rbx, %rax" << endl;
			if(parent->_falseExit)
			{
				out << "\t\t" << "jl Block" << parent->_trueExit->_id << endl;
				out << "\t\t" << "jmp Block" << parent->_falseExit->_id << endl;
	
			}

			break;
		}
		case Operators::LessEqual:		
		{
			out << _lhs.Move("%rax") << endl;
			out << _rhs.Move("%rbx") << endl;
			if(_lhs.type & Types::PrefixVariable)
				out << "\t\t" << "movq (%rax), %rax" << endl;
			if(_rhs.type & Types::PrefixVariable)
				out << "\t\t" << "movq (%rbx), %rbx" << endl;
			out << "\t\t" << "cmpq %rbx, %rax" << endl;
			if(parent->_falseExit)
			{
				out << "\t\t" << "jle Block" << parent->_trueExit->_id << endl;
				out << "\t\t" << "jmp Block" << parent->_falseExit->_id << endl;
	
			}

			break;
		}
		case Operators::Greater:		
		{
			out << _lhs.Move("%rax") << endl;
			out << _rhs.Move("%rbx") << endl;
			if(_lhs.type & Types::PrefixVariable)
				out << "\t\t" << "movq (%rax), %rax" << endl;
			if(_rhs.type & Types::PrefixVariable)
				out << "\t\t" << "movq (%rbx), %rbx" << endl;
			out << "\t\t" << "cmpq %rbx, %rax" << endl;
			if(parent->_falseExit)
			{
				out << "\t\t" << "jg Block" << parent->_trueExit->_id << endl;
				out << "\t\t" << "jmp Block" << parent->_falseExit->_id << endl;
	
			}

			break;
		}
		case Operators::GreaterEqual:		
		{
			out << _lhs.Move("%rax") << endl;
			out << _rhs.Move("%rbx") << endl;
			if(_lhs.type & Types::PrefixVariable)
				out << "\t\t" << "movq (%rax), %rax" << endl;
			if(_rhs.type & Types::PrefixVariable)
				out << "\t\t" << "movq (%rbx), %rbx" << endl;
			out << "\t\t" << "cmpq %rbx, %rax" << endl;
			if(parent->_falseExit)
			{
				out << "\t\t" << "jge Block" << parent->_trueExit->_id << endl;
				out << "\t\t" << "jmp Block" << parent->_falseExit->_id << endl;
	
			}

			break;
		}
		case Operators::NotEqual:		
		{
			out << _lhs.Move("%rax") << endl;
			out << _rhs.Move("%rbx") << endl;
			if(_lhs.type & Types::PrefixVariable)
				out << "\t\t" << "movq (%rax), %rax" << endl;
			if(_rhs.type & Types::PrefixVariable)
				out << "\t\t" << "movq (%rbx), %rbx" << endl;
			out << "\t\t" << "cmpq %rbx, %rax" << endl;
			if(parent->_falseExit)
			{
				out << "\t\t" << "jne Block" << parent->_trueExit->_id << endl;
				out << "\t\t" << "jmp Block" << parent->_falseExit->_id << endl;
	
			}

			break;
		}
		case Operators::Alloc:		
		{
			allocs << "\t\t" << ".globl " << strings[_result.data.String] << endl;
			allocs << "\t\t" << ".data " << endl;
			allocs << "\t\t" << ".align 8" << endl;
			allocs << "\t\t" << ".type " << strings[_result.data.String] << ", @object" << endl;
			allocs << "\t\t" << ".size " << strings[_result.data.String] << ", " << _rhs.data.Integer*8 << endl;
			allocs << strings[_result.data.String] << ":\t\t"  << ".quad " << _lhs.data.Integer - 1 ;
			for(long i = 1; i < _lhs.data.Integer; i++)
			{
				allocs << ", 0";
			}
			allocs << endl;
			break;
		}
		case Operators::Resolve:		
		{
			if(_result.type & Types::PrefixVariable)
			{

				if(!(_rhs.type & Types::Variable) && _rhs.type & Types::Integer)
				{
					if(_lhs.type & Types::PrefixVariable)
					{
						out << "\t\t" << "movq " << _lhs.Print() << ", %rax" << endl;
						out << "\t\t" << "movq (%rax), %rax" << endl;
						out << "\t\t" << "addq " << "$" << 8*_rhs.data.Integer << ", %rax" << endl;
						out << _result.MoveTo("%rax") << endl;
					}
					else
					{
						out << "\t\t" << "movq " << _lhs.Asm() << "(%rip), %rax"  << endl;			
						out << "\t\t" << "addq " << "$" << 8*_rhs.data.Integer << ", %rax" << endl;
						out << _result.MoveTo("%rax") << endl;
					}
				}
				else
				{
					out << _rhs.Move("%rax") << endl;
					out << "\t\t" << "movq $8, %rbx" << endl;
					out << "\t\t" << "mulq %rbx" << endl;
					if(_lhs.type & Types::PrefixVariable)
					{
						out << _lhs.Move("%rbx") << endl;
						out << "\t\t" << "movq (%rbx), %rbx" << endl;
						out << "\t\t" << "movq %rbx, _ret" << endl;
						out << "\t\t" << "movq _ret(%rip), %rbx"  << endl;		
					}
					else
					{		
						out << "\t\t" << "movq " << _lhs.Asm() << "(%rip), %rbx"  << endl;												
					}
					out << "\t\t" << "addq %rax" << ", %rbx" << endl;	
					out << _result.MoveTo("%rbx") << endl;
				}
				
			}
			else
				out << "\t\t" << "movq " << _lhs.Asm() << "+" << (_rhs.data.Integer*8) << ", " << _result.Asm() << endl;
			break;
		}
		case Operators::Hash:		
		{
			if(_lhs.type & Types::PrefixVariable)
			{
				out << _lhs.Move("%rax") << endl;
				out << "\t\t" << "movq (%rax), %rax" << endl;
				out << "\t\t" << "movq %rax, _ret" << endl;
				out << "\t\t" << "movq _ret(%rip), %rax"  << endl;		
			}
			else
			{
				out << "\t\t" << "movq " << _lhs.Asm() << "(%rip), %rax" << endl;
			}
			out << "\t\t" << "movq (%rax), %rax" << endl;
			out << _result.MoveTo("%rax") << endl;
			break;
		}
		case Operators::SaveVars:		
		{
			for(auto& v : vars)
			{
				if(v[0] != '#' && v != "_ret" && v != "_retAd")
				{
					out << "\t\t" << "movq " << v << ", %r11" << endl;
					out << "\t\t" << "pushq %r11" << endl;
				}
			}
			break;
		}
		case Operators::RestoreVars:		
		{
			for(set<string>::reverse_iterator i = vars.rbegin(); i != vars.rend(); i++)
			{
				auto& s = (*i);
				if(s[0] != '#' && s != "_ret" && s != "_retAd")
				{
					out << "\t\t" << "popq %r11" << endl;
					out << "\t\t" << "movq %r11, " << s << endl;	
				}	
			}
			break;
		}	
	}

}
BBlock::BBlock() :  _trueExit(nullptr), _falseExit(nullptr), _function(nullptr)

{

}
BBlock::BBlock(size_t id) :  _trueExit(nullptr), _falseExit(nullptr), _function(nullptr), _id(id)
{

}

void BBlock::AddFunction(BBlock* f)
{
	if(_function)
	{
		_function->AddFunction(f);
		return;
	}
	_function = f;
}
void BBlock::Add(const ThreeAd& tad)
{
	_instructions.push_back(move(tad));
}
string BBlock::GenTree()
{
	stringstream out;
	out << "\"" << _id << "\" [" << endl;
	out << "label = \"<f0> Block @ " << _id;
	int size = 1;
	for(auto& i : _instructions)
	{
		out << "| <f" << size++ << "> ";
		out << "\t" << i.GetFormat() << endl;
	}
	out << "\"" << endl;
	out << "shape = \"record\"" << endl;
	out << "];" << endl;

	size--;
	if(_falseExit)
	{
		out << "\"" << _id << "\":f" << size << "  ->  \"" << _falseExit->_id << "\":f0 [" << endl;
		out << "label = \"FalseExit\"" << endl;
		out << "];" << endl;

		out << "\"" << _id << "\":f" << size << "  ->  \"" << _trueExit->_id << "\":f0 [" << endl;
		out << "label = \"TrueExit\"" << endl;
		out << "];" << endl;
	}
	else if(_trueExit)
	{
		out << "\"" << _id << "\":f" << size << "  ->  \"" << _trueExit->_id << "\":f0 [" << endl;
		out << "label = \"TrueExit\"" << endl;
		out << "];" << endl;
	}

	return out.str();
}
void BBlock::Dump()
{
	cout << "BBlock @ " << _id << endl;
	for(auto& i : _instructions)
		i.Dump();

	cout << "True: " << (_trueExit ? _trueExit->_id : 0) << endl;
	cout << "False: " << (_falseExit ? _falseExit->_id : 0)  << endl << endl;
}
string BBlock::GetVars(set<string>& vars)
{
	stringstream out;
	for(auto& i : _instructions)
	{
		out << i.GetVars(vars) << endl;
	}

	return out.str();
}

void BBlock::GenAsm(stringstream& out, stringstream& allocs, set<string>& vars)
{
	out << "Block" << _id << ": " << endl;
	for(auto& i : _instructions)
	{
		out << "# " << i.GetFormat();
		out  << endl;
		i.GenAsm(this, out, allocs, vars);
		out << endl;
	}
	if(!_falseExit && _trueExit)
		out << "jmp Block" << _trueExit->_id << endl;

}
FBlock::FBlock() : BBlock()
{

}

FBlock::FBlock(size_t id) : BBlock(id)
{

}
string FBlock::GenTree()
{
	stringstream out;
	out << "\"" << _id << "\" [" << endl;
	out << "label = \"<f0> Function @ " << _id;
	int size = 1;
	for(auto& i : _instructions)
	{
		out << "| <f" << size++ << "> ";
		out << i.GetFormat();
	}
	out << "\"" << endl;
	out << "shape = \"record\"" << endl;
	out << "];" << endl;

	size--;
	if(_falseExit)
	{
		out << "\"" << _id << "\":f" << size << "  ->  \"" << _falseExit->_id << "\":f0 [" << endl;
		out << "label = \"FalseExit\"" << endl;
		out << "];" << endl;

		out << "\"" << _id << "\":f" << size << "  ->  \"" << _trueExit->_id << "\":f0 [" << endl;
		out << "label = \"TrueExit\"" << endl;
		out << "];" << endl;
	}
	else if(_trueExit)
	{
		out << "\"" << _id << "\":f" << size << "  ->  \"" << _trueExit->_id << "\":f0 [" << endl;
		out << "label = \"TrueExit\"" << endl;
		out << "];" << endl;
	}

	return out.str();
}

void FBlock::Dump()
{
	cout << "Function @ " << _id << endl;
	for(auto& i : _instructions)
		i.Dump();

	cout << "True: " << (_trueExit ? _trueExit->_id : 0) << endl;
	cout << "False: " << (_falseExit ? _falseExit->_id : 0)  << endl << endl;
}
void FBlock::GenAsm(stringstream& out, stringstream& allocs, set<string>& vars)
{
	out << "Func" << _id << ": " << endl;
	for(auto& i : _instructions)
	{
		out << "# " << i.GetFormat();
		out  << endl;
		i.GenAsm(this, out, allocs,vars);
		out << endl;
	}
}