Aggregate Functions:AVG ( DISTINCT|ALL expr )=Calculates the avarage value of <I>expr</I>.<P>\nIf <CODE>DISTINCT</CODE> is specified duplicates of the same number is only counted as one\nnumber, if <CODE>ALL</CODE> or nothing is specified all numbers are counted equally.<P>\n<B>Example:</B><P>\nThis example calculates the average amount of all purchases.\n<PRE>\nSELECT AVG(amount) "Average" FROM purchase;\n\n   Average\n----------\n124144.619\n</PRE>
Aggregate Functions:CORR ( expr1 , expr2 )=Calculate the coefficient of correlation of a set of number pairs.<P>\n\nPairs where either <I>expr1</I> or <I>expr2</I> are eliminated first. Then the following\ncalculation is performed:\n\n<PRE>\nCOVAR_POP(expr1, expr2) / (STDDEV_POP(expr1) * STDDEV_POP(expr2))\n</PRE>\n\nThe function returns a number, if applied to an empty set NULL is returned.<P>\n<B>Example:</B>\n<PRE>\n\nSELECT itemid,CORR(amount,quantity) FROM saleitems\n GROUP BY itemid;\n\nitemid CORR(quantity,amount)\n------ ---------------------\n     1                     1\n     2                     1\n     3                     1\n     4                     1\n</PRE>
Aggregate Functions:COUNT ( *|DISTINCT|ALL expr )=Return the number of rows in the resultset. <CODE>DISTINCT</CODE> specifies that only\ndistinct values are to be counted. If <I>expr</I> is NULL it is not counted. <CODE>*</CODE>\n and <CODE>ALL</CODE> indicates that all rows should be counted.<P>\n<B>Example:</B>\n<PRE>\nSELECT COUNT(*),COUNT(deliverydate) FROM sales;\n\nCOUNT(*) COUNT(deliverydate)\n-------- -------------------\n       8                   7\n</PRE>
Aggregate Functions:COVAR_POP ( expr1 , expr2 )=Get the population covariance of a set of pairs.<P>\n\nPairs where either <I>expr1</I> or <I>expr2</I> are eliminated first. Then the following\ncalculation is performed:\n\n<PRE>\n(SUM(expr1 * expr2) - SUM(expr2) * SUM(expr1) / n) / n\n</PRE>\n\nThe function returns a number, if applied to an empty set NULL is returned.<P>\n<B>Example:</B>\n<PRE>\nSELECT itemid,COVAR_POP(amount,profit) FROM saleitems GROUP BY itemid;\n\n    ITEMID COVAR_POP(AMOUNT,PROFIT)\n---------- ------------------------\n         1                    84000\n         2                   5062.5\n         3                   325000\n         4               4111.11111\n</PRE>
Aggregate Functions:COVAR_SAMP ( expr1 , expr2 )=Get the sample covariance of a set of pairs.<P>\n\nPairs where either <I>expr1</I> or <I>expr2</I> are eliminated first. Then the following\ncalculation is performed:\n\n<PRE>\n(SUM(expr1 * expr2) - SUM(expr2) * SUM(expr1) / n) / (n - 1)\n</PRE>\n\nThe function returns a number, if applied to an empty set NULL is returned.<P>\n<B>Example:</B>\n<PRE>\nSELECT itemid,COVAR_SAMP(amount,profit) FROM saleitems GROUP BY itemid;\n\n    ITEMID COVAR_POP(AMOUNT,PROFIT)\n---------- ------------------------\n         1                    84000\n         2                   5062.5\n         3                   325000\n         4               4111.11111\n</PRE>
Aggregate Functions:GROUPING ( expr )=This function is only usefull in select statements with a group by extension such as <CODE>ROLLUP</CODE>\nor <CODE>CUBE</CODE>. These function generate extra rows with nulls which is the group by aggregate.\nThis function can be use to distinguish these rows from rows that are actually null.<P>\nThe <I>expr</I> must match a group by expression. If the <I>expr</I> is a null that represent an\naggregate row this function returns 1, otherwise it returns 0.<P>\n<B>Example:</B>\n<PRE>\nSELECT DECODE(GROUPING(itemid),1,'Total',itemid) item,\n       SUM(amount) amount,\n       SUM(profit) profit\n  FROM saleitems\n GROUP BY ROLLUP (itemid);\n\nITEM  AMOUNT PROFIT\n----- ------ ------\n1      12000   2200\n2       2760    195\n3      10000   2200\n4       1300    170\nTotal  26060   4765\n</PRE>
Aggregate Functions:MAX=
Aggregate Functions:MIN=
Aggregate Functions:REGR_AVGX=
Aggregate Functions:REGR_AVGY=
Aggregate Functions:REGR_COUNT=
Aggregate Functions:REGR_INTERCEPT=
Aggregate Functions:REGR_R2=
Aggregate Functions:REGR_SLOPE=
Aggregate Functions:REGR_SXX=
Aggregate Functions:REGR_SXY=
Aggregate Functions:REGR_SYY=
Aggregate Functions:STDDEV=
Aggregate Functions:STDDEV_POP=
Aggregate Functions:STDDEV_SAMP=
Aggregate Functions:SUM=
Aggregate Functions:VARIANCE=
Aggregate Functions:VAR_POP=
Aggregate Functions:VAR_SAMP=
Analytic Functions:AVG ( DISTINCT|ALL expr ) OVER ( analytic_clause )=
Analytic Functions:CORR ( expr1 , expr2 ) OVER ( analytic_clause )=
Analytic Functions:COUNT ( *|DISTINCT|ALL expr ) OVER ( analytic_clause )=
Analytic Functions:COVAR_POP=
Analytic Functions:COVAR_SAMP=
Analytic Functions:CUME_DIST=
Analytic Functions:DENSE_RANK=
Analytic Functions:FIRST_VALUE=
Analytic Functions:LAG=
Analytic Functions:LAST_VALUE=
Analytic Functions:LEAD=
Analytic Functions:MAX=
Analytic Functions:MIN=
Analytic Functions:NTILE=
Analytic Functions:PERCENT_RANK=
Analytic Functions:RANK=
Analytic Functions:RATIO_TO_REPORT=
Analytic Functions:REGR_AVGX=
Analytic Functions:REGR_AVGY=
Analytic Functions:REGR_COUNT=
Analytic Functions:REGR_INTERCEPT=
Analytic Functions:REGR_R2=
Analytic Functions:REGR_SLOPE=
Analytic Functions:REGR_SXX=
Analytic Functions:REGR_SXY=
Analytic Functions:REGR_SYY=
Analytic Functions:ROW_NUMBER=
Analytic Functions:STDDEV=
Analytic Functions:STDDEV_POP=
Analytic Functions:STDDEV_SAMP=
Analytic Functions:SUM=
Analytic Functions:VARIANCE=
Analytic Functions:VAR_POP=
Analytic Functions:VAR_SAMP=
Char to Char Functions:CHR ( n USING NCHAR_CS )=
Char to Char Functions:CONCAT ( char1 , char2 )=
Char to Char Functions:INITCAP=
Char to Char Functions:LOWER=
Char to Char Functions:LPAD=
Char to Char Functions:LTRIM=
Char to Char Functions:NLSSORT=
Char to Char Functions:NLS_INITCAP=
Char to Char Functions:NLS_LOWER=
Char to Char Functions:NLS_UPPER=
Char to Char Functions:REPLACE=
Char to Char Functions:RPAD=
Char to Char Functions:RTRIM=
Char to Char Functions:SOUNDEX=
Char to Char Functions:SUBSTR=
Char to Char Functions:SUBSTRB=
Char to Char Functions:TRANSLATE=
Char to Char Functions:TRIM=
Char to Char Functions:UPPER=
Char to Number Functions:ASCII ( char )=
Char to Number Functions:INSTR=
Char to Number Functions:INSTRB=
Char to Number Functions:LENGTH=
Char to Number Functions:LENGTHB=
Conversion Functions:CHARTOROWID ( char )=
Conversion Functions:CONVERT ( char , dest_char_set , source_char_set )=
Conversion Functions:HEXTORAW=
Conversion Functions:NUMTODSINTERVAL=
Conversion Functions:NUMTOYMINTERVAL=
Conversion Functions:RAWTOHEX=
Conversion Functions:ROWIDTOCHAR=
Conversion Functions:TO_CHAR(dateconversion)=
Conversion Functions:TO_CHAR(numberconversion)=
Conversion Functions:TO_DATE=
Conversion Functions:TO_LOB=
Conversion Functions:TO_MULTI_BYTE=
Conversion Functions:TO_NUMBER=
Conversion Functions:TO_SINGLE_BYTE=
Conversion Functions:TRANSLATE...USING=
Conversion Functions:Test=
Date Functions:ADD_MONTHS=
Date Functions:LAST_DAY=
Date Functions:MONTHS_BETWEEN=
Date Functions:NEW_TIME=
Date Functions:NEXT_DAY=
Date Functions:ROUND=
Date Functions:SYSDATE=
Date Functions:TRUNC=
Misc Single-Row Functions:BFILENAME ( ' directory ' , ' filename ' )=
Misc Single-Row Functions:DUMP=
Misc Single-Row Functions:EMPTY_BLOB=
Misc Single-Row Functions:EMPTY_CLOB=
Misc Single-Row Functions:GREATEST=
Misc Single-Row Functions:LEAST=
Misc Single-Row Functions:NLS_CHARSET_DECL_LEN=
Misc Single-Row Functions:NLS_CHARSET_ID=
Misc Single-Row Functions:NLS_CHARSET_NAME=
Misc Single-Row Functions:NVL=
Misc Single-Row Functions:NVL2=
Misc Single-Row Functions:SYS_CONTEXT=
Misc Single-Row Functions:SYS_GUID=
Misc Single-Row Functions:UID=
Misc Single-Row Functions:USER=
Misc Single-Row Functions:USERENV=
Misc Single-Row Functions:VSIZE=
Number functions:ABS ( n )=ABS returns the absolute value of <I>n</I>.<P>\n<B>Example:</B><P>\n<PRE>\nSELECT ABS(-20) "Absolute" FROM DUAL;\n\n  Absolute\n----------\n        20\n</PRE>
Number functions:ACOS ( n )=
Number functions:ADD_MONTHS ( d , n )=
Number functions:ASIN ( n )=
Number functions:ATAN ( n )=
Number functions:ATAN2 ( n , m )=
Number functions:BITAND ( argument1 , argument2 )=
Number functions:CEIL ( n )=
Number functions:COS ( n )=
Number functions:COSH ( n )=
Number functions:EXP=
Number functions:FLOOR=
Number functions:LN=
Number functions:LOG=
Number functions:MOD=
Number functions:POWER=
Number functions:ROUND=
Number functions:SIGN=
Number functions:SIN=
Number functions:SINH=
Number functions:SQRT=
Number functions:TAN=
Number functions:TANH=
Number functions:TRUNC=
Object Functions:DEREF=
Object Functions:MAKE_REF=
Object Functions:REF=
Object Functions:REFTOHEX=
Object Functions:VALUE=
