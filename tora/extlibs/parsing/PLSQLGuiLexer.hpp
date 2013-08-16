/** \file
 *  This C++ header file was generated by $ANTLR version 3.5.1-SNAPSHOT
 *
 *     -  From the grammar source file : PLSQLGuiLexer.g
 *     -                            On : 2013-03-15 14:36:36
 *     -                 for the lexer : PLSQLGuiLexerLexer
 *
 * Editing it, at least manually, is not wise.
 *
 * C++ language generator and runtime by Gokulakannan Somasundaram ( heavy lifting from C Run-time by Jim Idle )
 *
 *
 * The lexer PLSQLGuiLexer has the callable functions (rules) shown below,
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

#ifndef	_PLSQLGuiLexer_H
#define _PLSQLGuiLexer_H
/* =============================================================================
 * Standard antlr3 C++ runtime definitions
 */
#include <antlr3.hpp>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */


#include "UserGuiTraits.hpp"
#include <vector>




/*
 * Oracle(c) PL/SQL 11g Parser  
 *
 * Copyright (c) 2009-2011 Alexandre Porcelli <alexandre.porcelli@gmail.com>
 * Copyright (c) 2012-2013 Ivan Brezina <ibre5041@ibrezina.net> 
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
 *
 * This code was forked from PL/SQL parser and adoptopted 
 * for QScintilla's custom lexer.
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
namespace  User  {

typedef PLSQLGuiLexerTraits PLSQLGuiLexerImplTraits;


class PLSQLGuiLexerTokens
{
public:
	/** Symbolic definitions of all the tokens that the 
lexer
 will work with.
	 * \{
	 *
	 * Antlr will define EOF, but we can't use that as it it is too common in
	 * in C header files and that would be confusing. There is no way to filter this out at the moment
	 * so we just undef it here for now. That isn't the value we get back from C recognizers
	 * anyway. We are looking for ANTLR_TOKEN_EOF.
	 */
	enum Tokens 
	{
		EOF_TOKEN = PLSQLGuiLexerImplTraits::CommonTokenType::TOKEN_EOF
		, AMPERSAND = 4 
		, APPROXIMATE_NUM_LIT = 5 
		, ASSIGN_OP = 6 
		, ASTERISK = 7 
		, AT_SIGN = 8 
		, BINDVAR = 9 
		, BIT_STRING_LIT = 10 
		, BUILDIN_FUNCTIONS = 11 
		, CARRET_OPERATOR_PART = 12 
		, CHAR_STRING = 13 
		, CHAR_STRING_PERL = 14 
		, COLON = 15 
		, COMMA = 16 
		, COMMENT = 17 
		, CONCATENATION_OP = 18 
		, DELIMITED_ID = 19 
		, DOUBLE_ASTERISK = 20 
		, DOUBLE_PERIOD = 21 
		, EQUALS_OP = 22 
		, EXACT_NUM_LIT = 23 
		, EXCLAMATION_OPERATOR_PART = 24 
		, FOR_NOTATION = 25 
		, GREATER_THAN_OP = 26 
		, GREATER_THAN_OR_EQUALS_OP = 27 
		, HEX_STRING_LIT = 28 
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
		, PLSQL_RESERVED = 39 
		, PLUS_SIGN = 40 
		, PROMPT = 41 
		, QS_ANGLE = 42 
		, QS_BRACE = 43 
		, QS_BRACK = 44 
		, QS_OTHER = 45 
		, QS_OTHER_CH = 46 
		, QS_PAREN = 47 
		, QUESTION_MARK = 48 
		, QUOTE = 49 
		, REGULAR_ID = 50 
		, RIGHT_BRACKET = 51 
		, RIGHT_PAREN = 52 
		, SEMICOLON = 53 
		, SIMPLE_LETTER = 54 
		, SOLIDUS = 55 
		, SPACE = 56 
		, TILDE_OPERATOR_PART = 57 
		, UNDERSCORE = 58 
		, UNSIGNED_INTEGER = 59 
		, VERTICAL_BAR = 60 
		, WHITE = 61 
	};

};

/** Context tracking structure for PLSQLGuiLexer
 */
class PLSQLGuiLexer : public PLSQLGuiLexerImplTraits::BaseLexerType, public PLSQLGuiLexerTokens
{
public:
	typedef PLSQLGuiLexerImplTraits ImplTraits;
	typedef PLSQLGuiLexer ComponentType;
	typedef ComponentType::StreamType StreamType;
	typedef PLSQLGuiLexerImplTraits::BaseLexerType BaseType;
	typedef ImplTraits::RecognizerSharedStateType<StreamType> RecognizerSharedStateType;
	typedef StreamType InputType;
	static const bool IsFiltered = false;


private:	
public:
    PLSQLGuiLexer(InputType* instream);
    PLSQLGuiLexer(InputType* instream, RecognizerSharedStateType* state);

    void init(InputType* instream  );



  void advanceInput()
  {
    RecognizerSharedStateType *state = get_state();
    state->set_tokenStartCharIndex(getCharIndex());    
    state->set_tokenStartCharPositionInLine(getCharPositionInLine());
    state->set_tokenStartLine(getLine());
  }

    
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
    void  mSIMPLE_LETTER( );
    void  mUNSIGNED_INTEGER( );
    void  mCOMMENT( );
    void  mPROMPT( );
    void  mNEWLINE( );
    void  mSPACE( );
    void  mWHITE( );
    void  mMINUS_SIGN( );
    void  mPLSQL_RESERVED( );
    void  mBUILDIN_FUNCTIONS( );
    void  mREGULAR_ID( );
    void  mTokens( );
    const char *    getGrammarFileName();
    void            reset();
    ~PLSQLGuiLexer();

};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//

/* End of token definitions for PLSQLGuiLexer
 * =============================================================================
 */

}

#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */
