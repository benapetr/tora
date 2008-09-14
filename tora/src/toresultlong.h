/* BEGIN_COMMON_COPYRIGHT_HEADER 
* END_COMMON_COPYRIGHT_HEADER */

#ifndef TORESULTLONG_H
#define TORESULTLONG_H

#include "config.h"
#include "tobackground.h"
#include "toconnection.h"
#include "toresultview.h"

class toTreeWidgetItem;
class toNoBlockQuery;
class toResultStats;
class toResultTip;
class toresultlong;

/** A widget which contains the result of a query run in the background.
 */
class toResultLong : public toResultView
{
    Q_OBJECT

    /** No blocking query object.
     */
    toNoBlockQuery *Query;
    /** Timer to poll for new data.
     */
    toBackground Timer;
    /** Indicator if headers have been parsed yet.
     */
    bool HasHeaders;
    /** Indicator if first row has been read yet.
     */
    bool First;
    /** Widget to store query statistics to.
     */
    toResultStats *Statistics;
    /** Description of query
     */
    toQDescList Description;
    /** Number of rows to fetch when starting. (-1 for all)
     */
    int MaxNumber;

    /** The query mode to use for the executed query.
     */
    toQuery::queryMode Mode;

    /** Check if at eof.
     */
    virtual bool eof(void);
    /** Clean up when query is done
     */
    void cleanup(void);
public:
    /** Create widget.
     * @param readable Indicate if columns are to be made more readable. This means that the
     * descriptions are capitalised and '_' are converted to ' '.
     * @param numCol If number column is to be displayed.
     * @param mode Query mode to use when executing query
     * @param parent Parent of list.
     * @param name Name of widget.
     * @param f Widget flags.
     */
    toResultLong(bool readable, bool numCol, toQuery::queryMode mode, QWidget *parent, const char *name = NULL, Qt::WFlags f = 0);
    /** Create widget. The columns are not readable and the number column is displayed.
     * @param parent Parent of list.
     * @param name Name of widget.
     * @param f Widget flags.
     */
    toResultLong(QWidget *parent, const char *name = NULL, Qt::WFlags f = 0);
    ~toResultLong();

    /** Set statistics widget.
     * @param stats Statistics widget.
     */
    void setStatistics(toResultStats *stats)
    {
        Statistics = stats;
    }

    /** Check if query is still running.
     * @return True if query is still running.
     */
    bool running(void)
    {
        return Query;
    }

    /** Reimplemented for internal reasons.
     */
    virtual void query(const QString &sql, const toQList &param);
    /** Reimplemented for internal reasons.
     */
    void query(const QString &sql)
    {
        toQList p;
        query(sql, p);
    }
    /** Reimplemented for internal reasons.
     */
    virtual void editReadAll(void);
signals:
    /** Emitted when query is finnished.
     */
    void done(void);
    /** Emitted when the first result is available.
     * @param sql SQL that was run.
     * @param res String describing result.
     * @param error Error has occurred.
     */
    void firstResult(const QString &sql, const toConnection::exception &res, bool error);

public slots:
    /** Stop running query.
     */
    void stop(void);
    /** Reimplemented for internal reasons.
     */
    virtual void addItem(void);
};

#endif
