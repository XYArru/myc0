#include "analyser.h"

#include <climits>
#include<iostream>
#include<string>
#include <stack>

namespace miniplc0 {
	int level = 0;
	bool const_flag = false;
	auto type_flag = TokenType::CHAR;
	//gg
	//stack<int> jmp_flag;
	std::stack<int> jmp_flag;
	std::string now;
	std::pair<std::vector<Opr*>, std::optional<CompilationError>> Analyser::Analyse() {
		auto err = analyseC0Program();
		if (err.has_value())
			return std::make_pair(std::vector<Opr*>(), err);
		else
			return std::make_pair(Sins, std::optional<CompilationError>());
	}

	// <C0-program> ::= {<variable-declaration>}{<function-definition>}
	std::optional<CompilationError> Analyser::analyseC0Program() {
		auto err = analyseVarDec();
		if (err.has_value())
			return err;
		err = analyseFunDef();
		if (err.has_value())
			return err;

		return {};
	}
	//Init时唯一出口是函数！；
	std::optional<CompilationError> Analyser::analyseVarDec() {

		while (true) {
			auto next = nextToken();
			if (!next.has_value())
				return {};
			if (next.value().GetType() != TokenType::VOID
				&& next.value().GetType() != TokenType::INT
				&& next.value().GetType() != TokenType::CONST)
			{
				unreadToken();
				return {};
			}
			if (next.value().GetType() == TokenType::CONST)
				const_flag = true;
			else
				unreadToken();

			next = nextToken();
			if (next.value().GetType() == TokenType::VOID || next.value().GetType() == TokenType::INT) {
				type_flag = next.value().GetType();
				next = nextToken();
				next = nextToken();
				if (next.value().GetType() == TokenType::ZKH) {
					unreadToken();
					unreadToken();
					unreadToken();
					return {};
				}
				unreadToken();
				unreadToken();
				auto errIDL = analyseInitDeclist();
				if (errIDL.has_value())
					return errIDL;
			}

			next = nextToken();
			if (next.value().GetType() == TokenType::SEMICOLON) {
				const_flag = false;
			}
		}

		return {};
	}
	std::optional<CompilationError> Analyser::analyseInitDeclist() {

		while (true) {
			auto next = nextToken();

			if (next.value().GetType() != TokenType::IDENTIFIER) {
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
			}
			auto me = next;
			next = nextToken();
			if (next.value().GetType() == TokenType::FZ) {
				if (level == 0) {
					miniplc0::Analyser::addGdt(me.value());
					Var* sth = getG(me.value().GetValueString());
					if (type_flag == TokenType::INT)
						sth->type = 'i';
					else
						sth->type = 'v';
					sth->_const = const_flag;
					sth->_init = true;

					Var* ne = getG(me.value().GetValueString());
				}
				else {
					addLdt(me.value());
					Var* sth = getL(me.value().GetValueString());
					if (type_flag == TokenType::INT)
						sth->type = 'i';
					else
						sth->type = 'v';
					sth->_const = const_flag;
					sth->_init = true;

					Var* ne = getL(me.value().GetValueString());
				}
				auto errExp = analyseExp();

				if (errExp.has_value())
					return errExp;
				return errExp;
			}
			else if (next.value().GetType() == TokenType::DOUHAO) {
				if (const_flag)
					return  std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrConstNoInit);
				//加入局部表&&压栈
				if (level == 0) {
					addGdt(me.value());
					Var* sth = getG(me.value().GetValueString());
					if (type_flag == TokenType::INT)
						sth->type = 'i';
					else
						sth->type = 'v';
					sth->_const = const_flag;
					sth->_init = false;
				}
				else {
					addLdt(me.value());
					Var* sth = getL(me.value().GetValueString());
					if (type_flag == TokenType::INT)
						sth->type = 'i';
					else
						sth->type = 'v';
					sth->_const = const_flag;
					sth->_init = false;
				}

				Opr* me = new Opr;
				me->_opr = "ipush";
				me->_x = "0";
				me->_y.empty();
				Sins.emplace_back(me);

			}
			else {

				if (level == 0) {
					addGdt(me.value());
					Var* sth = getG(me.value().GetValueString());
					if (type_flag == TokenType::INT)
						sth->type = 'i';
					else
						sth->type = 'v';
					sth->_const = const_flag;
					sth->_init = false;
				}
				else {
					addLdt(me.value());
					Var* sth = getL(me.value().GetValueString());
					if (type_flag == TokenType::INT)
						sth->type = 'i';
					else
						sth->type = 'v';
					sth->_const = const_flag;
					sth->_init = false;
				}
				Opr* me = new Opr;
				me->_opr = "ipush";
				me->_x = "0";
				me->_y.clear();
				Sins.emplace_back(me);
				unreadToken();
				break;
			}
		}

		return {};
	}

	std::optional<CompilationError> Analyser::analyseExp() {

		auto errAExp = analyseAExp();
		if (errAExp.has_value()) {
			return errAExp;
		}
		return {};
	}
	std::optional<CompilationError> Analyser::analyseAExp() {
		auto errMExp = analyseMExp();
		if (errMExp.has_value())
			return errMExp;

		auto next = nextToken();
		if (next.value().GetType() == TokenType::PLUS || next.value().GetType() == TokenType::MINUS) {

			//只考虑了全为int
			auto errMExp = analyseMExp();
			
			if (next.value().GetType() == TokenType::PLUS) {
				if (level == 0) {

					Opr* me = new Opr;
					me->_opr = "iadd";
					me->_x.clear();
					me->_y.clear();
					Sins.emplace_back(me);
				}
				else if (level == 1) {

					Opr* me = new Opr;
					me->_opr = "iadd";
					me->_x.clear();
					me->_y.clear();
					Ains[now].emplace_back(me);
				}
			}

			else if (next.value().GetType() == TokenType::MINUS) {
				if (level == 0) {
					Opr* me = new Opr;
					me->_opr = "isub";
					me->_x.clear();
					me->_y.clear();
					Sins.emplace_back(me);
				}
				else if (level == 1) {
					Opr* me = new Opr;
					me->_opr = "isub";
					me->_x.clear();
					me->_y.clear();
					Ains[now].emplace_back(me);
				}
			}
			return errMExp;
		}

		unreadToken();
		return {};

	}
	std::optional<CompilationError> Analyser::analyseMExp() {

		auto errUExp = analyseUExp();
		if (errUExp.has_value())
			return errUExp;

		auto next = nextToken();

		if (next.value().GetType() == TokenType::STAR || next.value().GetType() == TokenType::_DIV) {

			auto errUExp = analyseUExp();
			
			//操作show起来
			if (next.value().GetType() == TokenType::STAR) {
				if (level == 0) {
					Opr* me = new Opr;
					me->_opr = "imul";
					me->_x.clear();
					me->_y.clear();
					Sins.emplace_back(me);
				}
				else if (level == 1) {
					Opr* me = new Opr;
					me->_opr = "imul";
					me->_x.clear();
					me->_y.clear();
					Ains[now].emplace_back(me);
				}
			}
			else if (next.value().GetType() == TokenType::_DIV) {
				if (level == 0) {
					Opr* me = new Opr;
					me->_opr = "idiv";
					me->_x.clear();
					me->_y.clear();
					Sins.emplace_back(me);
				}
				else if (level == 1) {
					Opr* me = new Opr;
					me->_opr = "idiv";
					me->_x.clear();
					me->_y.clear();
					Ains[now].emplace_back(me);
				}
			}
			return errUExp;
		}

		unreadToken();
		return {};

	}
	std::optional<CompilationError> Analyser::analyseUExp() {
		auto next = nextToken();

		if (next.value().GetType() != TokenType::PLUS && next.value().GetType() != TokenType::MINUS) {
			unreadToken();
		}
		else if (next.value().GetType() == TokenType::MINUS) {
			auto errP = analysePExp();
			if (level == 0) {
				Opr* me = new Opr;
				me->_opr = "ineg";
				me->_x.clear();
				me->_y.clear();
				Sins.emplace_back(me);
			}
			else {
				Opr* me = new Opr;
				me->_opr = "ineg";
				me->_x.clear();
				me->_y.clear();
				Ains[now].emplace_back(me);
			}
			return errP;
		}
		auto errP = analysePExp();

		return errP;
	}
	std::optional<CompilationError> Analyser::analysePExp() {

		auto next = nextToken();

		if (next.value().GetType() == TokenType::ZKH) {
			auto errE = analyseExp();
			if (errE.has_value())
				return errE;
			next = nextToken();
			if (next.value().GetType() != TokenType::YKH)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		else if (next.value().GetType() == TokenType::IDENTIFIER) {
			if (isFunc(next.value().GetValueString())) {
				unreadToken();
				auto errC = analyseFunCall();
			}
			else {
				if (!isDclr(next.value().GetValueString())) {
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
				}
				if (!isInit(next.value().GetValueString()))
				{
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotInitialized);
				}
				if (isVoid(next.value().GetValueString())) {
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrCalcVoid);
				}

				Var* _var = getL(next.value().GetValueString());
				bool _L = true;
				if (_var == nullptr) {
					_var = getG(next.value().GetValueString());
					_L = false;
				}
				auto _index = _var->index;
				if (_L) {
					if (level == 0) {
						Opr* me = new Opr;
						me->_opr = "loada";
						me->_x = "0";
						me->_y = std::to_string(_index).c_str();
						Sins.emplace_back(me);

						me = new Opr;
						me->_opr = "iload";
						me->_x.clear();
						me->_y.clear();
						Sins.emplace_back(me);
					}
					else {
						Opr* me = new Opr;
						me->_opr = "loada";
						me->_x = "0";
						me->_y = std::to_string(_index).c_str();
						Ains[now].emplace_back(me);

						me = new Opr;
						me->_opr = "iload";
						me->_x.clear();
						me->_y.clear();
						Ains[now].emplace_back(me);
					}
				}
				else {
					if (level == 0) {

						Opr* me = new Opr;
						me->_opr = "loada";
						me->_x = "0";
						me->_y = std::to_string(_index).c_str();
						Sins.emplace_back(me);
						me = new Opr;
						me->_opr = "iload";
						me->_x.clear();
						me->_y.clear();
						Sins.emplace_back(me);
					}
					else {
						Opr* me = new Opr;
						me->_opr = "loada";
						me->_x = "1";
						me->_y = std::to_string(_index).c_str();
						Ains[now].emplace_back(me);

						me = new Opr;
						me->_opr = "iload";
						me->_x.clear();
						me->_y.clear();
						Ains[now].emplace_back(me);
					}
				}

			}
		}
		else if (next.value().GetType() == TokenType::UNSIGNED_INTEGER) {
			if (level == 0) {
				Opr* me = new Opr;
				me->_opr = "ipush";
				me->_x = next.value().GetValueString();
				me->_y.clear();
				Sins.emplace_back(me);

			}
			else {
				Opr* me = new Opr;
				me->_opr = "ipush";
				me->_x = next.value().GetValueString();
				me->_y.clear();
				Ains[now].emplace_back(me);
			}
		}

		return {};
	}

	std::optional<CompilationError> Analyser::analyseFunDef() {
		while (true) {
			auto next = nextToken();
			if (!next.has_value())
				return {};

			if (next.value().GetType() != TokenType::INT && next.value().GetType() != TokenType::VOID)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrTypedef);
			type_flag = next.value().GetType();
			next = nextToken();
			if (next.value().GetType() != TokenType::IDENTIFIER)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrMustBeIdentifier);
			if (!isFunc(next.value().GetValueString())) {
				addConstantF(next.value());
			}
			else
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrRedefine);
			now = next.value().GetValueString();
			level = 1;
			clrLdt();
			auto errP = analysePar();
			if (errP.has_value())
				return errP;
			int32_t num_par = _nextLp;
			auto type = type_flag;
			addFunc(next.value());
			Func* _f = getFunc(next.value().GetValueString());
			_f->num_par = num_par;
			_f->name_index = getConst(next.value().GetValueString())->index;
			_f->type = type_flag == TokenType::INT ? 'i' : 'v';
			_f->level = level;
			auto errComp = analyseComp();
			level = 0;
			if (errComp.has_value())
				return errComp;
		}
	}

	std::optional<CompilationError> Analyser::analysePar() {
		auto next = nextToken();
		if (next.value().GetType() != TokenType::ZKH)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		auto errPDL = analysePDL();
		if (errPDL.has_value())
			return errPDL;
		next = nextToken();

		if (next.value().GetType() != TokenType::YKH)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		return {};
	}
	std::optional<CompilationError> Analyser::analysePDL() {
		//根据IDENTIFIER求参数vector
		while (true) {
			auto errPD = analysePD();
			if (errPD.has_value())
				return errPD;
			auto next = nextToken();
			if (next.value().GetType() != TokenType::DOUHAO)
				break;
		}
		unreadToken();
		return {};
	}
	std::optional<CompilationError> Analyser::analysePD() {
		auto next = nextToken();

		bool _const_flag = true;
		if (next.value().GetType() != TokenType::CONST) {
			unreadToken();
			_const_flag = false;
		}
		next = nextToken();
		auto _type_flag = TokenType::CHAR;
		if (next.value().GetType() == TokenType::VOID || next.value().GetType() == TokenType::INT) {
			_type_flag = next.value().GetType();
			next = nextToken();
			if (next.value().GetType() != TokenType::IDENTIFIER) {
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
			}
			//进行符号表操作
			addLdt(next.value());

			Var* me = getL(next.value().GetValueString());
			me->type = _type_flag == TokenType::VOID ? 'v' : 'i';
			me->_const = _const_flag ? true : false;
			me->_init = true;
		}
		else
			unreadToken();
		return {};
	}

	std::optional<CompilationError> Analyser::analyseFunCall() {
		auto next = nextToken();

		auto func = next;
		if (!isFunc(func.value().GetValueString()))
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrMustBeIdentifier);

		next = nextToken();
		if (next.value().GetType() != TokenType::ZKH)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);

		auto errExpl = analyseExpl();
		if (errExpl.has_value()) {
			return errExpl;
		}
		next = nextToken();
		if (next.value().GetType() != TokenType::YKH)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);

		//操作 找到func在函数表中的位置 call
		int32_t _index = getFunc(func.value().GetValueString())->index;

		
		Opr* me = new Opr;
		me->_opr = "call";
		me->_x = std::to_string(_index);
		me->_y.clear();
		Ains[now].emplace_back(me);
		
		auto sz = Ains[now].size();

		return {};
	}
	std::optional<CompilationError> Analyser::analyseExpl() {
		//根据IDENTIFIER求参数vector
		while (true) {
			auto errPD = analyseExp();
			if (errPD.has_value())
				return errPD;
			//操作一下转类型之类的

			auto next = nextToken();
			if (next.value().GetType() != TokenType::DOUHAO)
				break;
		}
		unreadToken();
		return {};
	}

	std::optional<CompilationError> Analyser::analyseComp() {
		level = 1;
		auto next = nextToken();
		if (next.value().GetType() != TokenType::ZDKH)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		auto err = analyseVarDec();
		if (err.has_value())
			return err;

		auto errS = analyseStmtSeq();
		if (errS.has_value()) {
			return errS;
		}

		next = nextToken();
		if (next.value().GetType() != TokenType::YDKH)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);

		level = 0;
		return {};
	}


	std::optional<CompilationError> Analyser::analyseStmtSeq() {
		while (true) {
			auto next = nextToken();
			unreadToken();
			if (next.value().GetType() != TokenType::WHILE &&
				next.value().GetType() != TokenType::SCAN &&
				next.value().GetType() != TokenType::PRINT &&
				next.value().GetType() != TokenType::IDENTIFIER &&
				next.value().GetType() != TokenType::IF &&
				next.value().GetType() != TokenType::RETURN &&
				next.value().GetType() != TokenType::ZDKH &&
				next.value().GetType() != TokenType::SEMICOLON
				) {
				return {};
			}

			auto errStmt = analyseStmt();
			if (errStmt.has_value())
				return errStmt;

		}
	}
	std::optional<CompilationError> Analyser::analyseStmt() {
		auto next = nextToken();
		auto _opr = new Opr;
		unreadToken();
		std::optional<CompilationError> err = {};
		if (!next.has_value())
			return {};

		if (next.value().GetType() != TokenType::WHILE &&
			next.value().GetType() != TokenType::SCAN &&
			next.value().GetType() != TokenType::PRINT &&
			next.value().GetType() != TokenType::IDENTIFIER &&
			next.value().GetType() != TokenType::IF &&
			next.value().GetType() != TokenType::RETURN &&
			next.value().GetType() != TokenType::ZDKH &&
			next.value().GetType() != TokenType::SEMICOLON
			)
			return {};
		
		switch (next.value().GetType())
		{
		case TokenType::IF:
			err = analyseCondStmt();
			if (err.has_value())
				return err;
			break;
		case TokenType::WHILE:
			err = analyseLoopStmt();
			if (err.has_value())
				return err;
			break;
		case TokenType::RETURN:
			err = analyseJumpStmt();
			if (err.has_value())
				return err;
			break;
		case TokenType::PRINT:
			err = analysePrintStmt();
			if (err.has_value())
				return err;
			break;
		case TokenType::SCAN:
			err = analyseScanStmt();
			if (err.has_value())
				return err;
			break;
		case TokenType::IDENTIFIER:
			next = nextToken();
			if (isFunc(next.value().GetValueString())) {
				unreadToken();
				err = analyseFunCall();
				break;
			}
			else {
				auto me = next;
				next = nextToken();
				if (next.value().GetType() != TokenType::FZ)
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);

				if (!isDclr(me.value().GetValueString()))
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
				if (!isInit(me.value().GetValueString()))
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotInitialized);
				if (isVoid(me.value().GetValueString()))
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrCalcVoid);
				if (isConst(me.value().GetValueString()))
					return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrCalcVoid);

				Var* _var = getL(me.value().GetValueString());

				bool _L = true;
				if (_var == nullptr) {
					_var = getG(me.value().GetValueString());
					_L = false;
				}

				auto _index = _var->index;
				if (_L) {
					Opr* me = new Opr;
					me->_opr = "loada";
					me->_x = "0";
					me->_y = std::to_string(_index).c_str();
					Ains[now].emplace_back(me);
					auto errExp = analyseExp();

					me = new Opr;
					me->_opr = "istore";
					me->_x.clear();
					me->_y.clear();
					Ains[now].emplace_back(me);


				}
				else {
					Opr* me = new Opr;
					me->_opr = "loada";
					me->_x = "1";
					me->_y = std::to_string(_index);
					Ains[now].emplace_back(me);
					auto errExp = analyseExp();

					me = new Opr;
					me->_opr = "istore";
					me->_x.clear();
					me->_y.clear();
					Ains[now].emplace_back(me);
				}
				next = nextToken();
				unreadToken();
				printf("%s\n",next.value().GetValueString());
				break;
			}
		case TokenType::ZDKH:
			next = nextToken();
			//printf("%s\n", next.value().GetValueString().c_str());
			err = analyseStmtSeq();
			if (err.has_value())
				return err;
			next = nextToken();
			if (next.value().GetType() != TokenType::YDKH)
				return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
			break;

		case TokenType::SEMICOLON:
			_opr = new Opr;
			_opr->_opr = "nop";
			_opr->_x.clear();
			_opr->_y.clear();
			Ains[now].emplace_back(_opr);
			next = nextToken();
			break;
		default:
			break;
		}
		
		return {};
	}

	std::optional<CompilationError> Analyser::analyseCond() {
		auto errE = analyseExp();
		
		if (errE.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		auto next = nextToken();
		if (next.value().GetType() == TokenType::YKH)
		{
			Opr* me = new Opr;
			me->_opr = "bipush";
			me->_x = std::to_string(0);
			me->_y.clear();
			Ains[now].emplace_back(me);
			me = new Opr;
			me->_opr = "je";
			me->_x.clear();
			me->_y.clear();
			Ains[now].emplace_back(me);
			return {};
		}
		if (next.value().GetType() != TokenType::LESS &&
			next.value().GetType() != TokenType::GREATER &&
			next.value().GetType() != TokenType::LOE &&
			next.value().GetType() != TokenType::GOE &&
			next.value().GetType() != TokenType::NE &&
			next.value().GetType() != TokenType::EQ
			)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);

		errE = analyseExp();
		if (errE.has_value())
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);

		auto me = new Opr;
		me->_opr = "icmp";
		me->_x.clear();
		me->_y.clear();
		Ains[now].emplace_back(me);
		jmp_flag.push(Ains[now].size());
		me = new Opr;
		switch (next.value().GetType())
		{
			//缺操作数 后来分析完stmt后要填入

		case TokenType::LESS:
			me = new Opr;
			me->_opr = "jge";
			me->_x.clear();
			me->_y.clear();
			Ains[now].emplace_back(me);
			break;
		case TokenType::GREATER:
			me = new Opr;
			me->_opr = "jle";
			me->_x.clear();
			me->_y.clear();
			Ains[now].emplace_back(me);
			break;
		case TokenType::LOE:
			me = new Opr;
			me->_opr = "jg";
			me->_x.clear();
			me->_y.clear();
			Ains[now].emplace_back(me);
			break;
		case TokenType::GOE:
			me = new Opr;
			me->_opr = "jl";
			me->_x.clear();
			me->_y.clear();
			Ains[now].emplace_back(me);
			break;
		case TokenType::NE:
			me = new Opr;
			me->_opr = "je";
			me->_x.clear();
			me->_y.clear();
			Ains[now].emplace_back(me);
			break;
		case TokenType::EQ:
			me = new Opr;
			me->_opr = "jne";
			me->_x.clear();
			me->_y.clear();
			Ains[now].emplace_back(me);
			break;
		default:
			break;
		}
		
		return{};
	}

	std::optional<CompilationError> Analyser::analyseCondStmt() {
		auto next = nextToken();
		if (next.value().GetType() != TokenType::IF) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		next = nextToken();
		if (next.value().GetType() != TokenType::ZKH) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}

		auto errC = analyseCond();
		if (errC.has_value())
			return errC;

		next = nextToken();
		if (next.value().GetType() != TokenType::YKH) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		auto errS = analyseStmt();
		if (errS.has_value())
			return errS;


		auto chag = Ains[now];
		auto me = chag.at(jmp_flag.top());
		jmp_flag.pop();
		me->_x = std::to_string(Ains[now].size()).c_str();

		next = nextToken();
		if (next.value().GetType() == TokenType::ELSE) {
			errS = analyseStmt();
			if (errS.has_value())
				return errS;
		}
		else
			unreadToken();

		return {};
	}

	std::optional<CompilationError> Analyser::analyseLoopStmt() {
		
		auto next = nextToken();
		if (next.value().GetType() != TokenType::WHILE) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		next = nextToken();
		if (next.value().GetType() != TokenType::ZKH) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}

		auto errC = analyseCond();
		if (errC.has_value())
			return errC;
		next = nextToken();

		if (next.value().GetType() != TokenType::YKH) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}

		auto errS = analyseStmt();
		if (errS.has_value())
			return errS;
		//
		auto chag = Ains[now];
		auto me = chag.at(jmp_flag.top());
		jmp_flag.pop();
		me->_x = std::to_string(Ains[now].size());
		
		return {};
	}

	std::optional<CompilationError> Analyser::analyseJumpStmt() {
		
		auto next = nextToken();
		if (next.value().GetType() != TokenType::RETURN) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		auto errPD = analyseExp();
		if (errPD.has_value()) {
			return errPD;
		}

		next = nextToken();

		if (next.value().GetType() != TokenType::SEMICOLON) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		Func* _f = getFunc(now);

		if (_f->type == 'i') {
			auto me = new Opr;
			me->_opr = "iret";
			me->_x.empty();
			me->_y.empty();
			Ains[now].emplace_back(me);
		}
		else {
			auto me = new Opr;
			me->_opr = "ret";
			me->_x.empty();
			me->_y.empty();
			Ains[now].emplace_back(me);
		}
			
		return {};
	}
	std::optional<CompilationError> Analyser::analysePrint() {
		
		while (true) {
			auto errE = analyseExp();
			if (errE.has_value())
				return errE;
			auto next = nextToken();
			auto me = new Opr;
			me->_opr = "iprint";
			me->_x.empty();
			me->_y.empty();
			Ains[now].emplace_back(me);
			if (next.value().GetType() != TokenType::DOUHAO) {
				unreadToken();
				break;
			}

			me = new Opr;
			me->_opr = "bipush";
			me->_x = std::to_string(32);
			me->_y.empty();
			Ains[now].emplace_back(me);
			me = new Opr;
			me->_opr = "cprint";
			me->_x.empty();
			me->_y.empty();
			Ains[now].emplace_back(me);
		}
		return {};
	}
	std::optional<CompilationError> Analyser::analysePrintStmt() {
		auto next = nextToken();
		if (next.value().GetType() != TokenType::PRINT) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		next = nextToken();
		if (next.value().GetType() != TokenType::ZKH) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}

		auto errPD = analysePrint();
		if (errPD.has_value())
			return errPD;

		next = nextToken();
		if (next.value().GetType() != TokenType::YKH) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}

		next = nextToken();
		if (next.value().GetType() != TokenType::SEMICOLON) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		return {};
	}
	std::optional<CompilationError> Analyser::analyseScanStmt() {
		auto next = nextToken();
		
		if (next.value().GetType() != TokenType::SCAN) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		next = nextToken();
		if (next.value().GetType() != TokenType::ZKH) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		next = nextToken();
		if (next.value().GetType() != TokenType::IDENTIFIER)
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		auto me = next;

		if (!isDclr(me.value().GetValueString()))
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNotDeclared);
		if (isVoid(me.value().GetValueString()))
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrCalcVoid);
		if (isConst(me.value().GetValueString()))
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrCalcVoid);

		Var* _var = getL(me.value().GetValueString());
		bool _L = true;
		if (_var == nullptr) {
			_var = getG(me.value().GetValueString());
			_L = false;
		}
		auto _index = _var->index;
		if (_L) {

			auto me = new Opr;
			me->_opr = "loada";
			me->_x = std::to_string(0).c_str();
			me->_y = std::to_string(_index).c_str();
			Ains[now].emplace_back(me);
		}
		else {
			auto me = new Opr;
			me->_opr = "loada";
			me->_x = std::to_string(1).c_str();
			me->_y = std::to_string(_index).c_str();
			Ains[now].emplace_back(me);
		}

		next = nextToken();
		if (next.value().GetType() != TokenType::YKH) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		next = nextToken();
		if (next.value().GetType() != TokenType::SEMICOLON) {
			return std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrNoSemicolon);
		}
		//语义操作
		auto _opr = new Opr;
		_opr->_opr = "iscan";
		_opr->_x.clear();
		_opr->_y.clear();
		Ains[now].emplace_back(_opr);
		_opr = new Opr;
		_opr->_opr = "istore";
		_opr->_x.clear();
		_opr->_y.clear();
		Ains[now].emplace_back(_opr);
		
		return {};
	}

	std::optional<Token> Analyser::nextToken() {
		if (_offset == _tokens.size())
			return {};
		// 考虑到 _tokens[0..._offset-1] 已经被分析过了
		// 所以我们选择 _tokens[0..._offset-1] 的 EndPos 作为当前位置
		_current_pos = _tokens[_offset].GetEndPos();
		return _tokens[_offset++];
	}

	//大端法转换四字节
	void Analyser::binary4byte(int number, std::ostream& output) {
		char buffer[4];
		buffer[0] = ((number & 0xff000000) >> 24);
		buffer[1] = ((number & 0x00ff0000) >> 16);
		buffer[2] = ((number & 0x0000ff00) >> 8);
		buffer[3] = ((number & 0x000000ff));
		output.write(buffer, sizeof(buffer));
	}
	//大端法转换双字节
	void Analyser::binary2byte(int number, std::ostream& output) {
		char buffer[2];
		buffer[0] = ((number & 0x0000ff00) >> 8);
		buffer[1] = ((number & 0x000000ff));
		output.write(buffer, sizeof(buffer));
	}
	//输出二进制文件
	void Analyser::printBinary(std::ostream& output) {
		//首先书写固定字段magic和version
		char magic[4];
		magic[0] = 0x43;
		magic[1] = 0x30;
		magic[2] = 0x3a;
		magic[3] = 0x29;
		char version[4];
		version[0] = 0;
		version[1] = 0;
		version[2] = 0;
		version[3] = 1;
		output.write(magic, sizeof(magic));
		output.write(version, sizeof(version));
		int const_size = (int)_funcs.size();
		binary2byte(const_size, output);
		char buffer[1];
		buffer[0] = 0x00;
		output.write(buffer, sizeof(char));
		int len = _consts.size();
		auto citer = _consts.begin();
		binary2byte(len, output);
		while (citer != _consts.end()) {
			output << citer->second->index << "\t" << char(citer->second->type) << "\t\"" << citer->first << "\"\n";
			citer++;
		}

		binary2byte(Sins.size(), output);
		for (int i = 0; i < Sins.size(); i++) {
			printBinaryInstruction(Sins.at(i), output);
		}

		binary2byte(_funcs.size(), output);
		int i = 0;
		for (auto fiter = _funcs.begin(); fiter != _funcs.end();i++, fiter++) {
			binary2byte(i, output);
			binary2byte(fiter->second->num_par, output);
			binary2byte(1, output);
			binary2byte(Ains[fiter->first].size(), output);
			for (int j = 0, tt = Ains[fiter->first].size(); j < tt; j++) {
				printBinaryInstruction(Ains[fiter->first].at(i), output);
			}
		}

	}

	void Analyser::printBinaryInstruction(Opr* opr, std::ostream& output) {
		char buffer[1];
			if (opr->_opr == "nop") {
				buffer[0] = 0x00;
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "binpus") {
				buffer[0] = 0x01;
				output.write(buffer, sizeof(char));
				buffer[0] = (atoi(opr->_x.c_str()) & 0xff);
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "ipush") {
				buffer[0] = 0x02;
				output.write(buffer, sizeof(char));
				buffer[0] = (atoi(opr->_x.c_str()) & 0xff);
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "pop") {
				buffer[0] = 0x04;
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "loadc") {
				buffer[0] = 0x09;
				output.write(buffer, sizeof(char));
				buffer[0] = (atoi(opr->_x.c_str()) & 0xff);
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "loada") {
				buffer[0] = 0x0a;
				output.write(buffer, sizeof(char));
				buffer[0] = (atoi(opr->_x.c_str()) & 0xff);
				output.write(buffer, sizeof(char));
				buffer[0] = (atoi(opr->_y.c_str()) & 0xff);
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "iload") {
				buffer[0] = 0x10;
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "istore") {
				buffer[0] = 0x20;
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "iadd") {
				buffer[0] = 0x30;
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "isub") {
				buffer[0] = 0x34;
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "imul") {
				buffer[0] = 0x38;
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "idiv") {
				buffer[0] = 0x3c;
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "ineg") {
				buffer[0] = 0x40;
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "icmp") {
				buffer[0] = 0x44;
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "jmp") {
				buffer[0] = 0x70;
				output.write(buffer, sizeof(char));
				binary2byte(atoi(opr->_x.c_str()), output);
			}
			else if (opr->_opr == "je") {
				buffer[0] = 0x71;
				output.write(buffer, sizeof(char));
				binary2byte(atoi(opr->_x.c_str()), output);
			}
			else if (opr->_opr == "jl") {
				buffer[0] = 0x73;
				output.write(buffer, sizeof(char));
				binary2byte(atoi(opr->_x.c_str()), output);
			}
			else if (opr->_opr == "jge") {
				buffer[0] = 0x74;
				output.write(buffer, sizeof(char));
				binary2byte(atoi(opr->_x.c_str()), output);
			}
			else if (opr->_opr == "jg") {
				buffer[0] = 0x75;
				output.write(buffer, sizeof(char));
				binary2byte(atoi(opr->_x.c_str()), output);
			}
			else if (opr->_opr == "jle") {
				buffer[0] = 0x76;
				output.write(buffer, sizeof(char));
				binary2byte(atoi(opr->_x.c_str()), output);
			}
			else if (opr->_opr == "call") {
				buffer[0] = 0x80;
				output.write(buffer, sizeof(char));
				binary2byte(atoi(opr->_x.c_str()), output);
			}
			else if (opr->_opr == "ret") {
				buffer[0] = 0x88;
				output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "iret") {
			buffer[0] = 0x89;
			output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "iprint") {
			buffer[0] = 0xa0;
			output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "cprint") {
			buffer[0] = 0xa2;
			output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "printl") {
			buffer[0] = 0xaf;
			output.write(buffer, sizeof(char));
			}
			else if (opr->_opr == "iscan") {
			buffer[0] = 0xb0;
			output.write(buffer, sizeof(char));
			}
		}
	
	void Analyser::unreadToken() {
		if (_offset == 0)
			DieAndPrint("analyser unreads token from the begining.");
		_current_pos = _tokens[_offset - 1].GetEndPos();
		_offset--;
	}


	void Analyser::addConstantF(const Token& tk) {
		ConstTable* me = new ConstTable;
		me->type = 'S';
		me->index = _nextConst;
		_consts[tk.GetValueString()] = me;
		_nextConst++;
	}

	ConstTable* Analyser::getConst(const std::string& s) {
		return _consts[s];
	}

	void Analyser::addFunc(const Token& tk) {
		Func* me = new Func;
		me->type = 'S';
		me->index = _nextFunc;
		_funcs[tk.GetValueString()] = me;
		_nextFunc++;
	}
	bool Analyser::isFunc(const std::string& s) {
		return _funcs.find(s) != _funcs.end();
	}
	Func* Analyser::getFunc(const std::string& s) {
		return _funcs[s];
	}

	// 获得 {变量，常量} 
	Var* Analyser::getG(const std::string& s) {
		return _gdt[s];
	}
	// 
	Var* Analyser::getL(const std::string& s) {
		return _ldt[s];
	}
	void Analyser::addGdt(const Token& tk) {
		Var* me = new Var;
		me->index = _nextGp;
		if (tk.GetType() != TokenType::IDENTIFIER)
			DieAndPrint("only identifier can be added to the table.");

		_gdt[tk.GetValueString()] = me;
		_nextGp++;
	}
	void Analyser::addLdt(const Token& tk) {
		Var* me = new Var;
		me->index = _nextLp;
		if (tk.GetType() != TokenType::IDENTIFIER)
			DieAndPrint("only identifier can be added to the table.");
		_ldt[tk.GetValueString()] = me;
		_nextLp++;
	}
	void Analyser::clrLdt() {
		_ldt.clear();
		_nextLp = 0;
	}

	bool Analyser::isConst(const std::string& s) {
		if (_ldt.find(s) != _ldt.end())
			return _ldt.at(s)->_const;
		if (_ldt.find(s) != _ldt.end())
			return _gdt.at(s)->_const;
	}
	bool Analyser::isInit(const std::string& s) {
		if (_ldt.find(s) != _ldt.end())
			return _ldt.at(s)->_init;
		if (_gdt.find(s) != _gdt.end())
			return _gdt.at(s)->_init;
	}
	bool Analyser::isVoid(const std::string& s) {
		if (_ldt.find(s) != _ldt.end())
			return _ldt.at(s)->type == 'v';
		if (_gdt.find(s) != _gdt.end())
			return _gdt.at(s)->type == 'v';
	}
	bool Analyser::isDclr(const std::string& s) {
		return _consts.find(s) == _consts.end() || _ldt.find(s) == _ldt.end() || _gdt.find(s) == _gdt.end();
	}
}
/*

//输出二进制指令
}*/