#include <map>
#include <any>
#include <list>
#include <vector>
#include <functional>
#include "Datatype.h"
#include "Node.h"
#include <cmath>

using std::any;
using std::cout;
using std::endl;
using std::function;
using std::list;
using std::map;
using std::vector;

struct ReturnException
{
  any result; // 함수에서 반환되는 값을 보관하는 구조체
};
struct BreakException
{
  // 루프에서 'break'가 호출될 때 사용되는 예외
};
struct ContinueException
{
  // 루프에서 'continue'가 호출될 때 사용되는 예외
};

static map<string, any> global;                                      // 전역 변수를 저장하는 맵
static list<list<map<string, any>>> local;                           // 지역 변수를 저장하는 리스트
static map<string, Function *> functionTable;                        // 함수들을 저장하는 맵
extern map<string, function<any(vector<any>)>> builtinFunctionTable; // 내장 함수를 저장하는 맵

// 프로그램을 해석(interpret)하는 함수. 'main' 함수를 찾아 실행한다.
auto interpret(Program *program) -> void
{
  functionTable.clear(); // 함수 테이블을 초기화
  global.clear();        // 전역 변수를 초기화
  local.clear();         // 지역 변수를 초기화

  // 함수를 순회하면서 함수 테이블에 추가
  for (auto &node : program->functions)
    // 함수 이름을 키로, 함수 포인터를 값으로 저장
    functionTable[node->name] = node;
  // 'main' 함수가 없다면 실행하지 않음
  if (functionTable["main"] == nullptr)
    return;

  // 지역 변수 스택에 빈 맵을 추가
  /*
    initial local = []
    emplace_back()은  list의 맨 뒤에 새로운 요소를 추가하는 함수
    local = [[]]
    emplace_front()는 list의 맨 앞에 새로운 요소를 추가하는 함수
    local = [[{}]]
  */
  local.emplace_back().emplace_front();
  try
  {
    // 'main' 함수를 실행
    functionTable["main"]->interpret();
  }
  catch (ReturnException)
  {
    // 'return'이 호출되면 여기로 점프
  }
  catch (BreakException)
  {
    // 'break'가 호출되면 여기로 점프
  }
  catch (ContinueException)
  {
    // 'continue'가 호출되면 여기로 점프
  }
  // 지역 변수 스택에서 맨 위의 맵을 제거
  local.pop_back();
}

auto Function::interpret() -> void
{
  // Function 인스턴스의 Block속 Statement를 순회하며 interpret 하는 함수
  for (auto &node : block)
    node->interpret();
}

auto Return::interpret() -> void
{
  // Return을 해석
  // ReturnException을 담아 예외를 발생
  // 호출 스택을 통해 전파되어 함수를 중단 시키고 결과를 반환
  throw ReturnException{expression->interpret()}; // expression의 결과를 담아 예외를 발생
}

auto Variable::interpret() -> void
{
  // Variable을 해석
  // 변수를 선언하고 초기화
  // local.back() : 가장 최근 스코프
  // .front() : 가장 최근 스코프의 첫번째 요소
  // [name] = expression->interpret() : 변수의 이름을 키로, expression의 결과를 값으로 저장
  local.back().front()[name] = expression->interpret();
}

auto For::interpret() -> void
{
  // For문을 해석
  // local.back().emplace_front() : 가장 최근 스코프의 맨 앞에 빈 맵을 추가
  local.back().emplace_front();
  variable->interpret(); // 변수를 선언하고 초기화 for i = 0, i < 10, i = i + 1에서 i = 0; 부분)
  while (true)
  {
    auto result = condition->interpret(); // 조건식을 해석 for i = 0, i < 10, i = i + 1에서 i < 10 부분
    if (isTrue(result) == false)          // 조건식이 거짓이면 루프를 종료
      break;
    try
    {
      for (auto &node : block) // 블록을 순회하며 해석
        node->interpret();
    }
    catch (ContinueException)
    {
      // 'continue' 호출 시 다음 반복으로 점프
    }
    catch (BreakException)
    {
      // 'break' 호출 시 루프를 종료
      break;
    }
    expression->interpret(); // 증감식을 해석 for i = 0, i < 10, i = i + 1에서 i = i + 1 부분
  }
  // local.back().pop_front() : 가장 최근 스코프의 맨 앞에 있는 맵을 제거 (for문이 끝나면 변수를 제거)
  local.back().pop_front();
}

auto Break::interpret() -> void
{
  // Break를 해석
  // BreakException을 담아 예외를 발생
  throw BreakException();
}

auto Continue::interpret() -> void
{
  // Continue를 해석
  // ContinueException을 담아 예외를 발생
  throw ContinueException();
}

auto If::interpret() -> void
{
  // If문을 해석
  // if x < 10 { ... } else if x < 20 { ... } else { ... }
  for (size_t i = 0; i < conditions.size(); i++) // 조건식을 순회
  {
    auto result = conditions[i]->interpret(); // 조건식을 해석
    if (isTrue(result) == false)              // 조건식이 거짓이면 다음 조건식으로 점프  (if x < 10에서 x < 10이 거짓이면 else if x < 20으로 점프)
      continue;                               // 다음 조건식으로 점프
    local.back().emplace_front();             // 가장 최근 스코프의 맨 앞에 빈 맵을 추가
    for (auto &node : blocks[i])              // 블록을 순회하며 해석
      node->interpret();                      // 블록을 해석
    local.back().pop_front();                 // 가장 최근 스코프의 맨 앞에 있는 맵을 제거
    return;                                   // if문을 종료
  }
  if (elseBlock.empty())        // else 블록이 없으면 종료
    return;                     // if문을 종료
  local.back().emplace_front(); // 가장 최근 스코프의 맨 앞에 빈 맵을 추가
  for (auto &node : elseBlock)  // else 블록을 순회하며 해석
    node->interpret();          // 블록을 해석
  local.back().pop_front();     // 가장 최근 스코프의 맨 앞에 있는 맵을 제거
}

auto Print::interpret() -> void
{
  // Print를 해석
  // PrintLine을 해석
  // print x, y, z
  // printLine x, y, z

  for (auto &node : arguments) // 인수를 순회하며 해석
  {
    auto value = node->interpret(); // 인수를 해석
    cout << value;                  // 인수를 출력
  }
  if (lineFeed)   // 줄바꿈이 필요하면
    cout << endl; // 줄바꿈
}

auto ExpressionStatement::interpret() -> void
{
  // ExpressionStatement를 해석
  // 표현식을 해석하고 결과를 버림
  // x + y;
  expression->interpret(); // 표현식을 해석
}

auto Or::interpret() -> any
{
  // Or를 해석
  // x || y
  return isTrue(lhs->interpret()) ? true : rhs->interpret(); // 왼쪽 피연산자가 참이면 참, 아니면 오른쪽 피연산자를 반환
}

auto And::interpret() -> any
{
  // And를 해석
  // x && y
  return isFalse(lhs->interpret()) ? false : rhs->interpret(); // 왼쪽 피연산자가 거짓이면 거짓, 아니면 오른쪽 피연산자를 반환
}

auto Relational::interpret() -> any
{
  // Relational을 해석
  // x < y, x > y, x <= y, x >= y
  auto lValue = lhs->interpret();                              // 왼쪽 피연산자를 해석
  auto rValue = rhs->interpret();                              // 오른쪽 피연산자를 해석
  if (kind == Kind::Equal && isNull(lValue) && isNull(rValue)) // == 연산자이고, 두 피연산자가 모두 null이면
  {
    return true;
  }
  // == 연산자
  if (kind == Kind::Equal && isBoolean(lValue) && isBoolean(rValue)) // == 연산자이고, 두 피연산자가 모두 boolean이면
  {
    return toBoolean(lValue) == toBoolean(rValue); // 두 피연산자의 값을 비교 (true == true, false == false)
  }
  if (kind == Kind::Equal && isNumber(lValue) && isNumber(rValue)) // == 연산자이고, 두 피연산자가 모두 number이면
  {
    return toNumber(lValue) == toNumber(rValue); // 두 피연산자의 값을 비교 (1 == 1, 2 == 2)
  }
  if (kind == Kind::Equal && isString(lValue) && isString(rValue)) // == 연산자이고, 두 피연산자가 모두 string이면
  {
    return toString(lValue) == toString(rValue); // 두 피연산자의 값을 비교 ("hello" == "hello", "hello" == "world")
  }
  // != 연산자
  if (kind == Kind::NotEqual && isNull(lValue) && isNull(rValue)) // != 연산자이고, 두 피연산자가 모두 null이면
  {
    return false;
  }
  if (kind == Kind::NotEqual && isNull(lValue) || isNull(rValue)) // != 연산자이고, 두 피연산자 중 하나가 null이면
  {
    return true;
  }
  if (kind == Kind::NotEqual && isBoolean(lValue) && isBoolean(rValue)) // != 연산자이고, 두 피연산자가 모두 boolean이면
  {
    return toBoolean(lValue) != toBoolean(rValue); // 두 피연산자의 값을 비교 (true != false, false != true)
  }
  if (kind == Kind::NotEqual && isNumber(lValue) && isNumber(rValue)) // != 연산자이고, 두 피연산자가 모두 number이면
  {
    return toNumber(lValue) != toNumber(rValue); // 두 피연산자의 값을 비교 (1 != 2, 2 != 1)
  }
  if (kind == Kind::NotEqual && isString(lValue) && isString(rValue)) // != 연산자이고, 두 피연산자가 모두 string이면
  {
    return toString(lValue) != toString(rValue); // 두 피연산자의 값을 비교 ("hello" != "world", "hello" != "hello")
  }
  // < 연산자
  if (kind == Kind::LessThan && isNumber(lValue) && isNumber(rValue)) // < 연산자이고, 두 피연산자가 모두 number이면
  {
    return toNumber(lValue) < toNumber(rValue); // 두 피연산자의 값을 비교 (1 < 2, 2 < 1)
  }
  // > 연산자
  if (kind == Kind::GreaterThan && isNumber(lValue) && isNumber(rValue)) // > 연산자이고, 두 피연산자가 모두 number이면
  {
    return toNumber(lValue) > toNumber(rValue); // 두 피연산자의 값을 비교 (2 > 1, 1 > 2)
  }
  // <= 연산자
  if (kind == Kind::LessOrEqual && isNumber(lValue) && isNumber(rValue)) // <= 연산자이고, 두 피연산자가 모두 number이면
  {
    return toNumber(lValue) <= toNumber(rValue); // 두 피연산자의 값을 비교 (1 <= 2, 2 <= 2)
  }
  if (kind == Kind::GreaterOrEqual && isNumber(lValue) && isNumber(rValue)) // >= 연산자이고, 두 피연산자가 모두 number이면
  {
    return toNumber(lValue) >= toNumber(rValue); // 두 피연산자의 값을 비교 (2 >= 1, 2 >= 2)
  }
  return false; // 비교할 수 없는 경우 거짓
}

auto Arithmetic::interpret() -> any
{
  // Arithmetic을 해석
  // x + y, x - y, x * y, x / y, x % y
  auto lValue = lhs->interpret(); // 왼쪽 피연산자를 해석
  auto rValue = rhs->interpret(); // 오른쪽 피연산자를 해석
  // + 연산자
  if (kind == Kind::Add && isNumber(lValue) && isNumber(rValue)) // + 연산자이고, 두 피연산자가 모두 number이면
  {
    return toNumber(lValue) + toNumber(rValue); // 두 피연산자의 값을 더함 (1 + 2, 2 + 1)
  }
  if (kind == Kind::Add && isString(lValue) && isString(rValue)) // + 연산자이고, 두 피연산자가 모두 string이면
  {
    return toString(lValue) + toString(rValue); // 두 피연산자의 값을 이어붙임 ("hello" + "world")
  }
  // - 연산자
  if (kind == Kind::Subtract && isNumber(lValue) && isNumber(rValue)) // - 연산자이고, 두 피연산자가 모두 number이면
  {
    return toNumber(lValue) - toNumber(rValue); // 두 피연산자의 값을 뺌 (1 - 2, 2 - 1)
  }
  // * 연산자
  if (kind == Kind::Multiply && isNumber(lValue) && isNumber(rValue)) // * 연산자이고, 두 피연산자가 모두 number이면
  {
    return toNumber(lValue) * toNumber(rValue); // 두 피연산자의 값을 곱함 (1 * 2, 2 * 1)
  }
  // / 연산자
  if (kind == Kind::Divide && isNumber(lValue) && isNumber(rValue)) // / 연산자이고, 두 피연산자가 모두 number이면
  {
    return toNumber(rValue) == 0 ? 0.0 : toNumber(lValue) / toNumber(rValue); // 두 피연산자의 값을 나눔 (1 / 2, 2 / 1) (0으로 나누는 경우 0을 반환)
  }
  // % 연산자
  if (kind == Kind::Modulo && isNumber(lValue) && isNumber(rValue)) // % 연산자이고, 두 피연산자가 모두 number이면
  {
    return toNumber(rValue) == 0 ? toNumber(lValue) : fmod(toNumber(lValue), toNumber(rValue)); // 두 피연산자의 값을 나눈 나머지를 반환 (1 % 2, 2 % 1) (0으로 나누는 경우 왼쪽 피연산자를 반환)
  }
  return 0.0; // 연산할 수 없는 경우 0을 반환 왜 0.0인가? 0.0은 double형이고 0은 int형이기 때문에
}

auto Unary::interpret() -> any
{
  // Unary를 해석
  // !x, -x, +x
  auto value = sub->interpret();                 // 피연산자를 해석 (x)
  if (kind == Kind::Add && isNumber(value))      // + 연산자이고, 피연산자가 number이면
    return abs(toNumber(value));                 // 피연산자의 절댓값을 반환 (abs(-1) = 1)
  if (kind == Kind::Subtract && isNumber(value)) // - 연산자이고, 피연산자가 number이면
    return toNumber(value) * -1;                 // 피연산자의 음수를 반환  (-1 * -1 = 1)
  return 0.0;                                    // 연산할 수 없는 경우 0을 반환
}

auto Call::interpret() -> any
{
  // Call을 해석
  // sum(10, 20)
  auto value = sub->interpret(); // 함수를 해석
  if (isBuiltinFunction(value))  // 내장 함수라면 인수를 전달하고 실행
  {
    // length([1, 2, 3]) -> 3
    vector<any> values;                            // 인수를 저장할 벡터
    for (size_t i = 0; i < arguments.size(); i++)  // 인수를 순회
      values.push_back(arguments[i]->interpret()); // 인수를 해석하여 벡터에 추가
    return toBuiltinFunction(value)(values);       // 내장 함수를 실행
  }
  if (isFunction(value) == false) // 함수가 아니라면 종료
    return nullptr;
  map<string, any> parameters;                  // 매개변수를 저장할 맵 (sum(x, y)에서 x, y)
  for (size_t i = 0; i < arguments.size(); i++) // 인수를 순회
  {
    auto name = toFunction(value)->parameters[i]; // 매개변수 이름을 가져옴 (sum(x, y)에서 x, y)
    parameters[name] = arguments[i]->interpret(); // 매개변수 이름을 키로, 인수를 값으로 저장 (sum(10, 20)에서 x = 10, y = 20)
  }
  local.emplace_back().push_front(parameters); // 매개변수를 저장하는 맵을 스택에 추가
  try
  {
    toFunction(value)->interpret(); // 함수를 실행
  }
  catch (ReturnException exception) // 함수가 반환되면
  {
    local.pop_back();        // 매개변수를 저장하는 맵을 스택에서 제거
    return exception.result; // 반환된 값을 반환 (return 30;에서 10)
  }
  local.pop_back(); // 매개변수를 저장하는 맵을 스택에서 제거
  return nullptr;   // 반환된 값이 없으면 null을 반환
}

auto GetElement::interpret() -> any
{
  // GetElement를 해석
  // x[0], x["key"]

  auto object = sub->interpret();           // 객체를 해석 (x)
  auto index_ = index->interpret();         // 인덱스를 해석 (0)
  if (isArray(object) && isNumber(index_))  // 객체가 배열이고, 인덱스가 number이면
    return getValueOfArray(object, index_); // 배열에서 인덱스에 해당하는 값을 반환 (object[0])
  if (isMap(object) && isString(index_))    // 객체가 맵이고, 인덱스가 string이면
    return getValueOfMap(object, index_);   // 맵에서 인덱스에 해당하는 값을 반환 (object["key"])
  return nullptr;                           // 값을 찾을 수 없으면 null을 반환
}

auto SetElement::interpret() -> any
{
  // SetElement를 해석
  // x[0] = 10, x["key"] = 10
  auto object = sub->interpret();                   // 객체를 해석 (x)
  auto index_ = index->interpret();                 // 인덱스를 해석 (0)
  auto value_ = value->interpret();                 // 값을 해석 (10)
  if (isArray(object) && isNumber(index_))          // 객체가 배열이고, 인덱스가 number이면
    return setValueOfArray(object, index_, value_); // 배열에서 인덱스에 해당하는 값을 설정 (object[0] = 10)
  if (isMap(object) && isString(index_))            // 객체가 맵이고, 인덱스가 string이면
    return setValueOfMap(object, index_, value_);   // 맵에서 인덱스에 해당하는 값을 설정 (object["key"] = 10)
  return nullptr;                                   // 값을 설정할 수 없으면 null을 반환
}

auto GetVariable::interpret() -> any
{
  // GetVariable를 해석
  // x
  for (auto &variables : local.back()) // 스택의 맨 위부터 순회
  {
    if (variables.count(name)) // 변수를 찾으면
      return variables[name];  // 변수의 값을 반환
  }
  if (global.count(name))               // 전역 변수를 찾으면
    return global[name];                // 전역 변수의 값을 반환
  if (functionTable.count(name))        // 함수를 찾으면
    return functionTable[name];         // 함수의 포인터를 반환
  if (builtinFunctionTable.count(name)) // 내장 함수를 찾으면
    return builtinFunctionTable[name];  // 내장 함수의 포인터를 반환
  return nullptr;                       // 값을 찾을 수 없으면 null을 반환
}

auto SetVariable::interpret() -> any
{
  // SetVariable를 해석
  // x = 10
  for (auto &variables : local.back()) // 스택의 맨 위부터 순회 (지역 변수를 찾음)
  {
    if (variables.count(name))                     // 변수를 찾으면
      return variables[name] = value->interpret(); // 변수의 값을 설정하고 반환 (x = 10)
  }
  return global[name] = value->interpret(); // 변수를 찾을 수 없으면 전역 변수의 값을 설정하고 반환 (x = 10)
}

auto NullLiteral::interpret() -> any
{
  // NullLiteral를 해석
  return nullptr;
}

auto BooleanLiteral::interpret() -> any
{
  // BooleanLiteral를 해석
  return value; // true, false
}

auto NumberLiteral::interpret() -> any
{
  // NumberLiteral를 해석
  return value;
}

auto StringLiteral::interpret() -> any
{
  return value;
}

auto ArrayLiteral::interpret() -> any
{
  // ArrayLiteral를 해석
  // [1, 2, 3]
  auto result = new Array();                     // 배열을 생성
  for (auto &node : values)                      // 배열의 요소를 순회 (1, 2, 3)
    result->values.push_back(node->interpret()); // 요소를 해석하여 배열에 추가
  return result;                                 // 배열을 반환
}

auto MapLiteral::interpret() -> any
{
  // MapLiteral를 해석
  // { x: 10, y: 20 }
  auto result = new Map();                                    // 맵을 생성
  for (auto &[key, value] : values)                           // 맵의 요소를 순회 (key = x, value = 10)
    result->values.insert_or_assign(key, value->interpret()); // 요소를 해석하여 맵에 추가
  return result;
}
