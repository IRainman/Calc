// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//
// Created by Tamika Nomara on 28.05.2023.
//

#include "stdafx.h"

#include "issue_manager.h"

std::ostringstream& IssueManager::report(Issue issue) {
    _has_errors |= issue.severity == Issue::Severity::E;
    _errors.push_back(std::move(issue));
    return _errors.back().message;
}

std::ostringstream& IssueManager::report(Issue::Severity severity, size_t line, size_t pos) {
    return report(Issue{{}, severity, line, pos});
}

std::ostringstream& IssueManager::report(Issue::Severity severity, Token position) {
    return report(severity, position.line, position.pos);
}

std::ostringstream& IssueManager::info(size_t line, size_t pos) {
    return report(Issue::Severity::I, line, pos);
}
std::ostringstream& IssueManager::info(Token position) {
    return info(position.line, position.pos);
}

std::ostringstream& IssueManager::warning(size_t line, size_t pos) {
    return report(Issue::Severity::W, line, pos);
}
std::ostringstream& IssueManager::warning(Token position) {
    return warning(position.line, position.pos);
}

std::ostringstream& IssueManager::error(size_t line, size_t pos) {
    return report(Issue::Severity::E, line, pos);
}
std::ostringstream& IssueManager::error(Token position) {
    return error(position.line, position.pos);
}

std::ostream& operator<<(std::ostream& os, const Issue::Severity& severity) {
    switch (severity) {
        case Issue::Severity::I:
            return os << "Info";
        case Issue::Severity::W:
            return os << "Warning";
        case Issue::Severity::E:
            return os << "Error";
    }
}

std::ostream& operator<<(std::ostream& os, const Issue& issue) {
    os << issue.severity << " at line " << issue.line << ":" << issue.pos << ": " << issue.message.str();
    return os;
}

std::ostream& operator<<(std::ostream& os, const IssueManager& manager) {
    os << "Completed with "
       << manager.errors().size()
       << " message"
       << (manager.errors().size() == 1 ? "" : "s")
       << (manager.errors().empty() ? "." : ":")
       << "\n";

    for (auto& error: manager.errors()) {
        os << "  " << error << "\n";
    }

    return os;
}

#ifdef CALC_VER2_PATCHES

const std::string& IssueManager::to_string() const {
    static std::string out;
    out.clear();
    out += "Completed with "
        + std::to_string(errors().size())
        + " message"
        + (errors().size() == 1 ? "" : "s")
        + (errors().empty() ? "." : ":")
        + "\n";

    for (auto& error : errors()) {
        out += "  " + error.message.str() + "\n";
    }

    return out;
}

#endif
