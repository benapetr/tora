/*****
*
* TOra - An Oracle Toolkit for DBA's and developers
* Copyright (C) 2003-2008 Quest Software, Inc
* Portions Copyright (C) 2008 Other Contributors
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation;  only version 2 of
* the License is valid for this program.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*      As a special exception, you have permission to link this program
*      with the Oracle Client libraries and distribute executables, as long
*      as you follow the requirements of the GNU GPL in regard to all of the
*      software in the executable aside from Oracle client libraries.
*
*      Specifically you are not permitted to link this program with the
*      Qt/UNIX, Qt/Windows or Qt Non Commercial products of TrollTech.
*      And you are not permitted to distribute binaries compiled against
*      these libraries without written consent from Quest Software, Inc.
*      Observe that this does not disallow linking to the Qt Free Edition.
*
*      You may link this product with any GPL'd Qt library such as Qt/Free
*
* All trademarks belong to their respective owners.
*
*****/

#ifndef TOQVALUE_H
#define TOQVALUE_H

#include "config.h"

#include <list>
#include <QString>
#include <QVariant>


/**
 * This is now a wrapper around QVariant to avoid a lot of memory
 * copies. Choosing not to subclass QVariant though -- it's a
 * complicated class and it'd be easy to screw it up.
 *
 */
class toQValue
{
    QVariant Value;

public:
    /** Create null value.
     */
    toQValue(void);

    /** Create integer value.
     * @param i Value.
     */
    toQValue(int i);
    /** Create string value.
     * @param str Value.
     */
    toQValue(const QString &str);
    /** Create double value.
     * @param d Value.
     */
    toQValue(double d);
    /** Create qlonglong value.
     * @param d Value.
     */
    toQValue(qlonglong d);
    /** Create qulonglong value.
     * @param d Value.
     */
    toQValue(qulonglong d);
    /** Destruct query.
     */
    ~toQValue();

    /** Create a copy of a value.
     */
    toQValue(const toQValue &copy);
    /** Assign this value from another value.
     */
    const toQValue &operator = (const toQValue &copy);

    /** Less than operator
     */
    bool operator<(const toQValue &other) const;

    /** greater than operator
     */
    bool operator>(const toQValue &other) const;

    /** Less than operator
     */
    bool operator<=(const toQValue &other) const;

    /** greater than operator
     */
    bool operator>=(const toQValue &other) const;

    /** Check a value for equality. Requires same type and same value. NULL is equal to other NULL values.
     */
    bool operator == (const toQValue &) const;

    /** Check if this is an int value.
     */
    bool isInt(void) const;
    /** Check if this is a double value.
     */
    bool isDouble(void) const;
    /** Check if this is a string value.
     */
    bool isString(void) const;
    /** Check if this value is null.
     */
    bool isNull(void) const;
    /** Check if this value is one of the number types. Returns true for NULL.
     */
    bool isNumber(void) const;
    /** Check if this value is binary.
     */
    bool isBinary(void) const;

    /** Get toUtf8 format of this value.
     */
    QString toUtf8(void) const;
    /** Get integer representation of this value.
     */
    int toInt(void) const;
    /** Get double representation of this value.
     */
    double toDouble(void) const;

    /** Check if this value is long.
     */
    bool isLong(void) const;
    /** Get Long representation of this value
     */
    qlonglong toLong(void) const;

    /** Check if this value is ulong.
     */
    bool isuLong(void) const;
    /** Get Ulong representation of this value
     */
    qulonglong touLong(void) const;

    /** Convert value to a QVariant
     */
    QVariant toQVariant(void) const;

    /** Get binary representation of value. Can only be called when the data is actually binary.
     */
    const QByteArray toByteArray(void) const;

    /** Convert value to a string. If binary convert to hex.
     */
    operator QString() const;

    /** Convert value to a string. If binary convert to hex.
     */
    QString toString() const;

    QString toSIsize() const;

    /** Set numberformat.
     * @param format 0 = Default, 1 = Scientific, 2 = Fixed Decimals
     * @param decimals Number on decimals for fixed decimals.
     */
    static void setNumberFormat(int format, int decimals);
    /** Format a number according to current settings.
     */
    static QString formatNumber(double number);
    /** Get number format.
     * @return Format 0 = Default, 1 = Scientific, 2 = Fixed Decimals
     */
    static int numberFormat(void);
    /** Get decimals if fixed decimals.
     */
    static int numberDecimals(void);
    /** Create a binary value
     */
    static toQValue createBinary(const QByteArray &arr);
    /** Create a binary value from it's hex representation.
     */
    static toQValue createFromHex(const QByteArray &hex);
    /** Create a binary value from it's hex representation.
     */
    static toQValue createFromHex(const QString &hex);
    /** Create value from qvariant
     */
    static toQValue fromVariant(const QVariant &);
};

/** A short representation of list<toQuery::queryValue>
 */
typedef std::list<toQValue> toQList;

#endif
