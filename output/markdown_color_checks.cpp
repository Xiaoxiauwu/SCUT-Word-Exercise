#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <cctype>
#include <cassert>
#include <iostream>
enum class RunKind { Text, Strong, Emphasis, Strike, Code, Link, Image, Formula };
struct MarkdownRun { RunKind kind; std::string text, target; };
struct MarkdownBlock { std::vector<MarkdownRun> runs; float scale = 1.0f; bool code = false, rule = false; std::string language;
    std::vector<std::vector<std::string>> rows;
    std::vector<int> alignment;
};

std::string Trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r");
    return value.substr(first, last - first + 1);
}

std::string ReadGroup(const std::string& source, size_t& pos) {
    if (pos >= source.size() || source[pos] != '{') return {};
    const size_t start = ++pos;
    int depth = 1;
    while (pos < source.size() && depth) {
        if (source[pos] == '{') ++depth;
        else if (source[pos] == '}') --depth;
        ++pos;
    }
    return source.substr(start, depth ? source.size() - start : pos - start - 1);
}

std::string ScriptText(const std::string& source, bool superscript) {
    static const std::unordered_map<char, std::string> sup = {
        {'0',u8"⁰"},{'1',u8"¹"},{'2',u8"²"},{'3',u8"³"},{'4',u8"⁴"},{'5',u8"⁵"},
        {'6',u8"⁶"},{'7',u8"⁷"},{'8',u8"⁸"},{'9',u8"⁹"},{'+',u8"⁺"},{'-',u8"⁻"},
        {'=',u8"⁼"},{'(',u8"⁽"},{')',u8"⁾"},{'n',u8"ⁿ"},{'i',u8"ⁱ"}
    };
    static const std::unordered_map<char, std::string> sub = {
        {'0',u8"₀"},{'1',u8"₁"},{'2',u8"₂"},{'3',u8"₃"},{'4',u8"₄"},{'5',u8"₅"},
        {'6',u8"₆"},{'7',u8"₇"},{'8',u8"₈"},{'9',u8"₉"},{'+',u8"₊"},{'-',u8"₋"},
        {'=',u8"₌"},{'(',u8"₍"},{')',u8"₎"},{'a',u8"ₐ"},{'e',u8"ₑ"},{'i',u8"ᵢ"},
        {'o',u8"ₒ"},{'x',u8"ₓ"},{'n',u8"ₙ"},{'r',u8"ᵣ"},{'s',u8"ₛ"},{'t',u8"ₜ"},{'u',u8"ᵤ"}
    };
    const auto& table = superscript ? sup : sub;
    std::string out;
    for (char c : source) {
        auto found = table.find(c);
        if (found != table.end()) out += found->second;
        else out += c;
    }
    return out;
}

std::string LatexToUnicode(const std::string& source) {
    static const std::unordered_map<std::string, std::string> symbols = {
        {"alpha",u8"α"},{"beta",u8"β"},{"gamma",u8"γ"},{"delta",u8"δ"},
        {"epsilon",u8"ε"},{"theta",u8"θ"},{"lambda",u8"λ"},{"mu",u8"μ"},
        {"pi",u8"π"},{"rho",u8"ρ"},{"sigma",u8"σ"},{"tau",u8"τ"},
        {"phi",u8"φ"},{"omega",u8"ω"},{"Gamma",u8"Γ"},{"Delta",u8"Δ"},
        {"Theta",u8"Θ"},{"Lambda",u8"Λ"},{"Pi",u8"Π"},{"Sigma",u8"Σ"},
        {"Omega",u8"Ω"},{"times",u8"×"},{"cdot",u8"·"},{"div",u8"÷"},
        {"pm",u8"±"},{"le",u8"≤"},{"leq",u8"≤"},{"ge",u8"≥"},{"geq",u8"≥"},
        {"neq",u8"≠"},{"approx",u8"≈"},{"equiv",u8"≡"},{"infty",u8"∞"},
        {"sum",u8"∑"},{"prod",u8"∏"},{"int",u8"∫"},{"partial",u8"∂"},
        {"nabla",u8"∇"},{"in",u8"∈"},{"notin",u8"∉"},{"subset",u8"⊂"},
        {"cup",u8"∪"},{"cap",u8"∩"},{"to",u8"→"},{"rightarrow",u8"→"},
        {"leftarrow",u8"←"},{"Rightarrow",u8"⇒"},{"ldots",u8"…"},{"cdots",u8"⋯"}
    };
    std::string out;
    for (size_t i = 0; i < source.size();) {
        if (source[i] == '\\') {
            size_t j = i + 1;
            while (j < source.size() && std::isalpha(static_cast<unsigned char>(source[j]))) ++j;
            const std::string command = source.substr(i + 1, j - i - 1);
            if (command == "frac") {
                size_t p = j;
                const auto numerator = ReadGroup(source, p);
                const auto denominator = ReadGroup(source, p);
                if (p != j) { out += LatexToUnicode(numerator) + u8"⁄" + LatexToUnicode(denominator); i = p; continue; }
            } else if (command == "sqrt") {
                size_t p = j;
                const auto radicand = ReadGroup(source, p);
                if (p != j) { out += u8"√(" + LatexToUnicode(radicand) + ")"; i = p; continue; }
            } else if (command == "text" || command == "mathrm" || command == "mathbf") {
                size_t p = j;
                const auto contents = ReadGroup(source, p);
                if (p != j) { out += LatexToUnicode(contents); i = p; continue; }
            } else if (command == "left" || command == "right") {
                i = j; continue;
            } else if (command.empty() && j < source.size()) {
                out += source[j]; i = j + 1; continue;
            } else if (symbols.count(command)) {
                out += symbols.at(command); i = j; continue;
            }
            out += command.empty() ? "\\" : command;
            i = j;
            continue;
        }
        if (source[i] == '^' || source[i] == '_') {
            const bool superscript = source[i] == '^';
            size_t p = i + 1;
            const auto group = p < source.size() && source[p] == '{' ? ReadGroup(source, p)
                : p < source.size() ? source.substr(p, 1) : std::string();
            if (p < source.size() && source[i + 1] != '{') ++p;
            out += ScriptText(LatexToUnicode(group), superscript);
            i = p; continue;
        }
        if (source[i] == '{' || source[i] == '}') { ++i; continue; }
        if (source[i] == '~') { out += ' '; ++i; continue; }
        out += source[i++];
    }
    return out;
}

void AppendText(std::vector<MarkdownRun>& runs, RunKind kind, const std::string& text, const std::string& target = {}) {
    if (text.empty()) return;
    if (kind == RunKind::Text && !runs.empty() && runs.back().kind == kind && runs.back().target.empty())
        runs.back().text += text;
    else runs.push_back({kind, text, target});
}

size_t FindClosing(const std::string& source, size_t start, const std::string& delimiter) {
    return source.find(delimiter, start + delimiter.size());
}

void ParseInline(const std::string& source, std::vector<MarkdownRun>& runs, bool code = false) {
    if (code) { AppendText(runs, RunKind::Code, source); return; }
    size_t i = 0, plain = 0;
    const auto flush = [&](size_t end) { AppendText(runs, RunKind::Text, source.substr(plain, end - plain)); };
    while (i < source.size()) {
        bool consumed = false;
        const bool image = source.compare(i, 2, "![") == 0;
        const bool link = !image && source[i] == '[';
        if (image || link) {
            const size_t labelEnd = source.find(']', i + (image ? 2 : 1));
            if (labelEnd != std::string::npos && labelEnd + 1 < source.size() && source[labelEnd + 1] == '(') {
                size_t targetEnd = labelEnd + 2;
                int depth = 1;
                while (targetEnd < source.size() && depth) {
                    if (source[targetEnd] == '(') ++depth;
                    else if (source[targetEnd] == ')') --depth;
                    if (depth) ++targetEnd;
                }
                if (!depth) {
                    flush(i);
                    const std::string label = source.substr(i + (image ? 2 : 1), labelEnd - i - (image ? 2 : 1));
                    std::string target = Trim(source.substr(labelEnd + 2, targetEnd - labelEnd - 2));
                    const size_t title = target.find_first_of(" \t");
                    if (title != std::string::npos) target.resize(title);
                    runs.push_back({image ? RunKind::Image : RunKind::Link, image ? label : label, target});
                    i = targetEnd + 1; plain = i; consumed = true;
                }
            }
        }
        if (consumed) continue;
        std::string delimiter;
        RunKind kind = RunKind::Text;
        if (source.compare(i, 2, "**") == 0 || source.compare(i, 2, "__") == 0) { delimiter = source.substr(i, 2); kind = RunKind::Strong; }
        else if (source.compare(i, 2, "~~") == 0) { delimiter = "~~"; kind = RunKind::Strike; }
        else if (source[i] == '`') { delimiter = "`"; kind = RunKind::Code; }
        else if (source.compare(i, 2, "$$") == 0) { delimiter = "$$"; kind = RunKind::Formula; }
        else if (source[i] == '*' || source[i] == '_') { delimiter = source.substr(i, 1); kind = RunKind::Emphasis; }
        else if (source[i] == '$') { delimiter = "$"; kind = RunKind::Formula; }
        if (!delimiter.empty()) {
            const size_t end = FindClosing(source, i, delimiter);
            if (end != std::string::npos) {
                flush(i);
                std::string value = source.substr(i + delimiter.size(), end - i - delimiter.size());
                if (kind == RunKind::Formula) value = delimiter + value + delimiter;
                AppendText(runs, kind, value);
                i = end + delimiter.size(); plain = i; continue;
            }
        }
        ++i;
    }
    flush(source.size());
}

std::vector<std::string> TableCells(const std::string& line) {
    std::vector<std::string> cells;
    std::string cell;
    bool code = false;
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] == '\\' && i + 1 < line.size() && line[i + 1] == '|') {
            cell += '|'; ++i;
        } else if (line[i] == '`') { code = !code; cell += line[i]; }
        else if (line[i] == '|' && !code) { cells.push_back(Trim(cell)); cell.clear(); }
        else cell += line[i];
    }
    cells.push_back(Trim(cell));
    const std::string trimmed = Trim(line);
    if (!trimmed.empty() && trimmed.front() == '|' && !cells.empty()) cells.erase(cells.begin());
    if (!trimmed.empty() && trimmed.back() == '|' && !cells.empty()) cells.pop_back();
    return cells;
}

bool TableDelimiter(const std::string& line, std::vector<int>& alignment) {
    alignment.clear();
    for (auto cell : TableCells(line)) {
        if (cell.empty()) return false;
        const bool left = cell.front() == ':', right = cell.back() == ':';
        if (left) cell.erase(cell.begin());
        if (right && !cell.empty()) cell.pop_back();
        if (cell.size() < 3 || cell.find_first_not_of('-') != std::string::npos) return false;
        alignment.push_back(left && right ? 1 : right ? 2 : 0);
    }
    return !alignment.empty();
}

std::vector<MarkdownBlock> ParseMarkdown(const std::string& source) {
    std::vector<MarkdownBlock> blocks;
    std::istringstream input(source);
    std::string line;
    bool fenced = false;
    std::string codeLanguage;
    while (std::getline(input, line)) {
        const std::string rawLine = line;
        std::string trimmed = Trim(line);
        if (trimmed.rfind("```", 0) == 0 || trimmed.rfind("~~~", 0) == 0) {
            if (!fenced) codeLanguage = Trim(trimmed.substr(3));
            fenced = !fenced;
            continue;
        }
        if (fenced) {
            if (blocks.empty() || !blocks.back().code) {
                MarkdownBlock code;
                code.code = true;
                code.language = codeLanguage;
                blocks.push_back(std::move(code));
            }
            auto& code = blocks.back();
            if (!code.runs.empty()) code.runs.back().text += "\n";
            const std::string body = rawLine;
            size_t pos = 0;
            while (pos < body.size()) {
                const size_t start = pos;
                if (body.compare(pos, 2, "//") == 0 || body[pos] == '#') pos = body.size();
                else if (body[pos] == '"' || body[pos] == '\'') {
                    const char quote = body[pos++];
                    while (pos < body.size() && body[pos] != quote) {
                        if (body[pos] == '\\' && pos + 1 < body.size()) ++pos;
                        ++pos;
                    }
                    if (pos < body.size()) ++pos;
                } else if (std::isalpha(static_cast<unsigned char>(body[pos])) || body[pos] == '_') {
                    while (pos < body.size() && (std::isalnum(static_cast<unsigned char>(body[pos])) || body[pos] == '_')) ++pos;
                } else if (std::isspace(static_cast<unsigned char>(body[pos]))) {
                    while (pos < body.size() && std::isspace(static_cast<unsigned char>(body[pos]))) ++pos;
                } else ++pos;
                const std::string token = body.substr(start, pos - start);
                code.runs.push_back({RunKind::Code, token, {}});
            }
            if (code.runs.empty()) code.runs.push_back({RunKind::Code, "", {}});
            continue;
        }
        // A display formula is one inline run even when its source spans lines.
        // Do this outside fenced code, before Markdown block/table recognition.
        std::string formulaDelimiter;
        bool inlineCode = false;
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '\\') { ++i; continue; }
            if (line[i] == '`') { inlineCode = !inlineCode; continue; }
            if (inlineCode || line[i] != '$') continue;
            const std::string delimiter = line.compare(i, 2, "$$") == 0 ? "$$" : "$";
            if (formulaDelimiter.empty()) formulaDelimiter = delimiter;
            else if (formulaDelimiter == delimiter) formulaDelimiter.clear();
            i += delimiter.size() - 1;
        }
        if (!formulaDelimiter.empty()) {
            const auto resume = input.tellg();
            std::string combined = line, continuation;
            bool closed = false;
            while (std::getline(input, continuation)) {
                combined += "\n" + continuation;
                for (size_t i = 0; i < continuation.size(); ++i) {
                    if (continuation[i] == '\\') { ++i; continue; }
                    if (continuation.compare(i, formulaDelimiter.size(), formulaDelimiter) == 0) {
                        closed = true; break;
                    }
                }
                if (closed) break;
            }
            if (closed) { line = std::move(combined); trimmed = Trim(line); }
            else {
                input.clear();
                if (resume != std::streampos(-1)) input.seekg(resume);
                else input.setstate(std::ios::eofbit);
            }
        }
        if (line.find('|') != std::string::npos && line.find('\n') == std::string::npos) {
            const auto nextPosition = input.tellg();
            std::string delimiter;
            MarkdownBlock table;
            const auto header = TableCells(line);
            if (std::getline(input, delimiter) && TableDelimiter(delimiter, table.alignment) &&
                table.alignment.size() == header.size()) {
                table.rows.push_back(header);
                while (true) {
                    const auto rowPosition = input.tellg();
                    std::string row;
                    if (!std::getline(input, row)) break;
                    if (Trim(row).empty() || row.find('|') == std::string::npos) {
                        input.clear(); input.seekg(rowPosition); break;
                    }
                    auto cells = TableCells(row);
                    cells.resize(header.size());
                    table.rows.push_back(std::move(cells));
                }
                blocks.push_back(std::move(table));
                continue;
            }
            input.clear();
            if (nextPosition != std::streampos(-1)) input.seekg(nextPosition);
            else input.setstate(std::ios::eofbit);
        }
        // Keep leading whitespace in normal text. Structural checks use the
        // trimmed view, while the content itself retains indentation.
        const std::string structural = trimmed;
        MarkdownBlock block;
        if (structural.empty()) { blocks.push_back(block); continue; }
        if (structural == "---" || structural == "***" || structural == "___") { block.rule = true; blocks.push_back(block); continue; }
        int heading = 0;
        while (heading < static_cast<int>(structural.size()) && heading < 6 && structural[heading] == '#') ++heading;
        if (heading && heading < static_cast<int>(structural.size()) &&
            (structural[heading] == ' ' || structural[heading] == '\t')) {
            block.scale = heading == 1 ? 1.55f : heading == 2 ? 1.4f : heading == 3 ? 1.28f : heading == 4 ? 1.18f : heading == 5 ? 1.1f : 1.05f;
            line = Trim(structural.substr(heading + 1));
        } else if (structural.rfind("> ", 0) == 0) {
            block.scale = 0.95f;
            line = u8"│ " + structural.substr(2);
        } else if (structural.rfind("- ", 0) == 0 || structural.rfind("* ", 0) == 0 || structural.rfind("+ ", 0) == 0) {
            line = u8"• " + structural.substr(2);
        }
        ParseInline(line, block.runs);
        blocks.push_back(std::move(block));
    }
    return blocks;
}

std::string NormalizeColors(const std::string& tex, size_t& pos, bool grouped = false) {
    std::string out;
    while (pos < tex.size()) {
        if (tex[pos] == '}' && grouped) { ++pos; break; }
        if (tex.compare(pos, 6, "\\color") == 0 &&
            (pos + 6 == tex.size() || !std::isalpha(static_cast<unsigned char>(tex[pos + 6])))) {
            size_t p = pos + 6;
            while (p < tex.size() && std::isspace(static_cast<unsigned char>(tex[p]))) ++p;
            if (p < tex.size() && tex[p] == '{') {
                const size_t end = tex.find('}', p + 1);
                if (end != std::string::npos) {
                    const std::string color = tex.substr(p + 1, end - p - 1);
                    pos = end + 1;
                    out += "\\fgcolor{" + color + "}{" + NormalizeColors(tex, pos, grouped) + "}";
                    return out;
                }
            }
        }
        if (tex[pos] == '\\' && pos + 1 < tex.size()) {
            out += tex[pos++]; out += tex[pos++];
        } else if (tex[pos] == '{') {
            ++pos;
            out += "{" + NormalizeColors(tex, pos, true) + "}";
        } else out += tex[pos++];
    }
    return out;
}


int main() {
 auto inlineMath=ParseMarkdown("text $\\frac{\n1\n}{2}$ end");
 assert(inlineMath.size()==1 && inlineMath[0].runs.size()==3);
 assert(inlineMath[0].runs[1].kind==RunKind::Formula);
 assert(ParseMarkdown("$x$ and $$y$$").size()==1);
 assert(ParseMarkdown("`$literal`\nafter").size()==2);

 auto multiline=ParseMarkdown(R"(Before
$$
f\left(
   \left[
 \frac{
   1+\left\{x,y\right\}
 }{
   \left(\frac{x}{y}+\frac{y}{x}\right)\left(u+1\right)
 }+a
   \right]^{3/2}
\right)
$$
After)");
 assert(multiline.size()==3);
 assert(multiline[1].runs.size()==1 && multiline[1].runs[0].kind==RunKind::Formula);
 assert(multiline[1].runs[0].text.find("\\frac{\n")!=std::string::npos);
 auto unclosed=ParseMarkdown("$$\n# title\nafter");
 assert(unclosed.size()==3 && unclosed[1].scale>1);
 auto fenced=ParseMarkdown("```latex\n$$\nx\n$$\n```");
 assert(fenced.size()==1 && fenced[0].code);

 size_t p=0;
 assert(NormalizeColors(R"(\color{red}a+{\color{blue}b}+c)",p)==R"(\fgcolor{red}{a+{\fgcolor{blue}{b}}+c})");
 p=0; assert(NormalizeColors(R"(\textcolor{#F4E040}{x})",p)==R"(\textcolor{#F4E040}{x})");
 auto b=ParseMarkdown("| Name | Formula |\n| :--- | ---: |\n| red | $\\color{red}x$ |");
 assert(b.size()==1 && b[0].rows.size()==2 && b[0].alignment[1]==2);
 b=ParseMarkdown("| A | B |\n| --- | --- |\n| x | y |\n\n# Heading\n#compact");
 assert(b.size()==4 && b[2].scale>1 && b[3].scale==1);
 b=ParseMarkdown("| unfinished"); assert(b.size()==1 && b[0].rows.empty());
 assert(TableCells(R"(| a\|b | c |)")[0]=="a|b");
 std::cout << "Markdown table and scoped color checks passed\n";
}
