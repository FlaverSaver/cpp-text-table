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

    TextTable(char horizontal = '-', char vertical = '|', char corner = '+') :
            _horizontal(horizontal),
            _vertical(vertical),
            _corner(corner) {}

    void setAlignment(unsigned i, Alignment alignment) {
        _alignment[i] = alignment;
    }

    void setMinWidth(unsigned i, unsigned width) {
        _minWidth[i] = width;
    }

    Alignment alignment(unsigned i) const { return _alignment[i]; }

    char vertical() const { return _vertical; }

    char horizontal() const { return _horizontal; }

    void add(std::string const &content) {
        _current.push_back(content);
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
        setupAlignment();
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
    Row _current;
    std::vector<Row> _rows;
    std::vector<unsigned long> mutable _width;
    std::map<unsigned long, unsigned int> mutable _minWidth;
    std::map<unsigned long, Alignment> mutable _alignment;

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
                _alignment[i] = Alignment::LEFT;
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
    for (auto rowIterator = table.rows().begin(); rowIterator != table.rows().end(); ++rowIterator) {
        TextTable::Row const &row = *rowIterator;
        stream << table.vertical();
        for (unsigned i = 0; i < row.size(); ++i) {
            auto alignment = table.alignment(i) == TextTable::Alignment::LEFT ? std::left : std::right;
            stream << std::setw((int) table.width(i)) << alignment;

            if (table.alignment(i) == TextTable::Alignment::CENTER) {
                stream << centered(row[i]);
            } else {
                stream << row[i];
            }

            stream << table.vertical();
        }
        stream << "\n";
        stream << table.ruler() << "\n";
    }

    return stream;
}
