//***************************************************************************
/* $Id$
**
** Copyright (C) 2000-2001 GlobeCom AB.  All rights reserved.
**
** This file is part of the Toolkit for Oracle.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.globecom.net/tora/ for more information.
**
** Contact tora@globecom.se if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef __TOHTML_H
#define __TOHTML_H

#include <qstring.h>
#include <qcstring.h>
#include <map>

#define TO_HTML_MAX_QUAL 100

/** A small and hopefully pretty efficient parser of HTML tags. No attempt what
 * so ever is made trying to check the validity of the text so if you forget
 * a closing > you will probably end up with an exception.
 *
 * A simple traverser counting the number of paragraphs might look like this.
 *
<pre>
int paragraphs++;
toHtml count("Paragraphs<P>And some</P><P>Dobedoo");
while(!count.eof()) {
  count.nextToken();
  if (count.isTag()&&count.open()&&QString(count.tag())=="p")
    paragraphs++;
}
</pre>
 */

class toHtml {
  /** Actual data of file.
   */
  char *Data;
  /** Length of the data.
   */
  size_t Length;
  /** Current position in data.
   */
  size_t Position;

  /** Last character that was at the end of text written over by call to @ref mid.
   * This is a real cludge, but what don't you do for performance.
   */
  char LastChar;

  /** List of qualifiers for tab. This is not a list because of performance as well.
   * Not really a lot of memory wasted this way, in fact it is probably more efficient
   * than a dynamic list.
   */
  struct {
    /** Name of qualifier. Always lowercase, points into @ref Data.
     */
    const char *Name;
    /** Value of qualifier. Points into @ref Data.
     */
    const char *Value;
  } Qualifiers[TO_HTML_MAX_QUAL];

  /** Number of qualifiers used.
   */
  int QualifierNum;
  /** If last token read was a tag or plain text.
   */
  bool IsTag;
  /** Last tag read was an open tag. Only valid if @ref isTag is true.
   */
  bool Open;
  /** Name of tag. NULL terminated, points into @ref Data.
   */
  const char *Tag;
  /** Text if not tag. NULL terminated, points into @ref Data.
   */
  const char *Text;

  /** Walk forward in @ref Data until not whitespace.
   */
  void skipSpace(void);
  /** Extract a string from @ref Data. Insert 0 at end of string, store overwritten char
   * in LastChar.
   * @param start Start position of string to extract.
   * @param size Size of string to extract.
   * @return Pointer to string (Points into @ref Data)
   */
  const char *mid(size_t start,size_t size);
public:
  /** Create a HTML parser.
   * @param data Data to parse.
   */
  toHtml(const QCString &data);
  ~toHtml();

  /** Read the next token which can be either a open tag, close tag or text.
   *
   * Observe that nextToken destroys the data it has parsed, so you
   * can't search it later. Searhing must be done on an new toHtml,
   * several serches can be made though on the same toHtml.
   */

  void nextToken(void);

  /** Get value of a qualifier.
   * @param name Name of qualifier in lowercase.
   * @return Value of qualifier or NULL.
   */
  const char *value(const QCString &name);
  /** Return true if current token is a tag.
   * @return True if tag.
   */
  bool isTag(void)
  { return IsTag; }
  /** Return true if open tag. Only valid if @ref isTag is true.
   * @return True if open tag.
   */
  bool open(void)
  { return Open; }

  /** Get bread text if available. Expands &# characters are expanded before returned.
   * Only valid if @ref isTag returns false.
   * @return The parsed text.
   */

  QCString text(void);

  /** Get name of tag in lowercase. Only valid if @ref isTag returns true.
   * @return Name of tag (Excluding / if close tag).
   */
  const char *tag(void)
  { return Tag; }

  /** Check if at end of data
   * @return True if at end of data.
   */
  bool eof(void);

  /** Search data for a simple text.
   * @param data Data to search.
   * @param str Substring to search for in data.
   */
  static bool search(const QCString &data,const QString &str);
};

#endif
