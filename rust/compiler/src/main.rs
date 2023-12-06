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
