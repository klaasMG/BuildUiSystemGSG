//
// Created by klaas on 4/2/2026.
//

#ifndef SUPERBUILD_STRING_UTILS_H
#define SUPERBUILD_STRING_UTILS_H

inline std::string toLower(std::string s) {
    for (char& c : s)
        c = std::tolower((unsigned char)c);
    return s;
}

inline std::string insertLine(std::string text, int lineIndex, const std::string& newLine){
    size_t pos = 0;
    int line = 0;

    while (line < lineIndex && pos != std::string::npos)
    {
        pos = text.find('\n', pos);

        if (pos != std::string::npos)
            pos++; // move past '\n'

        line++;
    }

    if (pos == std::string::npos)
        text += newLine + "\n";   // append if too short
    else
        text.insert(pos, newLine + "\n");

    return text;
}

inline std::string trim(std::string s) {
    // left trim
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch) { return !std::isspace(ch); }));

    // right trim
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());

    return s;
}

inline std::pair<std::string, std::string> split_once(const std::string& s, char delim) {
    size_t pos = s.find(delim);
    if (pos == std::string::npos)
        return {s, ""};

    return {
        s.substr(0, pos),
        s.substr(pos + 1)
    };
}


inline std::string replace_first(const std::string& input,const std::string& sub,const std::string& repl){
    std::string s = input;
    size_t pos = s.find(sub);
    if (pos != std::string::npos) {
        s.replace(pos, sub.length(), repl);
    }
    return s;
}

inline char nextNonSpace(const std::string& s, size_t pos) {
    size_t i = pos + 1;  // start after current char
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) {
        ++i;
    }
    return (i < s.size()) ? s[i] : '\0'; // '\0' if none found
}

#endif //SUPERBUILD_STRING_UTILS_H