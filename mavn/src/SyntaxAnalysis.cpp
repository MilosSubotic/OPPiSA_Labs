#include "SyntaxAnalysis.h"
#include <iostream>

using namespace std;


SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis& lex) : lexicalAnalysis(lex), 
	errorFound(false), 
	m_instrs(getInstructions()), 
	m_memVars(getMemVariables()), 
	m_regVars(getRegVariables()), 
	m_labels(getLabels()),
	m_instrPosition(0),
	m_regVarPosition(0)
{
	tokenIterator = lexicalAnalysis.getTokenList().begin();
}


bool SyntaxAnalysis::Do()
{
	currentToken = getNextToken();
	q();
	
	// check if anything has been analyzed:
	if (--tokenIterator == lexicalAnalysis.getTokenList().begin())
		return false;
	else
		return !errorFound;
}


void SyntaxAnalysis::printSyntaxError(Token& token)
{
	string msg = "Syntax error! Token: " + token.getValue() + " unexpected";
	throw runtime_error(msg);
}


void SyntaxAnalysis::eat(TokenType t)
{
	if (currentToken.getType() == t)
	{
		//currentToken.printTokenValue();
		if (t != T_END_OF_FILE)
			currentToken = getNextToken();
	}
	else
	{
		printSyntaxError(currentToken);
		errorFound = true;
	}
}


Token SyntaxAnalysis::getNextToken()
{
	if (tokenIterator == lexicalAnalysis.getTokenList().end())
		throw runtime_error("End of input file reached");
	return *tokenIterator++;
}


void SyntaxAnalysis::q()
{
	s();
	eat(T_SEMI_COL);
	l();
}


void SyntaxAnalysis::s()
{
	Token name;
	Token val;
	switch(currentToken.getType())
	{
	case T_MEM:
		eat(T_MEM);
		name = currentToken;
		eat(T_M_ID);
		val = currentToken;
		eat(T_NUM);
		m_memVars.push_back(new MemoryVariable(name.getValue(), val.getValue()));
		break;
	case T_REG:
		eat(T_REG);
		name = currentToken;
		eat(T_R_ID);
		m_regVars.push_back(new RegisterVariable(name.getValue(), m_regVarPosition++));
		break;
	case T_FUNC:
		eat(T_FUNC);
		name = currentToken;
		eat(T_ID);
		getFunctionName().assign(name.getValue());
		break;
	case T_ID:
		name = currentToken;
		eat(T_ID);
		m_labels.push_back(new Label(name.getValue(), m_instrPosition));
		eat(T_COL);
		m_instrs.push_back(e());
		break;
	default:
		m_instrs.push_back(e());
		break;
	}
}


Instruction* SyntaxAnalysis::e()
{
	Variables dstList;
	Variables srcList;

	Variable* dst = NULL;
	Variable* src1 = NULL;
	Variable* src2 = NULL;
	Variable* src3 = NULL;
	Label* lab = NULL;

	InstructionType type = I_NO_TYPE;

	switch(currentToken.getType())
	{
	case T_ADD:
		type = I_ADD;
		eat(T_ADD);
		dst = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID);
		eat(T_COMMA);
		src1 = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID); 
		eat(T_COMMA);
		src2 = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID);
		break;
	case T_ADDI:
		type = I_ADDI;
		eat(T_ADDI);
		dst = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID); 
		eat(T_COMMA);
		src1 = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID); 
		eat(T_COMMA);
		src2 = new ConstVariable(currentToken.getValue());
		eat(T_NUM);
		break;
	case T_SUB:
		type = I_SUB;
		eat(T_SUB); 
		dst = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID);
		eat(T_COMMA); 
		src1 = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID); 
		eat(T_COMMA); 
		src2 = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID);
		break;
	case T_LA:
		type = I_LA;
		eat(T_LA);
		dst = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID);
		eat(T_COMMA);
		src1 = variableExists(currentToken.getValue(), m_memVars);
		eat(T_M_ID);
		break;
	case T_LW:
		type = I_LW;
		eat(T_LW);
		dst = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID); 
		eat(T_COMMA);
		src1 = new ConstVariable(currentToken.getValue());
		eat(T_NUM); 
		eat(T_L_PARENT);
		src2 = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID); 
		eat(T_R_PARENT);
		break;
	case T_LI:
		type = I_LI;
		eat(T_LI);
		dst = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID);
		eat(T_COMMA); 
		src1 = new ConstVariable(currentToken.getValue());
		eat(T_NUM);
		break;
	case T_SW:
		type = I_SW;
		eat(T_SW);
		src1 = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID); 
		eat(T_COMMA);
		src2 = new ConstVariable(currentToken.getValue());
		eat(T_NUM); 
		eat(T_L_PARENT);
		src3 = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID); 
		eat(T_R_PARENT);
		break;
	case T_B:
		type = I_B;
		eat(T_B);
		lab = labelExists(currentToken.getValue(), m_labels);
		eat(T_ID);
		break;
	case T_BLTZ:
		type = I_BLTZ;
		eat(T_BLTZ); 
		src1 = variableExists(currentToken.getValue(), m_regVars);
		eat(T_R_ID); 
		eat(T_COMMA);
		lab = labelExists(currentToken.getValue(), m_labels);
		eat(T_ID); 
		break;
	case T_NOP:
		type = I_NOP;
		eat(T_NOP);
		break;
	default:
		break;
	}
	if(dst != NULL){
		dstList.push_back(dst);
	}
	if(src1 != NULL){
		srcList.push_back(src1);
	}
	if(src2 != NULL){
		srcList.push_back(src2);
	}
	if(src3 != NULL){
		srcList.push_back(src3);
	}
	return new Instruction(m_instrPosition++, type, dstList, srcList, lab);
}

void SyntaxAnalysis::l()
{
	if (currentToken.getType() == T_END_OF_FILE)
		eat(T_END_OF_FILE);
	else
		q();
}
