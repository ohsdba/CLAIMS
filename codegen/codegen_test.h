/*
 * codegen_test.h
 *
 *  Created on: Mar 22, 2015
 *      Author: wangli
 */

#ifndef CODEGEN_TEST_H_
#define CODEGEN_TEST_H_
#include <gtest/gtest.h>
#include <map>
#include "CodeGenerator.h"
#include "../common/data_type.h"
#include "../common/Schema/Schema.h"
#include "../common/Expression/qnode.h"
#include "../common/Expression/initquery.h"
#include "ExpressionGenerator.h"

using std::map;

class CodeGenerationTest : public ::testing::Test{
protected:

	static void SetUpTestCase() {
		CodeGenerator::getInstance();
	}

	static void TearDownTestCase() {
		delete CodeGenerator::getInstance();
	}
};

TEST_F(CodeGenerationTest,AddInt){

	/* #      1# |2#
	 * Tuple: int|int
	 *         1   2
	 * Express: #1 + #2
	 */

	std::vector<column_type> columns;
	columns.push_back(data_type(t_int));
	columns.push_back(data_type(t_int));
	Schema* s=new SchemaFix(columns);
	map<std::string,int> column_index;
	column_index["a"]=0;
	column_index["b"]=1;

	QColcumns* a=new QColcumns("T","a",t_int,"a");
	QColcumns* b=new QColcumns("T","b",t_int,"b");

	QExpr_binary* op=new QExpr_binary(a,b,t_int,oper_add,t_qexpr_cal,"result");

	InitExprAtLogicalPlan(op,t_int,column_index,s);
	CodeGenerator::getInstance();

	expr_func_prototype f=getExprFunc(op,s);

	void* tuple=malloc(s->getTupleMaxSize());
	*(int*)tuple=1;
	*((int*)tuple+1)=2;
	int ret;
	f(tuple,&ret);
	delete tuple;
	delete s;
	delete op;
	EXPECT_EQ(ret,3);
}


TEST_F(CodeGenerationTest,AddInt2){
	/* #      #1 |#2 |#3
	 * Tuple: int|int|int
	 *         -1   2   3
	 * Express: #1 + #2 + #3
	 */
	std::vector<column_type> columns;
	columns.push_back(data_type(t_int));
	columns.push_back(data_type(t_int));
	columns.push_back(data_type(t_int));
	Schema* s=new SchemaFix(columns);
	map<std::string,int> column_index;
	column_index["#1"]=0;
	column_index["#2"]=1;
	column_index["#3"]=2;
	QColcumns* a=new QColcumns("T","#1",t_int,"#1");
	QColcumns* b=new QColcumns("T","#2",t_int,"#2");
	QColcumns* c=new QColcumns("T","#3",t_int,"#3");

	QExpr_binary* op1=new QExpr_binary(a,b,t_int,oper_add,t_qexpr_cal,"result");
	QExpr_binary* op2=new QExpr_binary(op1,c,t_int,oper_add,t_qexpr_cal,"result");

	InitExprAtLogicalPlan(op2,t_int,column_index,s);
	CodeGenerator::getInstance();

	expr_func_prototype f=getExprFunc(op2,s);

	void* tuple=malloc(s->getTupleMaxSize());
	*(int*)tuple=-1;
	*((int*)tuple+1)=2;
	*((int*)tuple+2)=3;
	int ret;
	f(tuple,&ret);
	delete tuple;
	delete s;
	delete op2;
	EXPECT_EQ(ret,4);
}
TEST_F(CodeGenerationTest,AddFloat){
	/* #      #1    |#2    |#3
	 * Tuple: float |float |float
	 *         0     -1.2    3.8
	 * Express: #1 + #2 + #3
	 */
	std::vector<column_type> columns;
	columns.push_back(data_type(t_float));
	columns.push_back(data_type(t_float));
	columns.push_back(data_type(t_float));
	Schema* s=new SchemaFix(columns);
	map<std::string,int> column_index;
	column_index["#1"]=0;
	column_index["#2"]=1;
	column_index["#3"]=2;
	QColcumns* a=new QColcumns("T","#1",t_float,"#1");
	QColcumns* b=new QColcumns("T","#2",t_float,"#2");
	QColcumns* c=new QColcumns("T","#3",t_float,"#3");

	QExpr_binary* op1=new QExpr_binary(a,b,t_float,oper_add,t_qexpr_cal,"result");
	QExpr_binary* op2=new QExpr_binary(op1,c,t_float,oper_add,t_qexpr_cal,"result");

	InitExprAtLogicalPlan(op2,t_float,column_index,s);

	expr_func_prototype f=getExprFunc(op2,s);

	void* tuple=malloc(s->getTupleMaxSize());
	*(float*)tuple=0;
	*((float*)tuple+1)=-1.2;
	*((float*)tuple+2)=3.8;
	float ret;
	f(tuple,&ret);
	delete tuple;
	delete s;
	delete op2;
	EXPECT_LE(abs(ret-2.6),0.00001);
}
TEST_F(CodeGenerationTest,AddFloatPromote){
	/* #      #1    |#2    |#3
	 * Tuple: int |float |float
	 *         1     -1.2    3.8
	 * Express: #1 + #2 + #3
	 */
	std::vector<column_type> columns;
	columns.push_back(data_type(t_int));
	columns.push_back(data_type(t_float));
	columns.push_back(data_type(t_float));
	Schema* s=new SchemaFix(columns);
	map<std::string,int> column_index;
	column_index["#1"]=0;
	column_index["#2"]=1;
	column_index["#3"]=2;
	QColcumns* a=new QColcumns("T","#1",t_int,"#1");
	QColcumns* b=new QColcumns("T","#2",t_float,"#2");
	QColcumns* c=new QColcumns("T","#3",t_float,"#3");

	QExpr_binary* op1=new QExpr_binary(a,b,t_float,oper_add,t_qexpr_cal,"result");
	QExpr_binary* op2=new QExpr_binary(op1,c,t_float,oper_add,t_qexpr_cal,"result");

	InitExprAtLogicalPlan(op2,t_float,column_index,s);

	expr_func_prototype f=getExprFunc(op2,s);

	void* tuple=malloc(s->getTupleMaxSize());
	*(int*)tuple=1;
	*((float*)tuple+1)=-1.2;
	*((float*)tuple+2)=3.8;
	float ret;
	f(tuple,&ret);
	delete tuple;
	delete s;
	delete op2;
	EXPECT_LE(abs(ret-3.6),0.00001);
}
TEST_F(CodeGenerationTest,AddFloatPromote2){
	/* #      #1    |#2    |#3
	 * Tuple: int |float |float
	 *         1     -1.2    3.8
	 * Express: #1 + #2 + #3
	 */
	std::vector<column_type> columns;
	columns.push_back(data_type(t_int));
	columns.push_back(data_type(t_int));
	columns.push_back(data_type(t_float));
	Schema* s=new SchemaFix(columns);
	map<std::string,int> column_index;
	column_index["#1"]=0;
	column_index["#2"]=1;
	column_index["#3"]=2;
	QColcumns* a=new QColcumns("T","#1",t_int,"#1");
	QColcumns* b=new QColcumns("T","#2",t_int,"#2");
	QColcumns* c=new QColcumns("T","#3",t_float,"#3");

	QExpr_binary* op1=new QExpr_binary(a,b,t_int,oper_add,t_qexpr_cal,"result");
	QExpr_binary* op2=new QExpr_binary(op1,c,t_float,oper_add,t_qexpr_cal,"result");

	InitExprAtLogicalPlan(op2,t_float,column_index,s);

	expr_func_prototype f=getExprFunc(op2,s);

	void* tuple=malloc(s->getTupleMaxSize());
	*(int*)tuple=1;
	*((int*)tuple+1)=-2;
	*((float*)tuple+2)=3.8;
	float ret;
	f(tuple,&ret);
	delete tuple;
	delete s;
	delete op2;
	EXPECT_LE(abs(ret-2.8),0.00001);
}
TEST_F(CodeGenerationTest,AddLongPromote){
	/* #      #1    |#2    |#3
	 * Tuple: int   |int   |long
	 *         1     -1     200
	 * Express: #1 + #2 + #3
	 */
	std::vector<column_type> columns;
	columns.push_back(data_type(t_int));
	columns.push_back(data_type(t_int));
	columns.push_back(data_type(t_u_long));
	Schema* s=new SchemaFix(columns);
	map<std::string,int> column_index;
	column_index["#1"]=0;
	column_index["#2"]=1;
	column_index["#3"]=2;
	QColcumns* a=new QColcumns("T","#1",t_int,"#1");
	QColcumns* b=new QColcumns("T","#2",t_int,"#2");
	QColcumns* c=new QColcumns("T","#3",t_u_long,"#3");

	QExpr_binary* op1=new QExpr_binary(a,b,t_int,oper_add,t_qexpr_cal,"result");
	QExpr_binary* op2=new QExpr_binary(op1,c,t_u_long,oper_add,t_qexpr_cal,"result");

	InitExprAtLogicalPlan(op2,t_u_long,column_index,s);

	expr_func_prototype f=getExprFunc(op2,s);

	void* tuple=malloc(s->getTupleMaxSize());
	*(int*)tuple=1;
	*((int*)s->getColumnAddess(1,tuple))=-1;
	*((long*)s->getColumnAddess(2,tuple))=200;
	long ret;
	f(tuple,&ret);
	delete tuple;
	delete s;
	delete op2;
	EXPECT_LE(ret,200);
}
TEST_F(CodeGenerationTest,SUB){
	/* #      #1    |#2    |#3
	 * Tuple: int   |int   |long
	 *         1     -1     200
	 * Express: #1 + #2 + #3
	 */
	std::vector<column_type> columns;
	columns.push_back(data_type(t_int));
	columns.push_back(data_type(t_int));
	columns.push_back(data_type(t_u_long));
	Schema* s=new SchemaFix(columns);
	map<std::string,int> column_index;
	column_index["#1"]=0;
	column_index["#2"]=1;
	column_index["#3"]=2;
	QColcumns* a=new QColcumns("T","#1",t_int,"#1");
	QColcumns* b=new QColcumns("T","#2",t_int,"#2");
	QColcumns* c=new QColcumns("T","#3",t_u_long,"#3");

	QExpr_binary* op1=new QExpr_binary(a,b,t_int,oper_minus,t_qexpr_cal,"result");
	QExpr_binary* op2=new QExpr_binary(op1,c,t_u_long,oper_minus,t_qexpr_cal,"result");

	InitExprAtLogicalPlan(op2,t_u_long,column_index,s);

	expr_func_prototype f=getExprFunc(op2,s);

	void* tuple=malloc(s->getTupleMaxSize());
	*(int*)tuple=1;
	*((int*)s->getColumnAddess(1,tuple))=-1;
	*((long*)s->getColumnAddess(2,tuple))=200;
	long ret;
	f(tuple,&ret);
	delete tuple;
	delete s;
	delete op2;
	EXPECT_LE(ret,-198);
}

#endif /* CODEGEN_TEST_H_ */