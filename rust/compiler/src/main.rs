mod scanner;
mod token;

use scanner::scan;
use token::Token;

fn main() {
    let source_code = r#"
        function main() {
            print 'Hello, World!';
        }
    "#;
    let token_list = scan(source_code);
    print_token_list(&token_list);
}

fn print_token_list(token_list: &[Token]) {
    println!("{:<12}{}", "KIND", "STRING");
    println!("{:-<23}", "");
    for token in token_list {
        println!("{}", token);
    }
}


// #[derive(Debug, Clone, Copy)]
// enum CharType {
//     Unknown,
//     WhiteSpace,
//     NumberLiteral,
//     StringLiteral,
//     IdentifierAndKeyword,
//     OperatorAndPunctuator,
// }

// fn get_char_type(c: char) -> CharType {
//     match c {
//         ' ' | '\t' | '\n' | '\r' => CharType::WhiteSpace,
//         '0'..='9' => CharType::NumberLiteral,
//         '\''  => CharType::StringLiteral,
//         'a'..='z' | 'A'..='Z' | '_' => CharType::IdentifierAndKeyword,
//         '!'..='/' | ':'..='@' | '['..='`' | '{'..='~' if c != '\'' => CharType::OperatorAndPunctuator,
//         _ => CharType::Unknown,
//     }

// }

// fn main() {
//     let test_chars = vec!['a', 'Z', '0', '9', ' ', '\t', '\n', '!', '=', '+', '[', '}', '\''];

//     for c in test_chars {
//         let char_type = get_char_type(c);
//         println!("'{}' - {:?}", c, char_type);
//     }
// }
