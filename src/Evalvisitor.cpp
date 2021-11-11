#include "Evalvisitor.h"

NameScope scope;

antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitTypedargslist(Python3Parser::TypedargslistContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitTfpdef(Python3Parser::TfpdefContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitStmt(Python3Parser::StmtContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitAugassign(Python3Parser::AugassignContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitTest(Python3Parser::TestContext *ctx) {
  return visitOr_test(ctx->or_test());
}

antlrcpp::Any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx) {
  bool ans = false;
  auto and_test = ctx->and_test();
  if (and_test.size() == 1) return visitAnd_test(and_test[0]);
  for (auto it : and_test) {
    RealAny tmp;
    if (visitAnd_test(it).is<std::string>())
      tmp = scope[visitAnd_test(it).as<std::string>()];
    else
      tmp = visitAnd_test(it).as<RealAny>();
    ans = ans || tmp;
    if (ans) return RealAny(true);
  }
  return RealAny(false);
}

antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx) {
  bool ans = true;
  auto not_test = ctx->not_test();
  if (not_test.size() == 1) return visitNot_test(not_test[0]);
  for (auto it : not_test) {
    RealAny tmp;
    if (visitNot_test(it).is<std::string>())
      tmp = scope[visitNot_test(it).as<std::string>()];
    else
      tmp = visitNot_test(it).as<RealAny>();
    ans = ans && tmp;
    if (!ans) return RealAny(false);
  }
  return RealAny(true);
}

antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx) {
  if (ctx->comparison()) return visitComparison(ctx->comparison());
  if (visitNot_test(ctx->not_test()).is<std::string>())
    return !scope[visitNot_test(ctx->not_test()).as<std::string>()];
  else
    return !visitNot_test(ctx->not_test()).as<RealAny>();
}

antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx) {
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx) {
  return visitChildren(ctx);  // Done.
}

// arith_expr: term (addorsub_op term)*;
antlrcpp::Any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx) {
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

antlrcpp::Any EvalVisitor::visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) {
  return visitChildren(ctx);  // Done.
}

antlrcpp::Any EvalVisitor::visitTerm(Python3Parser::TermContext *ctx) {
  auto factor_array = ctx->factor();  // 该类的 vector
  RealAny ans = visitFactor(factor_array[0]).as<RealAny>();
  // TODO 乘除法中的非法类型报错
  // 使用 RealAny 自动完成类型转换
  auto op_array = ctx->muldivmod_op();
  for (int i = 1; i < factor_array.size(); ++i) {
    auto op = op_array[i - 1];
    RealAny rhs = visitFactor(factor_array[i]).as<RealAny>();
    if (op->STAR())
      ans *= rhs;
    else if (op->DIV())
      ans = FloatDiv(ans, rhs);
    else if (op->IDIV())
      ans = IntDiv(ans, rhs);
    else
      ans %= rhs;
  }
  return ans;
}

antlrcpp::Any EvalVisitor::visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) {
  return visitChildren(ctx);  // Done.
}

antlrcpp::Any EvalVisitor::visitFactor(Python3Parser::FactorContext *ctx) {
  if (ctx->atom_expr()) return visitAtom_expr(ctx->atom_expr());
  if (ctx->ADD())
    return visitFactor(ctx->factor()).as<RealAny>();
  else
    return -visitFactor(ctx->factor()).as<RealAny>();
}

// atom_expr: atom trailer?;
antlrcpp::Any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx) {
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
  else                          // other defined function
    return visitChildren(ctx);  // TODO : 调用用户定义的函数
}

antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx) {
  if (!ctx->arglist()) return std::vector<RealAny>();
  return visitArglist(ctx->arglist());  // visit 即可，上级会 as
}

// atom: (NAME | NUMBER | STRING+| 'None' | 'True' | 'False' | ('(' test ')'));
antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx) {
  if (ctx->NUMBER()) {
    std::string str = ctx->NUMBER()->getText();
    // https://en.cppreference.com/w/cpp/string/basic_string/npos
    if (str.find('.') == std::string::npos)
      return RealAny(int2048(str));
    else
      return RealAny(StringToFloat(str));
  } else if (ctx->NAME())
    return ctx->NAME()->getText();  // 返回字符串形式，便于上级区分
  else if (ctx->NONE())
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
antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx) {
  // TODO : 注意变量
  return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx) {
  std::vector<RealAny> list_array;
  auto argument_array = ctx->argument();
  for (auto it : argument_array)
    list_array.push_back(visitArgument(it).as<RealAny>());
  return list_array;
}

// argument: ( test | test '=' test );
antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx) {
  if (!ctx->ASSIGN()) return visitTest(ctx->test(0));
  // TODO : Keyword argument (test '=' test)
  return visitChildren(ctx);
}
