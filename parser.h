/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"

//////////////////////////////////////////////////////
// Data structures
//////////////////////////////////////////////////////

// variable/input table

typedef struct input_table {
	int next_i;
	std::vector<std::pair<std::string, int>> var_map;
	std::vector<int> input_map;

	int add_var(std::string var_name);
	void add_input(int in);
	int get_var(std::string str);
	input_table();

} input_table;

// polynomial components

typedef struct monomial {
	int var_name;	// index of the variable within the polynomial's parameters
	int exp; 		// the power to which the variable is raised
	monomial();
} monomial;

typedef struct term {
	int coefficient;
	std::vector<monomial*> m_list;
	char op;
	term();
} term;

// polynomial declaration table

typedef struct polynomial {
	std::string name;
	int decl_lineno;
	int param_i;
	std::map<int, std::string> param_names;
	std::vector<term*> polynomial_body;
	void add_param(std::string p);
	int get_param(std::string p);
	void get_var(std::string str);
	polynomial();
} polynomial;


// polynomial_evaluation DS
struct arg;

typedef struct poly_eval {
	int lineno;
	int poly;
	std::vector<arg*>* alist;
	poly_eval();
} poly_eval; 

typedef struct arg {
	TokenType etype;
	int value;
	int index;
	poly_eval* peval;
	arg();
} arg;

// statement DS
typedef struct stmt {
	TokenType stmt_type;
	poly_eval* pe;
	int variable;
	stmt* next;
	stmt();
} stmt;

class Parser {
  public:
	void execute_program(stmt* start);
	int evaluate_polynomial(poly_eval* pe);
	void parse_input(); 
	stmt* parse_program(); 
	void parse_poly_decl_section(); 
	polynomial* parse_poly_decl(); 
	void parse_polynomial_header(polynomial* p); 
	void parse_id_list(polynomial* p); 
	std::string parse_polynomial_name(); 
	void parse_polynomial_body(polynomial* p); 
	void parse_term_list(polynomial* p); 
	term* parse_term(polynomial* p); 
	void parse_monomial_list(term* tr, polynomial* p); 
	monomial* parse_monomial(polynomial* p); 
	int parse_exponent(); 
	char parse_add_operator(); 
	int parse_coefficient(); 
	stmt* parse_start(); 
	void parse_inputs(); 
	void parse_statement_list(stmt* st); 
	stmt* parse_statement(); 
	poly_eval* parse_poly_evaluation_statement(); 
	int parse_input_statement(); 
	poly_eval* parse_polynomial_evaluation(); 
	void parse_argument_list(poly_eval* pe); 
	arg* parse_argument(poly_eval* pe); 

  private:
	int errorno;
    LexicalAnalyzer lexer;
	input_table i_table;
	std::vector<polynomial*> p_table;
	std::vector<int> error_t;
	void checkE1();
	void error_code_2();
	void error_code_3();
	void error_code_4();
	void error_code_5();
    void syntax_error(int lineno);
	int get_polyname(std::string str);
    Token expect(TokenType expected_type);
};



#endif

