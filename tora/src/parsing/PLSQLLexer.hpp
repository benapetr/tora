/** \file
 *  This C++ header file was generated by $ANTLR version 3.5.1-SNAPSHOT
 *
 *     -  From the grammar source file : PLSQLLexer.g
 *     -                            On : 2013-06-21 13:57:11
 *     -                 for the lexer : PLSQLLexerLexer
 *
 * Editing it, at least manually, is not wise.
 *
 * C++ language generator and runtime by Gokulakannan Somasundaram ( heavy lifting from C Run-time by Jim Idle )
 *
 *
 * The lexer PLSQLLexer has the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 *
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 */
// [The "BSD license"]
// Copyright (c) 2005-2009 Gokulakannan Somasundaram. 
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	_PLSQLLexer_H
#define _PLSQLLexer_H
/* =============================================================================
 * Standard antlr3 C++ runtime definitions
 */
#include <antlr3.hpp>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */


	#include "UserTraits.hpp"




/*
 * Oracle(c) PL/SQL 11g Parser  
 *
 * Copyright (c) 2009-2011 Alexandre Porcelli <alexandre.porcelli@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifdef	WIN32
// Disable: Unreferenced parameter,							- Rules with parameters that are not used
//          constant conditional,							- ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable					- tree rewrite variables declared but not needed
//          Unreferenced local variable						- lexer rule declares but does not always use _type
//          potentially unitialized variable used			- retval always returned from a rule
//			unreferenced local function has been removed	- susually getTokenNames or freeScope, they can go without warnigns
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4701 )
#endif
namespace 
User
 {

typedef PLSQLLexerTraits PLSQLLexerImplTraits;


class PLSQLLexerTokens
{
public:
	/** Symbolic definitions of all the tokens that the lexer will work with.
	 *
	 * Antlr will define EOF, but we can't use that as it it is too common in
	 * in C header files and that would be confusing. There is no way to filter this out at the moment
	 * so we just undef it here for now. That isn't the value we get back from C recognizers
	 * anyway. We are looking for ANTLR_TOKEN_EOF.
	 */
	enum Tokens 
	{
		EOF_TOKEN = PLSQLLexerImplTraits::CommonTokenType::TOKEN_EOF
		, AMPERSAND = 4 
		, APPROXIMATE_NUM_LIT = 5 
		, ASSIGN_OP = 6 
		, ASTERISK = 7 
		, AT_SIGN = 8 
		, BINDVAR = 9 
		, BIT_STRING_LIT = 10 
		, CARRET_OPERATOR_PART = 11 
		, CHAR_STRING = 12 
		, CHAR_STRING_PERL = 13 
		, COLON = 14 
		, COMMA = 15 
		, COMMENT = 16 
		, CONCATENATION_OP = 17 
		, DELIMITED_ID = 18 
		, DOUBLE_ASTERISK = 19 
		, DOUBLE_PERIOD = 20 
		, EQUALS_OP = 21 
		, EXACT_NUM_LIT = 22 
		, EXCLAMATION_OPERATOR_PART = 23 
		, FOR_NOTATION = 24 
		, GREATER_THAN_OP = 25 
		, GREATER_THAN_OR_EQUALS_OP = 26 
		, HEX_STRING_LIT = 27 
		, INTRODUCER = 28 
		, LEFT_BRACKET = 29 
		, LEFT_PAREN = 30 
		, LESS_THAN_OP = 31 
		, LESS_THAN_OR_EQUALS_OP = 32 
		, MINUS_SIGN = 33 
		, NATIONAL_CHAR_STRING_LIT = 34 
		, NEWLINE = 35 
		, NOT_EQUAL_OP = 36 
		, PERCENT = 37 
		, PERIOD = 38 
		, PLSQL_NON_RESERVED_CONNECT_BY_ROOT = 39 
		, PLSQL_NON_RESERVED_ELSIF = 40 
		, PLSQL_NON_RESERVED_MODEL = 41 
		, PLSQL_NON_RESERVED_PIVOT = 42 
		, PLSQL_NON_RESERVED_UNPIVOT = 43 
		, PLSQL_NON_RESERVED_USING = 44 
		, PLSQL_RESERVED_CLUSTERS = 45 
		, PLSQL_RESERVED_COLAUTH = 46 
		, PLSQL_RESERVED_COMPRESS = 47 
		, PLSQL_RESERVED_CRASH = 48 
		, PLSQL_RESERVED_EXCLUSIVE = 49 
		, PLSQL_RESERVED_IDENTIFIED = 50 
		, PLSQL_RESERVED_IF = 51 
		, PLSQL_RESERVED_INDEX = 52 
		, PLSQL_RESERVED_INDEXES = 53 
		, PLSQL_RESERVED_LOCK = 54 
		, PLSQL_RESERVED_MINUS = 55 
		, PLSQL_RESERVED_MODE = 56 
		, PLSQL_RESERVED_NOCOMPRESS = 57 
		, PLSQL_RESERVED_NOWAIT = 58 
		, PLSQL_RESERVED_RESOURCE = 59 
		, PLSQL_RESERVED_SHARE = 60 
		, PLSQL_RESERVED_START = 61 
		, PLSQL_RESERVED_TABAUTH = 62 
		, PLSQL_RESERVED_VIEWS = 63 
		, PLUS_SIGN = 64 
		, PROMPT = 65 
		, QS_ANGLE = 66 
		, QS_BRACE = 67 
		, QS_BRACK = 68 
		, QS_OTHER = 69 
		, QS_OTHER_CH = 70 
		, QS_PAREN = 71 
		, QUESTION_MARK = 72 
		, QUOTE = 73 
		, REGULAR_ID = 74 
		, RIGHT_BRACKET = 75 
		, RIGHT_PAREN = 76 
		, SEMICOLON = 77 
		, SEPARATOR = 78 
		, SIMPLE_LETTER = 79 
		, SOLIDUS = 80 
		, SPACE = 81 
		, SQL92_RESERVED_ALL = 82 
		, SQL92_RESERVED_ALTER = 83 
		, SQL92_RESERVED_AND = 84 
		, SQL92_RESERVED_ANY = 85 
		, SQL92_RESERVED_AS = 86 
		, SQL92_RESERVED_ASC = 87 
		, SQL92_RESERVED_BEGIN = 88 
		, SQL92_RESERVED_BETWEEN = 89 
		, SQL92_RESERVED_BY = 90 
		, SQL92_RESERVED_CASE = 91 
		, SQL92_RESERVED_CHECK = 92 
		, SQL92_RESERVED_CONNECT = 93 
		, SQL92_RESERVED_CREATE = 94 
		, SQL92_RESERVED_CURRENT = 95 
		, SQL92_RESERVED_CURSOR = 96 
		, SQL92_RESERVED_DATE = 97 
		, SQL92_RESERVED_DECLARE = 98 
		, SQL92_RESERVED_DEFAULT = 99 
		, SQL92_RESERVED_DELETE = 100 
		, SQL92_RESERVED_DESC = 101 
		, SQL92_RESERVED_DISTINCT = 102 
		, SQL92_RESERVED_DROP = 103 
		, SQL92_RESERVED_ELSE = 104 
		, SQL92_RESERVED_END = 105 
		, SQL92_RESERVED_EXCEPTION = 106 
		, SQL92_RESERVED_EXISTS = 107 
		, SQL92_RESERVED_FALSE = 108 
		, SQL92_RESERVED_FETCH = 109 
		, SQL92_RESERVED_FOR = 110 
		, SQL92_RESERVED_FROM = 111 
		, SQL92_RESERVED_GOTO = 112 
		, SQL92_RESERVED_GRANT = 113 
		, SQL92_RESERVED_GROUP = 114 
		, SQL92_RESERVED_HAVING = 115 
		, SQL92_RESERVED_IN = 116 
		, SQL92_RESERVED_INSERT = 117 
		, SQL92_RESERVED_INTERSECT = 118 
		, SQL92_RESERVED_INTO = 119 
		, SQL92_RESERVED_IS = 120 
		, SQL92_RESERVED_LIKE = 121 
		, SQL92_RESERVED_NOT = 122 
		, SQL92_RESERVED_NULL = 123 
		, SQL92_RESERVED_OF = 124 
		, SQL92_RESERVED_ON = 125 
		, SQL92_RESERVED_OPTION = 126 
		, SQL92_RESERVED_OR = 127 
		, SQL92_RESERVED_ORDER = 128 
		, SQL92_RESERVED_OVERLAPS = 129 
		, SQL92_RESERVED_PRIOR = 130 
		, SQL92_RESERVED_PROCEDURE = 131 
		, SQL92_RESERVED_PUBLIC = 132 
		, SQL92_RESERVED_REVOKE = 133 
		, SQL92_RESERVED_SELECT = 134 
		, SQL92_RESERVED_SIZE = 135 
		, SQL92_RESERVED_TABLE = 136 
		, SQL92_RESERVED_THE = 137 
		, SQL92_RESERVED_THEN = 138 
		, SQL92_RESERVED_TO = 139 
		, SQL92_RESERVED_TRUE = 140 
		, SQL92_RESERVED_UNION = 141 
		, SQL92_RESERVED_UNIQUE = 142 
		, SQL92_RESERVED_UPDATE = 143 
		, SQL92_RESERVED_VALUES = 144 
		, SQL92_RESERVED_VIEW = 145 
		, SQL92_RESERVED_WHEN = 146 
		, SQL92_RESERVED_WHERE = 147 
		, SQL92_RESERVED_WITH = 148 
		, TILDE_OPERATOR_PART = 149 
		, UNDERSCORE = 150 
		, UNSIGNED_INTEGER = 151 
		, VERTICAL_BAR = 152 
		, ZV = 153 
	};

	const ANTLR_UINT8* getTokenName(ANTLR_INT32 index) const;

protected:
	static ANTLR_UINT8* TokenNames[];
};

/** Context tracking structure for PLSQLLexer
 */
class PLSQLLexer : public PLSQLLexerImplTraits::BaseLexerType
    , public PLSQLLexerTokens
{
public:
	typedef PLSQLLexerImplTraits ImplTraits;
	typedef PLSQLLexer ComponentType;
	typedef ComponentType::StreamType StreamType;
	typedef PLSQLLexerImplTraits::BaseLexerType BaseType;
	typedef ImplTraits::RecognizerSharedStateType<StreamType> RecognizerSharedStateType;
	typedef StreamType InputType;
	static const bool IsFiltered = false;


private:	
public:
    PLSQLLexer(InputType* instream);
    PLSQLLexer(InputType* instream, RecognizerSharedStateType* state);

    void init(InputType* instream  );



//     // buffer (queue) to hold the emit()'d tokens
//     // private LinkedList<Token> tokenBuffer = new LinkedList<Token>();

//     // public void emit(Token t) {
//     //     tokenBuffer.add(t);
//     // }

//     // private void advanceInput(){
//     //     state.tokenStartCharIndex = input.index();
//     //     state.tokenStartCharPositionInLine = input.getCharPositionInLine();
//     //     state.tokenStartLine = input.getLine();
//     // }


    /**
     * Return a token from this source; i.e., match a token on the char stream.
     */
    // public Token nextToken() {
    //     while (true) {
    //         if (tokenBuffer.size() == 0) {
    //             state.token = null;
    //             state.channel = Token.DEFAULT_CHANNEL;
    //             state.tokenStartCharIndex = input.index();
    //             state.tokenStartCharPositionInLine = input.getCharPositionInLine();
    //             state.tokenStartLine = input.getLine();
    //             state.text = null;
    //             if (input.LA(1) == CharStream.EOF) {
    //                 return Token.EOF_TOKEN;
    //             }
    //             try {
    //                 int m = input.mark();
    //                 state.backtracking = 1;
    //                 state.failed = false;
    //                 mTokens();
    //                 state.backtracking = 0;

    //                 if (state.failed) {
    //                     input.rewind(m);
    //                     input.consume();
    //                 } else {
    //                     emit();
    //                 }
    //             } catch (RecognitionException re) {
    //                 // shouldn't happen in backtracking mode, but...
    //                 reportError(re);
    //                 recover(re);
    //             }
    //         } else {
    //             Token result = tokenBuffer.poll();
    //             if (result == Token.SKIP_TOKEN || result.getType() == Token.INVALID_TOKEN_TYPE || result == null)
    //             {
    //                 // discard
    //                 // SKIP & INVALID
    //                 // tokens
    //                 continue;
    //             }
    //             return result;
    //         }
    //     }
    // }

    
    void  mFOR_NOTATION( );
    void  mNATIONAL_CHAR_STRING_LIT( );
    void  mBIT_STRING_LIT( );
    void  mHEX_STRING_LIT( );
    void  mPERIOD( );
    void  mEXACT_NUM_LIT( );
    void  mCHAR_STRING( );
    void  mCHAR_STRING_PERL( );
    void  mQUOTE( );
    void  mQS_ANGLE( );
    void  mQS_BRACE( );
    void  mQS_BRACK( );
    void  mQS_PAREN( );
    void  mQS_OTHER_CH( );
    void  mQS_OTHER( );
    void  mDELIMITED_ID( );
    void  mPERCENT( );
    void  mAMPERSAND( );
    void  mLEFT_PAREN( );
    void  mRIGHT_PAREN( );
    void  mDOUBLE_ASTERISK( );
    void  mASTERISK( );
    void  mPLUS_SIGN( );
    void  mCOMMA( );
    void  mSOLIDUS( );
    void  mAT_SIGN( );
    void  mASSIGN_OP( );
    void  mBINDVAR( );
    void  mCOLON( );
    void  mSEMICOLON( );
    void  mLESS_THAN_OR_EQUALS_OP( );
    void  mLESS_THAN_OP( );
    void  mGREATER_THAN_OR_EQUALS_OP( );
    void  mNOT_EQUAL_OP( );
    void  mCARRET_OPERATOR_PART( );
    void  mTILDE_OPERATOR_PART( );
    void  mEXCLAMATION_OPERATOR_PART( );
    void  mGREATER_THAN_OP( );
    void  mQUESTION_MARK( );
    void  mCONCATENATION_OP( );
    void  mVERTICAL_BAR( );
    void  mEQUALS_OP( );
    void  mLEFT_BRACKET( );
    void  mRIGHT_BRACKET( );
    void  mINTRODUCER( );
    void  mSEPARATOR( );
    void  mSIMPLE_LETTER( );
    void  mUNSIGNED_INTEGER( );
    void  mCOMMENT( );
    void  mPROMPT( );
    void  mNEWLINE( );
    void  mSPACE( );
    void  mAPPROXIMATE_NUM_LIT( );
    void  mMINUS_SIGN( );
    void  mUNDERSCORE( );
    void  mDOUBLE_PERIOD( );
    void  mSQL92_RESERVED_ALL( );
    void  mSQL92_RESERVED_ALTER( );
    void  mSQL92_RESERVED_AND( );
    void  mSQL92_RESERVED_ANY( );
    void  mSQL92_RESERVED_AS( );
    void  mSQL92_RESERVED_ASC( );
    void  mSQL92_RESERVED_BEGIN( );
    void  mSQL92_RESERVED_BETWEEN( );
    void  mSQL92_RESERVED_BY( );
    void  mSQL92_RESERVED_CASE( );
    void  mSQL92_RESERVED_CHECK( );
    void  mPLSQL_RESERVED_CLUSTERS( );
    void  mPLSQL_RESERVED_COLAUTH( );
    void  mPLSQL_RESERVED_COMPRESS( );
    void  mSQL92_RESERVED_CONNECT( );
    void  mPLSQL_NON_RESERVED_CONNECT_BY_ROOT( );
    void  mPLSQL_RESERVED_CRASH( );
    void  mSQL92_RESERVED_CREATE( );
    void  mSQL92_RESERVED_CURRENT( );
    void  mSQL92_RESERVED_CURSOR( );
    void  mSQL92_RESERVED_DATE( );
    void  mSQL92_RESERVED_DECLARE( );
    void  mSQL92_RESERVED_DEFAULT( );
    void  mSQL92_RESERVED_DELETE( );
    void  mSQL92_RESERVED_DESC( );
    void  mSQL92_RESERVED_DISTINCT( );
    void  mSQL92_RESERVED_DROP( );
    void  mSQL92_RESERVED_ELSE( );
    void  mSQL92_RESERVED_END( );
    void  mSQL92_RESERVED_EXCEPTION( );
    void  mPLSQL_RESERVED_EXCLUSIVE( );
    void  mSQL92_RESERVED_EXISTS( );
    void  mSQL92_RESERVED_FALSE( );
    void  mSQL92_RESERVED_FETCH( );
    void  mSQL92_RESERVED_FOR( );
    void  mSQL92_RESERVED_FROM( );
    void  mSQL92_RESERVED_GOTO( );
    void  mSQL92_RESERVED_GRANT( );
    void  mSQL92_RESERVED_GROUP( );
    void  mSQL92_RESERVED_HAVING( );
    void  mPLSQL_RESERVED_IDENTIFIED( );
    void  mPLSQL_RESERVED_IF( );
    void  mSQL92_RESERVED_IN( );
    void  mPLSQL_RESERVED_INDEX( );
    void  mPLSQL_RESERVED_INDEXES( );
    void  mSQL92_RESERVED_INSERT( );
    void  mSQL92_RESERVED_INTERSECT( );
    void  mSQL92_RESERVED_INTO( );
    void  mSQL92_RESERVED_IS( );
    void  mSQL92_RESERVED_LIKE( );
    void  mPLSQL_RESERVED_LOCK( );
    void  mPLSQL_RESERVED_MINUS( );
    void  mPLSQL_RESERVED_MODE( );
    void  mPLSQL_RESERVED_NOCOMPRESS( );
    void  mSQL92_RESERVED_NOT( );
    void  mPLSQL_RESERVED_NOWAIT( );
    void  mSQL92_RESERVED_NULL( );
    void  mSQL92_RESERVED_OF( );
    void  mSQL92_RESERVED_ON( );
    void  mSQL92_RESERVED_OPTION( );
    void  mSQL92_RESERVED_OR( );
    void  mSQL92_RESERVED_ORDER( );
    void  mSQL92_RESERVED_OVERLAPS( );
    void  mSQL92_RESERVED_PRIOR( );
    void  mSQL92_RESERVED_PROCEDURE( );
    void  mSQL92_RESERVED_PUBLIC( );
    void  mPLSQL_RESERVED_RESOURCE( );
    void  mSQL92_RESERVED_REVOKE( );
    void  mSQL92_RESERVED_SELECT( );
    void  mPLSQL_RESERVED_SHARE( );
    void  mSQL92_RESERVED_SIZE( );
    void  mPLSQL_RESERVED_START( );
    void  mPLSQL_RESERVED_TABAUTH( );
    void  mSQL92_RESERVED_TABLE( );
    void  mSQL92_RESERVED_THE( );
    void  mSQL92_RESERVED_THEN( );
    void  mSQL92_RESERVED_TO( );
    void  mSQL92_RESERVED_TRUE( );
    void  mSQL92_RESERVED_UNION( );
    void  mSQL92_RESERVED_UNIQUE( );
    void  mSQL92_RESERVED_UPDATE( );
    void  mSQL92_RESERVED_VALUES( );
    void  mSQL92_RESERVED_VIEW( );
    void  mPLSQL_RESERVED_VIEWS( );
    void  mSQL92_RESERVED_WHEN( );
    void  mSQL92_RESERVED_WHERE( );
    void  mSQL92_RESERVED_WITH( );
    void  mPLSQL_NON_RESERVED_USING( );
    void  mPLSQL_NON_RESERVED_MODEL( );
    void  mPLSQL_NON_RESERVED_ELSIF( );
    void  mPLSQL_NON_RESERVED_PIVOT( );
    void  mPLSQL_NON_RESERVED_UNPIVOT( );
    void  mREGULAR_ID( );
    void  mZV( );
    void  mTokens( );
    const char *    getGrammarFileName();
    void            reset();
    ~PLSQLLexer();

};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//

/* End of token definitions for PLSQLLexer
 * =============================================================================
 */

}

#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */
