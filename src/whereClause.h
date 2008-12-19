// $Id$
// Author: John Wu <John.Wu at acm.org>
//      Lawrence Berkeley National Laboratory
// Copyright 2007-2008 the Regents of the University of California
/** \file Declares ibis::whereClause class. */
#ifndef IBIS_WHERECLAUSE_H
#define IBIS_WHERECLAUSE_H
#include "qExpr.h"

namespace ibis {
    class whereClause;
    class whereLexer;
    class whereParser;
}

/// A class to encapsulate the parsing of a string into an ibis::qExpr
/// object.  One may access the functions defined for ibis::qExpr through
/// the operator->.
///
/// A where clause is a set of range conditions joined together with
/// logical operators.  The supported logical operators are
/// @code
/// NOT, AND, OR, XOR, &&, ||.
/// @endcode

/// The supported range conditions are equality conditions, discrete
/// ranges, one-sided range conditions and two-sided range conditions.

/// - An equality condition is defined by the equal operator and its two
/// operands can be arithematic expressions, column names, numbers or
/// string literals.  On string valued columns, FastBit currently only
/// supports equality comparisons.  In such a case, the comparison is of
/// the form "column_name = column_value".  Internally, when FastBit detect
/// that the type of the column named "column_name" is ibis::CATEGORY or
/// ibis::TEXT, it will interpret the other side as literal string value to
/// be compared.  Note that if the left operand of the equality operator is
/// not a known column name, the evaluation function will examine the right
/// operand to see if it is a column name.  If the right operand is the
/// name of string-valued column, the left operand will be used as string
/// literal.

/// - A discrete range is defined by the operator "IN", e.g.,
/// @code
/// column_name IN ( list_of_strings_or_numbers )
/// @endcode
/// Note unquoted string values must start with an alphabet or a
/// underscore.  Strings starting with anything else must be quoted.

/// - A one-side range condtion can be defined with any of the following
/// operators, <, <=, >, and >=.  The two operands of the operator can be
/// any arithmetic expressions, column names or numbers.

/// - A two-sided range condtion can be defined with two operators selected
/// from <, <=, >, and >=, if their directions much agree.  Alternatively,
/// they can be defined with operators "... between ... and ...", where
/// @code
/// A between B and C
/// @endcode
/// is equivalent to
/// @code
/// B <= A <= C
/// @end
/// In this example, A, B, and C can be any arithematic expression, column
/// name or number.

/// An arithematic expression may contain operators +, -, *, /, %, ^, and
/// **, as well as common one-argument and two-argument functions defined
/// in the header file math.h.  Both operators ^ and ** denote the
/// exponential operation.
/// @note Operators & and | are reserved for bitwise logical operations.
/// These two operators are currently recognized but their operations are
/// not supported.
class ibis::whereClause {
public:
    /// Construct a where clause from a string.
    explicit whereClause(const char *cl=0);
    /// Construct a where clause from another where clause.
    whereClause(const whereClause&);
    /// Destructor.
    ~whereClause();

    /// Parse a new string.
    int parse(const char *cl);
    /// Set a new set of conditions directly.
    void setExpr(const ibis::qExpr *ex) {
	clause_.clear();
	delete expr_;
	expr_ = ex->dup();
    }
    void resetString() {
	if (expr_ != 0) {
	    std::ostringstream oss;
	    oss << *expr_;
	    clause_ = oss.str();
	}
	else {
	    clause_.clear();
	}
    }

    /// Clear the existing content.
    void clear() throw ();
    /// The where clause is considered empty if the expr_ is a nil pointer.
    bool empty() const {return (expr_ == 0);}

    /// Return a pointer to the string form of the where clause.
    const char* getString(void) const {
	if (clause_.size() > 0)
	    return clause_.c_str();
	else
	    return 0;
    }
    /// Return a pointer to the root of the expression tree for the where
    /// clause.
    ///@note Functions that modify this object may invalidate the pointer
    /// returned by this function.
    const ibis::qExpr* getExpr(void) const {return expr_;}
    ibis::qExpr* getExpr(void) {return expr_;}
    /// Simplify the query expression.
    void simplify() {ibis::qExpr::simplify(expr_);}
    /// Verify the names exist in the data partition p0.
    int verify(const ibis::part& p0);

    /// Member access operator redefined to point to ibis::qExpr.
    ibis::qExpr* operator->() {return expr_;}
    /// Member access operator redefined to point to const ibis::qExpr.
    const ibis::qExpr* operator->() const {return expr_;}

    /// Assignment operator.
    whereClause& operator=(const whereClause&);
    /// Swap the contents of two where clauses.
    void swap(whereClause& rhs) throw () {
	clause_.swap(rhs.clause_);
	ibis::qExpr* tmp = rhs.expr_;
	rhs.expr_ = expr_;
	expr_ = tmp;
    }

protected:
    std::string clause_;	// string version of the where clause
    ibis::qExpr *expr_;		// the expression tree

    /// Add conditions implied by self-join conditions.
    void amplify(const ibis::part&);
    int _verify(const ibis::part&, ibis::qExpr*&);

private:
    ibis::whereLexer *lexer;	// hold a pointer for the parser

    friend class ibis::whereParser;
};
#endif