/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <vector>
#include <string>
#include "parser.h"

using namespace std;

//////////////////////////////////////////////////////
// input table functions
//////////////////////////////////////////////////////

input_table::input_table() {
	next_i = 0;
}

// adds variable to var_map if it doesn't exist
// returns index of variable in var_map
int input_table::add_var(string var_name) {
	bool isInTable = false;
	int index = 0;
	for (auto& it : var_map) {
		if(it.first.compare(var_name) == 0) {
			isInTable = true;
			break;
		}
		index++;
	}

	if (!isInTable) {
		var_map.push_back(make_pair(var_name, -1));
	}

	return index;
}

void input_table::add_input(int in) {
		input_map.push_back(in);
		next_i += 1;
	return;
}

// returns index of variable (aka "str") in input_map
int input_table::get_var(string str) {
	for (int i = 0; i < var_map.size(); i++) {
		if(var_map[i].first.compare(str) == 0) 
			return i;
	}
	return -1;
}



//////////////////////////////////////////////////////
// polynomial functions
//////////////////////////////////////////////////////

void polynomial::add_param(string p) {
	param_names[param_i] = p;
	param_i += 1;
}

int polynomial::get_param(string p) {
	for (int i = 0; i < param_i; i++) {
		if (param_names[i].compare(p) == 0)
			return i;
	}

	return -1;
}

polynomial::polynomial() {
	name = "";
	param_i = 0;
}

//////////////////////////////////////////////////////
// monomial functions
//////////////////////////////////////////////////////

monomial::monomial() {
	var_name = 0;
	exp = 1;
}

//////////////////////////////////////////////////////
// term functions
//////////////////////////////////////////////////////

term::term() {
	coefficient = 1;
	op = 0;
}

//////////////////////////////////////////////////////
// poly_eval functions
//////////////////////////////////////////////////////

poly_eval::poly_eval() {
	poly = 0;
}
//
//////////////////////////////////////////////////////
// arg functions
//////////////////////////////////////////////////////

arg::arg() {
	etype = POLY;
	value = 0;
	index = 0;
	peval = new poly_eval();
}

//////////////////////////////////////////////////////
// stmt functions
//////////////////////////////////////////////////////

stmt::stmt() {
	stmt_type = INPUT;
	pe = 0;
	variable = 0;
	next = 0;
}


//////////////////////////////////////////////////////
// Error functions
//////////////////////////////////////////////////////


void Parser::syntax_error(int lineno)
{
    cout << "SYNTAX ERROR !&%!\n";
	//printf("called from line number: %d\n", lineno);
    exit(1);
}

// Error code 1 indicates that a polynomial has been declared more than once,
// and outputs the line numbers in which the duplicate (incl. original) 
// definitions of said polynomials occur.
void Parser::checkE1() {
	multimap<string, int> decls;
	map<string, int> uniqueKeys;
	vector<int> linenos;

	for (int i = 0; i < p_table.size(); i++) {
		decls.insert(pair<string, int>(p_table[i]->name, p_table[i]->decl_lineno));
		uniqueKeys[p_table[i]->name] += 1;
	}


	for (map<string, int>::iterator unique_it = uniqueKeys.begin(); unique_it != uniqueKeys.end(); unique_it++) {
		if (unique_it->second > 1) {

			auto key_it = decls.equal_range(unique_it->first);

			for (auto key_its = key_it.first; key_its != key_it.second; key_its++) {
				linenos.push_back(key_its->second);
			}
		}
	}

	if (linenos.size() != 0) {
		errorno = 1;
		sort(linenos.begin(), linenos.end());
		printf("Error Code 1: ");
		for (int i = 0; i < linenos.size(); i++)
			printf("%d ", linenos[i]);
	}

	if (errorno != 0)
		exit(1);
}


// Error code 2 indicates that a polynomial declaration includes a variable that
// is not specified in the polynomial header (e.g. F(x, y) = z^2 + x + y).
// Note: not specifying arguments of a polynomial results in a default variable of "x",
// so "F = x + y" would also trigger this error.
void Parser::error_code_2() {
	cout << "Error Code 2: ";

	sort(error_t.begin(), error_t.end());

	// print line numbers
	for (int i = 0; i < error_t.size(); i++) {
		printf("%d ", error_t[i]);
	}

	exit(1);
}

// Error code 3 indicates that a polynomial being attempted to be evaluated has not been declared.
void Parser::error_code_3() {
	cout << "Error Code 3: ";

	sort(error_t.begin(), error_t.end());

	// print line numbers
	for (int i = 0; i < error_t.size(); i++) {
		printf("%d ", error_t[i]);
	}

	exit(1);
}

// Error code 4 indicates that the number of arguments specified in a polynomial evaluation differs
// from the declaration of said polynomial.
void Parser::error_code_4() {
	cout << "Error Code 4: ";

	sort(error_t.begin(), error_t.end());

	// print line numbers
	for (int i = 0; i < error_t.size(); i++) {
		printf("%d ", error_t[i]);
	}

	exit(1);
}

// Error code 5 indicates that an argument within a polynomial evaluation has not been initialized.
// Note: if numerous violations happen on one line, output the line number twice.
void Parser::error_code_5() {
	cout << "Error Code 5: ";

	sort(error_t.begin(), error_t.end());

	// print line numbers
	for (int i = 0; i < error_t.size(); i++) {
		printf("%d ", error_t[i]);
	}

	exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error(__LINE__);

    return t;
}

int Parser::get_polyname(string str){
	int i = 0;
	for (auto it : p_table) {
		if(it->name.compare(str) == 0) {
			return i;
		}
		i += 1;
	}

	errorno = 3;
	return -1;
}

//////////////////////////////////////////////////////
// Execution
//////////////////////////////////////////////////////

void Parser::execute_program(stmt* start) {
	stmt* pc;
	int v;
	pc = start->next;
	int next_input = 0;
	while (pc != NULL) {
		switch(pc->stmt_type) {
			// case when poly-eval statement
			case POLY:
				v = evaluate_polynomial(pc->pe);
				cout << v << endl;
				break;

			// case when input statement
			case INPUT:
				i_table.var_map[pc->variable].second = next_input;
				next_input += 1;
				break;	
		}

		pc = pc->next;
	}
}

int Parser::evaluate_polynomial(poly_eval* pe) {
	int result = 0;
	int curr_val = 1;
	int op1 = 0;
	int op2 = 0;
	char op = 0;

	// get polynomial template
	polynomial* pn = p_table[pe->poly];

	for (auto curr_term : pn->polynomial_body) {
		if (curr_term->m_list.size() > 0) {
			for (auto curr_mon : curr_term->m_list) {
				// handle case when argument == poly eval
				if ((*pe->alist)[curr_mon->var_name]->etype == POLY) {
					op1 = evaluate_polynomial((*pe->alist)[curr_mon->var_name]->peval);
				// handle case when arg == ID
				} else if ((*pe->alist)[curr_mon->var_name]->etype == ID) {
					op1 = i_table.input_map[i_table.var_map[(*pe->alist)[curr_mon->var_name]->index].second];
				// handle case when arg == NUM
				} else {
					op1 = (*pe->alist)[curr_mon->var_name]->value;
				}
					op2 = (int)pow(op1, curr_mon->exp);
					curr_val *= op2;
			}
		} else {
			curr_val = 1;
		}

		if (op != 0) {
			switch (op) {
				case '+':
					result += curr_val * curr_term->coefficient;
					break;
				case '-':
					result -= curr_val * curr_term->coefficient;
					break;
			}
		} else
			result = curr_val * curr_term->coefficient;

		op = curr_term->op;
		curr_val = 1;
	}

	return result;
}

//////////////////////////////////////////////////////
// Parsing
//////////////////////////////////////////////////////

// input -> program inputs
void Parser::parse_input() {
	errorno = 0;
	stmt* st_list = parse_program();
	parse_inputs();
	bool hasErrors = false;

	Token t = lexer.peek(1);

	if (t.token_type != END_OF_FILE)
		syntax_error(__LINE__);

	if (errorno == 0)
		checkE1();

	switch(errorno) {
		case 2:
			error_code_2();
			break;
		case 3:
			error_code_3();
			break;
		case 4:
			error_code_4();
			break;
		case 5:
			error_code_5();
			break;
	}

	if (errorno == 0)
		execute_program(st_list);

}

// program -> poly_decl_section start
stmt* Parser::parse_program() {
	parse_poly_decl_section();
	stmt* head = parse_start();
	return head;
}

// poly_decl_section -> poly_decl | poly_decl poly_decl_section
void Parser::parse_poly_decl_section() {
	p_table.push_back(parse_poly_decl());

	Token t = lexer.peek(1);

	// determine if poly_decl_section via: POLY
	if (t.token_type == POLY) {
		parse_poly_decl_section();
		return;
	}
}

// poly_decl -> POLY polynomial_header EQUAL polynomial_body SEMICOLON
polynomial* Parser::parse_poly_decl() {
	polynomial* p = new polynomial;
	
	Token t = lexer.peek(1);
	
	// determine if next token is POLY
	if (t.token_type == POLY) {
		lexer.GetToken();
		t = lexer.peek(1);
	} else
		syntax_error(__LINE__);

	parse_polynomial_header(p);
	t = lexer.peek(1);
	
	// determine if next token is EQUAL
	if (t.token_type == EQUAL) {
		lexer.GetToken();
		t = lexer.peek(1);
	} else
		syntax_error(__LINE__);

	parse_polynomial_body(p);
	t = lexer.peek(1);

	// determine if next token is SEMICOLON
	if (t.token_type == SEMICOLON) {
		lexer.GetToken();
		t = lexer.peek(1);
	} else
		syntax_error(__LINE__);

	return p;
}

// polynomial_header -> polynomial_name | polynomial_name LPAREN id_list RPAREN
void Parser::parse_polynomial_header(polynomial* p) {
	Token t = lexer.peek(1);
	// check for polynomial_name via: ID token
	if (t.token_type == ID) {
		p->name = parse_polynomial_name();
		p->decl_lineno = t.line_no;
		t = lexer.peek(1);

		// determine if LPAREN is next token
		if (t.token_type == LPAREN) {
			lexer.GetToken();
			t = lexer.peek(1);

			// determine if id_list via: ID token
			if (t.token_type == ID) {
				parse_id_list(p);
				t = lexer.peek(1);

				// determine if RPAREN is next token
				if (t.token_type == RPAREN) {
					lexer.GetToken();
					t = lexer.peek(1);
					return;

					// determine if follow (EQUAL) is correct
					if (t.token_type != EQUAL)
						syntax_error(__LINE__);

				} else {
					syntax_error(__LINE__);
				}
			} else {
				syntax_error(__LINE__);
			}
		}
		p->add_param("x");
		return;
	}

	syntax_error(__LINE__);
}

// id_list -> ID | ID COMMA id_list
void Parser::parse_id_list(polynomial* p) {
	Token t = lexer.peek(1);

	// determine if ID is next token
	if (t.token_type == ID) {
		p->add_param(t.lexeme);
		lexer.GetToken();
		t = lexer.peek(1);

		// determine if COMMA is next token
		if (t.token_type == COMMA) {
			lexer.GetToken();
			t = lexer.peek(1);

			parse_id_list(p);
			t = lexer.peek(1);

			// determine if follow (RPAREN) is correct
			if (t.token_type != RPAREN) 
				syntax_error(__LINE__);

		// determine if follow (RPAREN) is correct
		} else if (t.token_type != RPAREN)
			syntax_error(__LINE__);

		return;
	}

	syntax_error(__LINE__);
}

// polynomial_name -> ID
string Parser::parse_polynomial_name() {
	Token t = lexer.peek(1);

	// determine if next ID is next token
	if (t.token_type == ID) {
		lexer.GetToken();

		return t.lexeme;
	}

	syntax_error(__LINE__);
	return 0;
}

// polynomial_body -> term_list
void Parser::parse_polynomial_body(polynomial* p) {
	Token t = lexer.peek(1);

	// determine if term_list via: ID, NUM token(s)
	if (t.token_type == ID || t.token_type == NUM) {
		parse_term_list(p);
		t = lexer.peek(1);

		// determine if follow (SEMICOLON) is correct
		if (t.token_type != SEMICOLON)
			syntax_error(__LINE__);

		return;
	}

	syntax_error(__LINE__);
}

// term_list -> term | term add_operator term_list
void Parser::parse_term_list(polynomial* p) {
	Token t = lexer.peek(1);
	term* tr = new term;

	// determine if term via: ID, NUM token(s)
	if (t.token_type == ID || t.token_type == NUM) {
		tr = parse_term(p);
		t = lexer.peek(1);

		// determine if add_operator via: PLUS, MINUS token(s)
		if (t.token_type == PLUS || t.token_type == MINUS) {
			tr->op = parse_add_operator();
			p->polynomial_body.push_back(tr);
			t = lexer.peek(1);
			
			// determine if term_list via: ID, NUM token(s)
			if (t.token_type == ID || t.token_type == NUM) {
				parse_term_list(p);
				t = lexer.peek(1);
				return;

			} else {
				syntax_error(__LINE__);
			}

		}
		
		p->polynomial_body.push_back(tr);
		return; 
	}

	syntax_error(__LINE__);
}

// term -> monomial_list | coefficient monomial_list | coefficient
term* Parser::parse_term(polynomial* p) {
	Token t = lexer.peek(1);
	term* tr = new term;

	// determine if monomial_list via: ID token
	if (t.token_type == ID) {
		parse_monomial_list(tr, p);
		tr->coefficient = 1;
		return tr;

	// determine if coefficient via: NUM token
	} else if (t.token_type == NUM) {
		tr->coefficient = parse_coefficient();
		t = lexer.peek(1);

		// determine if monomial_list via: ID token
		if (t.token_type == ID) {
			parse_monomial_list(tr, p);
		}

		return tr;
	}

	syntax_error(__LINE__);
	return 0;
}

// monomial_list -> monomial | monomial monomial_list
void Parser::parse_monomial_list(term* tr, polynomial* p) {
	Token t = lexer.peek(1);

	// determine if monomial via: ID token
	if (t.token_type == ID) {
		tr->m_list.push_back(parse_monomial(p));
		t = lexer.peek(1);

		// determine if monomial_list via: ID token
		if (t.token_type == ID) {
			parse_monomial_list(tr, p);
			return;
		}

		return;
	}

	syntax_error(__LINE__);
}

// monomial -> ID | ID exponent 
monomial* Parser::parse_monomial(polynomial* p) {
	Token t = lexer.peek(1);
	monomial* m = new monomial;

	// determine if ID is next token
	if (t.token_type == ID) {
		m->var_name = p->get_param(t.lexeme);
		if(m->var_name == -1) {
			errorno = 2;
			error_t.push_back(t.line_no);
		}
		lexer.GetToken();
		t = lexer.peek(1);

		// determine if exponent via: POWER token
		if (t.token_type == POWER) {
			m->exp = parse_exponent();
			return m;
		} else
			m->exp = 1;

		return m;
	}

	syntax_error(__LINE__);
	return 0;
}

// exponent -> POWER NUM
int Parser::parse_exponent() {
	Token t = lexer.peek(1);

	// determine if POWER is next token
	if (t.token_type == POWER) {
		lexer.GetToken();
		t = lexer.peek(1);

		// determine if NUM is next token
		if (t.token_type == NUM) {
			lexer.GetToken();
			return stoi(t.lexeme);
		}
	}

	syntax_error(__LINE__);
	return 0;
}

// add_operator -> PLUS | MINUS
char Parser::parse_add_operator() {
	Token t = lexer.peek(1);

	// determine if PLUS or MINUS
	if (t.token_type == PLUS) {
		lexer.GetToken();
		return '+';
	} else if (t.token_type == MINUS) {
		lexer.GetToken();
		return '-';
	}

	syntax_error(__LINE__);
	return 0;
}

// coefficient -> NUM
int Parser::parse_coefficient() {
	Token t = lexer.peek(1);

	// determine if token is NUM token
	if (t.token_type == NUM) {
		lexer.GetToken();
		return stoi(t.lexeme);
	}
	
	syntax_error(__LINE__);
	return 0;
}

// }}}

// start -> START statement_list
stmt* Parser::parse_start() {
	Token t = lexer.peek(1);
	stmt* head = new stmt;
	stmt* st = head;

	// determine if next token is START
	if (t.token_type == START) {
		lexer.GetToken();
		t = lexer.peek(1);

		// determine if statement_list via: INPUT, ID token(s)
		if (t.token_type == INPUT || t.token_type == ID) {
			parse_statement_list(st);
			return head;
		}
	}

	syntax_error(__LINE__);
	return 0;
}

// inputs -> NUM | NUM inputs
void Parser::parse_inputs() {
	Token t = lexer.peek(1);

	// determine if next token is NUM
	if (t.token_type == NUM) {
		i_table.add_input(stoi(t.lexeme));
		lexer.GetToken();
		t = lexer.peek(1);

		// determine if inputs via: NUM token
		if (t.token_type == NUM) {
			parse_inputs();
			return;
		}
	} else
		syntax_error(__LINE__);

	return;
}

// statement_list -> statement | statement statement_list
void Parser::parse_statement_list(stmt* stm) {
	Token t = lexer.peek(1);

	// determine if statement via: INPUT, ID token(s)
	if (t.token_type == INPUT || t.token_type == ID) {
		stm->next = parse_statement();
		t = lexer.peek(1);

		// determine if statement_list via: INPUT, ID token(s)
		if (t.token_type == INPUT || t.token_type == ID) {
			parse_statement_list(stm->next);
			return;
		}

		return;
	}

	syntax_error(__LINE__);
}

// statement -> input_statement | poly_evaluation_statement
stmt* Parser::parse_statement() {
	stmt* st = new stmt;
	Token t = lexer.peek(1);

	// determine if input_statement via: INPUT token
	if (t.token_type == INPUT) {
		st->stmt_type = INPUT;
		st->pe = NULL;
		st->variable = parse_input_statement(); 
		return st;
	
	// determine if poly_evaluation_statement via: ID token
	} else if (t.token_type == ID) {
		st->stmt_type = POLY;
		poly_eval* poly_e = parse_poly_evaluation_statement(); 
		st->pe = poly_e;

		return st;
	}

	syntax_error(__LINE__);
	return 0;
}

// poly_evaluation_statement -> polynomial_evaluation SEMICOLON
poly_eval* Parser::parse_poly_evaluation_statement() {
	Token t = lexer.peek(1);
	poly_eval* pe = new poly_eval;

	// determine if polynomial_evaluation via ID token
	if (t.token_type == ID) {
		pe = parse_polynomial_evaluation();
		t = lexer.peek(1);

		// determine if SEMICOLON token
		if (t.token_type == SEMICOLON) {
			lexer.GetToken();
			return pe;
		}
	}

	syntax_error(__LINE__);
	return 0;
}

// input_statement -> INPUT ID SEMICOLON
int Parser::parse_input_statement() {
	Token t = lexer.peek(1);

	// determine if token is INPUT token
	if (t.token_type == INPUT) {
		lexer.GetToken();
		t = lexer.peek(1);

		// determine if token is ID token 
		if (t.token_type == ID) {
			int var_loc = 0;
			var_loc = i_table.add_var(t.lexeme);
			lexer.GetToken();
			t = lexer.peek(1);

			// determine if next token is SEMICOLON
			if (t.token_type == SEMICOLON) {
				lexer.GetToken();
				return var_loc;
			}
		}
	}

	syntax_error(__LINE__);
	return 0;
}

// polynomial_evaluation -> polynomial_name LPAREN argument_list RPAREN
poly_eval* Parser::parse_polynomial_evaluation() {
	Token t = lexer.peek(1);
	poly_eval* pe = new poly_eval;
	pe->alist = new vector<arg*>();

	// determine if polynomial_name via: ID token
	if (t.token_type == ID) {
		pe->poly = get_polyname(parse_polynomial_name());
		pe->lineno = t.line_no;

		if ((pe->poly == -1) && (errorno == 3))
			error_t.push_back(t.line_no);

		t = lexer.peek(1);

		// determine if next token is LPAREN
		if (t.token_type == LPAREN) {
			lexer.GetToken();
			t = lexer.peek(1);

			// determine if argument_list via: ID, NUM token(s)
			if (t.token_type == ID || t.token_type == NUM) {
				parse_argument_list(pe);

				if ((pe->poly != -1) && (pe->alist->size() != p_table[pe->poly]->param_names.size())) {
					error_t.push_back(pe->lineno);
					errorno = 4;
				}

				t = lexer.peek(1);

				// determine if next token is RPAREN
				if (t.token_type == RPAREN) {
					lexer.GetToken();
					return pe;
				}
			}
		}
	}

	if (errorno != 3)
		syntax_error(__LINE__);

	return 0;
}

// argument_list -> argument | argument COMMA argument_list
void Parser::parse_argument_list(poly_eval* pe) {
	Token t = lexer.peek(1);

	vector<arg*>* alist = pe->alist;

	// determine if argument via: ID, NUM token(s)
	if (t.token_type == ID || t.token_type == NUM) {
		alist->push_back(parse_argument(pe));
		t = lexer.peek(1);

		// determine if next token is COMMA
		if (t.token_type == COMMA) {
			lexer.GetToken();
			t = lexer.peek(1);

			// determine if argument_list via: ID, NUM token(s)
			if (t.token_type == ID || t.token_type == NUM) {
				parse_argument_list(pe);
				return;

			} else {
				syntax_error(__LINE__);
			}
		} 

		return;
	}

	syntax_error(__LINE__);
}

// argument -> ID | NUM | polynomial_evaluation
arg* Parser::parse_argument(poly_eval* pe) {
	Token t = lexer.peek(1);
	arg* a = new arg;
	// determine if next token is ID
	if (t.token_type == ID) {
		Token p = lexer.peek(2);

		// determine if polynomial_evaluation via LPAREN token
		if (p.token_type == LPAREN) {
			a->etype = POLY;
			a->peval = parse_polynomial_evaluation();
			return a;
		// case when arg is an ID
		} else {
			a->etype = ID;
			//a->index = p_table[pe->poly]->get_param(t.lexeme);
			a->index = i_table.get_var(t.lexeme);
			if (a->index == -1) {
				errorno = 5;
				error_t.push_back(t.line_no);
			}
			a->peval = 0;
			lexer.GetToken();
			return a;
		}

	// determine if next token is NUM
	} else if (t.token_type == NUM) {
		a->etype = NUM;
		a->value = stoi(t.lexeme);
		lexer.GetToken();
		return a;

	} else
		syntax_error(__LINE__);
	
	return 0;

}

int main() {
	Parser parser;
	parser.parse_input();
	return(0);
}
