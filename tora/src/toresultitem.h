/* BEGIN_COMMON_COPYRIGHT_HEADER 
* END_COMMON_COPYRIGHT_HEADER */

#ifndef TORESULTITEM_H
#define TORESULTITEM_H

#include "config.h"
#include "tobackground.h"
#include "toresult.h"

#include <QScrollArea>
#include <qfont.h>
/* #include <QResizeEvent> */
#include <QGridLayout>

#include <vector>

class toSQL;
class toNoBlockQuery;

/** Display the first row of a query with each column with a separate label.
 * If the label of the item is a single '-' character the data is treated as
 * a horizontal label (Not bold or sunken), and the label is not printed.
 */

class toResultItem : public QScrollArea, public toResult
{
    Q_OBJECT

    /** Result widget.
     */
    QGridLayout *Result;

    /** Number of created widgets.
     */
    int NumWidgets;
    /** Last widget used.
     */
    int WidgetPos;
    /** number of columns
     */
    int Columns;
    /** List of allocated widgets.
     */
    std::vector<QWidget*> Widgets;

    /** If title names are to be made more readable.
     */
    bool ReadableColumns;
    /** If title names are to be displayed.
     */
    bool ShowTitle;
    /** Align widgets to the right.
     */
    bool Right;
    /** Font to display data with.
     */
    QFont DataFont;

    toNoBlockQuery *Query;
    toBackground Poll;

    /** Setup widget.
     * @param num Number of columns.
     * @param readable Make columns more readable.
     */
    void setup(int num, bool readable);
protected:
    /** Start new query, hide all widgets.
     */
    void start(void);
    /** Add a new widget.
     * @param title Title of this value.
     * @param value Value.
     */
    void addItem(const QString &title, const QString &value);
    /** Done with adding queries.
     */
    void done(void);
public:
    /** Create widget.
     * @param num Number of columns to arrange data in.
     * @param readable Indicate if columns are to be made more readable. This means that the
     * descriptions are capitalised and '_' are converted to ' '.
     * @param parent Parent of list.
     * @param name Name of widget.
     */
    toResultItem(int num, bool readable, QWidget *parent, const char *name = NULL);
    /** Create widget. Readable columns by default.
     * @param num Number of columns to arrange data in.
     * @param parent Parent of list.
     * @param name Name of widget.
     */
    toResultItem(int num, QWidget *parent, const char *name = NULL);
    /** Destroy object
     */
    ~toResultItem(void);

    /** Set if titles are to be shown.
     * @param val If titles are to be shown.
     */
    void showTitle(bool val)
    {
        ShowTitle = val;
    }
    /** Set if labels are to be aligned right.
     * @param val If labels are to be aligned right.
     */
    void alignRight(bool val)
    {
        Right = val;
    }
    /** Set the font to display data with.
     */
    void dataFont(const QFont &val)
    {
        DataFont = val;
    }

    /** Create title widget.
     * @param parent Parent of widget.
     * @return Title widget created.
     */
    virtual QWidget *createTitle(QWidget *parent);
    /** Create value widget.
     * @param parent Parent of widget.
     * @return Title widget created.
     */
    virtual QWidget *createValue(QWidget *parent);
    /** Set value of title widget.
     * @param widget Widget to use for title.
     * @param title The title of the new widget pair.
     * @param value Vaue of new title widget.
     */
    virtual void setTitle(QWidget *widget, const QString &title, const QString &value);
    /** Set value of value widget.
     * @param widget Widget to use for value.
     * @param title The title of the new widget pair.
     * @param value Vaue of new title widget.
     */
    virtual void setValue(QWidget *widget, const QString &title, const QString &value);

    /** Reimplemented for internal reasons.
     */
    virtual void query(const QString &sql, const toQList &param);

    /** Handle any connection
     */
    virtual bool canHandle(toConnection &)
    {
        return true;
    }

    // Why are these needed?
#if 1
    /** Set the SQL statement of this list
     * @param sql String containing statement.
     */
    void setSQL(const QString &sql)
    {
        toResult::setSQL(sql);
    }
    /** Set the SQL statement of this list. This will also affect @ref Name.
     * @param sql SQL containing statement.
     */
    void setSQL(const toSQL &sql)
    {
        toResult::setSQL(sql);
    }
    /** Set new SQL and run query.
     * @param sql New sql.
     * @see setSQL
     */
    void query(const QString &sql)
    {
        toResult::query(sql);
    }
    /** Set new SQL and run query.
     * @param sql New sql.
     * @see setSQL
     */
    void query(const toSQL &sql)
    {
        toResult::query(sql);
    }
    /** Set new SQL and run query.
     * @param sql New sql.
     * @see setSQL
     */
    void query(const toSQL &sql, toQList &par)
    {
        toResult::query(sql, par);
    }
#endif
public slots:
    /** Reimplemented for internal reasons.
     */
    virtual void refresh(void)
    {
        toResult::refresh();
    }
    /** Reimplemented for internal reasons.
     */
    virtual void changeParams(const QString &Param1)
    {
        toResult::changeParams(Param1);
    }
    /** Reimplemented For internal reasons.
     */
    virtual void changeParams(const QString &Param1, const QString &Param2)
    {
        toResult::changeParams(Param1, Param2);
    }
    /** Reimplemented for internal reasons.
     */
    virtual void changeParams(const QString &Param1, const QString &Param2, const QString &Param3)
    {
        toResult::changeParams(Param1, Param2, Param3);
    }
private slots:
    void poll(void);
};

#endif
