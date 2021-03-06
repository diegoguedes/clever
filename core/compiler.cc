/**
 * Clever programming language
 * Copyright (c) Clever Team
 *
 * This file is distributed under the MIT license. See LICENSE for details.
 */

#include "core/cstring.h"
#include "core/compiler.h"
#include "core/location.hh"
#include "core/astdump.h"
#include "core/codegen.h"
#include "core/evaluator.h"
#include "core/resolver.h"

namespace clever {

/// Compiler initialization phase
void Compiler::init(const CString* fname)
{
	if (m_flags & INITIALIZED) {
		return;
	}

	m_pkg.init();

	if (fname) {
		std::string path(*fname);

		path.erase(std::find(path.rbegin(), path.rend(), '/').base(), path.end());

		m_pkg.setIncludePath(path);
	}

	m_flags |= INITIALIZED;
}

/// Frees all resource used by the compiler
void Compiler::shutdown()
{
	delete m_builder;

	m_pkg.shutdown();

	clever_delete_var(g_cstring_tbl);
}

/// Displays an error message and exits
void Compiler::error(const char* msg)
{
	std::cerr << "Compile error: " << msg << std::endl;
	CLEVER_EXIT_FATAL();
}

/// Displays an error message
void Compiler::error(const std::string& message, const location& loc)
{
	if (loc.begin.filename) {
		std::cerr << "Compile error: " << message << " on "
			<< *loc.begin.filename << " line " << loc.begin.line << "\n";
	} else {
		std::cerr << "Compile error: " << message << " on line "
			<< loc.begin.line << "\n";
	}
	CLEVER_EXIT_FATAL();
}

/// Displays a formatted error message and abort the compilation
void Compiler::errorf(const location& loc, const char* format, ...)
{
	std::ostringstream out;
	va_list args;

	va_start(args, format);
	vsprintf(out, format, args);
	va_end(args);

	error(out.str(), loc);
}

void Compiler::genCode()
{
	if (!m_tree) {
		return;
	}

	ast::Node* tree = m_tree;

	if (m_flags & USE_OPTIMIZER) {
		ast::Evaluator evaluator;
		tree = m_tree->accept(evaluator);
	}

	if (m_flags & DUMP_AST) {
		ast::Dumper astdump;
		tree->accept(astdump);
	}

	ast::Resolver resolver(m_pkg, getNamespace());
	tree->accept(resolver);

	if (!(m_flags & PARSER_ONLY)) {
		m_global_env = resolver.getGlobalEnv();

		m_builder = new IRBuilder(m_global_env, resolver.getSymTable());

		ast::Codegen codegen(m_builder);
		tree->accept(codegen);

		m_builder->push(OP_HALT);
	}

	clever_delete_var(tree);
}

} // clever
