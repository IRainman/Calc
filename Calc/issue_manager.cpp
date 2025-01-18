// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "issue_manager.h"

std::vector<Issue> IssueManager::_errors;
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
std::vector<Issue> IssueManager::_warnings;
std::vector<Issue> IssueManager::_infos;
#endif
