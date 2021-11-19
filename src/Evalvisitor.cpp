#include "Evalvisitor.h"

NameScope scope;
std::unordered_map<std::string, Python3Parser::SuiteContext *> func;
enum StmtRes { kNormal, kBreak, kContinue, kReturn };

// To check if it is a variable. If so, return the value of it.
static RealAny &GetValue(antlrcpp::Any src) {
  if (src.is<std::string>()) return scope[src.as<std::string>()];
  return src.as<RealAny>();
}

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
  return ctx->simple_stmt() ? visitSimple_stmt(ctx->simple_stmt())
                            : visitCompound_stmt(ctx->compound_stmt());
}

antlrcpp::Any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) {
  return visitSmall_stmt(ctx->small_stmt());
}

antlrcpp::Any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) {
  return ctx->flow_stmt() ? visitFlow_stmt(ctx->flow_stmt())
                          : visitExpr_stmt(ctx->expr_stmt());
}

// No return value!
antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) {
  auto list_array = ctx->testlist();
  if (list_array.size() == 1) return visitTestlist(list_array[0]);
  std::vector<antlrcpp::Any> left_list, right_list;
  right_list = visitTestlist(list_array.back()).as<std::vector<antlrcpp::Any>>();
  // 初始时 right_list 为最右边的 testlist
  if (ctx->augassign()) {
    auto op = ctx->augassign();
    left_list = visitTestlist(list_array[0]).as<std::vector<antlrcpp::Any>>();
    RealAny &lhs = GetValue(left_list[0]), rhs = GetValue(right_list[0]);
    if (op->ADD_ASSIGN())
      lhs += rhs;
    else if (op->SUB_ASSIGN())
      lhs -= rhs;
    else if (op->MULT_ASSIGN())
      lhs *= rhs;
    else if (op->DIV_ASSIGN())
      lhs = FloatDiv(lhs, rhs);
    else if (op->IDIV_ASSIGN())
      lhs = IntDiv(lhs, rhs);
    else  // That means MOD_ASSIGN().
      lhs %= rhs;
  } else
    for (int i = list_array.size() - 2; ~i; --i) {
      left_list = visitTestlist(list_array[i]).as<std::vector<antlrcpp::Any>>();
      for (int j = 0; j < left_list.size(); ++j)
        GetValue(left_list[j]) = GetValue(right_list[j]);
      // TODO : 判断非变量与 list 大小不同的情况
      right_list = left_list;  // 减少 visit 次数
    }
  return kNormal;  // 无返回值
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
  // TODO : return_stmt
  visitTestlist(ctx->testlist());
  return kReturn;
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
  if (ctx->simple_stmt()) return visitSimple_stmt(ctx->simple_stmt());
  auto stmt_array = ctx->stmt();
  for (auto it : stmt_array) {
    StmtRes result = visitStmt(it).as<StmtRes>();
    if (result) return result;
  }
  return kNormal;
}

antlrcpp::Any EvalVisitor::visitTest(Python3Parser::TestContext *ctx) {
  return visitOr_test(ctx->or_test());
}

antlrcpp::Any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx) {
  bool ans = false;
  auto and_test = ctx->and_test();
  if (and_test.size() == 1) return visitAnd_test(and_test[0]);
  for (auto it : and_test) {
    ans = ans || GetValue(visitAnd_test(it));
    if (ans) return RealAny(true);  // different to real python
  }
  return RealAny(false);  // different to real python
}

antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx) {
  bool ans = true;
  auto not_test = ctx->not_test();
  if (not_test.size() == 1) return visitNot_test(not_test[0]);
  for (auto it : not_test) {
    ans = ans && GetValue(visitNot_test(it));
    if (!ans) return RealAny(false);  // different to real python
  }
  return RealAny(true);  // different to real python
}

antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx) {
  if (ctx->comparison()) return visitComparison(ctx->comparison());
  return (RealAny)!GetValue(visitNot_test(ctx->not_test()));
}

antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx) {
  auto arith_array = ctx->arith_expr();
  if (arith_array.size() == 1) return visitArith_expr(arith_array[0]);
  auto op_array = ctx->comp_op();
  RealAny lhs = GetValue(visitArith_expr(arith_array[0])), rhs;
  for (int i = 1; i < arith_array.size(); ++i) {
    rhs = GetValue(visitArith_expr(arith_array[i]));
    std::string op_str = op_array[i - 1]->getText();
    if (op_str == "<" && !(lhs < rhs) ||
        op_str == ">" && !(lhs > rhs) ||
        op_str == "==" && !(lhs == rhs) ||
        op_str == ">=" && !(lhs >= rhs) ||
        op_str == "<=" && !(lhs <= rhs) ||
        op_str == "!=" && !(lhs != rhs))
      return RealAny(false);
    lhs = rhs;
  }
  return RealAny(true);
}

antlrcpp::Any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx) {
  return visitChildren(ctx);  // Done.
}

// arith_expr: term (addorsub_op term)*;
antlrcpp::Any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx) {
  auto term_array = ctx->term();  // 该类的 vector
  if (term_array.size() == 1) return visitTerm(term_array[0]);
  RealAny ans = GetValue(visitTerm(term_array[0]));
  // TODO 加法中的非法类型报错
  // 使用 RealAny 自动完成类型转换
  auto op_array = ctx->addorsub_op();
  for (int i = 1; i < term_array.size(); ++i) {
    auto op = op_array[i - 1];
    RealAny rhs = GetValue(visitTerm(term_array[i]));
    ans = op->ADD() ? ans + rhs : ans - rhs;
  }
  return ans;
}

antlrcpp::Any EvalVisitor::visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) {
  return visitChildren(ctx);  // Done.
}

antlrcpp::Any EvalVisitor::visitTerm(Python3Parser::TermContext *ctx) {
  auto factor_array = ctx->factor();  // 该类的 vector
  if (factor_array.size() == 1) return visitFactor(factor_array[0]);
  RealAny ans = GetValue(visitFactor(factor_array[0]));
  // TODO 乘除法中的非法类型报错
  // 使用 RealAny 自动完成类型转换
  auto op_array = ctx->muldivmod_op();
  for (int i = 1; i < factor_array.size(); ++i) {
    auto op = op_array[i - 1];
    RealAny rhs = GetValue(visitFactor(factor_array[i]));
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
    return GetValue(visitFactor(ctx->factor()));
  else
    return -GetValue(visitFactor(ctx->factor()));
}

// atom_expr: atom trailer?;
antlrcpp::Any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx) {
  // 函数都有 RealAny 类型的返回值
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
  } else if (ctx->NAME()) {
    // TODO : 判断变量是否定义过
    return ctx->NAME()->getText();  // 返回字符串形式，便于上级区分
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
// TODO : What does the (',')? mean ??
antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx) {
  std::vector<antlrcpp::Any> ans;  // 用好两种 Any 类
  auto test_array = ctx->test();
  for (auto it : test_array) ans.push_back(visitTest(it));
  return ans;
}

antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx) {
  std::vector<RealAny> list_array;
  // TODO : 返回类型可能需要修改
  auto argument_array = ctx->argument();
  for (auto it : argument_array)
    list_array.push_back(GetValue(visitArgument(it)));
  return list_array;
}

// argument: ( test | test '=' test );
antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx) {
  if (!ctx->ASSIGN()) return visitTest(ctx->test(0));
  // TODO : Keyword argument (test '=' test)
  return visitChildren(ctx);
}