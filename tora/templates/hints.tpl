Access Methods:=
Access Methods:/*+ AND_EQUAL(table index index[ index]) */=Specify an explicit access path for joining one or more tables where the source table is specified by <I>table</I> and the other indexes specifies an index to use for joining adjacent tables. Any number of indexes can be specified.
Access Methods:/*+ CLUSTER(table) */=Instruct Oracle to use a cluster scan to access the table named <I>table</I>. Only applies to clustered tables. The tablename can also indicate an alias in the query.
Access Methods:/*+ FULL(table) */=Instruct Oracle to use a full tablescan when accessing the table named <I>table</I>. The tablename can also indicate an alias in the query.
Access Methods:/*+ HASH(table) */=Instruct Oracle to use a hash scan to access the table named <I>table</I>. Only applies to clustered tables. The tablename can also indicate an alias in the query.
Access Methods:/*+ INDEX(table[ index]) */=Instruct Oracle to use an index optionally specified by <I>index</I> when accessing the table named <I>table</I>. The tablename can also indicate an alias in the query. You can also specify several indexes in which case the optimizer tries to find the best on to use.
Access Methods:/*+ INDEX_ASC(table[ index]) */=Instruct Oracle to use an index optionally specified by <I>index</I> when accessing the table named <I>table</I>. The items found by the index will then be accessed in ascending order in a range scan. Since this is the default behaviour of Oracle this hint normally doesn't specify anything more than the <B>INDEX</B> hint. The tablename can also indicate an alias in the query. You can also specify several indexes in which case the optimizer tries to find the best on to use.
Access Methods:/*+ INDEX_COMBINE(table[ index]) */=Instruct Oracle to use a bitmap index optionally specified by <I>index</I> when accessing the table named <I>table</I>. The tablename can also indicate an alias in the query. You can also specify several indexes in which case the optimizer tries to find the best on to use.
Access Methods:/*+ INDEX_DESC(table[ index]) */=Instruct Oracle to use an index optionally specified by <I>index</I> when accessing the table named <I>table</I>. The items found by the index will then be accessed in descending order in a range scan. The tablename can also indicate an alias in the query. You can also specify several indexes in which case the optimizer tries to find the best on to use.
Access Methods:/*+ INDEX_FFS(table[ index]) */=Instruct Oracle to use a fast full indexscan when accessing the table named <I>table</I> insteaf of doing a full tablespace. The tablename can also indicate an alias in the query.
Access Methods:/*+ INDEX_JOIN(table[ index]) */=Instruct Oracle to use an index join as an access path in a join. Indexes used are optionally specified by <I>index</I> when accessing the table named <I>table</I>. The tablename can also indicate an alias in the query. You can also specify several indexes in which case the optimizer tries to find the best on to use.
Access Methods:/*+ NOREWRITE */=Specify the optimizer to never use a materialized.
Access Methods:/*+ NO_EXPAND */=Never transform queries using the <B>OR</B> operator to a <B>UNION ALL</B> statement.
Access Methods:/*+ NO_INDEX(table[ index]) */=Instruct Oracle not to use an index optionally specified by <I>index</I> when accessing the table named <I>table</I>. The tablename can also indicate an alias in the query. You can also specify several indexes in which case the optimizer tries to find the best on to use. Any index specified tells the optimizer not to use that index, if no index is specified no indexes at all are considered for access to the table.
Access Methods:/*+ REWRITE(view) */=Specify the optimizer to always use a materialized view speicified by <I>view</I>. Several views can be specified.
Access Methods:/*+ ROWID(table) */=Instruct Oracle to use a rowid when accessing the table named <I>table</I>. The tablename can also indicate an alias in the query.
Access Methods:/*+ USE_CONCAT */=Instruct the optimizer to always transform queries using the <B>OR</B> operator to a <B>UNION ALL</B> statement.
Join Operators:/*+ DRIVING_SITE(table) */=This hint can be used to force the optimizer to choose a specific driving site to perform the query in a distributed server specified by <I>table</I>.
Join Operators:/*+ HASH_AJ */=Use a anti hash join to perform an anti join. Should be specified in the subquery getting the resultset that should not be matched by the <B>IN</B> operator.<P>\n<PRE>\nSELECT COUNT(1) FROM user_tables a\n WHERE table_name NOT IN \n       (SELECT /*+ HASH_AJ */ table_name\n          FROM user_indexes b\n         WHERE a.table_name = b.table_name);\n</PRE>
Join Operators:/*+ HASH_SJ */=Use a hash semi join. Should be specified in the subquery getting the resultset for the semi join.<P>\n<PRE>\nSELECT COUNT(1) FROM user_tables a\n WHERE EXISTS \n       (SELECT /*+ HASH_SJ */ 'X'\n          FROM user_indexes b\n         WHERE a.table_name = b.table_name);\n</PRE>
Join Operators:/*+ LEADING(table) */=This hint can be used to force the optimizer to choose a specific leading table in a joined statement specified by <I>table</I> as a basis to perform the rest of the optimizations.
Join Operators:/*+ MERGE_AJ */=Use a anti merge join to perform an anti join. Should be specified in the subquery getting the resultset that should not be matched by the <B>IN</B> operator.<P>\n<PRE>\nSELECT COUNT(1) FROM user_tables a\n WHERE table_name NOT IN \n       (SELECT /*+ MERGE_AJ */ table_name\n          FROM user_indexes b\n         WHERE a.table_name = b.table_name);\n</PRE>
Join Operators:/*+ MERGE_SJ */=Use a merge semi join. Should be specified in the subquery getting the resultset for the semi join.<P>\n<PRE>\nSELECT COUNT(1) FROM user_tables a\n WHERE EXISTS \n       (SELECT /*+ MERGE_SJ */ 'X'\n          FROM user_indexes b\n         WHERE a.table_name = b.table_name);\n</PRE>
Join Operators:/*+ USE_HASH(table[ table]) */=Specify to the optimizer to join the tables using a row source with a hash join from the first <I>table</I> to the next <I>table</I>. Any number of tables can be specified.
Join Operators:/*+ USE_MERGE(table[ table]) */=Specify to the optimizer to join the tables using a row source with a sort-merge join from the first <I>table</I> to the next <I>table</I>. Any number of tables can be specified.
Join Operators:/*+ USE_NL(table[ table]) */=Specify to the optimizer to join the tables using a nested loop join from the first <I>table</I> to the next <I>table</I>. Any number of tables can be specified.
Join Order:/*+ ORDERED */=Indicate to the optimizer that the join order should be the same as the tables are specified in the <B>FROM</B> clause of the select.
Join Order:/*+ STAR */=Indicate to the optimizer to use a star join to perform the query. This means that the query should start with the largest table, then move to the second largest etc...
Optimizer Goals:/*+ ALL_ROWS */=Instruct optimizer to optimize for resource consumption. This means that it should try to finish the entire query as soon as posible instead of trying to get a part of the result as soon as posible.
Optimizer Goals:/*+ CHOOSE */=Instruct the optimizer to use a <B>CHOOSE</B> approach which means to use <B>COST</B> if analyzed statistics is available and otherwise use <B>RULE</B>.
Optimizer Goals:/*+ FIRST_ROWS */=Instruct the optimizer to optimize for response time of first row in result.
Optimizer Goals:/*+ RULE */=Use a rulebased approach to optimization.
Parallel Execution:/*+ APPEND */=When inserting data into a table always add data to the end of the table instead of checking for free space in the table. This is default when using parallel queries but can be overridden using the <B>NOAPPEND</B> hint.
Parallel Execution:/*+ NOAPPEND */=Override append mode and start looking for free allocated space already in the table.
Parallel Execution:/*+ NOPARALLEL(table) */=Don't use parallel queries to read the table specified by <I>table</I>.
Parallel Execution:/*+ NOPARALLEL_INDEX(table index) */=Don't use parallel execution when accessing the index specified by <I>table</I> and <I>index</I>. You can specify several indexes by separating them with <B>,</B>.
Parallel Execution:/*+ PARALLEL(table[,degree[,split]]) */=Specify how to split a query over parallel queries for a table specified by <I>table</I>. The <I>degree</I> specifies the degree of parallelism on the table and the <I>split</I> specifies how the query should be split among available instances.
Parallel Execution:/*+ PARALLEL_INDEX(table index degree split) */=Specify how to split an access to an index over parallel queries for a table specified by <I>table</I> and <I>index</I>. The <I>degree</I> specifies the degree of parallelism on the table and the <I>split</I> specifies how the query should be split among available instances. Several indexes, degrees and split can specified by separating them by <B>,</B>.
Parallel Execution:/*+ PQ_DISTRIBUTE(table,outer,inner) */=See <B>using optimizer hints</B> in <B>Designing and Tuning for Performance</B> available from Oracle for more information about this hint.
