#pragma once
#include <any>
#include <map>
#include <vector>
#include <string>
#include "Token.h"

using std::any;
using std::map;
using std::string;
using std::vector;

// 프로그램 노드
struct Program
{
  // Function 포인터들을 담는 벡터
  vector<struct Function *> functions;
};

// 문 노드
struct Statement
{
  virtual auto print(int) -> void = 0;
  virtual auto interpret() -> void = 0;
};

// 식 노드
struct Expression
{
  virtual auto print(int) -> void = 0;
  virtual auto interpret() -> any = 0;
};

// 문을 상속받는 노드들 (함수)
struct Function : Statement
{
  // 함수 이름
  string name;
  // 함수의 파라미터들
  vector<string> parameters;
  // 함수 본문 (문으로 이루어진 벡터)
  vector<Statement *> block;
  auto print(int) -> void;
  auto interpret() -> void;
};

// Return 문
struct Return : Statement
{
  // 반환식
  // return x + y;에서 x + y에 해당
  Expression *expression;
  auto print(int) -> void;
  auto interpret() -> void;
};

// 변수 선언문
// var x = 10;
struct Variable : Statement
{
  // 변수 이름 x
  string name;

  // 변수의 표현식 10
  Expression *expression;
  auto print(int) -> void;
  auto interpret() -> void;
};

// For 문
// for(var i = 0; i < 10; i = i++) { ... }에서
struct For : Statement
{
  Variable *variable;        // var i = 0;
  Expression *condition;     // i < 10;
  Expression *expression;    // i = i++
  vector<Statement *> block; // { ... }
  auto print(int) -> void;
  auto interpret() -> void;
};

// Break, Continue 문
struct Break : Statement
{
  auto print(int) -> void;
  auto interpret() -> void;
};

struct Continue : Statement
{
  auto print(int) -> void;
  auto interpret() -> void;
};

// If 문
// if(x < 10) { ... } else if(x < 20) { ... } else { ... }에서
struct If : Statement
{
  vector<Expression *> conditions;    // x < 10, x < 20
  vector<vector<Statement *>> blocks; // { ... }, { ... }
  vector<Statement *> elseBlock;      // { ... }
  auto print(int) -> void;
  auto interpret() -> void;
};

// Print, PrintLine 문
// printLine(x, y, z)에서
struct Print : Statement
{
  bool lineFeed = false;          // 줄바꿈 여부 (printLine인지 print인지 구분)
  vector<Expression *> arguments; // x, y, z
  auto print(int) -> void;
  auto interpret() -> void;
};

// 단일표현식 문
/*
# 왜 ExpressionStatement가 필요한가?

## 표현식도 독립된 "문"으로 사용되는 경우

일부 표현식은 그 자체로 하나의 동작을 수행.
예를 들어, x + y;는 x와 y를 더한 값을 반환하는 표현식이다.
하지만, x + y;는 그 자체로 하나의 문이기도 하다.
이런 표현식을 단일표현식 문이라고 한다.

## ExpressionStatement의 필요성

이러한 표현식을 독립된 동작으로 처리할 수 있어야 합니다.
이를 위해 표현식을 감싸는 문의 형태가 필요한데,
이것이 바로 ExpressionStatement의 역할.

*/
struct ExpressionStatement : Statement
{
  Expression *expression;
  auto print(int) -> void;
  auto interpret() -> void;
};

// ------------------------------

// 식을 상속받는 노드들 (표현식)

// OR 연산자
// x || y
struct Or : Expression
{
  Expression *lhs; // x
  Expression *rhs; // y
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// AND 연산자
// x && y
struct And : Expression
{
  Expression *lhs; // x
  Expression *rhs; // y
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 관계 연산자
// x < y, x > y, x <= y, x >= y, x == y, x != y
struct Relational : Expression
{
  Kind kind;       // <, >, <=, >=, ==, !=
  Expression *lhs; // x
  Expression *rhs; // y
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 산술 연산자
// x + y, x - y, x * y, x / y, x % y
struct Arithmetic : Expression
{
  Kind kind;       // +, -, *, /, %
  Expression *lhs; // x
  Expression *rhs; // y
  auto print(int) -> void;
  auto interpret() -> std::any;
};

/*
  단락 평가 (Short-circuit evaluation)

  논리 연산자는 두 개의 피연산자를 가지는데,
  이 두 피연산자는 모두 평가되어야 한다.
  그러나, 논리 연산자는 두 번째 피연산자를 평가하지 않을 수 있다.
  이를 단락 평가라고 한다.

  단락 평가는 두 번째 피연산자가 첫 번째 피연산자만으로도 평가가 가능할 때,
  두 번째 피연산자를 평가하지 않는다.
  이는 두 번째 피연산자가 첫 번째 피연산자에 의해 결정되기 때문이다.

  예를 들어, x && y에서 x가 false라면,
  두 번째 피연산자인 y는 무조건 false이므로,
  두 번째 피연산자인 y를 평가하지 않아도 된다.
  이는 두 번째 피연산자인 y가 첫 번째 피연산자인 x에 의해 결정되기 때문이다.
*/

// 단항 연산자 (Unary Operator)
// !x, -x, +x
struct Unary : Expression
{
  Kind kind;       // !, -, +
  Expression *sub; // x
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 호출식 (Call Expression)
// f(x, y, z)
struct Call : Expression
{
  Expression *sub;                // f
  vector<Expression *> arguments; // x, y, z
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 참조식 (Reference Expression)
// array[0], object["key"]
struct GetElement : Expression
{
  Expression *sub;   // array, object
  Expression *index; // 0, "key"
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 수정식 (Modification Expression)
// array[0] = 10, object["key"] = 10
struct SetElement : Expression
{
  Expression *sub;   // array, object
  Expression *index; // 0, "key"
  Expression *value; // 10
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 변수 참조식 (Variable Reference Expression)
// x
struct GetVariable : Expression
{
  string name; // x
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 변수 수정식 (Variable Modification Expression)
// x = 10
struct SetVariable : Expression
{
  string name;       // x
  Expression *value; // 10
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 리터럴 (Literal)

// null 리터럴
// null
struct NullLiteral : Expression
{
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 불리언 리터럴
// true, false
struct BooleanLiteral : Expression
{
  bool value = false; // true, false
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 숫자 리터럴
// 10, 3.14
struct NumberLiteral : Expression
{
  double value = 0.0; // 10, 3.14
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 문자열 리터럴
// "Hello, World!"
struct StringLiteral : Expression
{
  string value; // "Hello, World!"
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 배열 리터럴
// [1, 2, 3]
struct ArrayLiteral : Expression
{
  vector<Expression *> values; // 1, 2, 3
  auto print(int) -> void;
  auto interpret() -> std::any;
};

// 객체 리터럴
// { x: 10, y: 20 }
struct MapLiteral : Expression
{
  map<string, Expression *> values; // x: 10, y: 20
  auto print(int) -> void;
  auto interpret() -> std::any;
};
