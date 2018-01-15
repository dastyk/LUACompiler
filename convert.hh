#include <string>
#include <cstring>
#include <list>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
using namespace std;

#define CCR(name,type, count,...) \
class name\
{\
public:\
static const type __VA_ARGS__;\
static const unsigned int count;\
name() : _flags(type()) {}\
name(type singleFlag) : _flags(singleFlag) {}\
name(const name& original) : _flags(original._flags) {}\
bool operator==(const name& rhs) const { return _flags == rhs._flags; }\
name    operator =(type addValue) { this->_flags = addValue; return *this; }\
name&   operator |=(type addValue) { _flags |= addValue; return *this; }\
name    operator |(type addValue) { name  result(*this); result |= addValue; return result; }\
name&   operator &=(type maskValue) { _flags &= maskValue; return *this; }\
name    operator &(type maskValue) { name  result(*this); result &= maskValue; return result; }\
name    operator ~() { name  result(*this); result._flags = ~result._flags; return result; }\
unsigned int Count() { unsigned int cc = 0; for (unsigned int i = 0; i < sizeof(type); i++) { cc += _flags << i; } return cc; }\
operator type() { return _flags; }\
type _flags;\
std::size_t operator()(const name& key) const{return std::hash<type>()(key._flags);}\
};


CCR(Types, int, Num_Types = 11,
  String = 1 << 0,
  Integer = 1 << 1,
  Boolean = 1 << 2,
  Float = 1 <<3,
  Variable = 1 << 4,
  Pointer = 1 << 5,
  Nil = 1 << 6,
  Function = 1 << 7,
  PrefixVariable = 1 << 8,
  Scope = 1 << 9,
  FunctionCall = 1 << 10,
  FunctionPointer = 1 << 11,
  Asm = 1 << 12,
  Table = 1 << 13,
  FieldSize = 1 << 14);

class FBlock;
class BBlock;
struct FieldSizeData;
struct Type
{
  Types type;
  long haxxor;
  string haxxor2;
  union
  {
    size_t String;
    long Integer;
    float Float;
    bool Boolean;
  }data;

  Type();
  Type(const Type& other);
  Type(const string& s, bool var = false);
  Type(long i);
  Type(float f);
  Type(bool b);
  ~Type();

  Type& operator=(const Type& other);
  Type& operator=(const long& other);
  Type& operator=(const float& other);
  Type& operator=(const bool& other);
  Type& operator=(const string& other);

  string Print();
  string Asm();
  string Move(const string& reg);
  string MoveTo(const string& reg);
  string AsmI()const;
  void SetAsPointer();
};

enum class Operators : long
{
  Add = 0,
  Minus = 1,
  Mult = 2,
  Divide = 3,
  Equal = 4,
  Mod = 5,
  Less = 6,
  LessEqual = 7,
  Greater = 8,
  GreaterEqual = 9,
  NotEqual = 10,
  And = 11,
  Or = 12,
  DBDot = 13,
  Pow = 14,
  Hash = 15,
  Not = 16,
  Negative = 17,
  Copy = 18,
  FunctionCall = 19,
  Push = 20,
  Pop = 21,
  Resolve = 22,
  Alloc = 23,
  Return = 24,
  AllocStr = 25,
  SaveVars = 26,
  RestoreVars = 27
};


class ThreeAd
{
public:
  Operators _op;
  Type _result, _lhs, _rhs;

  ThreeAd(const Type& out, Operators o, const Type& l, const Type& r);
  string GetFormat();
  void Dump();
  string GetVars(set<string>& vars);
  void GenAsm(BBlock* parent, stringstream& out, stringstream& allocs, set<string>& vars);
};



class BBlock
{
  public:
  list<ThreeAd> _instructions;
  BBlock *_trueExit, *_falseExit;
  BBlock *_function;
  size_t _id;
  BBlock();
  BBlock(size_t id);
  virtual void AddFunction(BBlock* f);
  virtual void Add(const ThreeAd& tad);
  virtual string GenTree();
  virtual void Dump();
  virtual string GetVars(set<string>& vars);
  virtual void GenAsm(stringstream& out, stringstream& allocs, set<string>& vars);
};

class FBlock : public BBlock
{
  public:
  FBlock();
  FBlock(size_t id);
  string GenTree();
  void Dump();
  void GenAsm(stringstream& out, stringstream& allocs, set<string>& vars);
};