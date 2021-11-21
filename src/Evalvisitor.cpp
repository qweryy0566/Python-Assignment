#include "Evalvisitor.h"

Variable variable;
Function function;
// 利用栈的性质，只需保存当前的返回值
RealAny return_value;
enum StmtRes { kNormal, kBreak, kContinue, kReturn };

// To check if it is a variable. If so, return the value of it.
// 返回类型为 RealAny &
static RealAny &GetValue(antlrcpp::Any src) {
  if (src.is<string>()) return variable[src.as<string>()];
  return src.as<RealAny>();
}

// 无需确定返回类型
antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx) {
  return visitChildren(ctx);  // Done.
}

// 返回类型为 StmtRes
antlrcpp::Any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx) {
  string name = ctx->NAME()->getText();
  function.Suite(name) = ctx->suite();
  function.Parameters(name) = visitParameters(ctx->parameters()).as<ParametersType>();
  return kNormal;
}

// 返回类型为 ParametersType (vector<pair<string, RealAny>>)
antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx) {
  if (!ctx->typedargslist()) return ParametersType();
  return visitTypedargslist(ctx->typedargslist());
}

// 返回类型为 ParametersType (vector<pair<string, RealAny>>)
antlrcpp::Any EvalVisitor::visitTypedargslist(Python3Parser::TypedargslistContext *ctx) {
  auto tfpdef_array = ctx->tfpdef();
  auto test_array = ctx->test();
  ParametersType ans;
  int d = test_array.size() - tfpdef_array.size();  // 两个 size 的差
  for (int i = 0; i < tfpdef_array.size(); ++i) {
    RealAny default_val;
    if (i + d >= 0) default_val = GetValue(visitTest(test_array[i + d]));
    ans.push_back(make_pair(tfpdef_array[i]->NAME()->getText(), default_val));
  }
  return ans;
}

// 不会用到
antlrcpp::Any EvalVisitor::visitTfpdef(Python3Parser::TfpdefContext *ctx) {
  return visitChildren(ctx);  // Done.
}

// 自动 visit
// 返回类型为 StmtRes
antlrcpp::Any EvalVisitor::visitStmt(Python3Parser::StmtContext *ctx) {
  return visitChildren(ctx);  // Done.
}

// 返回类型为 StmtRes
antlrcpp::Any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) {
  return visitSmall_stmt(ctx->small_stmt());
}

// 自动 visit
// 返回类型为 StmtRes
antlrcpp::Any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) {
  return visitChildren(ctx);  // Done.
}

// 返回类型为 StmtRes(kNormal)
antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) {
  auto list_array = ctx->testlist();
  if (list_array.size() == 1)
    visitTestlist(list_array[0]);
  else {
    vector<antlrcpp::Any> left_list, right_list;
    right_list = visitTestlist(list_array.back()).as<vector<antlrcpp::Any>>();
    // 初始时 right_list 为最右边的 testlist
    if (ctx->augassign()) {
      auto op = ctx->augassign();
      left_list = visitTestlist(list_array[0]).as<vector<antlrcpp::Any>>();
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
        left_list = visitTestlist(list_array[i]).as<vector<antlrcpp::Any>>();
        for (int j = 0; j < left_list.size(); ++j)
          GetValue(left_list[j]) = GetValue(right_list[j]);
        // 注意这里的赋值规则和标准 python 略有不同
        // TODO : 判断非变量与 list 大小不同的情况
        right_list = left_list;  // 减少 visit 次数
      }
  }
  return kNormal;  // 无返回值
}

// 不会用到
antlrcpp::Any EvalVisitor::visitAugassign(Python3Parser::AugassignContext *ctx) {
  return visitChildren(ctx);  // Done.
}

// 自动 visit
// 返回类型为 StmtRes
antlrcpp::Any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) {
  return visitChildren(ctx);  // Done.
}

// 返回类型为 StmtRes
antlrcpp::Any EvalVisitor::visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) {
  return kBreak;
}

// 返回类型为 StmtRes
antlrcpp::Any EvalVisitor::visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) {
  return kContinue;
}

// 将返回值存到 return_value 中
// 返回类型为 StmtRes
antlrcpp::Any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) {
  if (!ctx->testlist())
    return_value = RealAny(kNone);
  else {
    auto list_array = visitTestlist(ctx->testlist()).as<vector<antlrcpp::Any>>();
    if (list_array.size() == 1)
      return_value = GetValue(list_array[0]);
    else {
      vector<RealAny> ans;
      for (auto it : list_array) ans.push_back(GetValue(it));
      return_value = ans;
    }
  }
  return kReturn;
}

// 自动 visit
// 返回类型为 StmtRes
antlrcpp::Any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) {
  return visitChildren(ctx);  // Done.
}

// 返回类型为 StmtRes
antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx) {
  auto test_array = ctx->test();
  auto suite_array = ctx->suite();
  int i, elif_size = ctx->ELIF().size();
  for (i = 0; i <= elif_size; ++i)
    if (GetValue(visitTest(test_array[i])).ToBool())
      return visitSuite(suite_array[i]);
  if (ctx->ELSE()) return visitSuite(suite_array[i]);
  return kNormal;
}

// 返回类型为 StmtRes
antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx) {
  while (GetValue(visitTest(ctx->test())).ToBool()) {
    auto result = visitSuite(ctx->suite()).as<StmtRes>();
    if (result == kBreak)
      break;
    else if (result == kReturn)
      return kReturn;
  }
  return kNormal;
}

// if while funcdef 中用到
// 返回类型为 StmtRes
antlrcpp::Any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx) {
  if (ctx->simple_stmt()) return visitSimple_stmt(ctx->simple_stmt());
  auto stmt_array = ctx->stmt();
  for (auto it : stmt_array) {
    StmtRes result = visitStmt(it).as<StmtRes>();
    if (result) return result;
  }
  return kNormal;
}

// 可能 test 是一个 testlist (函数返回值)
// 返回类型为 RealAny(可能为 tuple) 或 string
antlrcpp::Any EvalVisitor::visitTest(Python3Parser::TestContext *ctx) {
  return visitOr_test(ctx->or_test());
}

// 返回类型为 RealAny(可能为 tuple) 或 string
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

// 返回类型为 RealAny(可能为 tuple) 或 string
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

// 返回类型为 RealAny(可能为 tuple) 或 string
antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx) {
  if (ctx->comparison()) return visitComparison(ctx->comparison());
  return (RealAny)!GetValue(visitNot_test(ctx->not_test()));
}

// 返回类型为 RealAny(可能为 tuple) 或 string
antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx) {
  auto arith_array = ctx->arith_expr();
  if (arith_array.size() == 1) return visitArith_expr(arith_array[0]);
  auto op_array = ctx->comp_op();
  RealAny lhs = GetValue(visitArith_expr(arith_array[0])), rhs;
  for (int i = 1; i < arith_array.size(); ++i) {
    rhs = GetValue(visitArith_expr(arith_array[i]));
    string op_str = op_array[i - 1]->getText();
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

// 不会用到
antlrcpp::Any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx) {
  return visitChildren(ctx);  // Done.
}

// 返回类型为 RealAny(可能为 tuple) 或 string
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

// 不会用到
antlrcpp::Any EvalVisitor::visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) {
  return visitChildren(ctx);  // Done.
}

// 返回类型为 RealAny(可能为 tuple) 或 string
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

// 不会用到
antlrcpp::Any EvalVisitor::visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) {
  return visitChildren(ctx);  // Done.
}

// 返回类型为 RealAny(可能为 tuple) 或 string
antlrcpp::Any EvalVisitor::visitFactor(Python3Parser::FactorContext *ctx) {
  if (ctx->atom_expr()) return visitAtom_expr(ctx->atom_expr());
  if (ctx->ADD())
    return GetValue(visitFactor(ctx->factor()));
  else
    return -GetValue(visitFactor(ctx->factor()));
}

// 函数调用
// 返回类型为 RealAny(可能为 tuple) 或 string
antlrcpp::Any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx) {
  // 函数都有 RealAny 类型的返回值
  if (!ctx->trailer()) return visitAtom(ctx->atom());
  auto list_array = visitTrailer(ctx->trailer()).as<ParametersType>();
  // 在 Positional argument 情况下调用 .second 即可
  string func_name = ctx->atom()->getText();  // 函数名
  // Check_BuiltinFunction
  if (func_name == "print") {
    for (auto it : list_array) std::cout << it.second << ' ';
    std::cout << '\n';
    return RealAny();  // 返回值为 None
  } else if (func_name == "int")
    return RealAny(list_array.empty() ? int2048(0) : list_array[0].second.ToInt());
  else if (func_name == "float")
    return RealAny(list_array.empty() ? double() : list_array[0].second.ToFloat());
  else if (func_name == "str")
    return RealAny(list_array.empty() ? string() : list_array[0].second.ToStr());
  else if (func_name == "bool")
    return RealAny(list_array.empty() ? bool() : list_array[0].second.ToBool());
  else {  // other defined function
          // TODO : 调用用户定义的函数
    variable.AddLevel();
    auto para_array = function.Parameters(func_name);
    int list_size = list_array.size(), para_size = para_array.size();
    for (int i = 0; i < para_size; ++i) {
      string var_name;
      RealAny value;
      if (i < list_size) {
        var_name = list_array[i].first.empty() ? para_array[i].first : list_array[i].first;
        value = list_array[i].second;
      } else {
        var_name = para_array[i].first;
        value = para_array[i].second;
      }
      variable.LefValue(var_name) = value;
    }
    visitSuite(function.Suite(func_name));
    variable.DelLevel();
    return return_value;
  }
}

// 返回类型为 ParametersType (vector<pair<string, RealAny>>)
antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx) {
  if (!ctx->arglist()) return ParametersType();
  return visitArglist(ctx->arglist());  // visit 即可，上级会 as
}

// 注意有加括号回指 test
// 返回类型为 RealAny(可能为 tuple) 或 string
antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx) {
  if (ctx->NUMBER()) {
    string str = ctx->NUMBER()->getText();
    // https://en.cppreference.com/w/cpp/string/basic_string/npos
    if (str.find('.') == string::npos)
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
    string ans;
    auto str_array = ctx->STRING();
    for (auto it : str_array) {
      string tmp = it->getText();
      tmp.pop_back(), ans += tmp.substr(1);
    }
    return RealAny(ans);
  }
}

// 返回类型为 vector<antlrcpp::Any>
antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx) {
  vector<antlrcpp::Any> ans;  // 用好两种 Any 类
  auto test_array = ctx->test();
  for (auto it : test_array) {
    antlrcpp::Any tmp = visitTest(it);
    // 在 testlist 和 arglist 处将 tuple 解包装 
    if (tmp.is<RealAny>() && tmp.as<RealAny>().type == kTuple) {
      auto tuple_array = tmp.as<RealAny>().tuple;
      for (auto j : tuple_array) ans.push_back(j);
    } else {
      ans.push_back(tmp);
    }
  }
  return ans;
}

// 返回类型为 ParametersType (vector<pair<string, RealAny>>)
antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx) {
  ParametersType list_array;
  auto argument_array = ctx->argument();
  for (auto it : argument_array) {
    // 在 testlist 和 arglist 处将 tuple 解包装
    auto tmp = visitArgument(it).as<ArguType>();
    if (tmp.second.type == kTuple) {
      auto tuple_array = tmp.second.tuple;
      for (auto j : tuple_array) list_array.push_back(make_pair(string(), j));
    } else {
      list_array.push_back(tmp);
    }
  }
  return list_array;
}

// 此处发生 antlrcpp::Any -> RealAny 转换
// 返回类型为 ArguType (pair<string, RealAny>)
antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx) {
  if (!ctx->ASSIGN()) return make_pair(string(), GetValue(visitTest(ctx->test(0))));
  // Positional argument
  return make_pair(ctx->test(0)->getText(), GetValue(visitTest(ctx->test(1))));
  // Keyword argument
}