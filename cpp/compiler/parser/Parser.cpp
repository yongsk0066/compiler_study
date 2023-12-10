#include <set>
#include <iostream>
#include "Token.h"
#include "Node.h"

using std::cout;
using std::set;

static auto parseFunction() -> Function *;

static auto parseBlock() -> vector<Statement *>;
static auto parseExpressionStatement() -> ExpressionStatement *;
static auto parseVariable() -> Variable *;
static auto parseFor() -> For *;
static auto parseIf() -> If *;
static auto parsePrint() -> Print *;
static auto parseReturn() -> Return *;
static auto parseBreak() -> Break *;
static auto parseContinue() -> Continue *;

static auto skipCurrent(Kind) -> void;
static auto skipCurrentIf(Kind) -> bool;

static vector<Token>::iterator current;

auto parse(vector<Token> tokens) -> Program *
{
  auto result = new Program();              // 프로그램 노드
  current = tokens.begin();                 // current를 tokens의 시작점으로 설정
  while (current->kind != Kind::EndOfToken) // current가 끝 토큰이 아닐 때까지
  {
    switch (current->kind) // current의 종류에 따라
    {
    case Kind::Function: // 함수 토큰이면
    {
      auto functionNode = parseFunction(); // 함수 노드를 파싱

      result->functions.push_back(functionNode); // 프로그램 노드에 함수 노드를 추가
      break;
    }
    default:
    {
      cout << *current << " 잘못된 구문입니다.";
      exit(1);
    }
    }
  }
  return result;
}

/*
  function main(x, y) {
    var z = x + y;
    return z;
  }
*/
auto parseFunction() -> Function * // 함수 노드를 파싱하는 함수
{
  auto result = new Function();          // 함수 노드 생성
  skipCurrent(Kind::Function);           // current가 function 토큰인지 확인
  result->name = current->string;        // 함수 이름 설정
  skipCurrent(Kind::Identifier);         // current가 identifier(main) 토큰인지 확인
  skipCurrent(Kind::LeftParen);          // current가 ( 토큰인지 확인
  if (current->kind != Kind::RightParen) // current가 ) 토큰이 아닐 때까지
  {
    do // 파라미터가 끝날 때까지 반복
    {
      result->parameters.push_back(current->string); // x, y를 파라미터로 추가
      skipCurrent(Kind::Identifier);                 // current가 identifier(x) 토큰인지 확인
    } while (skipCurrentIf(Kind::Comma));            // current가 , 토큰이면 다음 파라미터로 넘어감. 아니면 반복문 종료
  }
  skipCurrent(Kind::RightParen); // current가 ) 토큰인지 확인
  skipCurrent(Kind::LeftBrace);  // current가 { 토큰인지 확인
  result->block = parseBlock();  // 함수 본문을 파싱
  skipCurrent(Kind::RightBrace); // current가 } 토큰인지 확인
  return result;                 // 함수 노드 반환
}

auto parseBlock() -> vector<Statement *> // 함수 본문을 파싱하는 함수
{
  vector<Statement *> result;               // 함수 본문을 담을 벡터
  while (current->kind != Kind::RightBrace) // current가 } 토큰이 아닐 때까지
  {
    switch (current->kind) // current의 종류에 따라
    {
    default:
      result.push_back(parseExpressionStatement()); // 식 노드를 파싱
      break;
    case Kind::Variable:
      result.push_back(parseVariable()); // 변수 선언문 노드를 파싱
      break;
    case Kind::For:
      result.push_back(parseFor()); // for 문 노드를 파싱
      break;
    case Kind::If:
      result.push_back(parseIf()); // if 문 노드를 파싱
      break;
    case Kind::Print:
    case Kind::PrintLine:
      result.push_back(parsePrint()); // print 문 노드를 파싱
      break;
    case Kind::Return:
      result.push_back(parseReturn()); // return 문 노드를 파싱
      break;
    case Kind::Break:
      result.push_back(parseBreak()); // break 문 노드를 파싱
      break;
    case Kind::Continue:
      result.push_back(parseContinue()); // continue 문 노드를 파싱
      break;
    case Kind::EndOfToken:                       // current가 끝 토큰이면
      cout << *current << " 잘못된 구문입니다."; // 잘못된 구문이라고 출력
      exit(1);
    }
  }
  return result;
}

auto parseVariable() -> Variable * // 변수 선언문 노드를 파싱하는 함수
{
  auto result = new Variable();           // 변수 선언문 노드 생성
  skipCurrent(Kind::Variable);            // current가 variable 토큰인지 확인
  result->name = current->string;         // 변수 이름 설정
  skipCurrent(Kind::Identifier);          // current가 identifier(x) 토큰인지 확인
  skipCurrent(Kind::Assignment);          // current가 = 토큰인지 확인
  result->expression = parseExpression(); // 변수의 표현식을 파싱
  if (result->expression == nullptr)      // 변수의 표현식이 없으면
  {
    cout << "변수 선언에 초기화식이 없습니다."; // 에러 메시지 출력
    exit(1);                                    // 프로그램 종료
  }
  skipCurrent(Kind::Semicolon); // current가 ; 토큰인지 확인
  return result;                // 변수 선언문 노드 반환
}

// 식 노드를 파싱하는 함수

auto parseExpressionStatement() -> ExpressionStatement * // 단순표현식 노드를 파싱하는 함수
{
  auto result = new ExpressionStatement(); // 단순표현식 노드 생성
  result->expression = parseExpression();  // 식 노드를 파싱
  skipCurrent(Kind::Semicolon);            // current가 ; 토큰인지 확인
  return result;                           // 식 노드 반환
}

/*
  # 이항 연산자의 우선순위
  +-------+---------------------------------+
  | 우선순위 | 연산자                           |
  +-------+---------------------------------+
  | 1     | 대입 연산자 (=)                    |
  | 2     | 논리 OR 연산자 (||)                |
  | 3     | 논리 AND 연산자 (&&)               |
  | 4     | 관계 연산자 (==, !=, <, >, <=, >=) |
  | 5     | 산술 연산자 (+, -)                 |
  | 6     | 산술 연산자 (*, /, %)              |
  +-------+---------------------------------+
*/

/*
  # 구문트리 예시
  1 * 2 + 3 * 4
    +
    / \
   *   *
  / \ / \
  1 2 3  4

  우선 순위가 높은 연산자를 먼저 파싱하고, 그 다음에 우선 순위가 낮은 연산자를 파싱해야 한다.
*/

// 우선순위가 낮은 대입 연산자부터 분석 시작

auto parseExpression() -> Expression *
{
  return parseAssignment();
}

auto parseAssignment() -> Expression * // 대입 연산자 노드를 파싱하는 함수
{
  auto result = parseOr();               // 우선순위가 높은 OR 연산자 노드를 파싱
  if (current->kind != Kind::Assignment) // current가 = 토큰이 아니면
    return result;                       // OR 연산자 노드 반환
  skipCurrent(Kind::Assignment);         // current가 = 토큰이면
  // auto result = parseOr(); // OR 연산자 노드를 파싱
  // if (skipCurrentIf(Kind::Assignment)) // current가 = 토큰이면
  // {
  //   auto assignment = new Assignment(); // 대입 연산자 노드 생성
  //   assignment->lhs = result;           // 대입 연산자 노드의 lhs에 OR 연산자 노드를 설정
  //   assignment->rhs = parseAssignment(); // 대입 연산자 노드의 rhs에 대입 연산자 노드를 설정
  //   result = assignment;                 // OR 연산자 노드 대신 대입 연산자 노드를 반환
  // }
  // return result; // 대입 연산자 노드 반환
}

auto skipCurrent() -> void
{
  current++;
}

auto skipCurrent(Kind kind) -> void
{
  if (current->kind != kind)
  {
    cout << toString(kind) + " 토큰이 필요합니다.";
    exit(1);
  }
  current++;
}

auto skipCurrentIf(Kind token_kind) -> bool
{
  if (current->kind != token_kind)
    return false;
  current++;
  return true;
}
