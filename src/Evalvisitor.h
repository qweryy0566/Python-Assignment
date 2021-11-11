#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H

#include "Python3BaseVisitor.h"
#include "real_any.h"

class EvalVisitor: public Python3BaseVisitor {
  // TODO : override all methods of Python3BaseVisitor
  // 统一在返回时不 as, 而在调用时用 as
 public:
  virtual antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitParameters(Python3Parser::ParametersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmt(Python3Parser::StmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAugassign(Python3Parser::AugassignContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSuite(Python3Parser::SuiteContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTest(Python3Parser::TestContext *ctx) override {
    return visitOr_test(ctx->or_test());
  }

  virtual antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *ctx) override {
    bool ans = false;
    auto and_test = ctx->and_test();
    for (auto it : and_test) {
      ans = ans || visitAnd_test(it).as<RealAny>();
      if (ans) return RealAny(true);
    }
    return RealAny(false);
  }

  virtual antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *ctx) override {
    bool ans = true;
    auto not_test = ctx->not_test();
    for (auto it : not_test) {
      ans = ans && visitNot_test(it).as<RealAny>();
      if (!ans) return RealAny(false);
    }
    return RealAny(true);
  }

  virtual antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *ctx) override {
    if (ctx->not_test()) return !visitNot_test(ctx->not_test()).as<RealAny>();
    return visitComparison(ctx->comparison());
  }

  virtual antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *ctx) override {
    return visitChildren(ctx);  // Done.
  }

  // arith_expr: term (addorsub_op term)*;
  virtual antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override {
    auto term_array = ctx->term();  // 该类的 vector
    RealAny ans = visitTerm(term_array[0]).as<RealAny>();
    // TODO 加法中的非法类型报错
    // 使用 RealAny 自动完成类型转换
    auto op_array = ctx->addorsub_op();
    for (int i = 1; i < term_array.size(); ++i) {
      auto op = op_array[i - 1];
      RealAny rhs = visitTerm(term_array[i]).as<RealAny>();
      ans = op->ADD() ? ans + rhs : ans - rhs;
    }
    return ans;
  }

  virtual antlrcpp::Any visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) override {
    return visitChildren(ctx);  // Done.
  }

  virtual antlrcpp::Any visitTerm(Python3Parser::TermContext *ctx) override {
    auto factor_array = ctx->factor();  // 该类的 vector
    RealAny ans = visitFactor(factor_array[0]).as<RealAny>();
    // TODO 乘除法中的非法类型报错
    // 使用 RealAny 自动完成类型转换
    auto op_array = ctx->muldivmod_op();
    for (int i = 1; i < factor_array.size(); ++i) {
      auto op = op_array[i - 1];
      RealAny rhs = visitFactor(factor_array[i]).as<RealAny>();
      if (op->STAR()) ans *= rhs;
      else if (op->DIV()) ans = FloatDiv(ans, rhs);
      else if (op->IDIV()) ans = IntDiv(ans, rhs);
      else ans %= rhs;
    }
    return ans;
  }

  virtual antlrcpp::Any visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) override {
    return visitChildren(ctx);  // Done.
  }

  virtual antlrcpp::Any visitFactor(Python3Parser::FactorContext *ctx) override {
    if (ctx->atom_expr()) return visitAtom_expr(ctx->atom_expr());
    if (ctx->ADD())
      return visitFactor(ctx->factor()).as<RealAny>();
    else
      return -visitFactor(ctx->factor()).as<RealAny>();
  }

  // atom_expr: atom trailer?;
  virtual antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {
    if (!ctx->trailer()) return visitAtom(ctx->atom());
    auto list_array = visitTrailer(ctx->trailer()).as<std::vector<RealAny>>();
    std::string func_name = ctx->atom()->getText();
    // CheckBuiltin
    if (func_name == "print") {
      for (auto it : list_array) std::cout << it << ' ';
      std::cout << '\n';
      return RealAny();  // 返回值为 None
    } else if (func_name == "int")
      return RealAny(list_array.empty() ? int2048(0) : list_array[0].ToInt());
    else if (func_name == "float")
      return RealAny(list_array.empty() ? double() : list_array[0].ToFloat());
    else if (func_name == "str")
      return RealAny(list_array.empty() ? std::string() : list_array[0].ToStr());
    else if (func_name == "bool")
      return RealAny(list_array.empty() ? bool() : list_array[0].ToBool());
    else  // other defined function
      return visitChildren(ctx);  // TODO : 调用用户定义的函数
  }

  virtual antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *ctx) override {
    if (!ctx->arglist()) return std::vector<RealAny>();
    return visitArglist(ctx->arglist());  // visit 即可，上级会 as
  }

  // atom: (NAME | NUMBER | STRING+| 'None' | 'True' | 'False' | ('(' test ')'));
  virtual antlrcpp::Any visitAtom(Python3Parser::AtomContext *ctx) override {
    if (ctx->NUMBER()) {
      std::string str = ctx->NUMBER()->getText();
      // https://en.cppreference.com/w/cpp/string/basic_string/npos
      if (str.find('.') == std::string::npos)
        return RealAny(int2048(str)); 
      else
        return RealAny(StringToFloat(str));
    } else if (ctx->NAME()) {
      std::string str = ctx->NAME()->getText();
      // TODO : atom: NAME
      return visitChildren(ctx);
    } else if (ctx->NONE())
      return RealAny();
    else if (ctx->TRUE())
      return RealAny(true);
    else if (ctx->FALSE())
      return RealAny(false);
    else if (ctx->test())
      return visitTest(ctx->test());
    else {  // That means STRING+.
      std::string ans;
      auto str_array = ctx->STRING();
      for (auto it : str_array) {
        std::string tmp = it->getText();
        tmp.pop_back(), ans += tmp.substr(1);
      }
      return RealAny(ans);
    }
  }

  // testlist: test (',' test)* (',')?;
  virtual antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *ctx) override {
    // TODO : 注意变量
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitArglist(Python3Parser::ArglistContext *ctx) override {
    std::vector<RealAny> list_array;
    auto argument_array = ctx->argument();
    for (auto it : argument_array)
      list_array.push_back(visitArgument(it).as<RealAny>());
    return list_array;
  }
  
  // argument: ( test | test '=' test );
  virtual antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *ctx) override {
    if (!ctx->ASSIGN()) return visitTest(ctx->test(0));
    // TODO : Keyword argument (test '=' test)
    return visitChildren(ctx);
  }

};

#endif //PYTHON_INTERPRETER_EVALVISITOR_H