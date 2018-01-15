
#include "node.hh"
#include <utility>

size_t NameCounter = 0;
string NewName()
{
  stringstream result;
  result << "_t" << NameCounter++;
  return result.str();
}

size_t BlockCounter = 1;
BBlock* NewBlock()
{
	BBlock* out = new BBlock(BlockCounter++);
	return out;
}

FBlock* NewFunction()
{
	FBlock* out = new FBlock(BlockCounter++);
	return out;
}

Node::Node()
{

}
Node::~Node()
{

}

void Node::Convert(BBlock **out)
{

}

Expression::Expression() : Node(), _nextExpression(nullptr)
{

}
Expression::Expression(Expression* exp) : _nextExpression(exp)
{

}
Expression::~Expression()
{
	delete _nextExpression;
}

void Expression::AddExpression(Expression* exp)
{
	if(_nextExpression)
		_nextExpression->AddExpression(exp);
	else
		_nextExpression = exp;
}
void Expression::NamePass(unordered_map<Expression*,Type> &nameMap)
{

}
void Expression::EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out)
{

}
Type Expression::ConvertExpression(BBlock *out)
{
	unordered_map<Expression*,Type> naming;
	NamePass(naming);
	EmitPass(naming, out);
	return naming[this];
}

Expression* Expression::GetNext()
{
	return _nextExpression;
}
Integer::Integer(const string& v) : Expression(), _value(stol(v))
{

}
Integer::~Integer()
{

}

void Integer::NamePass(unordered_map<Expression*,Type> &nameMap)
{
	nameMap[this] = _value;
}

Variable::Variable() : Expression()
{

}
Variable::~Variable()
{

}

NameVariable::NameVariable(const string& v) : Variable(), _name(v)
{

}
NameVariable::~NameVariable()
{

}
void NameVariable::NamePass(unordered_map<Expression*,Type> &nameMap)
{
	nameMap[this] = Type(_name, true);
}

PrefixVariable::PrefixVariable(Expression* pre, Expression* exp) : Variable(), _pre(pre), _expression(exp)
{

}
PrefixVariable::~PrefixVariable()
{
	delete _pre;
	delete _expression;
}

void PrefixVariable::NamePass(unordered_map<Expression*,Type> &nameMap)
{
	if(_pre && _expression)
	{
		_pre->NamePass(nameMap);
		_expression->NamePass(nameMap);
		Type t(NewName(), true);
		t.type |= Types::PrefixVariable;
		nameMap[this] = t;
	}
}
void PrefixVariable::EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out)
{
	if(_pre && _expression)
	{

		_expression->EmitPass(nameMap, out);
		_pre->EmitPass(nameMap, out);
		string hax =  nameMap[_expression].Print();
		string hax2 = nameMap[_pre].Print();
		if(nameMap[_pre].type & Types::PrefixVariable)
		{
			nameMap[this].type |= Types::PrefixVariable;
		}
		if(hax2 == "io" && (hax == "write" || hax == "read"))
		{
			nameMap[this].haxxor2 = hax;
		}
		else
		{
			out->Add(ThreeAd(nameMap[this], Operators::Resolve, nameMap[_pre], nameMap[_expression]));
		}
		
	}
}
Boolean::Boolean(bool v) : Expression(), _value(v)
{

}
Boolean::~Boolean()
{

}
void Boolean::NamePass(unordered_map<Expression*,Type> &nameMap)
{
	nameMap[this] = _value;
}
Float::Float(const std::string& v1, const std::string& v2) : Expression(), _value(stof(v1 + "." + v2))
{

}

Float::~Float()
{

}
void Float::NamePass(unordered_map<Expression*,Type> &nameMap)
{
	nameMap[this] = _value;
}
String::String(const string& v) : Expression(), _value(v)
{

}
String::~String()
{

}
void String::NamePass(unordered_map<Expression*,Type> &nameMap)
{
	nameMap[this] = Type(NewName(), true);
	nameMap[this].type |= Types::String;
}
void String::EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out)
{
	Type s(string(_value), false);
	out->Add(ThreeAd(nameMap[this], Operators::AllocStr, s, s));
}
Field::Field(Expression* exp) : Expression(), _expression(exp)
{

}
Field::~Field()
{
	delete _expression;
}
void Field::NamePass(unordered_map<Expression*,Type> &nameMap)
{
	if(_expression)
	{
		_expression->NamePass(nameMap);
	}
}
void Field::EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out)
{
	if(_expression)
	{
		_expression->EmitPass(nameMap, out);
		nameMap[this] = nameMap[_expression];
	}
}

NameIndexField::NameIndexField(const string& index, Expression* exp) : Field(exp), _index(index)
{

}
NameIndexField::~NameIndexField()
{

}
void NameIndexField::NamePass(unordered_map<Expression*,Type> &nameMap)
{

}
void NameIndexField::EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out)
{
	if(_expression)
	{
		Type o = _expression->ConvertExpression(out);

		Type t(_index, true);

		hash<string> hasher;
		long size = static_cast<long>(static_cast<uint16_t>(hasher(_index)));
		Type s(size);


		out->Add(ThreeAd(t, Operators::Copy, s, s));


		o.haxxor = size;
		o.type |= Types::FieldSize;

		nameMap[this] = o;
	}
}

ExpIndexField::ExpIndexField(Expression* index, Expression* exp) : Field(exp), _index(index)
{

}
ExpIndexField::~ExpIndexField()
{
	delete _index;
}
void ExpIndexField::NamePass(unordered_map<Expression*,Type> &nameMap)
{

}
void ExpIndexField::EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out)
{
	if(_expression && _index)
	{
		Type o = _expression->ConvertExpression(out);
		Type index = _index->ConvertExpression(out);


		o.haxxor = index.data.Integer;
		o.type |= Types::FieldSize;

		nameMap[this] = o;
	}
}

Table::Table(Field* field) : Expression(), _fields(field)
{

}

Table::~Table()
{
	delete _fields;
}

void Table::NamePass(unordered_map<Expression*,Type> &nameMap)
{
	Expression* e = _fields;

	while(e)
	{
		e->NamePass(nameMap);
		e = e->GetNext();
	}
	Type t(NewName(), true);
	t.type = Types::Table;
	nameMap[this] = t;
}
void Table::EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out)
{
	Expression* e = _fields;
	long i = 1;
	long size = 0;
	while(e)
	{
		e->EmitPass(nameMap, out);
		if(nameMap[e].type & Types::FieldSize)
		{
			size = size < nameMap[e].haxxor? nameMap[e].haxxor: size;
		}
		i++;
		e = e->GetNext();
	}
	if(size)
		i = size+1;
	out->Add(ThreeAd(nameMap[this], Operators::Alloc, i, i));

	std::vector<pair<Type, Type>> fp;
	i = 1;
	e = _fields;
	while(e)
	{
		Type t(NewName(), true);
		t.type |= Types::Pointer;
		pair<Type, Type> p;
		p.first = t;
		p.second = nameMap[e];
		fp.push_back(p);
		if(p.second.type & Types::FieldSize)
		{
			i--;
			out->Add(ThreeAd(t, Operators::Resolve, nameMap[this],p.second.haxxor));
		}
		else
			out->Add(ThreeAd(t, Operators::Resolve, nameMap[this],i));
		i++;
		e = e->GetNext();
	}

	for(auto& p : fp)
	{
		p.first.SetAsPointer();
		out->Add(ThreeAd(p.first, Operators::Copy,p.second, p.second));
	}

}
Nil::Nil() : Expression()
{

}
Nil::~Nil()
{

}
void Nil::NamePass(unordered_map<Expression*,Type> &nameMap)
{

}
Binop::Binop(Expression* l, Expression* r, Operators op) : Expression(), _left(l), _right(r), _op(op)
{

}

Binop::~Binop()
{
	delete _left;
	delete _right;
}
void Binop::NamePass(unordered_map<Expression*,Type> &nameMap)
{
	if(_left && _right)
	{
		_left->NamePass(nameMap);
		_right->NamePass(nameMap);
		nameMap[this] = Type(NewName(), true);
	}
}
void Binop::EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out)
{
	if(_left && _right)
	{
		_left->EmitPass(nameMap, out);
	    _right->EmitPass(nameMap, out);


		nameMap[this].type |= nameMap[_left].type | nameMap[_right].type;

	    out->Add(ThreeAd(nameMap[this], _op, nameMap[_left], nameMap[_right]));
	}
}

Unop::Unop(Expression* r, Operators op) : Expression(), _right(r), _op(op)
{

}
Unop::~Unop()
{
	delete _right;
}
void Unop::NamePass(unordered_map<Expression*,Type> &nameMap)
{
	if(_right)
	{
		_right->NamePass(nameMap);
		nameMap[this] = Type(NewName(), true);
	}
}
void Unop::EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out)
{
	if(_right)
	{
	    _right->EmitPass(nameMap, out);

	    out->Add(ThreeAd(nameMap[this], _op, nameMap[_right], nameMap[_right]));
	}
}

Statement::Statement() : _nextStatement(nullptr)
{

}
Statement::~Statement()
{
	delete _nextStatement;
}
void Statement::AddStatement(Statement* statement)
{
	if(_nextStatement)
		_nextStatement->AddStatement(statement);
	else
		_nextStatement= statement;
}
Statement* Statement::GetNext()
{
	return _nextStatement;
}
void Statement::ConvertStatement(BBlock **out)
{

}
Assignment::Assignment(Variable* vars, Expression* exps) : Statement(), _variables(vars), _expressions(exps)
{

}
Assignment::~Assignment()
{
	delete _variables;
	delete _expressions;
}



void Assignment::ConvertStatement(BBlock **out)
{
	std::vector<Type> vars;
	std::vector<Type> exps;
	Expression* e = _expressions;

	while(e)
	{
		const Type& o2 = e->ConvertExpression(*out);
		exps.push_back(o2);
		e = e->GetNext();
	}

	e = _variables;
	int i = 0;
	bool pref = false;
	while(e)
	{
		Type o = e->ConvertExpression(*out);
		if(o.type &  Types::Table)
			o.type |= Types::Pointer;
		else if(exps[i].type &  Types::PrefixVariable)
		{
			pref = true;
			o.type = o.type;
		}
		else
			o.type |= exps[i].type;
		vars.push_back(o);
		e = e->GetNext();
		i++;
	}

	if(vars.size() == 1)
	{
		if(exps.size() > 0)
		{
			cout << vars[0].Print() << " " << vars[0].type << " " << exps[0].Print() << " " << exps[0].type << endl;
			(*out)->Add(ThreeAd(vars[0], Operators::Copy, exps[0], exps[0]));
		}
		else
			(*out)->Add(ThreeAd(vars[0], Operators::Copy, Type(), Type()));
	}
	else
	{
		
		size_t size = min(vars.size(), exps.size());
		if(pref)
		{
			for(size_t i = 0; i < size; i++)
			{
				(*out)->Add(ThreeAd(exps[i], Operators::Push, exps[i], exps[i]));	
			}	
			for(size_t i = 0; i < size; i++)
			{
				(*out)->Add(ThreeAd(vars[size-i-1], Operators::Pop, vars[size-i-1], vars[size-i-1]));	
			}	
		}
		else
		{
			for(size_t i = 0; i < size; i++)
			{
				(*out)->Add(ThreeAd(vars[i], Operators::Copy, exps[i], exps[i]));	
			}	
		}
		
	}
	
}
FunctionCall::FunctionCall(Expression* pexp, Expression* exp) : Statement(), Expression(), _pexp(pexp), _expressions(exp)
{

} 
FunctionCall::~FunctionCall()
{
	delete _pexp;
	delete _expressions;
} 
void FunctionCall::NamePass(unordered_map<Expression*,Type> &nameMap)
{
	if(_pexp)
	{
		_pexp->NamePass(nameMap);


		Expression* e = _expressions;
		while(e)
		{
			e->NamePass(nameMap);
			e = e->GetNext();
		}

		nameMap[this] = Type(NewName(), true);
	}
}
void FunctionCall::EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out)
{
	if(_pexp)
	{
		_pexp->EmitPass(nameMap, out);


		Expression* e = _expressions;
		while(e)
		{
			e->EmitPass(nameMap, out);
			e = e->GetNext();
		}

		if(nameMap[_pexp].haxxor2 == "read")
		{
			nameMap[_pexp] = string("read");
			nameMap[_pexp].type = Types::FunctionCall;
			out->Add(ThreeAd(nameMap[_expressions], Operators::Push, nameMap[_expressions], nameMap[_expressions]));
			out->Add(ThreeAd(nameMap[this], Operators::FunctionCall, nameMap[_pexp], nameMap[_pexp]));
		}
		else
		{
			Type t2((long)0);
			out->Add(ThreeAd(t2, Operators::SaveVars, t2,t2));
			// Put args in regs
			// And all that stuff
			e = _expressions;
			long i = 0;
			while(e)
			{
				out->Add(ThreeAd(nameMap[e], Operators::Push, nameMap[e], nameMap[e]));
				i++;
				e = e->GetNext();
			}
			
			// Use call function in asm.
			nameMap[_pexp].type |= Types::FunctionCall;
			out->Add(ThreeAd(nameMap[this], Operators::FunctionCall, nameMap[_pexp], nameMap[_pexp]));
		}
	}
}
void FunctionCall::ConvertStatement(BBlock **out)
{
	if(_pexp)
	{
		Type iden = _pexp->ConvertExpression(*out);
		iden.type |= Types::FunctionCall;

		std::vector<Type> args;

		Expression* e = _expressions;

		while(e)
		{
			const Type& o = e->ConvertExpression(*out);
			args.push_back(o);
			e = e->GetNext();
		}

		if(iden.Print() == "print" || iden.haxxor2 == "write")
		{
			for(long i = 0; i < args.size(); i++)
			{
				if(args[i].type & Types::String)
					iden = string("prints");
				else
					iden = string("printn");
				cout << args[i].Print() << " " << args[i].type << endl;
				iden.type = Types::FunctionCall;
				(*out)->Add(ThreeAd(args[i], Operators::Push, args[i], args[i]));
				(*out)->Add(ThreeAd(Type("_ret", true), Operators::FunctionCall, iden, iden));
			}
		}
		else if(iden.Print() == "prints" || iden.haxxor2 == "printn")
		{	
			// Put args in regs
			// And all that stuff
			for(long i = 0; i < args.size(); i++)
			{
				(*out)->Add(ThreeAd(args[i], Operators::Push, args[i], args[i]));
			}
			
			// Use call function in asm.
			(*out)->Add(ThreeAd(Type("_ret", true), Operators::FunctionCall, iden, iden));
		}
		else if(iden.haxxor2 == "read")
		{
			iden = string("read");
			iden.type = Types::FunctionCall;
			(*out)->Add(ThreeAd(args[0], Operators::Push, args[0], args[0]));
			(*out)->Add(ThreeAd(Type("_ret", true), Operators::FunctionCall, iden, iden));
		}
		else
		{	
			Type t2((long)0);
			(*out)->Add(ThreeAd(t2, Operators::SaveVars, t2,t2));

			// Put args in regs
			// And all that stuff
			for(long i = 0; i < args.size(); i++)
			{
				(*out)->Add(ThreeAd(args[i], Operators::Push, args[i], args[i]));
			}
			
			// Use call function in asm.
			(*out)->Add(ThreeAd(Type("_ret", true), Operators::FunctionCall, iden, iden));
		}
	}


}
DoEnd::DoEnd(Chunk* chunk) : Statement(), _chunk(chunk)
{

}
DoEnd::~DoEnd()
{
	delete _chunk;
}

WhileDoEnd::WhileDoEnd(Chunk* chunk, Expression* cond) : Statement(), _chunk(chunk), _condition(cond)
{

}
WhileDoEnd::~WhileDoEnd()
{
	delete _chunk;
	delete _condition;
}

For::For(Variable* v, Expression* exp, Expression* limit, Chunk* chunk) : Statement(), _chunk(chunk), _variable(v), _expression(exp), _limit(limit), _inc(new Integer("1"))
{

}
For::For(Variable* v, Expression* exp, Expression* limit, Expression* inc, Chunk* chunk) : Statement(), _chunk(chunk), _variable(v), _expression(exp), _limit(limit), _inc(inc)
{

}
For::~For()
{
	delete _chunk;
	delete _variable;
	delete _expression;
	delete _limit;
	delete _inc;
}
void For::ConvertStatement(BBlock **out)
{
	if(_expression && _variable && _limit && _inc)
	{
		const Type& e = _expression->ConvertExpression(*out);
		const Type& v = _variable->ConvertExpression(*out);

		(*out)->Add(ThreeAd(v, Operators::Copy, e, e));

		BBlock* forBlock = NewBlock();
		
		const Type& l = _limit->ConvertExpression(forBlock);

		forBlock->Add(ThreeAd(v, Operators::LessEqual, v, l));

		(*out)->_trueExit = forBlock;


		BBlock* bodyBlock = NewBlock();	
		BBlock* endBlock = NewBlock();	

		forBlock->_trueExit = bodyBlock;
		endBlock->_trueExit = forBlock;

		_chunk->Convert(&bodyBlock);
		const Type& i = _inc->ConvertExpression(endBlock);
		endBlock->Add(ThreeAd(v, Operators::Add ,v,i ));

		BBlock* exitBlock = NewBlock();
		bodyBlock->_trueExit = endBlock;
		forBlock->_falseExit = exitBlock;
		

		(*out)= exitBlock;
	}
}
ForIn::ForIn(Chunk* chunk) : Statement(), _chunk(chunk)
{

}
ForIn::~ForIn()
{
	delete _chunk;
}

If::If(Expression* cond, Chunk* trueC, If* elseIf) : Statement(), _condition(cond), _trueC(trueC), _elseC(nullptr), _elseIf(elseIf)
{

}
If::If(Expression* cond, Chunk* trueC, Chunk* elseC, If* elseIf) : Statement(), _condition(cond), _trueC(trueC), _elseC(elseC), _elseIf(elseIf)
{

}
void If::AddElseIf(If* eif)
{
	if(_elseIf)
		_elseIf->AddElseIf(eif);
	else
		_elseIf = eif;
}
void If::SetElse(Chunk* elseC)
{
	_elseC = elseC;
}
If::~If()
{
	delete _condition;
	delete _elseIf;
	delete _trueC;
	delete _elseC;
}
void If::ConvertStatement(BBlock **out)
{
	if(_condition && _trueC)
	{
		_condition->ConvertExpression(*out);

		BBlock* trueblock = NewBlock();
		BBlock* falseblock = NewBlock();
		BBlock* p = (*out);

		(*out)->_trueExit = trueblock;
		(*out)->_falseExit = falseblock;

		_trueC->Convert(&trueblock);

		if(_elseIf)
		{
			_elseIf->SetElse(_elseC);
			_elseC = nullptr;
			_elseIf->ConvertStatement(&falseblock);
		}
		else if(_elseC)
		{
			_elseC->Convert(&falseblock);
			
		}

		BBlock* exitBlock = NewBlock();
		trueblock->_trueExit = exitBlock;
		
		if(_elseC || _elseIf)
		{
			falseblock->_trueExit = exitBlock;
		}
		else
		{
			delete falseblock;
			p->_falseExit = exitBlock;
		}


		(*out) = exitBlock;
	}
}
RepeatUntil::RepeatUntil(Chunk* chunk, Expression* condition) : Statement(), _chunk(chunk), _condition(condition)
{

}
RepeatUntil::~RepeatUntil()
{
	delete _chunk;
	delete _condition;
}
void RepeatUntil::ConvertStatement(BBlock **out)
{
	if(_condition && _chunk)
	{
		BBlock* bodyBlock = NewBlock();
		BBlock* conBlock = NewBlock();
		conBlock->_falseExit = bodyBlock;
		(*out)->_trueExit = bodyBlock;
		
		_chunk->Convert(&bodyBlock);

		bodyBlock->_trueExit = conBlock;

		const Type& o = _condition->ConvertExpression(conBlock);

		BBlock* exitBlock = NewBlock();
		conBlock->_trueExit = exitBlock;
		
		(*out) = exitBlock;

	}
}
FuncName::FuncName(const std::string& name) : Node(), _name(name), _preName(nullptr)
{

}
FuncName::FuncName(FuncName* pre, const std::string& name) : Node(), _name(name), _preName(pre)
{

}
FuncName::~FuncName()
{
	delete _preName;
}
const string& FuncName::GetName()
{
	return _name;
}
Type FuncName::ConvertName()
{
	FuncName* f = _preName;
	string out = "";
	while(f)
	{
		Type t = f->GetName();
		t.type = Types::Scope;
	}

}
NameList::NameList(const std::string& name) : Node(), _name(name), _nextName(nullptr)
{

}
NameList::NameList(NameList* pre, const std::string& name) : Node(), _name(name), _nextName(pre)
{

}
NameList::~NameList()
{
	delete _nextName;
}
NameList* NameList::GetNext()
{
	return _nextName;
}
Type NameList::ConvertName()
{
	return _name;
}
void NameList::AddName(NameList* l)
{
	if(_nextName)
	{
		_nextName->AddName(l);
		return;
	}
	_nextName = l;
}
FunctionBody::FunctionBody(NameList* args, Chunk* chunk) : Expression(), _chunk(chunk), _args(args)
{

}
FunctionBody::~FunctionBody()
{
	delete _chunk;
	delete _args;
}
Type FunctionBody::ConvertExpression(BBlock *out)
{
	unordered_map<Expression*,Type> naming;

	

	NameList* a = _args;
	BBlock* bodyBlock = NewFunction();
	Type re("_retAd", true);
	bodyBlock->Add(ThreeAd(re, Operators::Pop, re,re));
	std::vector<NameList*> args;
	while(a)
	{
		args.push_back(a);
		a = a->GetNext();
	}

	for(std::vector<NameList*>::reverse_iterator it = args.rbegin(); it != args.rend(); it++)
	{
		a = *it;
		Type v = a->ConvertName();
		v.type = Types::Variable;
		naming[(Expression*)a] = v;
		
		bodyBlock->Add(ThreeAd(v, Operators::Pop, v, v ));

	}
	

	bodyBlock->Add(ThreeAd(re, Operators::Push, re,re));





	Type t(NewName(), true);
	t.type = Types::Function | Types::Variable;
	Type f((long)bodyBlock->_id);
	f.type = Types::Function;

	out->AddFunction(bodyBlock);

	out->Add(ThreeAd(t, Operators::Copy, f, f));


	_chunk->Convert(&bodyBlock);

	Type as("%rax", true);
	as.type = Types::Asm;
	Type t2((long)0);
	bodyBlock->Add(ThreeAd(as, Operators::Copy, t2,t2));


	bodyBlock->Add(ThreeAd(re, Operators::Pop, re,re));

	
	bodyBlock->Add(ThreeAd(t2, Operators::RestoreVars, t2,t2));



	bodyBlock->Add(ThreeAd(re, Operators::Push, re,re));




	bodyBlock->Add(ThreeAd(t2,Operators::Return, t2,t2));
	
	return t;
}

Function::Function(FuncName* name, FunctionBody* body) : Statement(), _body(body), _name(name)
{

}
Function::~Function()
{
	delete _body;
	delete _name;
}

void Function::ConvertStatement(BBlock **out)
{
	if(_body && _name)
	{
		Type t(_name->GetName(), true);
		t.type = Types::Function | Types::Variable;

		const Type& o = _body->ConvertExpression(*out);
		(*out)->Add(ThreeAd(t, Operators::Copy, o, o));

	}
}
Print::Print() : Function(nullptr, nullptr)
{

}
Print::~Print()
{

}
void Print::ConvertStatement(BBlock **out)
{
	unordered_map<Expression*,Type> naming;

	BBlock* bodyBlock = NewFunction();
	BBlock* integer = NewBlock();
	BBlock* str = NewBlock();

	Type f((long)bodyBlock->_id);
	f.type = Types::Function;

	Type t("prints", true);
	t.type |= Types::Function;

	(*out)->AddFunction(bodyBlock);
	(*out)->Add(ThreeAd(t, Operators::Copy, f, f));

	Type re("_retAd", true);
	bodyBlock->Add(ThreeAd(re, Operators::Pop, re,re));



	// Get all args

		Type a2("_arg"+to_string(0), true);
		bodyBlock->Add(ThreeAd(a2, Operators::Pop, a2,a2));

	bodyBlock->Add(ThreeAd(re, Operators::Push, re,re));


	Type a((long)0);
	Type as("%rax", true);
	as.type = Types::Asm;

	bodyBlock->Add(ThreeAd(as, Operators::Copy, a,a));


	Type as2("%rdi", true);
	as2.type = Types::Asm;
	bodyBlock->Add(ThreeAd(as2, Operators::Copy, a2,a2));

	Type p(string("printf"));
	p.type = Types::Asm;
	bodyBlock->Add(ThreeAd(p, Operators::FunctionCall,p,p));

	bodyBlock->Add(ThreeAd(as, Operators::Copy, a,a));

	bodyBlock->Add(ThreeAd(a, Operators::Return,a,a));



}
PrintN::PrintN() : Function(nullptr, nullptr)
{

}
PrintN::~PrintN()
{

}
void PrintN::ConvertStatement(BBlock **out)
{
	unordered_map<Expression*,Type> naming;

	BBlock* bodyBlock = NewFunction();

	String* str = new String("%d");
	Type f((long)bodyBlock->_id);
	f.type = Types::Function;

	Type t("printn", true);
	t.type |= Types::Function;

	(*out)->AddFunction(bodyBlock);
	(*out)->Add(ThreeAd(t, Operators::Copy, f, f));

	Type re("_retAd", true);
	bodyBlock->Add(ThreeAd(re, Operators::Pop, re,re));



	// Get all args




	Type a2("_arg"+to_string(0), true);
	bodyBlock->Add(ThreeAd(a2, Operators::Pop, a2,a2));

	bodyBlock->Add(ThreeAd(re, Operators::Push, re,re));


	Type a((long)0);
	Type as("%rax", true);
	as.type = Types::Asm;

	bodyBlock->Add(ThreeAd(as, Operators::Copy, a,a));


	Type as2("%rdi", true);
	as2.type = Types::Asm;
	Type st = str->ConvertExpression(*out);
	delete str;
	bodyBlock->Add(ThreeAd(as2, Operators::Copy, st,st));

	Type as3("%rsi", true);
	as3.type = Types::Asm;
	bodyBlock->Add(ThreeAd(as3, Operators::Copy, a2,a2));



	Type p(string("printf"));
	p.type = Types::Asm;
	bodyBlock->Add(ThreeAd(p, Operators::FunctionCall,p,p));

	bodyBlock->Add(ThreeAd(as, Operators::Copy, a,a));

	bodyBlock->Add(ThreeAd(a, Operators::Return,a,a));



}
Read::Read() : Function(nullptr, nullptr)
{

}
Read::~Read()
{

}

void Read::ConvertStatement(BBlock **out)
{
	unordered_map<Expression*,Type> naming;

	BBlock* bodyBlock = NewFunction();

	String* str = new String("%d");
	Type f((long)bodyBlock->_id);
	f.type = Types::Function;

	Type t("read", true);
	t.type |= Types::Function;

	(*out)->AddFunction(bodyBlock);
	(*out)->Add(ThreeAd(t, Operators::Copy, f, f));

	Type re("_retAd", true);
	bodyBlock->Add(ThreeAd(re, Operators::Pop, re,re));

	Type a2("_arg"+to_string(0), true);
	bodyBlock->Add(ThreeAd(a2, Operators::Pop, a2,a2));

	bodyBlock->Add(ThreeAd(re, Operators::Push, re,re));

	Type a((long)0);
	Type as("%rax", true);
	as.type = Types::Asm;

	bodyBlock->Add(ThreeAd(as, Operators::Copy, a,a));

	Type as2("%rdi", true);
	as2.type = Types::Asm;
	Type st = str->ConvertExpression(*out);
	delete str;
	bodyBlock->Add(ThreeAd(as2, Operators::Copy, st,st));

	Type as3("%rsi", true);
	as3.type = Types::Asm;
	a2.type = Types::String | Types::Variable;
	bodyBlock->Add(ThreeAd(as3, Operators::Copy, a2,a2));



	Type p(string("scanf"));
	p.type = Types::Asm;
	bodyBlock->Add(ThreeAd(p, Operators::FunctionCall,p,p));

	a2.type = Types::Integer | Types::Variable;
	bodyBlock->Add(ThreeAd(as, Operators::Copy, a2,a2));

	bodyBlock->Add(ThreeAd(a, Operators::Return,a2,a2));

}
Return::Return(Expression* exp) : Statement(), _expression(exp)
{

}
Return::~Return()
{
	delete _expression;
}


void Return::ConvertStatement(BBlock **out)
{
	if(_expression)
	{
		
		Type o = _expression->ConvertExpression(*out);


		Type as("%rax", true);
		as.type = Types::Asm;
		(*out)->Add(ThreeAd(as, Operators::Copy, o,o));

		Type re("_retAd", true);
		(*out)->Add(ThreeAd(re, Operators::Pop, re,re));

		Type t((long)0);
		(*out)->Add(ThreeAd(t, Operators::RestoreVars, t,t));

	

		(*out)->Add(ThreeAd(re, Operators::Push, re,re));




		(*out)->Add(ThreeAd(o,Operators::Return, o,o));

	}
}

Chunk::Chunk() : Node(), _statements(nullptr)
{

}
Chunk::Chunk(Statement* statement) : Node(), _statements(statement)
{
	
}
Chunk::~Chunk()
{
	delete _statements;
}
void Chunk::AddStatement(Statement* statement)
{
	if(_statements)
		_statements->AddStatement(statement);
	else
		_statements = statement;
}

void Chunk::Convert(BBlock **out)
{
	Statement* s = _statements;
	while(s)
	{
		s->ConvertStatement(out);
		s = s->GetNext();

	}

}