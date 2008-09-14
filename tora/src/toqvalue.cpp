/* BEGIN_COMMON_COPYRIGHT_HEADER 
* END_COMMON_COPYRIGHT_HEADER */

#include "utils.h"

#include "toconf.h"
#include "toqvalue.h"

#include <QVariant>
#include <QApplication>

static int NumberFormat;
static int NumberDecimals;


toQValue::toQValue(int i)
{
    Value = i;
}

toQValue::toQValue(double i)
{
    Value = i;
}

toQValue::toQValue(qlonglong d)
{
    Value = d;
}

toQValue::toQValue(qulonglong d)
{
    Value = d;
}

toQValue::toQValue(const toQValue &copy)
{
    Value = copy.Value;
}

toQValue::toQValue(const QString &str)
{
    Value = str;
}

toQValue::toQValue()
{
}

toQValue::~toQValue()
{
}

bool toQValue::operator<(const toQValue &other) const
{
    if (isInt() && other.isInt())
        return toInt() < other.toInt();
    if (isDouble() && other.isDouble())
        return toDouble() < other.toDouble();
    if (isLong() && other.isLong())
        return toLong() < other.toLong();
    if (isuLong() && other.isuLong())
        return touLong() < other.touLong();
    if (isBinary() && other.isBinary())
        return Value.toByteArray() < other.Value.toByteArray();

    // otherwise, try to convert to double for comparison
    bool ok;
    double d1 = toString().toDouble(&ok);
    if (ok)
    {
        double d2 = other.toString().toDouble(&ok);
        if (ok)
            return d1 < d2;
    }

    return toString() < other.toString();
}


bool toQValue::operator>(const toQValue &other) const
{
    return !operator<(other);
}


bool toQValue::operator<=(const toQValue &other) const
{
    if (isInt() && other.isInt())
        return toInt() <= other.toInt();
    if (isDouble() && other.isDouble())
        return toDouble() <= other.toDouble();
    if (isLong() && other.isLong())
        return toLong() <= other.toLong();
    if (isuLong() && other.isuLong())
        return touLong() <= other.touLong();
    if (isBinary() && other.isBinary())
        return Value.toByteArray() <= other.Value.toByteArray();

    // otherwise, try to convert to double for comparison
    bool ok;
    double d1 = toString().toDouble(&ok);
    if (ok)
    {
        double d2 = other.toString().toDouble(&ok);
        if (ok)
            return d1 <= d2;
    }

    return toString() <= other.toString();
}


bool toQValue::operator>=(const toQValue &other) const
{
    return !operator<=(other);
}


const toQValue &toQValue::operator = (const toQValue & copy)
{
    Value = copy.Value;
    return *this;
}

bool toQValue::isNumber() const
{
    return isInt() || isDouble() || isLong() || isuLong();
}

bool toQValue::operator == (const toQValue &val) const
{
    return Value == val.Value;
}

QVariant toQValue::toQVariant() const
{
    return Value;
}

bool toQValue::isInt() const
{
    return Value.type() == QVariant::Int;
}

bool toQValue::isDouble() const
{
    return Value.type() == QVariant::Double;
}

bool toQValue::isuLong() const
{
    return Value.type() == QVariant::ULongLong;
}

bool toQValue::isLong() const
{
    return Value.type() == QVariant::LongLong;
}

bool toQValue::isString() const
{
    return Value.type() == QVariant::String;
}

bool toQValue::isBinary() const
{
    return Value.type() == QVariant::ByteArray;
}

bool toQValue::isNull() const
{
    return Value.isNull();
}

const QByteArray toQValue::toByteArray() const
{
    return Value.toByteArray();
}

QString toQValue::toUtf8() const
{
    return Value.toString();
}

QString toQValue::toString() const
{
    return Value.toString();
}

int toQValue::toInt() const
{
    return Value.toInt();
}

double toQValue::toDouble() const
{
    return Value.toDouble();
}

qlonglong toQValue::toLong() const
{
    return Value.toLongLong();
}

qulonglong toQValue::touLong() const
{
    return Value.toULongLong();
}

void toQValue::setNumberFormat(int format, int decimals)
{
    NumberFormat = format;
    NumberDecimals = decimals;
}

QString toQValue::formatNumber(double number)
{
    if (number == int(number))
        return QString::number(number);
    switch (NumberFormat)
    {
    case 1:
    {
        char buf[100];
        sprintf(buf, "%E", number);
        return buf;
    }
    case 2:
    {
        char buf[100];
        sprintf(buf, "%0.*f", NumberDecimals, number);
        return buf;
    }
    default:
        return QString::number(number);
    }
}

int toQValue::numberFormat()
{
    return NumberFormat;
}

int toQValue::numberDecimals()
{
    return NumberDecimals;
}

toQValue toQValue::fromVariant(const QVariant &val)
{
    toQValue ret;
    ret.Value = val;
    return ret;
}

toQValue toQValue::createBinary(const QByteArray &arr)
{
    toQValue ret;
    ret.Value = arr;
    return ret;
}

toQValue toQValue::createFromHex(const QByteArray &hex)
{
    QByteArray arr;
    for (int i = 0;i < hex.length();i += 2)
    {
        int num;
        char c = hex.at(i);
        if (c >= 'a')
            num = c - 'a';
        else if (c >= 'A')
            num = c - 'A';
        else
            num = c - '0';
        num <<= 4;
        c = hex.at(i + 1);
        if (c >= 'a')
            num += c - 'a';
        else if (c >= 'A')
            num += c - 'A';
        else
            num += c - '0';
//         arr.at(i / 2) = num;
        arr[i / 2] = num;
    }
    return createBinary(arr);
}

toQValue toQValue::createFromHex(const QString &hex)
{
    QByteArray arr;
    for (int i = 0;i < hex.length();i += 2)
    {
        int num;
// qt4        char c = hex.at(i);
        char c = hex.at(i).toAscii();
        if (c >= 'a')
            num = c - 'a';
        else if (c >= 'A')
            num = c - 'A';
        else
            num = c - '0';
        num <<= 4;
// qt4         c = hex.at(i + 1);
        c = hex.at(i + 1).toAscii();
        if (c >= 'a')
            num += c - 'a';
        else if (c >= 'A')
            num += c - 'A';
        else
            num += c - '0';
        arr[i / 2] = num;
    }
    return createBinary(arr);
}

toQValue::operator QString() const
{
    return Value.toString();
}


QString toQValue::toSIsize() const
{
    if (this->isNull())
        return NULL;

    double size = toDouble();
    QString s = "%1";
    int i = 0;

    while (size / 1024 >= 10)
    {
        i++;
        size = size / 1024;
    }

    switch (i)
    {
    case 0:
        break;
    case 1:
        s.append("K");
        break;
    case 2:
        s.append("M");
        break;
    case 3:
        s.append("G");
        break;
    case 4:
        s.append("T");
        break;
    default:
        s.append("E");
    }

    return s.arg(size, 0, 'f', 0);
}
