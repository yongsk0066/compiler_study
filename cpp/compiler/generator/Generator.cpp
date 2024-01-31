#include <map>
#include <list>
#include <tuple>
#include <vector>
#include <iomanip>
#include "Node.h"
#include "Code.h"

using std::cout;
using std::endl;
using std::list;
using std::map;
using std::max;
using std::setw;
using std::tuple;
using std::vector;

static auto getLocal(string) -> size_t;
static auto setLocal(string) -> void;
static auto initBlock() -> void;
static auto pushBlock() -> void;
static auto popBlock() -> void;
static auto writeCode(Instruction) -> size_t;
static auto writeCode(Instruction, any) -> size_t;
static auto patchAddress(size_t) -> void;
static auto patchOperand(size_t, size_t) -> void;

static vector<Code> codeList;
static map<string, size_t> functionTable;
static list<map<string, size_t>> symbolStack;
static vector<size_t> offsetStack;
static size_t localSize;
static vector<vector<size_t>> continueStack;
static vector<vector<size_t>> breakStack;

auto generate(Program *program) -> tuple<vector<Code>, map<string, size_t>>
{
  codeList.clear();
  functionTable.clear();
  writeCode(Instruction::GetGlobal, string("main"));
  writeCode(Instruction::Call, static_cast<size_t>(0));
  writeCode(Instruction::Exit);
  for (auto &node : program->functions)
    node->generate();
  return {codeList, functionTable};
}

// 함수 코드를 생성하는 함수
auto Function::generate() -> void
{
  functionTable[name] = codeList.size();      // 함수 테이블에 함수 이름과 함수 코드의 시작 주소를 저장
  auto temp = writeCode(Instruction::Alloca); // 함수의 지역 변수를 위한 공간을 할당 (임시로 예약을 해둠)
  initBlock();                                // 함수의 지역 변수를 위한 블록을 초기화
  for (auto &name : parameters)               // 함수의 파라미터들을 지역 변수로 설정
    setLocal(name);
  for (auto &node : block)        // 함수의 본문을 생성
    node->generate();             // 함수의 본문을 생성
  popBlock();                     // 함수의 지역 변수를 위한 블록을 제거
  patchOperand(temp, localSize);  // 함수의 지역 변수를 위한 공간을 할당
  writeCode(Instruction::Return); // 함수의 반환
}

// 지역 변수를 처리하는 함수
auto Variable::generate() -> void
{
  setLocal(name);                                   // 지역 변수를 생성
  expression->generate();                           // 지역 변수의 초기값을 생성
  writeCode(Instruction::SetLocal, getLocal(name)); // 지역 변수에 초기값을 할당
  writeCode(Instruction::PopOperand);               // 스택 정리
}

auto For::generate() -> void
{
  breakStack.emplace_back();
  continueStack.emplace_back();
  pushBlock();
  variable->generate();
  auto jumpAddress = codeList.size();
  condition->generate();
  auto conditionJump = writeCode(Instruction::ConditionJump);
  for (auto &node : block)
    node->generate();
  auto continueAddress = codeList.size();
  expression->generate();
  writeCode(Instruction::PopOperand);
  writeCode(Instruction::Jump, jumpAddress);
  patchAddress(conditionJump);
  popBlock();
  for (auto &jump : continueStack.back())
    patchOperand(jump, continueAddress);
  continueStack.pop_back();
  for (auto &jump : breakStack.back())
    patchAddress(jump);
  breakStack.pop_back();
}

auto Continue::generate() -> void
{
  if (continueStack.empty())
    return;
  auto jumpCode = writeCode(Instruction::Jump);
  continueStack.back().push_back(jumpCode);
}

auto Break::generate() -> void
{
  if (breakStack.empty())
    return;
  auto jumpCode = writeCode(Instruction::Jump);
  breakStack.back().push_back(jumpCode);
}

auto If::generate() -> void
{
  vector<size_t> jumpList;
  for (size_t i = 0; i < conditions.size(); i++)
  {
    conditions[i]->generate();
    auto conditionJump = writeCode(Instruction::ConditionJump);
    pushBlock();
    for (auto &node : blocks[i])
      node->generate();
    popBlock();
    jumpList.push_back(writeCode(Instruction::Jump));
    patchAddress(conditionJump);
  }
  if (elseBlock.empty() == false)
  {
    pushBlock();
    for (auto &node : elseBlock)
      node->generate();
    popBlock();
  }
  for (auto &jump : jumpList)
    patchAddress(jump);
}

auto Print::generate() -> void
{
  for (auto i = arguments.size(); i > 0; i--)
    arguments[i - 1]->generate();
  writeCode(Instruction::Print, arguments.size());
  if (lineFeed)
    writeCode(Instruction::PrintLine);
}

auto Return::generate() -> void
{
  expression->generate();
  writeCode(Instruction::Return);
}

auto ExpressionStatement::generate() -> void
{
  expression->generate();
  writeCode(Instruction::PopOperand);
}

auto Or::generate() -> void
{
  lhs->generate();
  auto logicalOr = writeCode(Instruction::LogicalOr);
  rhs->generate();
  patchAddress(logicalOr);
}

auto And::generate() -> void
{
  lhs->generate();
  auto logicalAnd = writeCode(Instruction::LogicalAnd);
  rhs->generate();
  patchAddress(logicalAnd);
}

auto Relational::generate() -> void
{
  map<Kind, Instruction> instructions = {
      {Kind::Equal, Instruction::Equal},
      {Kind::NotEqual, Instruction::NotEqual},
      {Kind::LessThan, Instruction::LessThan},
      {Kind::GreaterThan, Instruction::GreaterThan},
      {Kind::LessOrEqual, Instruction::LessOrEqual},
      {Kind::GreaterOrEqual, Instruction::GreaterOrEqual}};
  lhs->generate();
  rhs->generate();
  writeCode(instructions[kind]);
}

auto Arithmetic::generate() -> void
{
  map<Kind, Instruction> instructions = {
      {Kind::Add, Instruction::Add},
      {Kind::Subtract, Instruction::Subtract},
      {Kind::Multiply, Instruction::Multiply},
      {Kind::Divide, Instruction::Divide},
      {Kind::Modulo, Instruction::Modulo},
  };
  lhs->generate();
  rhs->generate();
  writeCode(instructions[kind]);
}

auto Unary::generate() -> void
{
  map<Kind, Instruction> instructions = {
      {Kind::Add, Instruction::Absolute},
      {Kind::Subtract, Instruction::ReverseSign}};
  sub->generate();
  writeCode(instructions[kind]);
}

auto Call::generate() -> void
{
  for (auto i = arguments.size(); i > 0; i--)
    arguments[i - 1]->generate();
  sub->generate();
  writeCode(Instruction::Call, arguments.size());
}

auto GetElement::generate() -> void
{
  sub->generate();
  index->generate();
  writeCode(Instruction::GetElement);
}

auto SetElement::generate() -> void
{
  value->generate();
  sub->generate();
  index->generate();
  writeCode(Instruction::SetElement);
}

auto NullLiteral::generate() -> void
{
  writeCode(Instruction::PushNull);
}

auto BooleanLiteral::generate() -> void
{
  writeCode(Instruction::PushBoolean, value);
}

auto NumberLiteral::generate() -> void
{
  writeCode(Instruction::PushNumber, value);
}

auto StringLiteral::generate() -> void
{
  writeCode(Instruction::PushString, value);
}

auto ArrayLiteral::generate() -> void
{
  for (auto i = values.size(); i > 0; i--)
    values[i - 1]->generate();
  writeCode(Instruction::PushArray, values.size());
}

auto MapLiteral::generate() -> void
{
  for (auto &[key, value] : values)
  {
    writeCode(Instruction::PushString, key);
    value->generate();
  }
  writeCode(Instruction::PushMap, values.size());
}

auto GetVariable::generate() -> void
{
  if (getLocal(name) == SIZE_MAX)
    writeCode(Instruction::GetGlobal, name);
  else
    writeCode(Instruction::GetLocal, getLocal(name));
}

auto SetVariable::generate() -> void
{
  value->generate();
  if (getLocal(name) == SIZE_MAX)
    writeCode(Instruction::SetGlobal, name);
  else
    writeCode(Instruction::SetLocal, getLocal(name));
}

auto getLocal(string name) -> size_t // 지역 변수의 주소를 가져오는 함수
{
  for (auto &symbolTable : symbolStack) // 현재 블록부터 역순으로 검색
  {
    if (symbolTable.count(name)) // 지역 변수가 존재하면
      return symbolTable[name];  // 지역 변수의 주소를 반환
  }
  return SIZE_MAX; // 지역 변수가 존재하지 않으면 SIZE_MAX를 반환
}

auto setLocal(string name) -> void // 지역 변수를 생성하는 함수
{
  symbolStack.front()[name] = offsetStack.back(); // 지역 변수의 이름과 주소를 저장
  offsetStack.back() += 1;                        // 지역 변수의 주소를 증가
  localSize = max(localSize, offsetStack.back()); // 지역 변수의 주소를 증가
}

auto initBlock() -> void
{
  localSize = 0;
  offsetStack.push_back(0);
  symbolStack.emplace_front();
}

auto pushBlock() -> void
{
  symbolStack.emplace_front();
  offsetStack.push_back(offsetStack.back());
}

auto popBlock() -> void
{
  offsetStack.pop_back();
  symbolStack.pop_front();
}

auto writeCode(Instruction instruction) -> size_t
{
  codeList.push_back({instruction});
  return codeList.size() - 1;
}

auto writeCode(Instruction instruction, any operand) -> size_t
{
  codeList.push_back({instruction, operand});
  return codeList.size() - 1;
}

auto patchAddress(size_t codeIndex) -> void
{
  codeList[codeIndex].operand = codeList.size();
}

auto patchOperand(size_t codeIndex, size_t operand) -> void
{
  codeList[codeIndex].operand = operand;
}
