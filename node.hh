#include <string>
#include <unordered_map>
#include "convert.hh"
using namespace std;




class Node
{
public:
	Node();
	virtual ~Node();
	virtual void Convert(BBlock **out);
};

class Chunk;

class Expression : public virtual Node
{
protected:
	Expression* _nextExpression;
public:
	Expression();
	Expression(Expression* exp);
	virtual ~Expression();
	virtual void AddExpression(Expression* exp);

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
	virtual void EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out);
	virtual Type ConvertExpression(BBlock *out);
	virtual Expression* GetNext();
};

class Integer : public Expression
{
protected:
	long _value;
public:
	Integer(const string& v);
	~Integer();

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
};

class Variable : public Expression
{
public:
	Variable();
	virtual ~Variable();

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap)=0;
};

class NameVariable : public Variable
{
protected:
	string _name;
public:
	NameVariable(const string& v);
	virtual ~NameVariable();

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
};

class PrefixVariable : public Variable
{
protected:
	Expression* _pre;
	Expression* _expression;
public:
	PrefixVariable(Expression* pre, Expression* exp);
	virtual ~PrefixVariable();

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
	virtual void EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out);
};

class Boolean : public Expression
{
protected:
	bool _value;
public:
	Boolean(bool v);
	~Boolean();

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
};

class Float : public Expression
{
protected:
	float _value;
public:
	Float(const std::string& v1, const std::string& v2);
	~Float();

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
};

class String : public Expression
{
protected:
	string _value;
public:
	String(const string& v);
	~String();

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
	virtual void EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out);
};

class Field : public Expression
{
protected:
	Expression* _expression;
	Field* _nextField;
public:
	Field(Expression* exp);
	virtual ~Field();
	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
	virtual void EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out);
};

class NameIndexField : public Field
{
protected:
	string _index;
public:
	NameIndexField(const string& index, Expression* exp);
	~NameIndexField();
	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
	virtual void EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out);
};

class ExpIndexField : public Field
{
protected:
	Expression* _index;
public:
	ExpIndexField(Expression* index, Expression* exp);
	~ExpIndexField();
	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
	virtual void EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out);
};
class Table : public Expression
{
protected:
	Field* _fields;
public:
	Table(Field* field);
	virtual ~Table();

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
	virtual void EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out);
};

class Nil : public Expression
{
public:
	Nil();
	~Nil();

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
};
class Binop : public Expression
{
protected:
	Expression* _left;
	Expression* _right;
	Operators _op;
public:
	Binop(Expression* l, Expression* r, Operators op);
	virtual ~Binop();

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
	virtual void EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out);
};

class Unop : public Expression
{
protected:
	Operators _op;
	Expression* _right;
public:
	Unop(Expression* r, Operators op);
	virtual ~Unop();

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
	virtual void EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out);
};

class Statement : public virtual Node
{
protected:
	Statement* _nextStatement;
public:
	Statement();
	virtual ~Statement();
	virtual void AddStatement(Statement* statement);
	virtual Statement* GetNext();
	virtual void ConvertStatement(BBlock **out);
};

class Assignment : public Statement
{
protected:
	Expression* _expressions;
	Variable* _variables;
public:
	Assignment(Variable* vars, Expression* exps);
	virtual ~Assignment();
	virtual void ConvertStatement(BBlock **out);
};

class FunctionCall : public Statement, public Expression
{
protected:
	Expression* _expressions;
	Expression* _pexp;
public:
	FunctionCall(Expression* pexp, Expression* exp); 
	virtual ~FunctionCall();

	virtual void NamePass(unordered_map<Expression*,Type> &nameMap);
	virtual void EmitPass(unordered_map<Expression*,Type> &nameMap, BBlock *out);
	virtual void ConvertStatement(BBlock **out);
};

class DoEnd : public Statement
{
protected:
	Chunk* _chunk;
public:
	DoEnd(Chunk* chunk);
	virtual ~DoEnd();
};

class WhileDoEnd : public Statement
{
protected:
	Chunk* _chunk;
	Expression* _condition;
public:
	WhileDoEnd(Chunk* chunk, Expression* cond);
	virtual ~WhileDoEnd();
};

class For : public Statement
{
protected:
	Chunk* _chunk;
	Variable* _variable;
	Expression* _expression;
	Expression* _limit;
	Expression* _inc;
public:
	For(Variable* v, Expression* exp, Expression* limit, Chunk* chunk);
	For(Variable* v, Expression* exp, Expression* limit, Expression* inc, Chunk* chunk);
	virtual ~For();

	virtual void ConvertStatement(BBlock **out);
};

class ForIn : public Statement
{
protected:
	Chunk* _chunk;
public:
	ForIn(Chunk* chunk);
	virtual ~ForIn();
};

class If : public Statement
{
protected:
	Expression* _condition;
	If* _elseIf;
	Chunk* _trueC;
	Chunk* _elseC;
public:
	If(Expression* cond, Chunk* trueC, If* elseIf = nullptr);
	If(Expression* cond, Chunk* trueC, Chunk* elseC, If* elseIf = nullptr);
	virtual void AddElseIf(If* eif);
	virtual void SetElse(Chunk* elseC);
	virtual ~If();
	virtual void ConvertStatement(BBlock **out);
};

class RepeatUntil : public Statement
{
protected:
	Chunk* _chunk;
	Expression* _condition;
public:
	RepeatUntil(Chunk* chunk, Expression* condition);
	virtual ~RepeatUntil();
	virtual void ConvertStatement(BBlock **out);
};

class FuncName : public Node
{
protected:
	string _name;
	FuncName* _preName;
public:
	FuncName(const std::string& name);
	FuncName(FuncName* pre, const std::string& name);
	virtual const string& GetName();
	virtual Type ConvertName();
	virtual ~FuncName();
};

class NameList : public Node
{
protected:
	string _name;
	NameList* _nextName;
public:
	NameList(const std::string& name);
	NameList(NameList* pre, const std::string& name);
	virtual void AddName(NameList* l);
	virtual Type ConvertName();
	virtual NameList* GetNext();
	virtual ~NameList();
};

class FunctionBody : public Expression
{
protected:
	NameList* _args;
	Chunk* _chunk;
public:
	FunctionBody(NameList* args, Chunk* chunk);
	virtual ~FunctionBody();
	virtual Type ConvertExpression(BBlock *out);

};

class Function : public Statement
{
protected:
	FuncName* _name;
	FunctionBody* _body;
public:
	Function(FuncName* name, FunctionBody* body);
	virtual ~Function();

	virtual void ConvertStatement(BBlock**out);
};

class Print : public Function
{
public:
	Print();
	virtual ~Print();
	virtual void ConvertStatement(BBlock**out);
};
class PrintN : public Function
{
public:
	PrintN();
	virtual ~PrintN();
	virtual void ConvertStatement(BBlock**out);
};
class Read : public Function
{
public:
	Read();
	virtual ~Read();
	virtual void ConvertStatement(BBlock**out);
};

class Return : public Statement
{
protected:
	Expression* _expression;
public:
	Return(Expression* exp);
	virtual ~Return();	
	virtual void ConvertStatement(BBlock**out);
};

class Chunk : public Node
{
protected:
	Statement* _statements;
public:
	Chunk();
	Chunk(Statement* statement);
	virtual ~Chunk();
	virtual void AddStatement(Statement* statement);

	virtual void Convert(BBlock **out);
};