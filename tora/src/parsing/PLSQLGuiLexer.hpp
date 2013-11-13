/** \file
 *  This C++ header file was generated by $ANTLR version 3.5.2-SNAPSHOT
 *
 *     -  From the grammar source file : PLSQLGuiLexer.g
 *     -                            On : 2013-11-13 14:50:25
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

/* ========================
 * BACKTRACKING IS ENABLED
 * ========================
 */

namespace  Antlr3GuiImpl  {

typedef PLSQLGuiLexerTraits PLSQLGuiLexerImplTraits;

 struct synpred1_PLSQLGuiLexer {}; 
 struct synpred2_PLSQLGuiLexer {}; 
 struct synpred3_PLSQLGuiLexer {}; 
 struct synpred4_PLSQLGuiLexer {}; 
 struct synpred5_PLSQLGuiLexer {}; 
 struct synpred6_PLSQLGuiLexer {}; 
 struct synpred7_PLSQLGuiLexer {}; 

class PLSQLGuiLexerTokens
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
		EOF_TOKEN = PLSQLGuiLexerImplTraits::CommonTokenType::TOKEN_EOF
		, ARROW = 4 
		, ASSIGN_OP = 5 
		, ASTERISK = 6 
		, AT_SIGN = 7 
		, BINDVAR = 8 
		, BUILDIN_FUNCTIONS = 9 
		, CARRET_OPERATOR_PART = 10 
		, CHAR_STRING = 11 
		, CHAR_STRING_PERL = 12 
		, COLON = 13 
		, COMMA = 14 
		, COMMENT_ML = 15 
		, COMMENT_SL = 16 
		, CONCATENATION_OP = 17 
		, CONNECT = 18 
		, CONNECT_BY = 19 
		, DDL_COMMAND_INTRODUCER = 20 
		, DELIMITED_ID = 21 
		, DML_COMMAND_INTRODUCER = 22 
		, DOUBLE_PERIOD = 23 
		, EQUALS_OP = 24 
		, EXCLAMATION_OPERATOR_PART = 25 
		, EXECUTE = 26 
		, EXIT = 27 
		, FOR_NOTATION = 28 
		, GREATER_THAN_OP = 29 
		, GREATER_THAN_OR_EQUALS_OP = 30 
		, LEFT_BRACKET = 31 
		, LEFT_PAREN = 32 
		, LESS_THAN_OP = 33 
		, LESS_THAN_OR_EQUALS_OP = 34 
		, LINEEND = 35 
		, LLABEL = 36 
		, MINUS_SIGN = 37 
		, NATIONAL_CHAR_STRING_LIT = 38 
		, NEWLINE = 39 
		, NOT_EQUAL_OP = 40 
		, NUMBER_LIT = 41 
		, OTHER_COMMAND_INTRODUCER = 42 
		, PERCENT = 43 
		, PERIOD = 44 
		, PLSQL_COMMAND_INTRODUCER = 45 
		, PLSQL_RESERVED = 46 
		, PLUS_SIGN = 47 
		, QS_ANGLE = 48 
		, QS_BRACE = 49 
		, QS_BRACK = 50 
		, QS_OTHER = 51 
		, QS_OTHER_CH = 52 
		, QS_PAREN = 53 
		, QUESTION_MARK = 54 
		, QUOTE = 55 
		, REGULAR_ID = 56 
		, RIGHT_BRACKET = 57 
		, RIGHT_PAREN = 58 
		, RLABEL = 59 
		, R_AS = 60 
		, R_CASE = 61 
		, R_END = 62 
		, R_IF = 63 
		, R_IS = 64 
		, R_LOOP = 65 
		, R_THEN = 66 
		, SELECT_COMMAND_INTRODUCER = 67 
		, SEMICOLON = 68 
		, SET = 69 
		, SIMPLE_LETTER = 70 
		, SOLIDUS = 71 
		, SPACE = 72 
		, SPACE_FRAGMENT = 73 
		, SQLPLUS_COMMAND = 74 
		, SQLPLUS_COMMAND_INTRODUCER = 75 
		, SQLPLUS_SOLIDUS = 76 
		, SWALLOW_TO_NEWLINE_FRAGMENT = 77 
		, TILDE_OPERATOR_PART = 78 
		, TOKEN_FAILURE = 79 
		, UNSIGNED_INTEGER = 80 
		, VERTICAL_BAR = 81 
	};
       const ANTLR_UINT8* getTokenName(ANTLR_INT32 index) const;

protected:
       static ANTLR_UINT8* TokenNames[];
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


      void advanceInput();


    
    void  mSELECT_COMMAND_INTRODUCER( );
    void  mDML_COMMAND_INTRODUCER( );
    void  mDDL_COMMAND_INTRODUCER( );
    void  mPLSQL_COMMAND_INTRODUCER( );
    void  mOTHER_COMMAND_INTRODUCER( );
    void  mCONNECT_BY( );
    void  mSET( );
    void  mEXIT( );
    void  mEXECUTE( );
    void  mCONNECT( );
    void  mSQLPLUS_COMMAND_INTRODUCER( );
    void  mR_IF( );
    void  mR_THEN( );
    void  mR_LOOP( );
    void  mR_CASE( );
    void  mR_END( );
    void  mR_AS( );
    void  mR_IS( );
    void  mPLSQL_RESERVED( );
    void  mBUILDIN_FUNCTIONS( );
    void  mFOR_NOTATION( );
    void  mPERIOD( );
    void  mNUMBER_LIT( );
    void  mNATIONAL_CHAR_STRING_LIT( );
    void  mCHAR_STRING( );
    void  mCHAR_STRING_PERL( );
    void  mQUOTE( );
    void  mQS_ANGLE( );
    void  mQS_BRACE( );
    void  mQS_BRACK( );
    void  mQS_PAREN( );
    void  mQS_OTHER_CH( );
    void  mQS_OTHER( );
    void  mLLABEL( );
    void  mRLABEL( );
    void  mPERCENT( );
    void  mLEFT_PAREN( );
    void  mRIGHT_PAREN( );
    void  mASTERISK( );
    void  mPLUS_SIGN( );
    void  mMINUS_SIGN( );
    void  mCOMMA( );
    void  mSOLIDUS( );
    void  mAT_SIGN( );
    void  mASSIGN_OP( );
    void  mARROW( );
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
    void  mCONCATENATION_OP( );
    void  mVERTICAL_BAR( );
    void  mEQUALS_OP( );
    void  mLEFT_BRACKET( );
    void  mRIGHT_BRACKET( );
    void  mSIMPLE_LETTER( );
    void  mUNSIGNED_INTEGER( );
    void  mCOMMENT_SL( );
    void  mCOMMENT_ML( );
    void  mNEWLINE( );
    void  mSWALLOW_TO_NEWLINE_FRAGMENT( );
    void  mLINEEND( );
    void  mSPACE( );
    void  mSPACE_FRAGMENT( );
    void  mQUESTION_MARK( );
    void  mBINDVAR( );
    void  mREGULAR_ID( );
    void  mDELIMITED_ID( );
    void  mTOKEN_FAILURE( );
    void  mTokens( );







      bool msynpred( antlr3::ClassForwarder< synpred1_PLSQLGuiLexer > );
        void msynpred1_PLSQLGuiLexer_fragment ();
      bool msynpred( antlr3::ClassForwarder< synpred2_PLSQLGuiLexer > );
        void msynpred2_PLSQLGuiLexer_fragment ();
      bool msynpred( antlr3::ClassForwarder< synpred3_PLSQLGuiLexer > );
        void msynpred3_PLSQLGuiLexer_fragment ();
      bool msynpred( antlr3::ClassForwarder< synpred4_PLSQLGuiLexer > );
        void msynpred4_PLSQLGuiLexer_fragment ();
      bool msynpred( antlr3::ClassForwarder< synpred5_PLSQLGuiLexer > );
        void msynpred5_PLSQLGuiLexer_fragment ();
      bool msynpred( antlr3::ClassForwarder< synpred6_PLSQLGuiLexer > );
        void msynpred6_PLSQLGuiLexer_fragment ();
      bool msynpred( antlr3::ClassForwarder< synpred7_PLSQLGuiLexer > );
        void msynpred7_PLSQLGuiLexer_fragment ();
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
