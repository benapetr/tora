#include "utils.hpp"
#include "UserGuiTraits.hpp"
#include "PLSQLGuiLexer.hpp"

#include <sys/types.h>

#include <iostream>
#include <sstream>
#include <fstream>

using namespace Antlr3Impl;
using namespace std;

static    PLSQLGuiLexer*		    lxr;

int 
main	(int argc, char *argv[])
{
	// Create the input stream based upon the argument supplied to us on the command line
	// for this example, the input will always default to ./input if there is no explicit
	// argument, otherwise we are expecting potentially a whole list of 'em.
	//
	if (argc < 2 || argv[1] == NULL)
	{
		Utils::processDir("./input"); // Note in VS2005 debug, working directory must be configured
	}
	else
	{
		int i;

		for (i = 1; i < argc; i++)
		{
			Utils::processDir(argv[i]);
		}
	}

	printf("finished parsing OK\n");	// Finnish parking is pretty good - I think it is all the snow

	return 0;
}

void parseFile(const char* fName, int fd)
{
	PLSQLGuiLexerTraits::InputStreamType*    input;
	PLSQLGuiLexerTraits::TokenStreamType*	tstream;

	///byIvan input	= new PLSQLTraits::InputStreamType(fName, ANTLR_ENC_8BIT);
#if defined __linux
	string data = Utils::slurp(fd);
#else
	string data = Utils::slurp(fName);
#endif
	input	= new PLSQLGuiLexerTraits::InputStreamType((const ANTLR_UINT8 *)data.c_str(),
						   ANTLR_ENC_8BIT,
						   data.length(), //strlen(data.c_str()),
						   (ANTLR_UINT8*)fName);

	input->setUcaseLA(true);
    
	// Our input stream is now open and all set to go, so we can create a new instance of our
	// lexer and set the lexer input to our input stream:
	//  (file | memory | ?) --> inputstream -> lexer --> tokenstream --> parser ( --> treeparser )?
	//
	if (lxr == NULL)
	{
		lxr	    = new PLSQLGuiLexer(input);	    // javaLexerNew is generated by ANTLR
	}
	else
	{
		lxr->setCharStream(input);
	}

	tstream = new PLSQLGuiLexerTraits::TokenStreamType(ANTLR_SIZE_HINT, lxr->get_tokSource());

	// //putc('L', stdout); fflush(stdout);
	// {
	// 	ANTLR_INT32 T = 0;
	// 	while	(T != PLSQLGuiLexer::EOF_TOKEN)
	// 	{
	// 		T = tstream->_LA(1);
	// 		PLSQLGuiLexerTraits::CommonTokenType const* token = tstream->_LT(1);
	// 		ANTLR_UINT8 const *name = lxr->getTokenName(T);
			  
	// 		printf("%d %s\t\"%s\" %d %d\n",
	// 		       T,
	// 		       name,
	// 		       token->getText().c_str(),
	// 		       token->getBlockContext(),
	// 		       token->get_channel()
	// 		       );
	// 		//std::cerr << token->getText();
	// 		tstream->consume();
	// 	}
	// }
	
	int n = tstream->getTokens()->size();
	{
		for (int i=0; i<n; i++)	       
		{
			PLSQLGuiLexerTraits::CommonTokenType const* token = tstream->get(i);
			ANTLR_UINT8 const *name = lxr->getTokenName(token->get_type());
			
			printf("%d %s\t\"%s\" %d %d %d %d\n",
			       token->get_type(),
			       name,
			       token->getText().c_str(),
			       token->getBlockContext(),
			       token->get_channel(),
			       token->get_line(),
			       token->get_charPositionInLine()
			       );
			//std::cerr << token->getText();
			tstream->consume();
		}
	}

	delete tstream; 
	delete lxr; lxr = NULL;
	delete input; 
}
