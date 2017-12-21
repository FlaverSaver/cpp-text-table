#include <iostream>
#include <map>
#include <iomanip>
#include <vector>
#include <string>

class TextTable {

public:
    enum class Alignment {
        LEFT, RIGHT, CENTER
    };
    typedef std::vector<std::string> Row;

    TextTable(char horizontal = '-', char vertical = '|', char corner = '+', bool headerOnlyTable = false) :
            _horizontal(horizontal),
            _vertical(vertical),
            _corner(corner),
            _headerOnlyTable(headerOnlyTable) {}

    void setAlignment(unsigned column, Alignment alignment) {
        _alignmentDefault[column] = alignment;
    }

    void setAlignment(unsigned row, unsigned column, Alignment alignment) {
        _alignment[row][column] = alignment;
    }

    void setMinWidth(unsigned column, unsigned width) {
        _minWidth[column] = width;
    }

    Alignment alignment(unsigned row, unsigned column) const {
        if (_alignment.count(row) > 0 && _alignment[row].count(column) > 0) {
            return _alignment[row][column];
        }

        if(_alignmentDefault.count(column) > 0) {
            return _alignmentDefault[column];
        }

        return Alignment::LEFT;
    }

    char vertical() const { return _vertical; }

    char horizontal() const { return _horizontal; }

    bool isHeaderOnlyTable() const { return  _headerOnlyTable; }

    void add(std::string const &content) {
        _current.push_back(content);
    }

    void add(std::string const &content, Alignment alignment) {
        setAlignment((unsigned int) _rows.size(), (unsigned int) _current.size(), alignment);
        add(content);
    }

    void endOfRow() {
        _rows.push_back(_current);
        _current.assign(0, "");
    }

    template<typename Iterator>
    void addRow(Iterator begin, Iterator end) {
        for (auto i = begin; i != end; ++i) {
            add(*i);
        }
        endOfRow();
    }

    template<typename Container>
    void addRow(Container const &container) {
        addRow(container.begin(), container.end());
    }

    std::vector<Row> const &rows() const {
        return _rows;
    }

    void setup() const {
        determineWidths();
    }

    std::string ruler() const {
        std::string result;
        result += _corner;
        for (auto width = _width.begin(); width != _width.end(); ++width) {
            result += repeat(*width, _horizontal);
            result += _corner;
        }

        return result;
    }

    unsigned long width(unsigned i) const { return _width[i]; }

private:
    char _horizontal;
    char _vertical;
    char _corner;
    bool _headerOnlyTable;
    Row _current;
    std::vector<Row> _rows;
    std::vector<unsigned long> mutable _width;
    std::map<unsigned long, unsigned int> mutable _minWidth;
    std::map<unsigned long, Alignment> mutable _alignmentDefault;
    std::map<unsigned long, std::map<unsigned int, Alignment>> mutable _alignment;

    static std::string repeat(unsigned long times, char c) {
        std::string result;
        for (; times > 0; --times)
            result += c;

        return result;
    }

    unsigned long columns() const {
        return _rows[0].size();
    }

    void determineWidths() const {
        _width.assign(columns(), 0);
        for (auto rowIterator = _rows.begin(); rowIterator != _rows.end(); ++rowIterator) {
            Row const &row = *rowIterator;
            for (unsigned i = 0; i < row.size(); ++i) {
                _width[i] = std::max((unsigned long) std::max(_width[i], row[i].size()), (unsigned long) _minWidth[i]);
            }
        }
    }

    void setupAlignment() const {
        for (unsigned i = 0; i < columns(); ++i) {
            if (_alignment.find(i) == _alignment.end()) {
                _alignment[i][0] = Alignment::LEFT; //_alignment[zeile] = [spalte, Alignment]
            }
        }
    }
};

//region Alignment.Center
//https://stackoverflow.com/a/14861289
template<typename charT, typename traits = std::char_traits<charT> >
class center_helper {
    std::basic_string<charT, traits> str_;
public:
    center_helper(std::basic_string<charT, traits> str) : str_(str) {}

    template<typename a, typename b>
    friend std::basic_ostream<a, b> &operator<<(std::basic_ostream<a, b> &s, const center_helper<a, b> &c);
};

template<typename charT, typename traits = std::char_traits<charT> >
center_helper<charT, traits> centered(std::basic_string<charT, traits> str) {
    return center_helper<charT, traits>(str);
}

// redeclare for std::string directly so we can support anything that implicitly converts to std::string
center_helper<std::string::value_type, std::string::traits_type> centered(const std::string &str) {
    return center_helper<std::string::value_type, std::string::traits_type>(str);
}

template<typename charT, typename traits>
std::basic_ostream<charT, traits> &
operator<<(std::basic_ostream<charT, traits> &s, const center_helper<charT, traits> &c) {
    std::streamsize w = s.width();
    if (w > c.str_.length()) {
        std::streamsize left = (w + c.str_.length()) / 2;
        s.width(left);
        s << c.str_;
        s.width(w - left);
        s << "";
    } else {
        s << c.str_;
    }
    return s;
}
//endregion

std::ostream &operator<<(std::ostream &stream, TextTable const &table) {
    table.setup();
    stream << table.ruler() << "\n";
    unsigned int rowNum = 0;
    for (auto rowIterator = table.rows().begin(); rowIterator != table.rows().end(); ++rowIterator, rowNum++) {
        TextTable::Row const &row = *rowIterator;
        stream << table.vertical();
        for (unsigned column = 0; column < row.size(); ++column) {
            auto alignment = table.alignment(rowNum, column) == TextTable::Alignment::LEFT ? std::left : std::right;
            stream << std::setw((int) table.width(column)) << alignment;

            if (table.alignment(rowNum, column) == TextTable::Alignment::CENTER) {
                stream << centered(row[column]);
            } else {
                stream << row[column];
            }

            stream << table.vertical();
        }
        stream << "\n";

        if(!table.isHeaderOnlyTable() || rowNum < 1) {
            stream << table.ruler() << "\n";
        }
    }

    return stream;
}
