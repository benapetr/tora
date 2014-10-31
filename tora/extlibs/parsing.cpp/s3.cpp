// Example of a grammar for parsing Java sources,
// Adapted from Java equivalent example, by Terence Parr
// Author: Jim Idle - April 2007
// Permission is granted to use this example code in any way you want, so long as
// all the original authors are cited.
//

// set ts=4,sw=4
// Tab size is 4 chars, indent is 4 chars

// Notes: Although all the examples provided are configured to be built
//        by Visual Studio 2005, based on the custom build rules
//        provided in $(ANTLRSRC)/code/antlr/main/runtime/C/vs2005/rulefiles/antlr3.rules
//        there is no reason that this MUST be the case. Provided that you know how
//        to run the antlr tool, then just compile the resulting .c files and this
//    file together, using say gcc or whatever: gcc *.c -I. -o XXX
//    The C code is generic and will compile and run on all platforms (please
//        report any warnings or errors to the antlr-interest newsgroup (see www.antlr.org)
//        so that they may be corrected for any platofrm that I have not specifically tested.
//
//    The project settings such as addinotal library paths and include paths have been set
//        relative to the place where this source code sits on the ANTLR perforce system. You
//        may well need to change the settings to locate the includes and the lib files. UNIX
//        people need -L path/to/antlr/libs -lantlr3c (release mode) or -lantlr3cd (debug)
//
//        Jim Idle (jimi cut-this at idle ws)
//

// You may adopt your own practices by all means, but in general it is best
// to create a single include for your project, that will include the ANTLR3 C
// runtime header files, the generated header files (all of which are safe to include
// multiple times) and your own project related header files. Use <> to include and
// -I on the compile line (which vs2005 now handles, where vs2003 did not).
//

#include "utils.hpp"
#include "UserMiniTraits.hpp"
#include "S3Lexer.hpp"
#include "S3Parser.hpp"

#include <sys/types.h>

#include <iostream>
#include <sstream>
#include <fstream>

using namespace Antlr3Mini;
using namespace std;

// The lexer is of course generated by ANTLR, and so the lexer type is not upper case.
// The lexer is supplied with a pANTLR3_INPUT_STREAM from whence it consumes its
// input and generates a token stream as output.
//
static    S3Lexer*          lxr;

// Main entry point for this example
//
int
main    (int argc, char *argv[])
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

    printf("finished parsing OK\n");    // Finnish parking is pretty good - I think it is all the snow

    return 0;
}

void parseFile(const char* fName, int fd)
{
    // Now we declare the ANTLR related local variables we need.
    // Note that unless you are convinced you will never need thread safe
    // versions for your project, then you should always create such things
    // as instance variables for each invocation.
    // -------------------

    // The ANTLR3 character input stream, which abstracts the input source such that
    // it is easy to privide inpput from different sources such as files, or
    // memory strings.
    //
    // For an ASCII/latin-1 memory string use:
    //      input = antlr3NewAsciiStringInPlaceStream (stringtouse, (ANTLR3_UINT64) length, NULL);
    //
    // For a UCS2 (16 bit) memory string use:
    //      input = antlr3NewUCS2StringInPlaceStream (stringtouse, (ANTLR3_UINT64) length, NULL);
    //
    // For input from a file, see code below
    //
    // Note that this is essentially a pointer to a structure containing pointers to functions.
    // You can create your own input stream type (copy one of the existing ones) and override any
    // individual function by installing your own pointer after you have created the standard
    // version.
    //
    S3LexerTraits::InputStreamType*    input;


    // The token stream is produced by the ANTLR3 generated lexer. Again it is a structure based
    // API/Object, which you can customise and override methods of as you wish. a Token stream is
    // supplied to the generated parser, and you can write your own token stream and pass this in
    // if you wish.
    //
    S3LexerTraits::TokenStreamType* tstream;

    // The C parser is also generated by ANTLR and accepts a token stream as explained
    // above. The token stream can be any source in fact, so long as it implements the
    // ANTLR3_TOKEN_SOURCE interface. In this case the parser does not return anything
    // but it can of course specify any kind of return type from the rule you invoke
    // when calling it.
    //
    S3Parser*           psr;

    // Create the input stream using the supplied file name
    // (Use antlr3AsciiFileStreamNew for UCS2/16bit input).
    //
    ///byIvan input = new PLSQLTraits::InputStreamType(fName, ANTLR_ENC_8BIT);
#if defined __linux
    string data = Utils::slurp(fd);
#else
    string data = Utils::slurp(fName);
#endif
    input   = new S3LexerTraits::InputStreamType((const ANTLR_UINT8 *)data.c_str(),
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
        lxr     = new S3Lexer(input);       // javaLexerNew is generated by ANTLR
    }
    else
    {
        lxr->setCharStream(input);
    }

    // Our lexer is in place, so we can create the token stream from it
    // NB: Nothing happens yet other than the file has been read. We are just
    // connecting all these things together and they will be invoked when we
    // call the parser rule. ANTLR3_SIZE_HINT can be left at the default usually
    // unless you have a very large token stream/input. Each generated lexer
    // provides a token source interface, which is the second argument to the
    // token stream creator.
    // Note tha even if you implement your own token structure, it will always
    // contain a standard common token within it and this is the pointer that
    // you pass around to everything else. A common token as a pointer within
    // it that should point to your own outer token structure.
    //
    tstream = new S3LexerTraits::TokenStreamType(ANTLR_SIZE_HINT, lxr->get_tokSource());

    // Finally, now that we have our lexer constructed, we can create the parser
    //
    psr = new S3Parser(tstream);  // javaParserNew is generated by ANTLR3
    S3Parser::start_rule_return r = psr->start_rule();
    std::cout << r.getTree()->toStringTree() << std::endl;
    std::cout << "Message: " << r.message << std::endl;
    std::cout << "Evaluator.Message: " << (r.evaluator ? r.evaluator->toString() : "NULL") << std::endl;

    // Note that this means only that the methods are always called via the object
    // pointer and the first argument to any method, is a pointer to the structure itself.
    // It also has the side advantage, if you are using an IDE such as VS2005 that can do it
    // that when you type ->, you will see a list of tall the methods the object supports.
    //
    putc('L', stdout);
    fflush(stdout);
    {
        ANTLR_INT32 T = 0;
        while   (T != S3Lexer::EOF_TOKEN)
        {
            T = tstream->_LA(1);
            S3LexerTraits::CommonTokenType const* token = tstream->_LT(1);
            ANTLR_UINT8 const *name = lxr->getTokenName(T);

            printf("%d %s\t\"%s\"\n",
                   T,
                   name,
                   tstream->_LT(1)->getText().c_str()
                  );
            tstream->consume();
        }
    }

    tstream->_LT(1);    // Don't do this mormally, just causes lexer to run for timings here

    delete tstream;
    delete lxr;
    lxr = NULL;
    delete input;
}
